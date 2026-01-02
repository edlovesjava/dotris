# dotris
Sketches and other info on Micro DOTris, a tetris like game using an 8x8 LED matrix and the ATtiny85

## Getting Started

This project uses [PlatformIO](https://platformio.org/) for development and building.

### Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation.html) or [PlatformIO IDE](https://platformio.org/platformio-ide)
- USBasp programmer (or compatible) for uploading to ATtiny85

### Building the Project

```bash
# Build the project
pio run

# Upload to ATtiny85
pio run --target upload

# Clean build files
pio run --target clean
```

### Project Structure

- `src/` - Source files (main.cpp)
- `include/` - Header files
- `lib/` - Project-specific libraries
- `test/` - Unit tests
- `platformio.ini` - PlatformIO configuration

### Hardware

- **Microcontroller**: ATtiny85
- **Display**: 8x8 LED matrix
- **Clock**: 8MHz (internal oscillator)

## Development

The project is configured for the ATtiny85 microcontroller running at 8MHz. You can modify the `platformio.ini` file to adjust settings for your specific hardware setup.
