import serial
import time

port = serial.Serial('COM5', 115200)
time.sleep(1)

# DEFINITIONS
LED_BUILTIN = 13 # LED PIN
PRESCALER = 8
FREQ = 16000000
TIME_PER_CYCLE = PRESCALER/FREQ
MAX_TIME = TIME_PER_CYCLE*0xFFFFFFFF

def checksum(byte_list):
    sum = 0
    for b in byte_list:
        sum = sum + b

    return sum & 0xFF
    
def sendPacket(byte_list):
    # Compute checksum of the packet we're sending
    # and set the checksum of the response to something impossible
    sent_checksum = checksum(byte_list)
    # print("OUT\t" + str(bytes(byte_list)) + " * " + str(sent_checksum))
    resp_checksum = -1

    while(resp_checksum != sent_checksum):
        port.write(bytearray(byte_list))
        response = list(port.read(8))
        resp_checksum = response[0]
    
    # print("IN\t" + str(bytes(response)))
    return response

# Send an output state to the device
# Return true if the write is successful
def digitalWrite(pin, state,delay):
    # Signal output state write
    packet = [0x02]

    # Add pin and state
    packet.extend([pin,state])

    # Add time delay
    packet.extend(list(delay.to_bytes(4,'big')))

    # Zero pad packet
    packet.extend([0])
    resp = sendPacket(packet)
    return resp[1] == 0

# Request items left
# Return number of items that can be sent
def requestItemsLeft():
    # Request number of items that can be written
    resp = sendPacket([0x01,0x01,0,0,0,0,0,0])

    # Read bytes in from the serial port and
    # Convert the bytes into an unsigned integer
    items_left_bytes = resp[1:3]
    return int.from_bytes(items_left_bytes, byteorder='big', signed=False)

# Calculate counter value for the MCU
def calcDelay(seconds):
    if(seconds > MAX_TIME):
        print("ERR: Gave time more than MAX_TIME")
        return 0xFFFFFFFF
    return int(seconds/TIME_PER_CYCLE)


led_pin_state = 0
while(True):
    while(requestItemsLeft() > 0):
        digitalWrite(LED_BUILTIN, led_pin_state,calcDelay(0.1))
        led_pin_state = not led_pin_state