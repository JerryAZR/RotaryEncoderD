# RotaryEncoderD Library for Arduino

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Arduino Lint](https://github.com/JerryAZR/RotaryEncoderD/actions/workflows/arduino-lint-action.yml/badge.svg)](https://github.com/JerryAZR/RotaryEncoderD/actions/workflows/arduino-lint-action.yml)
[![Compile Examples](https://github.com/JerryAZR/RotaryEncoderD/actions/workflows/compile-examples.yml/badge.svg)](https://github.com/JerryAZR/RotaryEncoderD/actions/workflows/compile-examples.yml)

## Introduction

A library for debouncing any [KY-040-like](https://www.handsontec.com/dataspecs/module/Rotary%20Encoder.pdf) rotary encoder.
The "D" in the library name stands for "Debounce".


This library implements an interrupt-driven debouncing technique. If you are
looking for a polling-only technique, I would suggets the
[RotaryEncoder](https://github.com/mathertel/RotaryEncoder) library by Matthias
Hertel, which implements a state-machine-based approach.

## Download

There are two ways to download this library:

1. Download the entire directory as a zip archive and extract it to
`<Arduino_sketch_root_folder>/libraries/`. This method gives you access to all
the examples and syntax highlighting.

2. Download the [RotaryEncoderD.h](src/RotaryEncoderD.h) and place it in your
sketch folder, next to the `.ino` file where the `setup()` and `loop()`
functions are defined. This method makes it very easy to modify the source code.

## Quick Start Guide

First, declare the encoder object. Note that both `CLK_PIN` and `DT_PIN`
should support interrupt.

```cpp
#include <RotaryEncoderD.h>

const uint8_t CLK_PIN = 0;
const uint8_t DT_PIN  = 1;

RotaryEncoderD<CLK_PIN, DT_PIN> encoder;
```

Next, initialize the encoder in the `setup()` function.

```cpp
void setup() {
  // Calling begin() would set both pins as INPUT and
  // attach corresponding ISRs
  encoder.begin();

  // if the encoder does not have interal pull up,
  // redeclare those ports as INPUT_PULLUP
  // pinMode(CLK_PIN, INPUT_PULLUP);
  // pinMode(DT_PIN, INPUT_PULLUP);
}
```

Finally, call `read()` in the `loop()` function to retrieve the information of
the last pulse. The pulse information is updated on each encoder tick and is
cached until the next call to `read()`. This design choice makes it very
unlikely to miss a pulse.

```cpp
void loop() {
  // Read the encoder output
  int encoder_output = encoder.read();
  // Perform actions based on the output
  if (encoder_output == encoder.FORWARD) {
    // do something
  } else if (encoder_output == encoder.BACKWARD) {
    // do something else
  }
}
```



## Background

Contact bounce is a common problem with mechanical switches and rotary encoders.
When I was building a volume-control knob with an Arduino and some cheap rotary
encoder. I noticed that the volume is often changing randomly, regardless of
which way I turn the knob (due to contact bounce). As a hardware engineer, I'm
aware of several debouncing circuits and ICs (i.e., MAX6816/6817/6818). However,
I would like my product to be as small as possible, so I started looking into
software debouncing techniques.


Most of the posts I found online suggested using the `delay()` function.
Although this method did mitigate the contact bounce problem, it did not fix
the problem entirely. I was still seeing occational random changes in volume
even after I have carefully tuned the delay parameters. While trying the
delay-based method, I have also noticed a limitation with any polling-based
encoder readers. In general, the software determines the rotation direction of
the encoder by reading the DT (data) pin at the rising or falling edge of the
clock. If a polling-based method missed the clock edge, it might return an
incorrect value. Therefore, I have concluded that an interrupt-based method is
required for any reliable encoder reader.


Luckily, I came across [this post](http://www.technoblogy.com/show?1YHJ) which
states the key observation:

> When one of the signals (either CLK or DT) of a rotary encoder is switching
(bouncing), the other must be stable (unless the knob is turned too fast).

Based on this observation, I implemented an interrupt-based rotary encoder
reader. IN contrast to the original implemenation, mine uses two interrupt pins
and generates exactly one pulse each step. In addition, no pulses are lost when
starting the rotation or changing the direction.
