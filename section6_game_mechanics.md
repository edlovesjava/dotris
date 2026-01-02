# Section 6: Game Mechanics (Expanded)

This file contains expanded content for sections 6.3 through 6.7. Include this content in Learning_the_ATTiny.md.

---

## 6.3 Adding Left/Right Movement

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

## 6.4 Independent Gravity + Movement Timing

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

## 6.5 Soft Drop

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

## 6.6 Row Clearing

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

## 6.7 Game Over

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
