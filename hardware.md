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
| PB3 (Pin 2)  | D3          | BTN_LEFT | Left Button  |
| PB4 (Pin 3)  | D4          | BTN_RIGHT| Right Button |
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
