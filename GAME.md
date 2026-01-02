# Game Information

## How to Play

Micro DOTris is a falling-block puzzle game where you arrange falling tetromino pieces to create complete horizontal lines.

### Objective
- Arrange falling pieces to create complete horizontal lines
- Clear lines to score points
- Prevent pieces from stacking to the top of the screen

### Game Mechanics

1. **Piece Movement**
   - Pieces fall automatically from top to bottom
   - Use LEFT and RIGHT buttons to move pieces horizontally
   - Use ROTATE button to rotate pieces 90 degrees

2. **Line Clearing**
   - When a horizontal line is completely filled with blocks, it clears
   - All blocks above the cleared line shift down one row
   - Score increases with each cleared line

3. **Game Over**
   - Game ends when a new piece cannot spawn because the stack reaches the top
   - Game automatically resets and starts over

## Tetromino Pieces

The game includes 7 classic tetromino shapes:

```
I-Piece (Line)    ████
                  
O-Piece (Square)  ██
                  ██

T-Piece           ███
                   █

S-Piece          ██
                  ██

Z-Piece           ██
                 ██

L-Piece          ███
                 █

J-Piece          ███
                   █
```

## Controls Reference

### Button Layout
```
[LEFT]  [RIGHT]  [ROTATE]
```

### Control Actions

| Button | Action | Description |
|--------|--------|-------------|
| LEFT   | Move Left | Moves current piece one column left |
| RIGHT  | Move Right | Moves current piece one column right |
| ROTATE | Rotate | Rotates current piece 90° clockwise |

### Button Behavior
- **Debounced Input**: 200ms delay prevents accidental multiple presses
- **Hold Detection**: Currently not implemented (single press only)
- **Edge Protection**: Pieces cannot move beyond screen boundaries

## Scoring

Current implementation:
- **1 point** per line cleared
- Score resets when game restarts

### Future Scoring Options
- Combo bonuses for multiple lines cleared at once
- Increasing points for consecutive clears
- Level-based scoring multipliers
- High score tracking in EEPROM

## Display Information

### LED Matrix Layout
```
  0 1 2 3 4 5 6 7  (Column)
0 □ □ □ □ □ □ □ □
1 □ □ □ □ □ □ □ □
2 □ □ □ □ □ □ □ □
3 □ □ □ █ █ □ □ □  ← Current piece
4 □ □ □ █ █ □ □ □
5 □ □ □ □ □ □ □ □
6 █ █ □ █ █ █ □ □  ← Locked pieces
7 █ █ █ █ █ █ █ █  ← Complete line (will clear)
```

### Display States
- **Lit LED**: Active block (part of current piece or locked)
- **Dark LED**: Empty space
- **Initialization**: All LEDs briefly light during startup
- **Game Over**: Brief flash before reset

## Game Parameters

### Adjustable Settings

#### Speed Control
```cpp
const int GAME_SPEED = 500; // milliseconds between piece drops
```
- Default: 500ms (0.5 seconds per move)
- Faster: 250ms for experienced players
- Slower: 1000ms for beginners

#### Display Brightness
```cpp
sendCommand(0x0A, 0x08); // Intensity: 0x00 (min) to 0x0F (max)
```
- Default: 0x08 (medium)
- Adjust based on ambient light
- Lower brightness conserves power

## Tips and Strategies

1. **Plan Ahead**
   - Consider where the piece will land before rotating
   - Think about creating flat surfaces for future pieces

2. **Clear Lines Quickly**
   - Don't let the stack get too high
   - Focus on clearing lines before they reach the top

3. **Positioning**
   - Try to keep the stack level
   - Avoid creating tall columns with gaps

4. **Rotation Timing**
   - Rotate pieces early while there's space
   - Last-minute rotations can be difficult near boundaries

## Known Limitations

### Current Version Constraints

1. **Piece Representation**
   - Simplified 8x8 matrix representation
   - Some tetromino shapes may be approximated
   - Rotation may be limited due to space

2. **Input**
   - No "drop" button (pieces fall at constant speed)
   - No hold/swap piece feature
   - No preview of next piece

3. **Display**
   - Monochrome display (no color differentiation)
   - No score display on screen (internal only)
   - No animation effects

4. **Memory**
   - Limited flash space on ATtiny85 (8KB total)
   - Current implementation uses ~4-6KB
   - Complex features may exceed available memory

## Future Features

Planned enhancements:

### Gameplay
- [ ] Next piece preview
- [ ] Fast drop button
- [ ] Piece hold/swap functionality
- [ ] Progressive speed increase
- [ ] Multiple difficulty levels

### Display
- [ ] Score visualization
- [ ] Level indicator
- [ ] Game over animation
- [ ] Startup splash screen

### Sound
- [ ] Piezo buzzer integration
- [ ] Sound effects for line clears
- [ ] Background music/melody
- [ ] Game over sound

### Persistence
- [ ] High score saving to EEPROM
- [ ] Statistics tracking
- [ ] Settings memory

### Advanced
- [ ] Power saving sleep mode
- [ ] Battery level indicator
- [ ] Two-player mode (if space permits)

## Technical Notes

### Game Loop
The main game loop runs continuously:
1. Check button inputs (with debouncing)
2. Update game state on timer
3. Refresh display

### Timing
- Game update: Based on GAME_SPEED constant
- Button debounce: 200ms
- Display refresh: Every loop iteration

### Memory Usage
- Game board: 8 bytes (1 per row)
- Tetromino definitions: 28 bytes (7 pieces × 4 bytes)
- Game state variables: ~10 bytes
- Code: ~4-6 KB

## Troubleshooting Gameplay

### Pieces falling too fast
- Increase `GAME_SPEED` value in code
- Re-upload sketch

### Buttons unresponsive
- Check button connections
- Verify pull-up resistors or internal pull-ups
- Test button continuity with multimeter

### Display flickering
- Check power supply stability
- Verify SPI signal integrity
- Add decoupling capacitor

### Game resets unexpectedly
- Check power supply current capacity
- Verify all connections are secure
- Check for short circuits

## Credits

Game concept inspired by Tetris (1984) by Alexey Pajitnov.

Implementation optimized for ATtiny85 microcontroller.
