# Micro DOTris

A minimalist Tetris-like game for the ATtiny85 microcontroller with an 8x8 LED matrix display.

![Project Status](https://img.shields.io/badge/status-active-success.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)

## Overview

Micro DOTris is a compact implementation of the classic falling-block puzzle game, designed to run on the ATtiny85 microcontroller with minimal components. The game uses an 8x8 LED matrix (with MAX7219 driver) for display and three buttons for controls.

### Features

- Classic Tetris-style gameplay
- 7 different tetromino pieces
- Line clearing and scoring
- Compact design using ATtiny85 (8KB flash)
- Low power consumption
- Simple 2-button control scheme (expandable to 3 buttons)
- Adjustable game speed and display brightness

## Hardware Requirements

### Core Components
- **ATtiny85** microcontroller (8-bit AVR)
- **8x8 LED Matrix** with MAX7219 driver
- **2 Push Buttons** (left, right movement)
- **Power Supply** (5V, USB or battery)

*Note: Rotate button requires RESET pin configuration or I/O expander (see hardware.md)*

See [BOM.md](BOM.md) for complete bill of materials and purchasing information.

## Project Structure

```
dotris/
├── dotris/
│   └── dotris.ino          # Main Arduino sketch
├── hardware.md             # Hardware documentation and schematics
├── BOM.md                  # Bill of materials
├── ASSEMBLY.md             # Assembly and build instructions
├── README.md               # This file
└── LICENSE                 # License information
```

## Quick Start

### 1. Gather Components
Review [BOM.md](BOM.md) and acquire all necessary components.

### 2. Assemble Hardware
Follow the detailed instructions in [ASSEMBLY.md](ASSEMBLY.md) to:
- Wire components on breadboard
- Connect the ATtiny85 to LED matrix
- Connect control buttons

### 3. Set Up Software
- Install Arduino IDE
- Install ATTinyCore board support
- Configure programmer (USBtinyISP or similar)

### 4. Upload Code
- Open `dotris/dotris.ino` in Arduino IDE
- Select ATtiny85 board and configure settings
- Burn bootloader (one time)
- Upload sketch to ATtiny85

### 5. Play!
- Power on the device
- Use buttons to move and rotate pieces
- Clear lines to score points

## Documentation

- **[hardware.md](hardware.md)** - Detailed pin configurations, circuit diagrams, and hardware specifications
- **[BOM.md](BOM.md)** - Complete parts list with specifications and suppliers
- **[ASSEMBLY.md](ASSEMBLY.md)** - Step-by-step build and programming instructions

## Controls

| Button | Function |
|--------|----------|
| LEFT   | Move piece left |
| RIGHT  | Move piece right |

*Note: Rotate functionality requires RESET pin configuration or I/O expander. See [hardware.md](hardware.md) for details.*

## Customization

The code can be easily customized:

- **Game Speed**: Adjust `GAME_SPEED` constant (milliseconds between moves)
- **Display Brightness**: Modify MAX7219 intensity setting (0x00-0x0F)
- **Pin Assignments**: Change pin definitions if using different connections

See [ASSEMBLY.md](ASSEMBLY.md) for detailed customization instructions.

## Technical Specifications

| Specification | Value |
|--------------|-------|
| Microcontroller | ATtiny85 @ 8MHz |
| Program Memory | ~4-6 KB (of 8KB available) |
| Display | 8x8 LED matrix |
| Input | 2 push buttons (expandable) |
| Power | 5V, ~200mA typical |
| Interface | SPI (bit-banged for MAX7219) |

## Troubleshooting

### Display not working
- Verify SPI connections (DATA, CLK, CS)
- Check power supply connections
- Ensure MAX7219 is receiving 5V

### Buttons not responding
- Check button connections to ground
- Verify internal pull-ups are enabled
- Test buttons with multimeter

### Code won't upload
- Verify programmer connections
- Check that correct board is selected
- Try burning bootloader again

See [ASSEMBLY.md](ASSEMBLY.md) for more troubleshooting tips.

## Future Enhancements

Potential improvements and features:
- Sound effects using piezo buzzer
- Multiple difficulty levels
- High score saving to EEPROM
- Preview of next piece
- Custom game modes
- Battery power optimization
- 3D printed enclosure design

## Contributing

Contributions are welcome! Feel free to:
- Report bugs and issues
- Suggest new features
- Submit pull requests
- Share your builds and modifications

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by the classic Tetris game
- Built with the Arduino ecosystem
- Uses ATTinyCore by Spence Konde

## Resources

- [Arduino Official Site](https://www.arduino.cc/)
- [ATTinyCore Documentation](https://github.com/SpenceKonde/ATTinyCore)
- [ATtiny85 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf)
- [MAX7219 Datasheet](https://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf)

## Contact

For questions, suggestions, or to share your build, please open an issue on GitHub.

---

**Enjoy building and playing Micro DOTris!**
