# Overview

This is some example starting firmware for a V2 'STEAMGal' board driven by an STM32 chip - now with a color OLED display!

Some comments might not be completely accurate since parts of this code have been copy/pasted between projects, but I think most of it should be decent.

# Current Status

Mostly working; the program can display to the onboard SSD1331 display using the drawing functions in `util_c.c`, and the buttons listen with falling-edge hardware interrupts defined in `interrupts_c.c`. There are also utility methods for configuring basic timer interrupts.

It is intended as a quick starting point when writing projects for the board.

Currently, only the STM32F051K8 and STM32F031K6 are supported, but I hope to add the STM32F303K8 as well if time permits, along with L0-series chips, which should also work with the board if an LSE crystal is used in place of an HSE.
