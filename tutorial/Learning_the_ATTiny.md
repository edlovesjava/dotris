# Summary

This series introduces the ATtiny85, a tiny yet capable 8-bit
microcontroller from Microchip Technology. Readers will explore its
features, learn how to program it using the Arduino IDE, and progress
from basic LED blinking to creating a simple video game. The guide aims
to teach microcontroller programming, hardware interfacing, and game
logic in an accessible way for both beginners and experienced makers.

TL;DR;  [Here is the code](../ATTiny_Tetrix_buttons/ATTiny_Tetrix_buttons.ino) you can see right away. Read at your leasure.  To see the game in action, visit https://youtube.com/shorts/krgI_fMw9mU?si=cYCCL6R_st3__IlO

## Introduction — What We're Building

In this series of posts, I am excited to introduce my new friend: the
ATtiny85. This microcontroller unit (MCU), made by Microchip Technology,
is incredibly tiny---about the size of a pea---but packs a surprising
amount of power for an 8-bit processor.

When I was young, I learned programming on a Timex Sinclair 1000, which
used the 8-bit ZX81 processor. Back then, we barely had 1 MHz of clock
speed and only 1.2 KB of RAM to work with, including both program and
memory (after accounting for the screen buffer and other essential
functions). It also had to run a version of BASIC. Programming an MCU
today is much more sophisticated, yet just as fun.

In this series, we'll briefly get to know the ATtiny85, learn how to
program it (starting with the classic \"Hello World\" of the MCU/Arduino
world: the blink program), and then move on to something more exciting: a
simple but real video game! While limited, it will be a great
demonstration of programming the ATtiny85.

### What is the ATtiny85?

The ATtiny85 is a compact yet powerful 8-bit AVR microcontroller from
Microchip Technology. Despite its tiny size---roughly the size of a
pea---it offers an 8-bit RISC architecture, 8 KB of flash memory, 512
bytes of SRAM, and 6 general-purpose I/O pins. This makes it an
excellent choice for embedded projects where space and power consumption
are critical.

### Why Choose the ATtiny85?

Its small footprint, low power consumption, and affordability have made
the ATtiny85 a favorite among hobbyists, makers, and professionals alike.
It supports a variety of communication protocols such as SPI and I2C and
can be programmed using popular tools like the Arduino IDE, making it
accessible for beginners and experts.

### What You Will Learn

Throughout this series, you will learn how to program the ATtiny85
starting from basic LED blinking to building a simple handheld
falling-block game. Along the way, you'll gain insights into
microcontroller programming, hardware interfacing, and game logic
implementation.

### Result

By the end of this series, you'll have built a tiny handheld game
console powered by the ATtiny85, complete with a MAX7219 LED matrix
display and button controls.

Images and wiring diagrams will be included to guide you through each
step of the build.

*References:*

-   \"Beginner's Guide to the ATtiny85: Tiny, Cheap, Powerful,\" AVR8
    Team, 2025【1】

-   \"ATTiny85 Microcontroller: Guide for DIY Projects,\" Reversepcb,
    2023【2】

-   \"ATtiny85 Microcontroller Pinout, Features, Applications and
    Programming,\" MicrocontrollersLab【3】

## Meet the ATtiny85

The ATtiny85 is an 8-bit AVR RISC-based microcontroller from Microchip
Technology. It comes in an 8-pin DIP package as well as smaller
surface-mount versions. For prototyping without soldering, the DIP-8
package is ideal as it fits easily into breadboards.

This microcontroller is known for its low power consumption, making it
excellent for battery-powered projects. When running continuously, a
setup with 3 AAA batteries might last a few days, but with efficient
intermittent use, battery life can extend to months or even years.

Key specifications include 8 KB of flash memory for program storage, 512
bytes of EEPROM for non-volatile data retention between power cycles,
and 512 bytes of SRAM for fast runtime data access. These resources make
it suitable for a wide range of embedded applications, from sensor
interfacing to simple control systems.

While 8 KB of flash might seem limited compared to modern computers, it
is quite sufficient for many embedded projects. However, it is not
designed for running complex software that would take a lot of memory,
program space or high speed like large language models.

For detailed technical information, refer to the official datasheet:
<https://ww1.microchip.com/downloads/en/devicedoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf>

### Use of MCUs vs MPUs

Microcontrollers are designed to be low cost, lower power, lower
physical footprint, but the applications are usually limited to
supporting hardware with intelligence and control, particularly
integrating with sensors and controlling connected devices such as
motors, power, lights, cameras, etc. In contrast microprocessors are
much beefier and tend to support advanced human interaction capabilities
and are intended to support the software itself as the device. While you
could create a web-based application and host it on a MCU, it would be
limited, even if there was a way to connect to it. Some MCUs might
bridge the gap and have enough power for such a thing. I like the ESP32
series for its powerful processors, wireless connectivity options and
nice headroom in RAM and flash, but they tend to be power hungry and a
bit more expensive. But this article is not a guide on what MCU to
choose, it's just focused on the joy of using them.

## Turning an Arduino Nano into an ATtiny85 Programmer

See [Programming the ATTiny](./section3_programming_the_ATTiny.md)

## Building the Game Board

See [Building the Game Board](./section4_building_the_game_board.md)

## Building the Falling‑Block Game

See [Writing the game DOTris](./section6_writing_the_Game.md)

## Possible additions

See [Possible Additions](./section7_possible_additions.md)

## Full Code Listing

-   Final polished version of the game

See [The completed example](../ATTiny_Tetrix_buttons/ATTiny_Tetrix_buttons.ino)

