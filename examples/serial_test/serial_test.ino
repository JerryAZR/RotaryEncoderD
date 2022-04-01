/**
 * @file serial_test.ino
 * @author Zerui An (anzerui@126.com / jerryazr@gmail.com)
 * @brief Basic rotary encoder demo using a serial monitor.
 * @version 1.0.0
 * @date 2022-03-31
 * 
 * @copyright Copyright (c) 2022 Zerui An
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>. 
 * 
 */

#include <RotaryEncoderD.h>

// Both CLK_PIN and DT_PIN should support interrupt mode CHANGE
// Set them according to your circuit and board specification
const uint8_t CLK_PIN = 0;
const uint8_t DT_PIN  = 1;

RotaryEncoderD<CLK_PIN, DT_PIN> encoder;

void setup() {
  // Start the serial port
  Serial.begin(9600);

  // initialize the encoder
  encoder.begin();
}

void loop() {
  // Read the encoder output
  int encoder_output = encoder.read();
  // Perform actions based on the output
  if (encoder_output == encoder.FORWARD) {
    Serial.println("Forward rotation detected");
  } else if (encoder_output == encoder.BACKWARD) {
    Serial.println("Backward rotation detected");
  }

  // Add a limit to encoder update frequency using delay()
  delay(100);
}
