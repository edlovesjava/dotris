# Section 6: Building the Falling-Block Game

This file contains content for sections 6 (intro), 6.1, and 6.2. Include this content in Learning_the_ATTiny.md.

---

## 6. Building the Falling-Block Game

**What are we building?**

A game like Tetris but with simple falling dots.

Initially I just wanted to do something different with the matrix and I thought about random dropping LED lights. Then I thought about stacking them—if the light below another was on, it would stop and another light would start at the top. But that looked like Tetris, so I thought: why not make a row disappear if it's completed? Then I added some effects to show off the removed row. Then I increased the speed of dropping over time, then added buttons! And it became a game.

We'll call it DOTris.

---

## 6.1 Framebuffer + Display Driver

**The Matrix as Memory**

The display consists of a straightforward 8x8 grid of bits, structured as a single-dimensional array of eight bytes. Each byte represents one row, and each bit within that byte represents a column. The first row corresponds to array index 0 (since arrays are zero-indexed), and within each byte, bit 0 refers to column 0 while bit 7 represents column 7.

```c
byte matrix[8];
```

This is our "framebuffer"—a region of memory that mirrors what's on the display. When we want to change what's shown, we modify this array and then push it to the hardware.

**Bitwise Operations**

Bitwise operations such as `&` (AND) and `|` (OR) make setting or clearing bits efficient and compact. To set a bit in a byte, you OR it with a mask. To clear a bit, you AND it with the inverse of the mask.

```c
// Set bit 3 (turn on column 3)
matrix[row] |= (1 << 3);

// Clear bit 3 (turn off column 3)
matrix[row] &= ~(1 << 3);

// Check if bit 3 is set
if (matrix[row] & (1 << 3)) {
  // Column 3 is on
}
```

To position a 1 at a particular bit, you shift `<<` the value 1 by the number of positions. `(1 << 3)` gives you `00001000` in binary—a single bit set at position 3.

**Memory Conservation**

When programming microcontrollers, conserving memory is essential—every byte and bit matters. You'll frequently see the `uint8_t` type defined in `<stdint.h>`, which guarantees an 8-bit unsigned integer across all platforms. Avoiding generic `int` types prevents unnecessary memory usage—`int` can be two bytes on some platforms and four on others, potentially wasting valuable space. On the ATtiny85 with only 512 bytes of RAM, this discipline matters.

**How Does the 8x8 Matrix Module Operate?**

By multiplexing and scanning.

Individual LEDs aren't directly addressed, which would require 64 separate connections (plus 64 resistors!). Instead, the MAX7219 chip uses multiplexing: it rapidly activates each row in turn, lighting the appropriate columns for that row, then moves to the next row. This happens so fast (800Hz) that your eye perceives a continuous, stable image—a phenomenon called *persistence of vision*.

**The MAX7219 Interface**

The MAX7219 uses three pins for communication, following the SPI (Serial Peripheral Interface) pattern:

1. **DIN (Data In)** — Serial data input
2. **CLK (Clock)** — Clock signal to synchronize data transfer  
3. **CS (Chip Select/Load)** — Pulled low to begin transmission, high to latch data

Note: The ATtiny85 has a USI (Universal Serial Interface) rather than true hardware SPI, so we bit-bang the protocol manually in software. This gives us full control over timing and works reliably with the MAX7219.

**Sending Data**

You send data for each row as an 8-bit byte; a 1 in a bit position turns that LED on, and 0 turns it off. The MAX7219 also has registers you can address for controlling aspects like brightness and scan limits. You can even daisy-chain multiple MAX7219 modules for larger displays.

**Pin Assignments**

We define our connections at the top of the sketch:

```c
const uint8_t DIN = 0;    // ATtiny85 pin 0 -> MAX7219 DIN
const uint8_t CLK = 1;    // ATtiny85 pin 1 -> MAX7219 CLK
const uint8_t CS = 2;     // ATtiny85 pin 2 -> MAX7219 CS
const uint8_t LEFT_B = 3; // ATtiny85 pin 3 -> Left button
const uint8_t RIGHT_B = 4;// ATtiny85 pin 4 -> Right button
```

That's all 5 usable GPIO pins on the ATtiny85 accounted for. No room for feature creep!

---

## 6.2 Basic Falling Pixel

