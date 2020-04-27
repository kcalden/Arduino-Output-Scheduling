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

# Send an output state to the device
def digitalWrite(pin, state,delay):
    time_delay_bytelist = list(delay.to_bytes(4,'big'))
    sched_item_bytelist = [pin,state]
    sched_item_bytelist.extend(time_delay_bytelist)
    print(sched_item_bytelist)
    port.write(bytearray(sched_item_bytelist))


# TODO: Make sure that we read in two bytes
# Request items left 
def getItemsLeft():
    # Request number of items that can be written
    port.write(bytearray([0x01]))
    # Read bytes in from the serial port
    # Convert the bytes into an unsigned integer
    items_left_bytes = port.read(2)
    return int.from_bytes(items_left_bytes, byteorder='big', signed=False)

# Calculate counter value for the MCU
def calcDelay(seconds):
    if(seconds > MAX_TIME): return 0xFFFFFFFF
    return int(seconds/TIME_PER_CYCLE)

## TEST CODE ##
items_left = 10

digitalWrite(13, 1, calcDelay(0.25))
digitalWrite(13, 0, calcDelay(0.25))
digitalWrite(13, 1, calcDelay(0.25))
digitalWrite(13, 0, calcDelay(0.25))
# while(items_left > 0):
#     digitalWrite(LED_BUILTIN, 1, 0xFFFFF)
#     items_left = items_left - 1
#     digitalWrite(LED_BUILTIN, 0, 0xFFFFF)
#     items_left = items_left - 1

while(True):
    pass
