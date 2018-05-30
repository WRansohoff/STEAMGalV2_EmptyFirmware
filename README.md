# Overview

This is some example starting firmware for a V2 'STEAMGal' board driven by an STM32 chip - now with a color OLED display!

Some comments might not be completely accurate since parts of this code have been copy/pasted between projects, but I think most of it should be decent.

# Current Status

Mostly working; the program can display to the onboard SSD1331 display using the drawing functions in `util_c.c`, and the buttons listen with falling-edge hardware interrupts defined in `interrupts_c.c`. There are also utility methods for configuring basic timer interrupts.

It is intended as a quick starting point when writing projects for the board.

Currently, only the F0 and L0 lines are supported - specifically, only the F031K6, F051K8, and L082KZ. On these 32-pin chips, the L0 lines use a 32.768KHz 'LSE' crystal oscillator, while the F0 lines use an 8MHz 'HSE' one. But this project does not any use external oscillators, and I hope to add the STM32F303K8 as well if time permits.
