
# Building the Game DOTris

**What are we building?**

A game like Tetris but with simple falling dots.

Initially I just wanted to do something different with the matrix and I thought about random dropping LED lights. Then I thought about stacking them—if the light below another was on, it would stop and another light would start at the top. But that looked like Tetris, so I thought: why not make a row disappear if it's completed? Then I added some effects to show off the removed row. Then I increased the speed of dropping over time, then added buttons! And it became a game.

We'll call it DOTris.

---

## Framebuffer + Display Driver

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

## Basic Falling Pixel

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

## Adding Left/Right Movement

With a pixel falling down the screen, we need a way to control where it lands. This is where our two buttons come into play.

**Reading the Buttons**

We set up our button pins using the internal pull-up resistors:

```c
pinMode(LEFT_B, INPUT_PULLUP);
pinMode(RIGHT_B, INPUT_PULLUP);
```

With `INPUT_PULLUP`, the pin reads HIGH when the button is not pressed (the internal resistor pulls it up to VCC). When you press the button, it connects the pin to ground, and the pin reads LOW. This means our button logic is "active low"—a pressed button gives us a 0, not a 1:

```c
bool left = (digitalRead(LEFT_B) == LOW);
bool right = (digitalRead(RIGHT_B) == LOW);
```

**Why Pull-Up Instead of Pull-Down?**

The ATtiny85 has built-in pull-up resistors but no pull-down resistors. Using `INPUT_PULLUP` means we don't need any external resistors for our buttons—just wire one leg to the pin and the other to ground. Fewer components, simpler circuit.

**Moving the Pixel**

To move the pixel left or right, we shift the bit within the byte. Remember, each row is a byte where each bit represents a column:

```c
// Move left: shift bit right (toward lower columns)
if (col > 0 && !(matrix[row] & (bit >> 1))) {
  col--;
  bit >>= 1;
}

// Move right: shift bit left (toward higher columns)
if (col < 7 && !(matrix[row] & (bit << 1))) {
  col++;
  bit <<= 1;
}
```

**Bounds Checking**

Before moving, we check that we won't go off the edge. `col > 0` ensures we don't move left past column 0, and `col < 7` ensures we don't move right past column 7.

**Collision Checking**

We also check if the destination is already occupied by a landed pixel. The expression `matrix[row] & (bit >> 1)` performs a bitwise AND between the current row's landed pixels and the position we want to move to. If the result is non-zero, something is already there, and we block the move.

**Key Repeat (Auto-Repeat)**

If you've ever held down a key on a keyboard, you know it doesn't just register once—after a short delay, it starts repeating. We implement the same behavior for a better feel:

```c
uint8_t moveDelay = 200;  // ms before repeat starts
uint8_t moveRepeat = 80;  // ms between repeats

static uint16_t leftHold = 0;

if (left) {
  leftHold += moveInterval;
  
  // First press OR held long enough
  if (leftHold == moveInterval || leftHold > moveDelay) {
    // ... do the move ...
    if (leftHold > moveDelay) leftHold -= moveRepeat;
  }
} else {
  leftHold = 0;  // Reset when released
}
```

The first time you press the button, `leftHold` equals `moveInterval`, so the move happens immediately. If you keep holding, `leftHold` accumulates until it exceeds `moveDelay` (200ms), then moves repeat every `moveRepeat` (80ms). When you release, the counter resets.

---

## Independent Gravity + Movement Timing

Here's a problem: if we use a simple `delay()` for the falling animation, we can't read button inputs during that delay. The game would feel sluggish and unresponsive.

The solution is to separate our timing into independent counters that we check in a tight loop.

**The Core Loop**

Instead of blocking delays, we use a 1ms delay as our base time slice and increment counters:

```c
uint16_t dropInterval = 800 - (speed * 20);  // Gravity speed
uint8_t moveInterval = 30;                    // Input polling rate

uint16_t dropCounter = 0;
uint8_t moveCounter = 0;

while (/* pixel is still falling */) {
  delay(1);  // 1ms time slice
  
  // Check for movement every 30ms
  moveCounter++;
  if (moveCounter >= moveInterval) {
    moveCounter = 0;
    // ... handle button input ...
  }
  
  // Drop the pixel based on gravity
  dropCounter++;
  if (dropCounter >= dropInterval) {
    dropCounter = 0;
    // ... move pixel down one row ...
  }
}
```

**Why Two Different Intervals?**

`dropInterval` controls how fast the pixel falls—this starts slow (800ms between drops) and gets faster as the game speeds up. We want gravity to feel deliberate.

`moveInterval` controls how often we check for button input—30ms gives us responsive controls without wasting cycles polling constantly. This stays constant regardless of game speed.

**The Result**

With this approach, you can tap or hold the buttons and see immediate response, even while the pixel is mid-fall. The game feels responsive because input handling and gravity are decoupled.

---

## Soft Drop

Sometimes you know exactly where you want the pixel to land and waiting for gravity is tedious. Soft drop lets the player speed things up.

**Detecting the Chord**

We detect soft drop when both buttons are pressed simultaneously:

```c
bool softDrop = left && right;
```

This is called a "chord"—two buttons pressed together to trigger a different action than either alone.