Now let's make something fall.

**Starting State**

The program starts with a blank 8x8 matrix—an array of zeros. Sending this to the MAX7219 turns off all LEDs:

```c
for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
updateDisplay();
```

**Picking a Random Column**

Each new falling pixel starts at a random column:

```c
uint8_t col = random(0, 8);  // 0-7
uint8_t row = 0;             // Start at top
uint8_t bit = (1 << col);    // Bitmask for this column
```

We track both the column number (`col`) and its bitmask (`bit`). The bitmask is what we actually use for display and collision detection—having both avoids repeated shift operations.

**The Display Routines**

Three functions handle all communication with the MAX7219:

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

`sendByte` shifts out 8 bits one at a time, MSB first. For each bit:
1. Pull CLK low
2. Set DIN to the bit value (0 or 1)
3. Pull CLK high (the MAX7219 latches on the rising edge)

`sendCmd` sends a two-byte command: first the register address, then the data. The CS pin frames the transaction—low to start, high to latch.

`updateDisplay` pushes the entire matrix to the display by sending each row to its corresponding register (registers 1-8 are the digit/row registers).

**MAX7219 Registers**

The MAX7219 has several control registers:

```c
#define MAX7219_REG_NOOP        0x00  // No operation
#define MAX7219_REG_DIGIT0      0x01  // Row 0 (we use 1-8)
// ... through DIGIT7 at 0x08
#define MAX7219_REG_DECODE_MODE 0x09  // BCD decode (we disable this)
#define MAX7219_REG_INTENSITY   0x0A  // Brightness 0-15
#define MAX7219_REG_SCAN_LIMIT  0x0B  // How many digits/rows to scan
#define MAX7219_REG_SHUTDOWN    0x0C  // 0=shutdown, 1=normal
#define MAX7219_REG_DISPLAY_TEST 0x0F // 1=all LEDs on (test mode)
```

During setup, we initialize the display:

```c
sendCmd(MAX7219_REG_DISPLAY_TEST, 0x00);  // Normal operation
sendCmd(MAX7219_REG_DECODE_MODE, 0x00);   // No BCD decode
sendCmd(MAX7219_REG_SCAN_LIMIT, 0x07);    // Scan all 8 rows
sendCmd(MAX7219_REG_INTENSITY, 0x08);     // Medium brightness
sendCmd(MAX7219_REG_SHUTDOWN, 0x01);      // Exit shutdown mode
```

**Collision Detection**

Before moving the pixel down, we check if the space below is free:

```c
while (row < 7 && !(matrix[row + 1] & bit)) {
  // ... pixel can keep falling ...
  row++;
}
```

Two conditions must be true for the pixel to fall:
1. `row < 7` — We haven't hit the bottom of the display
2. `!(matrix[row + 1] & bit)` — The cell below isn't already occupied

The expression `matrix[row + 1] & bit` checks if our column's bit is set in the row below. If it is, something's already there and we stop.

**Rendering the Falling Pixel**

While falling, we temporarily show the pixel at its current position without permanently writing to the matrix:

```c
sendCmd(row + 1, matrix[row] | bit);
```

This ORs the falling pixel's bit with whatever's already landed in that row, displaying both. We send directly to the display rather than modifying `matrix[]` because the pixel hasn't landed yet.

**Landing**

When the pixel can't fall anymore (either it hit the bottom or something below it), we permanently write it into the matrix:

```c
matrix[row] |= bit;
updateDisplay();
```

Now it's part of the playfield, and the next pixel will collide with it.

**The Basic Loop**

Putting it together, the simplest version looks like:

```c
void loop() {
  uint8_t col = random(0, 8);
  uint8_t row = 0;
  uint8_t bit = (1 << col);
  
  // Fall until we can't
  while (row < 7 && !(matrix[row + 1] & bit)) {
    sendCmd(row + 1, matrix[row] | bit);  // Show pixel
    delay(500);                            // Wait
    sendCmd(row + 1, matrix[row]);         // Erase pixel
    row++;                                 // Move down
  }
  
  // Land
  matrix[row] |= bit;
  updateDisplay();
}
```

This creates a simple falling-dot animation. Pixels drop from random columns and stack up. But there's no player control yet, and that `delay(500)` makes it unresponsive. We'll fix both in the next sections.
