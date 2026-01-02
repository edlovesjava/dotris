# Bill of Materials (BOM)

## Electronic Components

| Quantity | Component | Specifications | Notes |
|----------|-----------|----------------|-------|
| 1 | ATtiny85 Microcontroller | 8-bit AVR, DIP-8 package | Main controller |
| 1 | 8x8 LED Matrix Module | MAX7219 driver, red LEDs | Display |
| 3 | Tactile Push Button | 6x6mm, through-hole | Game controls |
| 1 | Capacitor | 0.1µF ceramic, 50V | Power decoupling |
| 3 | Resistor | 10kΩ, 1/4W | Button pull-ups (optional) |
| 1 | IC Socket | 8-pin DIP | For ATtiny85 (optional) |

## Power Supply

| Quantity | Component | Specifications | Notes |
|----------|-----------|----------------|-------|
| 1 | USB Cable | Micro or Mini USB | For 5V power |
| 1 | Power Adapter | 5V, 500mA minimum | Alternative to USB |
| OR 1 | Battery Holder | 3x AAA or 2x AA | Portable option |
| OR 1 | Step-up converter | 3V to 5V | If using battery |

## Hardware & Misc

| Quantity | Component | Specifications | Notes |
|----------|-----------|----------------|-------|
| 1 | Breadboard | 400 or 830 points | For prototyping |
| 1 set | Jumper Wires | Male-to-male, various lengths | Connections |
| 1 | AVR Programmer | USBtinyISP or similar | For uploading code |

## Optional Components

| Quantity | Component | Specifications | Notes |
|----------|-----------|----------------|-------|
| 1 | Piezo Buzzer | 5V active | Sound effects |
| 1 | Potentiometer | 10kΩ linear | Difficulty adjustment |
| 1 | Custom PCB | - | For permanent assembly |
| 1 | Enclosure | - | Project case |

## Where to Buy

### Online Retailers
- **DigiKey** - www.digikey.com
- **Mouser Electronics** - www.mouser.com
- **SparkFun** - www.sparkfun.com
- **Adafruit** - www.adafruit.com
- **AliExpress** - www.aliexpress.com (budget option)
- **Amazon** - www.amazon.com

### Suggested Kits/Modules
- **8x8 LED Matrix Module**: Search for "MAX7219 8x8 LED Matrix"
- **ATtiny85 Development Board**: Some boards include USB programming capability
- **Arduino Starter Kit**: Often includes buttons, resistors, and breadboard

## Estimated Cost

| Component Category | Approximate Cost (USD) |
|-------------------|------------------------|
| Electronics | $8-15 |
| Power Supply | $3-8 |
| Hardware/Misc | $10-20 |
| Programmer | $15-25 (if not owned) |
| **Total** | **$36-68** |

*Note: Prices vary by supplier and region. Buying in bulk or kits can reduce costs.*

## Component Substitutions

### LED Matrix
- Can use individual LEDs arranged in a matrix (more complex wiring)
- Can use different sizes (4x4, 16x16) with code modifications
- Different LED colors are compatible

### Microcontroller
- ATtiny45 (4KB flash) - Limited space for features
- ATtiny25 (2KB flash) - Very limited, not recommended
- Arduino Nano/Uno - Overkill but works with minor pin changes

### MAX7219 Driver
- Can use shift registers (74HC595) - Requires significant code changes
- Can use other LED matrix controllers with similar SPI interface

## Notes
- All resistors should be 1/4W or higher
- Capacitor voltage rating should be at least 2x the operating voltage
- Buttons can be normally-open tactile switches or any momentary switches
- Wire gauge: 22-26 AWG solid core for breadboard, stranded for permanent connections
