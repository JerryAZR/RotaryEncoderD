/**
 * @file LED_brightness.ino
 * @author Zerui An (anzerui@126.com / jerryazr@gmail.com)
 * @brief Use the rotary encoder to control the brightness of the builtin LED
 * @version 1.0.0
 * @date 2022-03-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <RotaryEncoderDebounced.h>

const uint8_t CLK_PIN = 0;
const uint8_t DT_PIN  = 1;
const uint8_t SW_PIN  = 2;

const int pwm_period  = 1000; // 1000 microseconds
const int pwm_step    = 20;
int pwm_on_time       = 0;

RotaryEncoderDebounced<CLK_PIN, DT_PIN> encoder;

void setup() {
  // Initialize the encoder
  encoder.begin();

  // Configure the builtin LED as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Some encoders have a SW (switch) pin
  pinMode(SW_PIN, INPUT);
}

void loop() {
  // Read the encoder output
  int encoder_output = encoder.read();
  // Perform actions based on the output
  if (encoder_output == encoder.FORWARD) {
    // Increase brightness
    if (pwm_on_time < pwm_period) {
      pwm_on_time += pwm_step;
    }
  } else if (encoder_output == encoder.BACKWARD) {
    // Decrease brightness
    if (pwm_on_time > 0) {
      pwm_on_time -= pwm_step;
    }
  }

  // read the switch pin input
  // Assuming active low
  if (digitalRead(SW_PIN) == 0) {
    // toggle the on/off state by setting pwm duty cycle to 100% or 0%
    if (pwm_on_time > 0) {
      pwm_on_time = 0;
    } else {
      pwm_on_time = pwm_period;
    }

    // debounce the switch
    delay(10);
    while (digitalRead(SW_PIN) == 0); // wait for the switch to be released
    delay(10);
  }

  // run PWM generator
  // Not all pins support analogWrite, so we use delayMicoseconds and
  // digitalWrite to achieve a similar effect
  // Reverse the two digitalWrite() calls for active low LED
  digitalWrite(LED_BUILTIN, HIGH);
  delayMicroseconds(pwm_on_time);
  digitalWrite(LED_BUILTIN, LOW);
  delayMicroseconds(pwm_period - pwm_on_time);

  // What would happen if the knob is turned during delay?
  // The value (FORWARD / BACKWARD) is preserved until read() is called.
}
