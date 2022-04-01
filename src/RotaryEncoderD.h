/**
 * @file RotaryEncoderD.h
 * @author Zerui An (anzerui@126.com / jerryazr@gmail.com)
 * 
 * @brief This is a template class for debouncing KY-040-like rotary encoders.
 * The "D" in the library name stands for "Debounce".
 * Unlike many other rotary encoder libraries which use delay-based debouncing,
 * this library uses an interrupt-based approach.
 * Note that this library does NOT debounce the sw pin.
 * 
 * @todo Add support for user-defined interrupts
 * 
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
 */

#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include <Arduino.h>

template<uint8_t clk_pin, uint8_t dt_pin>
class RotaryEncoderD {
private:
    /* data */
    // clk pin to encoder map
    static RotaryEncoderD<clk_pin, dt_pin>* map[11];
    // pin numbers (declared as template)
    // uint8_t clk_pin;    // clock pin
    // uint8_t dt_pin;     // data pin
    bool active_low;
    // pin values
    volatile uint8_t clk_val;
    volatile uint8_t dt_val;
    volatile int state;

    static void clk_trigger_isr(); // update dt value
    static void dt_trigger_isr(); // update clk value
public:
    // return values
    static const int NO_ACTION  = 0;
    static const int FORWARD    = 1;
    static const int BACKWARD   = 2;

    RotaryEncoderD(bool active_low = true);
    ~RotaryEncoderD();
    virtual void begin();
    virtual int read();
};

template<uint8_t clk_pin, uint8_t dt_pin>
RotaryEncoderD<clk_pin, dt_pin>* RotaryEncoderD<clk_pin, dt_pin>::map[11] = {0};

/**
 * @brief Construct a new RotaryEncoderD<clk_pin, dt_pin>::RotaryEncoderD object
 * 
 * @tparam clk_pin - pin number of the clk (clock) pin
 * @tparam dt_pin - pin number of the dt (data) pin
 * @param active_low - whether the encoder is active low. defaults to true
 */
template<uint8_t clk_pin, uint8_t dt_pin>
RotaryEncoderD<clk_pin, dt_pin>::RotaryEncoderD(bool active_low)
: active_low(active_low), state(NO_ACTION), clk_val(1), dt_val(1) {
    map[clk_pin] = this;
}

template<uint8_t clk_pin, uint8_t dt_pin>
RotaryEncoderD<clk_pin, dt_pin>::~RotaryEncoderD() {
    map[clk_pin] = 0;
    detachInterrupt(digitalPinToInterrupt(clk_pin));
    detachInterrupt(digitalPinToInterrupt(dt_pin));
}

/**
 * @brief Set up pin modes and interrupt handlers for both the clk and dt pin
 * By default the INPUT mode is used. if other mode is desired, call pinMode()
 * on these pins again with the desired mode after calling begin().
 * 
 * @tparam clk_pin 
 * @tparam dt_pin 
 */
template<uint8_t clk_pin, uint8_t dt_pin>
void RotaryEncoderD<clk_pin, dt_pin>::begin() {
    pinMode(clk_pin, INPUT);
    pinMode(dt_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(clk_pin), clk_trigger_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(dt_pin), dt_trigger_isr, CHANGE);
}

/**
 * @brief Read the encoder state and reset it to NO_ACTION.
 * Note that the encoder state does not reset automatically. Manual reset
 * can be achieved by calling read().
 * 
 * @tparam clk_pin 
 * @tparam dt_pin 
 * @return int - one of NO_ACTION, FORWARD, or BACKWARD
 */
template<uint8_t clk_pin, uint8_t dt_pin>
int RotaryEncoderD<clk_pin, dt_pin>::read() {
    int oldState = state;
    state = NO_ACTION;
    return oldState;
}

/**
 * @brief Interrupt Service Routine (ISR) triggered by clock pin on CHANGE.
 * Updates the di pin reading.
 * 
 * @tparam clk_pin 
 * @tparam dt_pin 
 */
template<uint8_t clk_pin, uint8_t dt_pin>
void RotaryEncoderD<clk_pin, dt_pin>::clk_trigger_isr() {
    RotaryEncoderD<clk_pin, dt_pin>* encoder = map[clk_pin];
    uint8_t tmp = digitalRead(dt_pin);
    encoder->dt_val = encoder->active_low ? tmp : !tmp;
}

/**
 * @brief Interrupt Service Routine (ISR) triggered by dt pin on CHANGE.
 * Updates the clk pin reading. Also updates state if conditions are met.
 * 
 * @tparam clk_pin 
 * @tparam dt_pin 
 */
template<uint8_t clk_pin, uint8_t dt_pin>
void RotaryEncoderD<clk_pin, dt_pin>::dt_trigger_isr() {
    RotaryEncoderD<clk_pin, dt_pin>* encoder = map[clk_pin];
    uint8_t previous_clk = encoder->clk_val;
    uint8_t tmp = digitalRead(clk_pin);
    encoder->clk_val = encoder->active_low ? tmp : !tmp;

    // we want the data pin to be active (dt_val == 0)
    if (encoder->dt_val) return;
    // only read at clock edge
    if (previous_clk == encoder->clk_val) return;
    encoder->state = encoder->clk_val ? FORWARD : BACKWARD;
}

#endif
