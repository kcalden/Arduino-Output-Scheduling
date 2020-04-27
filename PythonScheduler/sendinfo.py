import serial
import time

port = serial.Serial('COM5', 115200)
time.sleep(1)

def digitalWrite(pin, state,delay):
    time_delay_bytelist = list(delay.to_bytes(4,'big'))
    sched_item_bytelist = [pin,state]
    sched_item_bytelist.extend(time_delay_bytelist)
    # print(sched_item_bytelist)
    port.write(bytearray(sched_item_bytelist))
    
pulses = 15
## SCHEDULED
# for x in range(int(pulses)):
while(True):
    digitalWrite(13,1,0xFFFFF)
    time.sleep(0.4)
    digitalWrite(13,0,0xFFFFF)
    time.sleep(0.4)

# digitalWrite(13,1,0xFFFFF)

## LIVE
# DUTY = 0.2
# while(True):
#     digitalWrite(13, 1, int(20*DUTY))
#     digitalWrite(13,0, int(20*(1-DUTY)))
#     time.sleep(0.02)

while(True):
    pass