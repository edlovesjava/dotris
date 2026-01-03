# Future Additions & Improvements

This document outlines potential enhancements for Micro DOTris, organized by complexity and hardware requirements.

## Current Resource Budget

| Resource | Capacity | Used | Available |
|----------|----------|------|-----------|
| Flash | 8192 B | ~3000 B | ~5000 B |
| SRAM | 512 B | ~60 B | ~450 B |
| GPIO Pins | 5 | 5 | 0 |
| EEPROM | 512 B | 0 B | 512 B |

---

## Software-Only Enhancements

### Easy (< 100 bytes Flash each)

| Feature | Description | Est. Cost |
|---------|-------------|-----------|
| **Score tracking** | Count cleared lines, show on game over | ~100B Flash |
| **High score (EEPROM)** | Persist best score across power cycles | ~50B Flash |
| **Start screen** | Animated pattern before game begins | ~100B Flash |
| **Level indicator** | Flash pattern to show current speed level | ~30B Flash |
| **Difficulty select** | Hold button on startup to choose starting speed | ~50B Flash |
| **Better game over** | Elaborate death animation (rain, explode, spiral) | ~100B Flash |

### Medium (100-300 bytes Flash)

| Feature | Description | Est. Cost |
|---------|-------------|-----------|
| **True Tetrominos** | 7 classic 4-block pieces (I, O, T, S, Z, L, J) | ~300B Flash, +28B SRAM |
| **Next piece preview** | Briefly show upcoming piece in top row | ~50B Flash |
| **Ghost piece** | Show landing position (toggled pixels) | ~80B Flash |
| **Combo bonus** | Track consecutive line clears | ~60B Flash |
| **Hard drop** | Instant drop when both buttons released together | ~40B Flash |
| **Line clear counter** | Binary display of total lines cleared | ~80B Flash |

### Hard (300+ bytes Flash)

| Feature | Description | Est. Cost |
|---------|-------------|-----------|
| **Piece rotation** | Rotate via button combo (hold+tap or double-tap) | ~200B Flash |
| **Wall kicks** | Allow rotation when against walls | ~150B Flash |
| **T-spin detection** | Bonus scoring for T-piece rotation clears | ~100B Flash |
| **Multiple game modes** | Marathon, Sprint, Ultra variations | ~400B Flash |

---

## Two-Button Control Schemes

Since all GPIO pins are used, rotation and additional controls must use button combinations:

| Action | Control Scheme |
|--------|----------------|
| Move left | LEFT button |
| Move right | RIGHT button |
| Soft drop | Hold BOTH buttons |
| Hard drop | Tap BOTH buttons simultaneously |
| Rotate CW | Hold LEFT + tap RIGHT |
| Rotate CCW | Hold RIGHT + tap LEFT |
| Pause | Hold BOTH for 2 seconds |
| Menu select | Move left/right on start screen |

---

## Hardware Additions

### Additional Button Options

**Option A: Use RESET pin (PB5)**
- Requires high-voltage programming
- Set RSTDISBL fuse (cannot reprogram without HV programmer)
- Gains 1 pin for rotate button

**Option B: Shift register (74HC165)**
- Connect to existing SPI pins (shared with MAX7219)
- Add 8 button inputs
- Cost: ~20B Flash for driver code

**Option C: Analog button ladder**
- Multiple buttons on single ADC pin (PB3 or PB4)
- Use resistor divider to create unique voltage per button
- Cost: ~50B Flash, requires resistors

### Sound (Piezo Buzzer) - Detailed Analysis

#### Hardware Connection Options

**Option 1: Share CLK pin (PB1)** - Recommended
```
ATtiny85 PB1 ──┬──> MAX7219 CLK
               └──> Piezo (+)
           GND ──> Piezo (-)
```
- Pros: No extra pins needed, simple wiring
- Cons: Audible clicks during display updates (~1.3ms every frame)
- Mitigation: Update display less frequently, or embrace the "8-bit aesthetic"

**Option 2: Share CS pin (PB2)**
```
ATtiny85 PB2 ──┬──> MAX7219 CS (directly)
               └──> Piezo (+) via 100Ω resistor
           GND ──> Piezo (-)
```
- Pros: CS only toggles briefly, less noise
- Cons: May cause display glitches if piezo loads the signal

