# Hardware Documentation

## Components Required

### Microcontroller
- **ATtiny85** - 8-bit AVR microcontroller
  - 8KB Flash memory
  - 512 bytes SRAM
  - 512 bytes EEPROM
  - 5 I/O pins available

### Display
- **8x8 LED Matrix** with MAX7219 driver
  - Common cathode or common anode (configure accordingly)
  - Red, green, or other color as preferred

### Input Controls
- **2x Push Buttons** (normally open)
  - Left movement
  - Right movement
- **Optional**: 3rd button for rotate (requires RESET pin configuration or I/O expander)

### Power
- **5V Power Supply** (USB or battery)
  - Current draw: ~200mA typical

### Additional Components
- 10kΩ resistors (2x) - for button pull-ups (if not using internal pull-ups)
- 0.1µF capacitor - power supply decoupling
- Jumper wires
- Breadboard or custom PCB

## Pin Configuration

### ATtiny85 Pinout
```
        ┌─────┐
 RESET ─┤1   8├─ VCC (+5V)
  PB3  ─┤2   7├─ PB2 (SCK/CLK)
  PB4  ─┤3   6├─ PB1 (MISO/DATA)
  GND  ─┤4   5├─ PB0 (MOSI/CS)
        └─────┘
```

### Connection Table

| ATtiny85 Pin | Arduino Pin | Function | Connected To |
|--------------|-------------|----------|--------------|
| PB0 (Pin 5)  | D0          | DATA     | MAX7219 DIN  |
| PB1 (Pin 6)  | D1          | CLK      | MAX7219 CLK  |
| PB2 (Pin 7)  | D2          | CS       | MAX7219 CS   |
| PB3 (Pin 2)  | D3          | BTN_RIGHT| Right Button |
| PB4 (Pin 3)  | D4          | BTN_LEFT | Left Button  |
| RESET (Pin 1)| RESET       | BTN_ROTATE*| Rotate Button (Optional) |
| VCC (Pin 8)  | VCC         | Power    | +5V          |
| GND (Pin 4)  | GND         | Ground   | GND          |

*Note: Using RESET pin for button requires special configuration and fuse settings (disabling reset functionality). Alternative: use only 2 buttons (left/right) or add an external shift register or GPIO expander for additional inputs. Current sketch is configured for 2-button operation.

## Circuit Diagram

### MAX7219 to ATtiny85 Connection
```
ATtiny85                    MAX7219
  PB0 (D0) ──────────────> DIN
  PB1 (D1) ──────────────> CLK
  PB2 (D2) ──────────────> CS/LOAD
  VCC      ──────────────> VCC
  GND      ──────────────> GND
```

### Button Connections
```
Each button:
  One terminal ──> ATtiny85 Pin (PB3, PB4, or alternative)
  Other terminal ──> GND

Note: Internal pull-ups are enabled in software
```

### Power Supply
```
  +5V ──┬──> ATtiny85 VCC (Pin 8)
        ├──> MAX7219 VCC
        └──> 0.1µF capacitor to GND
        
  GND ──┬──> ATtiny85 GND (Pin 4)
        ├──> MAX7219 GND
        ├──> All buttons (common)
        └──> 0.1µF capacitor to +5V
```

## Assembly Notes

1. **Decoupling Capacitor**: Place 0.1µF capacitor as close as possible to ATtiny85 VCC and GND pins
2. **Button Debouncing**: Software debouncing is implemented in the code
3. **Power Consideration**: Ensure power supply can handle LED matrix current draw
4. **Programming**: Use a USBtinyISP or similar AVR programmer to upload the sketch
5. **Bootloader**: No bootloader is required for ATtiny85

## Programming the ATtiny85

### Using Arduino IDE

1. Install ATtiny board support:
   - Add to Board Manager URLs: `https://drazzy.com/package_drazzy.com_index.json`
   - Install "ATTinyCore" by Spence Konde

2. Select board:
   - Board: "ATtiny25/45/85 (No bootloader)"
   - Chip: "ATtiny85"
   - Clock: "8 MHz (internal)"
   - Programmer: "USBtinyISP" (or your programmer)

3. Connect programmer to ATtiny85
4. Burn bootloader (sets fuses)
5. Upload sketch

### Fuse Settings
- Low Fuse: 0xE2 (8MHz internal oscillator)
- High Fuse: 0xDF (default)
- Extended Fuse: 0xFF (default)

## Troubleshooting

- **Display not working**: Check SPI connections (DATA, CLK, CS)
- **Random behavior**: Add decoupling capacitor near ATtiny85
- **Buttons not responding**: Verify internal pull-ups are enabled in code
- **Dim LEDs**: Adjust intensity in software or check power supply current capacity

## Resource Usage Analysis

### Summary

| Resource | Capacity | Estimated Usage | Available |
|----------|----------|-----------------|-----------|
| **SRAM** | 512 B | ~60 B (12%) | ~450 B |
| **Flash** | 8192 B | ~3000 B (37%) | ~5000 B |
| **Power** | - | ~80 mA @ 5V | - |
| **Clock** | 8 MHz | adequate | - |

### SRAM Usage

The game uses an efficient 8-byte bitmap for the display buffer:

| Variable | Type | Size |
|----------|------|------|
| `matrix[8]` | `byte[8]` | 8 bytes |
| `tick` | `uint16_t` | 2 bytes |
| `speed` | `uint8_t` | 1 byte |
| `rightHoldTimer` | `uint16_t` | 2 bytes |
| `leftHoldTimer` | `uint16_t` | 2 bytes |
| Local variables in `loop()` | various | ~12 bytes |
| Stack overhead | - | ~20-40 bytes |

**Total: ~50-70 bytes** - SRAM is abundant for this application.

### Flash Usage

| Component | Estimated Size |
|-----------|----------------|
| Initialization code | ~200 bytes |
| SPI functions (`sendByte`, `sendCmd`, `updateDisplay`) | ~150 bytes |
| Main `loop()` logic | ~800 bytes |
| Arduino core overhead | ~1500-2500 bytes |
| **Total** | **~2,600-3,600 bytes** |

### Speed/Timing

| Operation | Timing |
|-----------|--------|
| Clock frequency | 8 MHz (125 ns/cycle) |
| Bit-bang SPI clock | ~50-100 kHz |
| Full display update | ~1.3 ms |
| Main loop iteration | 1 ms minimum |
| Button polling interval | 30 ms |
| Piece drop rate | 800 ms (speed=1) to 200 ms (speed=30) |

### Power Consumption

| Component | Current |
|-----------|---------|
| ATtiny85 @ 8MHz, 5V | ~5-10 mA |
| MAX7219 (logic) | ~5 mA |
| LEDs (8x8, intensity 8/15) | ~40-100 mA |
| **Total** | **~50-120 mA @ 5V** |

### How to Measure

**Flash/SRAM (compile-time):**
```bash
# Arduino IDE: Sketch → Verify/Compile → Check output window

# Command line with avr-gcc:
avr-gcc -mmcu=attiny85 -Os -o output.elf sketch.cpp
avr-size --mcu=attiny85 -C output.elf
```

**Power (runtime):**
- Use a USB power meter between supply and circuit
- Or insert a multimeter (ammeter mode) in series with VCC
- Measure at different LED intensities (0x00 to 0x0F)

**Speed (runtime):**
- Toggle an unused pin at loop boundaries, measure with oscilloscope
- Or use `micros()` timing (adds code overhead)
