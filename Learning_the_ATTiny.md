## Summary

This series introduces the ATtiny85, a tiny yet capable 8-bit
microcontroller from Microchip Technology. Readers will explore its
features, learn how to program it using the Arduino IDE, and progress
from basic LED blinking to creating a simple video game. The guide aims
to teach microcontroller programming, hardware interfacing, and game
logic in an accessible way for both beginners and experienced makers.

**1. Introduction — What We're Building**

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

**What is the ATtiny85?**

The ATtiny85 is a compact yet powerful 8-bit AVR microcontroller from
Microchip Technology. Despite its tiny size---roughly the size of a
pea---it offers an 8-bit RISC architecture, 8 KB of flash memory, 512
bytes of SRAM, and 6 general-purpose I/O pins. This makes it an
excellent choice for embedded projects where space and power consumption
are critical.

**Why Choose the ATtiny85?**

Its small footprint, low power consumption, and affordability have made
the ATtiny85 a favorite among hobbyists, makers, and professionals alike.
It supports a variety of communication protocols such as SPI and I2C and
can be programmed using popular tools like the Arduino IDE, making it
accessible for beginners and experts.

**What You Will Learn**

Throughout this series, you will learn how to program the ATtiny85
starting from basic LED blinking to building a simple handheld
falling-block game. Along the way, you'll gain insights into
microcontroller programming, hardware interfacing, and game logic
implementation.

**Result**

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

**2. Meet the ATtiny85**

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

**Use of MCUs vs MPUs**

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

**3. Turning an Arduino Nano into an ATtiny85 Programmer**

-   Why we need a programmer

-   Wiring table (Nano → ATtiny85)

-   Optional 10 μF capacitor on Nano RESET

-   Uploading ArduinoISP

-   Installing ATtiny core in Arduino IDE

-   Setting board, clock, programmer

-   Burning bootloader (sets fuses)

**Overview**

To program the ATtiny85, you need a programmer because the chip itself
doesn't have a built-in USB interface. While dedicated programmers
exist, you can use an Arduino Nano as a cost-effective and flexible ISP
(In-System Programmer). This section guides you through the wiring,
setup, and programming steps.

**Parts and Tools Needed**

-   Breadboard

-   Arduino Nano (any clone works)

-   ATtiny85 microcontroller

-   Jumper wires (solid gauge .64mm/22 AWG is best IMHO)

-   10 μF electrolytic capacitor

-   LED and 220 Ohm resistor (for test program)

-   Computer with Arduino IDE installed

-   USB cable (for Nano)

-   Breadboard power supply (optional, for standalone power)

**Wiring Connections**

Connect the Arduino Nano to the ATtiny85 as follows:

| Arduino Nano Pin | ATtiny85 Pin    | Function            |
|------------------|-----------------|---------------------|
| D13              | Pin 7 (SCK)     | Serial Clock        |
| D12              | Pin 6 (MISO)    | Master In Slave Out |
| D11              | Pin 5 (MOSI)    | Master Out Slave In |
| D10              | Pin 1 (RESET)   | Reset               |
| 5V               | Pin 8 (Vcc)     | Power               |
| GND              | Pin 4 (GND)     | Ground              |

**Note:** You will need to connect a 10 μF capacitor between RESET and
GND on the Nano to prevent auto-reset during programming. Remember the
negative (striped) side of the capacitor goes to GND.

**Step-by-Step Tutorial**

**1. Prepare the Arduino Nano**

1.  Plug the Nano into the breadboard.

2.  Wire the Nano to the ATtiny85 as shown in the table above.

**2. Set Up the Arduino IDE**

1.  Open Arduino IDE on your computer.

2.  Go to **File \> Examples \> 11.ArduinoISP** and select the
    ArduinoISP sketch.

3.  Choose the correct board (**Arduino Nano**) and port.

4.  Upload the ArduinoISP sketch to the Nano.

5.  Insert the 10 μF capacitor between the Nano's RESET and GND pins.

**3. Install the ATtiny Core**

1.  In the Arduino IDE, go to **Tools \> Board \> Boards Manager**.

