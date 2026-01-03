# Programming the ATTiny with an Arduino NANO

-   Why we need a programmer
-   Wiring table (Nano → ATtiny85)
-   Optional 10 μF capacitor on Nano RESET
-   Uploading ArduinoISP
-   Installing ATtiny core in Arduino IDE
-   Setting board, clock, programmer
-   Burning bootloader (sets fuses)

**Overview**

To program the ATtiny85, you need a programmer because the chip itself
doesn't have a built-in USB interface. While dedicated programmers
exist, you can use an Arduino Nano as a cost-effective and flexible ISP
(In-System Programmer). This section guides you through the wiring,
setup, and programming steps.

**Parts and Tools Needed**

-   Breadboard
-   Arduino Nano (any clone works)
-   ATtiny85 microcontroller
-   Jumper wires (solid gauge .64mm/22 AWG is best IMHO)
-   10 μF electrolytic capacitor
-   LED and 220 Ohm resistor (for test program)
-   Computer with Arduino IDE installed
-   USB cable (for Nano)
-   Breadboard power supply (optional, for standalone power)

**Wiring Connections**

Connect the Arduino Nano to the ATtiny85 as follows:

| Arduino Nano Pin | ATtiny85 Pin    | Function            |
|------------------|-----------------|---------------------|
| D13              | Pin 7 (SCK)     | Serial Clock        |
| D12              | Pin 6 (MISO)    | Master In Slave Out |
| D11              | Pin 5 (MOSI)    | Master Out Slave In |
| D10              | Pin 1 (RESET)   | Reset               |
| 5V               | Pin 8 (Vcc)     | Power               |
| GND              | Pin 4 (GND)     | Ground              |

**Note:** You will need to connect a 10 μF capacitor between RESET and
GND on the Nano to prevent auto-reset during programming. Remember the
negative (striped) side of the capacitor goes to GND.

**Step-by-Step Tutorial**

**1. Prepare the Arduino Nano**

1.  Plug the Nano into the breadboard.

2.  Wire the Nano to the ATtiny85 as shown in the table above.

**2. Set Up the Arduino IDE**

1.  Open Arduino IDE on your computer.

2.  Go to **File \> Examples \> 11.ArduinoISP** and select the
    ArduinoISP sketch.

3.  Choose the correct board (**Arduino Nano**) and port.

4.  Upload the ArduinoISP sketch to the Nano.

5.  Insert the 10 μF capacitor between the Nano's RESET and GND pins.

**3. Install the ATtiny Core**

1.  In the Arduino IDE, go to **Tools \> Board \> Boards Manager**.

2.  Add the ATtiny core using the provided URL:\
    https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json

3.  Install 'attiny' by David A Mellis.

4.  Select **ATtiny 25/45/85** as the board.

5.  Set the clock to **8 MHz**.

6.  Choose **ATtiny85** as the processor.

7.  Set the programmer to **Arduino as ISP**.

**4. Burn the Bootloader (you only need to do this once for a new ATTiny
chip)**

1.  Place the ATtiny85 on the breadboard and wire it to the Nano.

2.  In Arduino IDE, go to **Tools \> Burn Bootloader** to set the fuses
    for the ATtiny85.

**5. Upload Your First Program**

1.  Write or select your sketch (e.g., Blink).

2.  Modify the sketch to use pin 0 instead of LED_BUILTIN.

3.  Use **Sketch \> Upload Using Programmer** (not just Upload) to
    program the ATtiny85.

**6. Test the Setup**

1.  Connect pin 5 of the ATtiny85 to one end of a 220 Ohm resistor.

2.  Connect the other end to the anode of an LED; cathode to ground.

3.  Power the circuit and confirm the LED blinks.

**Troubleshooting Tips**

-   Double-check all wiring, especially the capacitor orientation.

-   Ensure the correct board, processor, and programmer are selected in
    Arduino IDE.

-   If programming fails, try resetting the Nano or re-uploading the
    ArduinoISP sketch.

The blink code

```c
// Blink for ATtiny85 on Pin 0 (physical pin 5)
void setup() {
  pinMode(0, OUTPUT); // ATtiny85 pin 0
}

void loop() {
  digitalWrite(0, HIGH);
  delay(500);
  digitalWrite(0, LOW);
  delay(500);
}
```