**Option 3: Use RESET pin (PB5)**
```
ATtiny85 PB5 ──> Piezo (+)
           GND ──> Piezo (-)
```
- Pros: Dedicated sound pin, no conflicts
- Cons: Requires RSTDISBL fuse, needs high-voltage programmer to reprogram
- Fuse change: High fuse 0xDF → 0x5F

**Option 4: Analog mux on button pin**
```
                 ┌──> Piezo (+)
ATtiny85 PB4 ──┬─┤
               │ └──> 10kΩ ──> Button ──> GND
               └──> 100kΩ pull-up to VCC
```
- Pros: Keeps button functional
- Cons: Complex, may affect button reading, requires ADC instead of digitalRead

**Option 5: External I2C DAC or sound chip**
- Use ATtiny85's USI for I2C (PB0=SDA, PB2=SCL)
- Conflicts with MAX7219 pins
- Not practical without hardware redesign

#### Recommended: Option 1 (CLK sharing)

**Wiring:**
```
               ATtiny85
              ┌────────┐
        RESET─┤1      8├─VCC
   RIGHT(PB3)─┤2      7├─PB2 (CS)──────────> MAX7219 CS
    LEFT(PB4)─┤3      6├─PB1 (CLK)──┬──────> MAX7219 CLK
          GND─┤4      5├─PB0 (DIN)  │  ┌───> MAX7219 DIN
              └────────┘            │  │
                                    │  │
                              Piezo(+) │
                                │      │
                              Piezo(-) │
                                │      │
                               GND─────┘
```

**Component:** Passive piezo buzzer (not active buzzer with built-in oscillator)
- Frequency range: 2-4 kHz optimal
- Examples: Murata PKM13EPYH4000-A0, generic 12mm passive piezo

---

#### Software Implementation

**Timer Options on ATtiny85:**

| Timer | Output Pin | Conflict | Notes |
|-------|------------|----------|-------|
| Timer0 | PB0, PB1 | PB0=DIN, PB1=CLK | Used by Arduino millis() |
| Timer1 | PB1, PB4 | PB1=CLK, PB4=LEFT | High-speed PWM capable |

**Approach: Bit-bang tone generation**

Since hardware timers conflict with used pins, generate tones by toggling the CLK pin between display updates:

```c
// Tone generation (blocking, simple)
void playTone(uint16_t frequency, uint16_t duration_ms) {
  uint16_t period_us = 1000000UL / frequency;
  uint16_t half_period = period_us / 2;
  uint32_t cycles = (uint32_t)frequency * duration_ms / 1000;

  for (uint32_t i = 0; i < cycles; i++) {
    digitalWrite(CLK, HIGH);
    delayMicroseconds(half_period);
    digitalWrite(CLK, LOW);
    delayMicroseconds(half_period);
  }
}

// Non-blocking tone (integrates with game loop)
uint16_t toneFreq = 0;
uint16_t toneDuration = 0;
uint32_t toneStart = 0;
uint32_t lastToggle = 0;

void updateSound() {
  if (toneFreq == 0) return;

  uint32_t now = micros();
  if (now - toneStart > (uint32_t)toneDuration * 1000) {
    toneFreq = 0;  // Stop sound
    return;
  }

  uint16_t half_period = 500000UL / toneFreq;
  if (now - lastToggle >= half_period) {
    digitalWrite(CLK, !digitalRead(CLK));
    lastToggle = now;
  }
}

void startTone(uint16_t freq, uint16_t duration) {
  toneFreq = freq;
  toneDuration = duration;
  toneStart = micros();
  lastToggle = micros();
}
```

---

#### Sound Event Definitions

**1. Clock/Tick Sound (Ambient)**
```c
// Subtle tick on each gravity drop
void onGravityTick() {
  playTone(100, 5);  // Very low, very short - barely audible pulse
}

// Alternative: Pitch increases with speed level
void onGravityTick() {
  uint16_t freq = 80 + (speed * 10);  // 80Hz at level 1, 380Hz at level 30
  playTone(freq, 3);
}
```