2.  Add the ATtiny core using the provided URL:\
    https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json

3.  Install 'attiny' by David A Mellis.

4.  Select **ATtiny 25/45/85** as the board.

5.  Set the clock to **8 MHz**.

6.  Choose **ATtiny85** as the processor.

7.  Set the programmer to **Arduino as ISP**.

**4. Burn the Bootloader (you only need to do this once for a new ATTiny
chip)**

1.  Place the ATtiny85 on the breadboard and wire it to the Nano.

2.  In Arduino IDE, go to **Tools \> Burn Bootloader** to set the fuses
    for the ATtiny85.

**5. Upload Your First Program**

1.  Write or select your sketch (e.g., Blink).

2.  Modify the sketch to use pin 0 instead of LED_BUILTIN.

3.  Use **Sketch \> Upload Using Programmer** (not just Upload) to
    program the ATtiny85.

**6. Test the Setup**

1.  Connect pin 5 of the ATtiny85 to one end of a 220 Ohm resistor.

2.  Connect the other end to the anode of an LED; cathode to ground.

3.  Power the circuit and confirm the LED blinks.

**Troubleshooting Tips**

-   Double-check all wiring, especially the capacitor orientation.

-   Ensure the correct board, processor, and programmer are selected in
    Arduino IDE.

-   If programming fails, try resetting the Nano or re-uploading the
    ArduinoISP sketch.

The blink code

```c
// Blink for ATtiny85 on Pin 0 (physical pin 5)
void setup() {
  pinMode(0, OUTPUT); // ATtiny85 pin 0
}

void loop() {
  digitalWrite(0, HIGH);
  delay(500);
  digitalWrite(0, LOW);
  delay(500);
}
```

**6. Building the Falling‑Block Game**

What we are building?

A game like Tetris but with simple falling dots

Initially I just wanted to do something different with the matrix and I
thought about random dropping led lights. Then I thought about stacking
them where if the light below another was on, it would stop and another
light would start at the top. But that looked like Tetris so I thought
why not make a row disappear if it is completed. Then I added some
effects to show off the removed row. Then I increased the speed of
dropping over time, then added buttons! And it became a game.

**6.1 Framebuffer + Display Driver**

The display consists of a straightforward 8x8 grid of bits, structured
as a single-dimensional array of eight bytes, with each byte
representing one row and each bit indicating a column. The first row
corresponds to array index 0 (since arrays are zero-indexed), and within
each byte (using \`uint8_t\`), bit 0 refers to column 0, while bit 7
represents column 7.

```c
byte matrix[8];
```

Bitwise operations such as `&` and `|` make setting or clearing bits
efficient and compact. To set a bit in a byte you can OR it with a row.
To set a 1 at a particular bit of a byte you can shift `>>` a 1 the
number of bits and then OR (`|`) with the row.

When programming microcontrollers (MCUs), conserving memory is
essential, so every byte and bit matters. Not only do you use 'byte' and
bitwise operations but you'll frequently see us use the `uint8_t`
type defined in `<stdint.h>`, which ensures consistent size and
range across platforms. Avoiding generic `int` types prevents
unnecessary memory usage—`int` can be at least two bytes and often
four, wasting valuable space.

**How does the 8x8 matrix module operate?**

By multiplexing and scanning.

Individual LEDs aren't directly addressed, which would require 64
separate connections. Instead, shift registers and multiplexing
techniques are used to select specific rows and then columns within
those rows in rapid succession. The grid values are stored in memory,
and the LEDs refresh quickly enough that the human eye cannot detect any
flicker.

The MAX7219 interface uses three pins as part of the Serial Peripheral
Interface (SPI):

1.  Data In

2.  Clock

3.  Chip Select/Load

Note: The ATtiny85 has a USI (Universal Serial Interface) rather than
true hardware SPI, so we bit-bang the protocol manually in software.
This gives us full control over timing and works reliably with the
MAX7219.

You send data for each row as an 8-bit byte; a 1 in a bit position
represents an LED as on, and 0 as OFF. The MAX7219 includes registers
you can address for controlling aspects of the matrix, such as
brightness and scan limits. (and you can daisy-chain multiple matrix
modules for longer strings of displays).

The matrix's 8 row pins (*anodes*) and 8 column pins
(*cathodes*) are connected. The system quickly activates
each row in turn, sending signals to columns to light selected LEDs for
each row. Due to rapid switching, your eye perceives a continuous image
(*persistence of vision*).

**6.2 Basic Falling Pixel**

-   Random column

-   Falling one row at a time

-   Collision detection

-   Landing logic

-   Writing into `matrix[row]`

The program starts out with a blank 8x8 matrix represented by an array of
0's. Sending this to the matrix using the connected pins will turn off
all LEDs.

The workhorse routine for this is sendCmd, sendByte and updateDisplay

```c
void sendByte(uint8_t b) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(CLK, LOW);
    digitalWrite(DIN, (b >> i) & 1);
    digitalWrite(CLK, HIGH);
  }
}

