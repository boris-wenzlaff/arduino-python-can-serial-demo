/*
  Sketch arduino-python-can-serial-demo

  Sketch for testing and demonstrating the serial communication with the python-can serial interface.
  The sending timestamp is a 4 byte unsigned long, the time in ms since start.
  The Sketch will also print the formated incoming serial frame to the serial output.

  Two modes:
    Cyclic Mode: Sends a data frame every second.
    Echo   Mode: Receives a data frame, increments the first byte by one and sends it back with the CAN ID 2.

  Default values:
    Mode: Echo Mode
    CAN ID: 2
    Baudrate: 115200

    Delay for cyclic send: 1 second
    Data for cyclic send: 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
    DLC for cyclic send: 8

  The circuit:
  Connect to PC via USB.

  Copyright 2017 Boris Wenzlaff

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#define START_FRAME       0xAA       // start byte for serial frame
#define END_FRAME         0xBB       // end byte for serial frame

#define CAN_ID            2L         // can id for outgoing messages
#define SERIAL_BAUDRATE   115200     // baud rate for serial communication 115200
#define MODE_CYCLIC       0
#define MODE_ECHO         1
#define CYCLIC_DELAY      1000       // delay in ms for cyclic send mode
#define CYCLIC_DLC        8          // length of data

const byte CYCLIC_DATA[CYCLIC_DLC] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}; // data for cyclic send mode

const int mode = MODE_ECHO; // set the mode for the sketch
byte can_serial_frame[19];    // buffer for the serial frame

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("arduino-python-can-serial-demo");
}

void loop() {
  // cyclic mode
  if (mode == MODE_CYCLIC) {
    int frame_length = create_frame(CAN_ID, CYCLIC_DATA, CYCLIC_DLC);
    Serial.write(can_serial_frame, frame_length);
    delay(CYCLIC_DELAY);
  }

  // echo mode
  if (mode == MODE_ECHO) {
    byte buf[4];
    if (Serial.available() > 0) {
      // start of frame
      if (Serial.read() == START_FRAME) {
        Serial.print("Rx");
        buf[0] = START_FRAME;
        printBytesHex(buf, 1);
        // 4 byte timestamp
        Serial.readBytes(buf, 4);
        printBytesHex(buf, 4);
        // dlc
        byte dlc;
        dlc = Serial.read();
        printBytesHex(buf, 1);
        // can id
        Serial.readBytes(buf, 4);
        printBytesHex(buf, 4);
        if (dlc > 0x00) {
          // payload / data
          byte data[dlc];
          Serial.readBytes(data, dlc);
          printBytesHex(data, dlc);
          // end of frame
          buf[0] = Serial.read();
          printBytesHex(buf, 1);
          if (buf[0] == END_FRAME) {
            // if frame IO send it back
            // icrement first data byte
            data[0]++;
            int frame_length = create_frame(CAN_ID, data, dlc);
            Serial.print("Tx[");
            Serial.write(can_serial_frame, frame_length);
          }
        }
        else {
          printBytesHex({0x00}, 0);
          // no data received, add data byte with value 1
          byte data[1] = {0x01};
          int frame_length = create_frame(CAN_ID, data, 1);
          Serial.print("Tx[");
          Serial.write(can_serial_frame, frame_length);
        }
      }
      Serial.println("]");
    }
  }
}

int create_frame(unsigned long can_id, byte* data, int dlc) {
  int a_position  = 0;
  byte buf[4];
  unsigned long timestamp = millis(); // micros();

  // start of frame
  can_serial_frame[a_position++] = START_FRAME;

  // timestamp
  buf[0] = timestamp;
  buf[1] = timestamp >> 8;
  buf[2] = timestamp >> 16;
  buf[3] = timestamp >> 24;
  for (int i = 0; i < 4; i++) {
    can_serial_frame[a_position++] = buf[i];
  }

  // dlc
  can_serial_frame[a_position++] = dlc;

  // can id
  buf[0] = can_id;
  buf[1] = can_id >> 8;
  buf[2] = can_id >> 16;
  buf[3] = can_id >> 24;
  for (int i = 0; i < 4; i++) {
    can_serial_frame[a_position++] = buf[i];
  }

  // payload
  for (int i = 0; i < dlc; i++) {
    can_serial_frame[a_position++] = data[i];
  }

  // end of frame
  can_serial_frame[a_position] = END_FRAME;
  return ++a_position;
}

/**
   Prints bytes as hex values
*/
void printBytesHex(byte *b, int l) {
  Serial.print("[");
  for (int i = 0; i < l; i++) {
    if (b[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(b[i], HEX);
    if (i < l - 1) {
      Serial.print(" ");
    }
  }
  Serial.print("]");
}

