/**
 * @file serial_test.ino
 * @author Zerui An (anzerui@126.com / jerryazr@gmail.com)
 * @brief Basic rotary encoder demo using a serial monitor.
 * @version 1.0.0
 * @date 2022-03-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <RotaryEncoderDebounced.h>

const uint8_t CLK_PIN = 0;
const uint8_t DT_PIN  = 1;

RotaryEncoderDebounced<CLK_PIN, DT_PIN> encoder;

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
