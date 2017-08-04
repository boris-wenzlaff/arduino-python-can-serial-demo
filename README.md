# Sketch arduino-python-can-serial-demo
Sketch for testing and demonstrating the serial communication with the [python-can](https://github.com/hardbyte/python-can) serial interface.
The timestamp is a 4 byte unsigned long (the time in ms since start). The Sketch will also print the formated incoming serial frame to the serial output.

See also [arduino-serial-hex-sniffer](https://github.com/boris-wenzlaff/arduino-serial-hex-sniffer) to observe the raw python-can serial communication (to do so, deactivate all serial debug outputs in this sketch).

## Two modes
### Cyclic Mode
Sends a data frame every second.

### Echo Mode
Receives a data frame, increments the first byte by one and sends it back with the CAN ID 2.

## Default values
Mode: Echo Mode
CAN ID: 2
Baudrate: 115200
Delay for cyclic send: 1 second
Data for cyclic send: 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
DLC for cyclic send: 8

## The circuit
Connect to PC via USB.

## History
Originally written to test the serial communication of the [python-can](https://github.com/hardbyte/python-can) serial interface.