**2. Piece Motion Sound (Horizontal Movement)**
```c
// Short blip when piece moves left/right
void onPieceMove() {
  playTone(440, 10);  // A4, 10ms - quick tick
}

// Direction-aware (lower=left, higher=right)
void onPieceMove(bool movingRight) {
  playTone(movingRight ? 523 : 392, 10);  // C5 or G4
}
```

**3. Piece Landed (Placed on Stack)**
```c
// Solid "thunk" when piece locks
void onPieceLand() {
  playTone(150, 30);  // Low thud
  delay(10);
  playTone(100, 20);  // Lower follow-up
}

// Alternative: pitch based on landing height (higher = more urgent)
void onPieceLand(uint8_t row) {
  uint16_t freq = 100 + (7 - row) * 50;  // 100Hz at bottom, 450Hz at top
  playTone(freq, 40);
}
```

**4. Row Cleared**
```c
// Ascending arpeggio for line clear
void onLineClear(uint8_t linesCleared) {
  switch (linesCleared) {
    case 1:  // Single
      playTone(523, 50);   // C5
      break;
    case 2:  // Double
      playTone(523, 40); delay(30);
      playTone(659, 50);   // C5 → E5
      break;
    case 3:  // Triple
      playTone(523, 30); delay(20);
      playTone(659, 30); delay(20);
      playTone(784, 50);   // C5 → E5 → G5
      break;
    case 4:  // Tetris!
      playTone(523, 30); delay(20);
      playTone(659, 30); delay(20);
      playTone(784, 30); delay(20);
      playTone(1047, 80);  // C5 → E5 → G5 → C6
      break;
  }
}
```

**5. Soft Drop**
```c
// Continuous falling sound during soft drop
void onSoftDropTick() {
  static uint16_t dropPitch = 800;
  playTone(dropPitch, 5);
  dropPitch -= 20;
  if (dropPitch < 200) dropPitch = 800;  // Reset for next drop
}

// Alternative: Single swoosh when soft drop starts
void onSoftDropStart() {
  for (uint16_t f = 600; f > 150; f -= 30) {
    playTone(f, 3);
  }
}
```

**6. Game Over**
```c
void onGameOver() {
  // Descending sad trombone
  playTone(392, 150);  delay(50);  // G4
  playTone(370, 150);  delay(50);  // F#4
  playTone(349, 150);  delay(50);  // F4
  playTone(330, 300);              // E4 (hold)
}
```

**7. Level Up**
```c
void onLevelUp() {
  playTone(523, 60);  delay(30);  // C5
  playTone(659, 60);  delay(30);  // E5
  playTone(784, 100);             // G5
}
```

---

#### Integration with Game Loop

```c
// In main loop, call sound updates between display operations
void loop() {
  // ... existing game logic ...

  // Before display update - pause any sound
  noInterrupts();

  // Update display (uses CLK pin)
  updateDisplay();

  // Resume sound generation
  interrupts();

  // Continue non-blocking sound
  updateSound();

  // ... rest of game logic ...
}
```

---

#### Frequency Reference (Musical Notes)

| Note | Freq (Hz) | Use Case |
|------|-----------|----------|
| C4 | 262 | Low tones |
| E4 | 330 | Game over |
| G4 | 392 | Mid tones |
| A4 | 440 | Standard reference |
| C5 | 523 | Line clear |
| E5 | 659 | Arpeggio |
| G5 | 784 | High accent |
| C6 | 1047 | Tetris fanfare |

---

#### Resource Cost Estimate

| Component | Flash Cost | SRAM Cost |
|-----------|------------|-----------|
| `playTone()` function | ~60 bytes | 0 |
| Non-blocking tone system | ~120 bytes | 10 bytes |
| All sound events | ~200 bytes | 0 |
| **Total** | **~380 bytes** | **~10 bytes** |

---

#### Bill of Materials Addition

| Part | Quantity | Notes |
|------|----------|-------|
| Passive piezo buzzer | 1 | 12mm, 2-4kHz |
| 100Ω resistor (optional) | 1 | Current limiting |

---

#### Considerations

**Pros of adding sound:**
- Greatly enhances game feel
- Audio feedback for actions
- Makes the game feel more complete