void sendCmd(uint8_t addr, uint8_t data) {
  digitalWrite(CS, LOW);
  sendByte(addr);
  sendByte(data);
  digitalWrite(CS, HIGH);
}

void updateDisplay() {
  for (uint8_t row = 0; row < 8; row++) {
    sendCmd(row + 1, matrix[row]);
  }
}
```

sendByte is made up of 8 bits representing each of the 8 LEDs in the
row with 1 is on and 0 as off. So each of these bits is sent to the
interface using the clock to signal when the bit is to be latched and
when it is to be consumed and ready for the next bit. Thus the loop for
each bit

1.  pull the clock low

2.  send the bit value (1 or 0)

3.  pull the clock high

sendCmd deals with setting either a specific row or setting register on
the MAX to control values such as intensity. The first addresses 1-8
represent the rows, 9-15 are registers that can control different
aspects of the display. You call sendCmd given an address and a byte
value to send. It first writes the address byte then the data byte using
sendByte described above.


```c
// MAX7219 Register Addresses
#define MAX7219_REG_DECODE_MODE 0x09
#define MAX7219_REG_INTENSITY 0x0A  // brightness 0-15
#define MAX7219_REG_SCAN_LIMIT 0x0B
#define MAX7219_REG_SHUTDOWN 0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F
```


updateDisplay is responsible for taking the entire 8x8 matrix and sending
them to the MAX to turn on and off all the LEDs according to the
current state of the board being managed in the matrix array.

**6.3 Adding Left/Right Movement**

-   Reading buttons

-   Debouncing

-   Moving the falling pixel horizontally

-   Bounds checking

-   Collision checking

**6.4 Independent Gravity + Movement Timing**

-   Why we separate movement speed from fall speed

-   dropInterval vs moveInterval

-   dropCounter and moveCounter

-   Smooth, responsive controls

**6.5 Soft Drop**

-   Why soft drop is useful

-   Detecting chorded buttons

-   Disabling movement while soft‑dropping

-   Fast gravity mode

**6.6 Row Clearing**

-   Detecting full rows (0xFF)

-   Flashing animation

-   Shifting rows down

-   Clearing row 0

-   Updating display

**6.7 Game Over**

-   Detecting top‑row occupancy

-   Flashing intensity

-   Resetting the board

**7. Powering the Game Like a Handheld**

-   Using 3×AA/AAA battery holders

-   Using NiMH rechargeables

-   Wiring the battery pack to the breadboard

-   Optional power switch

-   Optional LiPo + boost converter

-   Notes on current draw and MAX7219 brightness

**8. Optional: Enclosure Ideas**

-   Simple cardboard prototype

-   3D‑printed shell concepts

-   Button placement

-   Battery compartment

-   Cable strain relief

**9. Next Steps and Extensions**

-   Adding scoring

-   Adding multi‑block shapes

-   Adding rotation

-   Chaining multiple MAX7219 modules

-   Adding sound (piezo)

-   Adding a start screen

-   Adding a pause button

-   Turning it into a tiny console platform

**10. Full Code Listing**

-   Final polished version of the game

-   Clean comments

-   Modular structure

-   Notes on memory usage
