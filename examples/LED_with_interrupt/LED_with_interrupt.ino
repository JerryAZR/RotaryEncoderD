/**
 * @file LED_brightness.ino
 * @author Zerui An (anzerui@126.com / jerryazr@gmail.com)
 * @brief Use the rotary encoder to control the brightness of the builtin LED,
 * but this time using custom interrupt service routines (ISRs).
 * @version 1.1.0
 * @date 2022-04-01
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

const int pwm_period  = 1000; // 1000 microseconds
const int pwm_step    = 20;
volatile int pwm_on_time = 0; // volatile because it will be modified by ISRs.
// Typically, one would want to use locks or atomic operations when modifying
// variables shared between threads. However, I have decided that it is not
// necessary in this program because:
//  1. The CPU is fast while the rotary encoder is slow
//  2. By default interrupts are disabled during ISR so no race condition is
//     possible between two ISRs.

RotaryEncoderD<CLK_PIN, DT_PIN> encoder;

void setup() {
  // Initialize the encoder
  encoder.begin();

  // Configure the builtin LED as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Attach custom ISRs
  // The increment_duty_cycle function will be called on every forward step
  encoder.attachForwardInterrupt(increment_duty_cycle);
  // The decrement_duty_cycle function will be called on every backward step
  encoder.attachBackwardInterrupt(decrement_duty_cycle);
}

void loop() {
  // In the basic LED_brightness example, we would read the encoder output
  // here and update pwm_on_time accordingly. This is no longer necessary
  // because the updates are now handled by the ISRs

  // run PWM generator
  // Not all pins support analogWrite, so we use delayMicoseconds and
  // digitalWrite to achieve a similar effect
  // Reverse the two digitalWrite() calls for active low LED
  digitalWrite(LED_BUILTIN, HIGH);
  delayMicroseconds(pwm_on_time);
  digitalWrite(LED_BUILTIN, LOW);
  delayMicroseconds(pwm_period - pwm_on_time);
}

void increment_duty_cycle() {
  if (pwm_on_time < pwm_period) {
    pwm_on_time += pwm_step;
  }
}

void decrement_duty_cycle() {
  if (pwm_on_time > 0) {
    pwm_on_time -= pwm_step;
  }
}