**Cons:**
- CLK sharing causes display noise during sound
- Blocking tones pause game momentarily
- Adds ~400 bytes to Flash

**Mitigation strategies:**
- Keep tones very short (5-30ms)
- Use non-blocking sound where possible
- Accept "8-bit lo-fi" aesthetic
- Option: Add mute toggle (hold button on startup)

### Larger Display

**Chained MAX7219 modules:**
```
ATtiny85 → MAX7219 #1 → MAX7219 #2 → ...
             8x8          8x8
```

| Configuration | Use Case |
|---------------|----------|
| 8x16 (2 modules vertical) | Taller play field |
| 16x8 (2 modules horizontal) | Score display + game |
| 8x24 (3 modules) | Preview + game + score |

Cost: ~50B Flash for daisy-chain support

### MCU Upgrade Path

| MCU | Flash | SRAM | I/O | Benefit |
|-----|-------|------|-----|---------|
| ATtiny85 (current) | 8 KB | 512 B | 5 | - |
| ATtiny84 | 8 KB | 512 B | 11 | More buttons |
| ATtiny1614 | 16 KB | 2 KB | 12 | Modern AVR, more resources |
| ATmega328P | 32 KB | 2 KB | 23 | Arduino Uno compatible |

---

## Game Modes

### Marathon (Default)
- Play until board fills
- Speed increases every 10 pieces
- Track total lines cleared

### Sprint
- Clear 20 lines as fast as possible
- Display time on completion (scrolling)

### Ultra
- 2-minute timed mode
- Maximize score/lines
- Flash warning at 30 seconds

### Zen
- No speed increase
- Relaxed play
- No game over (auto-clear when full)

---

## Visual Enhancements

### Start Screen Ideas
```
Frame 1:    Frame 2:    Frame 3:
□□□□□□□□    □□■□□□□□    □□■□□□□□
□□□□□□□□    □□■□□□□□    □□■□□□□□
□□□□□□□□    □□■□□□□□    □□■■□□□□
□□□□□□□□    □□□□□□□□    □□□□□□□□
(falling piece animation)
```

### Game Over Animations
1. **Rain down**: All pixels fall to bottom
2. **Explosion**: Pixels scatter outward from center
3. **Spiral**: Board clears in spiral pattern
4. **Fade**: Decrease intensity to 0, then reset

### Line Clear Effects
1. **Flash**: Current behavior
2. **Collapse**: Rows visibly fall
3. **Sparkle**: Random pixels blink before clear

---

## Scoring System

### Point Values
| Action | Points |
|--------|--------|
| Single line | 100 |
| Double | 300 |
| Triple | 500 |
| Tetris (4 lines) | 800 |
| Soft drop | 1 per row |
| Hard drop | 2 per row |
| Combo multiplier | +50% per consecutive clear |

### Display Options
- Binary on top row (8 bits = 0-255)
- Scrolling decimal on game over
- LED blink count for hundreds

---

## Implementation Priority

### Phase 1: Polish (Easy wins)
1. EEPROM high score
2. Start screen animation
3. Better game over sequence

### Phase 2: Gameplay (Core improvements)
4. True Tetromino shapes
5. Rotation via button combo
6. Ghost piece

### Phase 3: Features (Nice to have)
7. Multiple game modes
8. Sound effects (if hardware added)
9. Score display

---

## Technical Notes

### Tetromino Data Structure
```c
// Each piece: 4 bytes (4 rotations), each byte is 4x4 bitmap
// Stored in PROGMEM to save SRAM
const uint8_t PROGMEM pieces[7][4] = {
  {0x0F, 0x22, 0x0F, 0x22}, // I
  {0x66, 0x66, 0x66, 0x66}, // O
  {0x27, 0x32, 0x72, 0x26}, // T
  // ... etc
};
```

### EEPROM Layout
```
Address 0x00: High score (low byte)
Address 0x01: High score (high byte)
Address 0x02: Total games played
Address 0x03: Settings/flags
```

### Timing Considerations
- Current loop: 1ms minimum delay
- Button debounce: 30ms polling
- Rotation detection: <100ms for double-tap
- Hold detection: 200ms before repeat
