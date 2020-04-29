## Serial data

The MCU is configured with a baudrate of 115200 bps and a timeout of 100ms.

### Output states
Output states are sent in packets of 6 bytes:

`[ PIN STATE DELAY3 DELAY2 DELAY1 DELAY0 ]`

### Commands
Commands are sent as a single byte followed by atleast a 100ms delay.


Command | Byte
---| --
Request items left | 0x01