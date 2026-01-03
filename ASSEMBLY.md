# Assembly and Build Instructions

## Prerequisites

### Software Requirements
1. **Arduino IDE** (version 1.8.x or 2.x)
   - Download from: https://www.arduino.cc/en/software

2. **ATtiny Board Support**
   - Install ATTinyCore by Spence Konde
   - In Arduino IDE: File → Preferences → Additional Board Manager URLs
   - Add: `https://drazzy.com/package_drazzy.com_index.json`
   - Tools → Board → Boards Manager → Search "ATTinyCore" → Install

3. **AVR Programmer Drivers**
   - Install drivers for your specific programmer (e.g., USBtinyISP)

### Hardware Requirements
- All components from BOM.md
- Soldering iron (if building permanent version)
- Multimeter (for testing)
- Computer with USB port

## Step 1: Breadboard Assembly

### 1.1 Place Components
1. Insert ATtiny85 (or DIP socket) into breadboard
2. Place LED matrix module nearby
3. Insert three push buttons with space between them
4. Add decoupling capacitor near ATtiny85

### 1.2 Power Connections
```
1. Connect ATtiny85 Pin 8 (VCC) to breadboard power rail (+5V)
2. Connect ATtiny85 Pin 4 (GND) to breadboard ground rail
3. Connect 0.1µF capacitor between power and ground near ATtiny85
4. Connect LED matrix VCC to +5V rail
5. Connect LED matrix GND to ground rail
```

### 1.3 LED Matrix Connections
```
ATtiny85 Pin 5 (PB0) → LED Matrix DIN
ATtiny85 Pin 6 (PB1) → LED Matrix CLK  
ATtiny85 Pin 7 (PB2) → LED Matrix CS
```

### 1.4 Button Connections
```
Button 1 (RIGHT):  One side to ATtiny85 Pin 2 (PB3), other side to GND
Button 2 (LEFT):   One side to ATtiny85 Pin 3 (PB4), other side to GND

Optional Button 3 (ROTATE): Requires RESET pin configuration or I/O expander
```

### 1.5 Power Supply
Connect 5V power adapter or USB cable to breadboard power rails.

## Step 2: Programming Setup

### 2.1 Connect Programmer
Connect your AVR programmer to the ATtiny85:

| Programmer | ATtiny85 Pin |
|------------|--------------|
| VCC | Pin 8 (VCC) |
| GND | Pin 4 (GND) |
| RESET | Pin 1 (RESET) |
| SCK | Pin 7 (PB2) |
| MISO | Pin 6 (PB1) |
| MOSI | Pin 5 (PB0) |

### 2.2 Configure Arduino IDE
1. Open Arduino IDE
2. Select: **Tools → Board → ATtinyCore → ATtiny25/45/85 (No bootloader)**
3. Configure settings:
   - **Chip**: ATtiny85
   - **Clock**: 8 MHz (internal)
   - **B.O.D**: Disabled
   - **Save EEPROM**: EEPROM retained
   - **Timer 1 Clock**: CPU
   - **LTO**: Enabled
   - **millis()/micros()**: Enabled
   - **Programmer**: USBtinyISP (or your programmer)
   - **Port**: (Select your programmer's port)

### 2.3 Burn Bootloader (Set Fuses)
**Important**: This step configures the ATtiny85 to use 8MHz internal clock.

1. In Arduino IDE: **Tools → Burn Bootloader**
2. Wait for "Done burning bootloader" message
3. This only needs to be done once per chip

## Step 3: Upload Code

### 3.1 Open Sketch
1. Navigate to the `ATTiny_Tetrix_buttons` folder
2. Open `ATTiny_Tetrix_buttons.ino` in Arduino IDE

### 3.2 Verify Compilation
1. Click the **Verify** button (checkmark icon)
2. Check that it compiles without errors
3. Note the memory usage in the output

### 3.3 Upload to ATtiny85
1. Ensure programmer is connected
2. Click **Upload** button (right arrow icon)
3. Wait for upload to complete
4. You should see "Done uploading" message

### 3.4 Disconnect Programmer
1. Carefully disconnect the programmer
2. The ATtiny85 will now run the uploaded code
3. LED matrix should initialize

## Step 4: Testing

### 4.1 Power On Test
1. Connect power to the circuit
2. LED matrix should light up briefly (initialization)
3. Game should start with a falling piece

### 4.2 Button Test
1. Press **LEFT** button - piece should move left
2. Press **RIGHT** button - piece should move right
3. Note: Rotate functionality requires additional hardware (see hardware.md)

### 4.3 Gameplay Test
1. Let a piece fall to the bottom
2. A new piece should appear at the top
3. Complete a row - it should clear

### 4.4 Troubleshooting Tests
If not working:
1. **Check power**: Measure 5V on VCC pin with multimeter
2. **Check connections**: Verify all wiring matches diagrams
3. **Check orientation**: Ensure ATtiny85 is inserted correctly
4. **Check LED matrix**: Test with a simple pattern in code
5. **Re-upload**: Try uploading the code again

## Step 5: Permanent Assembly (Optional)

### 5.1 PCB Option
1. Design or order a custom PCB based on the schematic
2. Solder components to PCB
3. Test before enclosing

### 5.2 Perfboard Option
1. Transfer circuit from breadboard to perfboard
2. Solder all connections
3. Add strain relief for power cable

### 5.3 Enclosure
1. Measure assembled circuit
2. Design or purchase suitable enclosure
3. Drill holes for buttons and LED matrix
4. Mount components securely

## Step 6: Customization

### 6.1 Adjust Initial Game Speed
In `ATTiny_Tetrix_buttons.ino`, find the `loop()` function and modify:
```cpp
uint16_t dropInterval = 800 - (speed * 20);  // Base value 800ms, decrease for faster
```

### 6.2 Adjust Display Brightness
In `setup()` function, modify:
```cpp
sendCmd(MAX7219_REG_INTENSITY, 0x08);  // Range: 0x00 (min) to 0x0F (max)
```

### 6.3 Change Pin Assignments
If using different pins, update constants at top of sketch:
```cpp
const uint8_t DIN = 0;      // PB0 - Data pin
const uint8_t CLK = 1;      // PB1 - Clock pin
const uint8_t CS = 2;       // PB2 - Chip select
const uint8_t LEFT_B = 3;   // PB3 - Left button
const uint8_t RIGHT_B = 4;  // PB4 - Right button
```

## Verification Checklist

- [ ] All components placed correctly
- [ ] Power connections verified (5V, GND)
- [ ] LED matrix connections correct (DIN, CLK, CS)
- [ ] Button connections verified
- [ ] Decoupling capacitor installed
- [ ] Bootloader burned (fuses set)
- [ ] Code compiled without errors
- [ ] Code uploaded successfully
- [ ] LED matrix initializes on power-up
- [ ] All buttons respond correctly
- [ ] Game plays as expected

## Safety Notes

- Never connect power supply with reversed polarity
- Keep maximum current draw within power supply limits
- Avoid short circuits
- Use appropriate ventilation when soldering
- Handle hot soldering iron with care
- Disconnect power when making circuit changes

## Next Steps

- Experiment with the code
- Add sound effects with a piezo buzzer
- Implement scoring display
- Add difficulty levels
- Create custom tetromino shapes
- Build an enclosure for portability

## Getting Help

If you encounter issues:
1. Review the hardware.md file for detailed pin information
2. Check BOM.md to ensure you have correct components
3. Verify connections with a multimeter
4. Search Arduino forums for ATtiny85 projects
5. Check the ATTinyCore documentation

Enjoy your Micro DOTris game!