**Speeding Up Gravity**

When soft drop is active, we dramatically reduce the drop interval:

```c
if (softDrop) {
  dropInterval = 20;  // Very fast!
} else {
  dropInterval = 800 - (speed * 20);  // Normal gravity
}
```

At 20ms per row, the pixel zips down almost instantly.

**Disabling Horizontal Movement**

Notice that the soft drop check comes before the left/right movement code. When `softDrop` is true, we skip the horizontal movement logic entirely. This prevents accidental sideways movement while you're trying to drop quickly—your thumbs are pressing both buttons, after all.

```c
if (softDrop) {
  dropInterval = 20;
} else {
  // Only check left/right if NOT soft-dropping
  if (left) { /* move left */ }
  if (right) { /* move right */ }
}
```

**Design Choice**

Some games let you move while soft-dropping. We chose not to because with only two buttons, pressing both is the only way to trigger soft drop, leaving no way to also indicate direction. With more buttons, you could have a dedicated "down" button and still allow lateral movement.

---

## Row Clearing

When a row fills up completely, it should disappear and everything above it should fall down. This is the core mechanic that makes it a game rather than just pixels piling up.

**Detecting a Full Row**

Since each row is a byte, a full row means all 8 bits are set. That's `0xFF` (binary 11111111):

```c
if (matrix[row] == 0xFF) {
  // Row is complete!
}
```

This is beautifully simple—no loops, no counting, just one comparison.

**The Clearing Animation**

A filled row deserves some fanfare. We flash it a couple times before clearing:

```c
// Flash the completed row
delay(60);
sendCmd(row + 1, 0x00);  // Off
delay(60);
sendCmd(row + 1, 0xFF);  // On
delay(60);
sendCmd(row + 1, 0x00);  // Off
delay(60);
sendCmd(row + 1, 0xFF);  // On
delay(60);
```

This creates a satisfying blink-blink effect that gives the player visual feedback: "Yes! You completed a row!"

**Shifting Rows Down**

After the animation, we need to remove the completed row and drop everything above it:

```c
for (int r = row; r > 0; r--) {
  matrix[r] = matrix[r - 1];
}
matrix[0] = 0x00;  // Clear the top row
```

We start at the completed row and copy each row from the one above it. Row 5 gets row 4's data, row 4 gets row 3's, and so on. Finally, we clear row 0 since there's nothing above it to copy from.

**Updating the Display**

After modifying the matrix array, we push the changes to the MAX7219:

```c
updateDisplay();
```

The player sees all the rows above the cleared one drop down by one position.

**Note on Multiple Rows**

In this simple version, we only check the row where the pixel just landed. If you wanted to handle multiple simultaneous line clears (which would require multi-block pieces), you'd need to scan all rows and handle them in a loop, clearing from bottom to top.

---

## Game Over

Eventually the pixels pile up to the top. When they do, the game ends.

**Detecting Game Over**

We check if the top row has any pixels in it after a new piece would spawn:

```c
if (matrix[0] != 0) {
  // Game over!
}
```

If row 0 has any bits set, there's no room for a new falling pixel to start.

**The Game Over Animation**

We flash the entire display by toggling the brightness:

```c
sendCmd(MAX7219_REG_INTENSITY, 0x0D);  // Bright
delay(60);
sendCmd(MAX7219_REG_INTENSITY, 0x08);  // Normal
delay(60);
sendCmd(MAX7219_REG_INTENSITY, 0x0D);  // Bright
delay(60);
sendCmd(MAX7219_REG_INTENSITY, 0x08);  // Normal
```

Using intensity rather than turning pixels on/off means the player can see their final board state while it flashes—a nice "here's how you did" moment before we wipe the slate clean.

**Resetting the Board**

After a pause to let the game over sink in, we clear everything:

```c
delay(1000);  // Let them see their demise

for (uint8_t i = 0; i < 8; i++) {
  matrix[i] = 0;
}
updateDisplay();

delay(500);  // Brief pause before new game
```

The game automatically restarts. No title screen, no "Press Start"—just straight back into the action. For a tiny handheld game, this immediacy feels right.

**Progressive Difficulty**

The game gets faster over time through the `speed` variable, which increases with each piece:

```c
tick++;
if (tick % 10 == 0) {
  speed++;
  if (speed > 30) speed = 30;  // Cap the speed
}
```

Every 10 pieces, `speed` increases by 1. Since `dropInterval = 800 - (speed * 20)`, this means gravity accelerates from 800ms per row down to 200ms at max speed. The cap at 30 prevents the math from going negative or making the game literally unplayable.

---

## Putting It All Together

The complete game loop now looks like this:

1. Pick a random column for the new pixel
2. While the pixel can still fall:
   - Poll buttons, handle movement and soft drop
   - Apply gravity on its own timer
   - Render the falling pixel
3. Land the pixel (write it into the matrix)
4. Check for completed rows, clear if found
5. Check for game over, reset if needed
6. Repeat

All of this fits comfortably in the ATtiny85's 8KB of flash. The game state (the 8-byte matrix plus a handful of variables) easily fits in 512 bytes of RAM. It's a complete, playable game on a chip the size of a pea.
