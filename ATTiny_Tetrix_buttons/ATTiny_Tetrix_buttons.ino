// v1.1 - Added score tracking and display
#include <avr/pgmspace.h>

// MAX7219 Register Addresses
#define MAX7219_REG_NOOP 0x00
#define MAX7219_REG_DIGIT0 0x01
#define MAX7219_REG_DIGIT1 0x02
#define MAX7219_REG_DIGIT2 0x03
#define MAX7219_REG_DIGIT3 0x04
#define MAX7219_REG_DIGIT4 0x05
#define MAX7219_REG_DIGIT5 0x06
#define MAX7219_REG_DIGIT6 0x07
#define MAX7219_REG_DIGIT7 0x08

#define MAX7219_REG_DECODE_MODE 0x09
#define MAX7219_REG_INTENSITY 0x0A  // brightness 0–15
#define MAX7219_REG_SCAN_LIMIT 0x0B
#define MAX7219_REG_SHUTDOWN 0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F


const uint8_t DIN = 0;
const uint8_t CLK = 1;
const uint8_t CS = 2;
const uint8_t RIGHT_BUTTON = 3;
const uint8_t LEFT_BUTTON = 4;

byte matrix[8];
uint16_t score = 0;
uint8_t startLevel = 1;  // Starting level 1-10 (selected at game start)

// 3x5 pixel font for digits 0-9 (stored in program memory)
const uint8_t PROGMEM font3x5[10][5] = {
  {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
  {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
  {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
  {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
  {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
  {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
  {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
  {0b111, 0b001, 0b010, 0b010, 0b010}, // 7
  {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
  {0b111, 0b101, 0b111, 0b001, 0b111}, // 9
};

void sendByte(uint8_t b) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(CLK, LOW);
    digitalWrite(DIN, (b >> i) & 1);
    digitalWrite(CLK, HIGH);
  }
}

void sendCmd(uint8_t addr, uint8_t data) {
  digitalWrite(CS, LOW);
  sendByte(addr);
  sendByte(data);
  digitalWrite(CS, HIGH);
}

void updateDisplay() {
  for (uint8_t row = 0; row < 8; row++) {
    sendCmd(row + 1, matrix[row]);
  }
}

// Display a single 3x5 digit centered on the 8x8 matrix
void displayDigit(uint8_t digit) {
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
  for (uint8_t row = 0; row < 5; row++) {
    matrix[row + 1] = pgm_read_byte(&font3x5[digit][row]) << 2; // Center horizontally
  }
  updateDisplay();
}

// Display level select screen and wait for player to choose
// LEFT decrements, RIGHT increments, BOTH buttons starts game
void selectStartLevel() {
  uint8_t lastRight = HIGH;
  uint8_t lastLeft = HIGH;

  while (true) {
    // Display current level (1-10, show as 1-9 and "10" as 0 with dot indicator)
    for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;

    if (startLevel == 10) {
      // Show "10" - display 1 and 0 side by side
      for (uint8_t row = 0; row < 5; row++) {
        uint8_t one = pgm_read_byte(&font3x5[1][row]);
        uint8_t zero = pgm_read_byte(&font3x5[0][row]);
        matrix[row + 1] = (one << 5) | (zero << 1);
      }
    } else {
      // Show single digit centered
      for (uint8_t row = 0; row < 5; row++) {
        matrix[row + 1] = pgm_read_byte(&font3x5[startLevel][row]) << 2;
      }
    }

    // Add level indicator dots at bottom (shows difficulty)
    matrix[7] = (0xFF >> (10 - startLevel)) << (10 - startLevel) / 2;

    updateDisplay();
    delay(50);

    // Read buttons
    uint8_t rightBtn = digitalRead(RIGHT_BUTTON);
    uint8_t leftBtn = digitalRead(LEFT_BUTTON);

    // Check for chord (both pressed) - start game
    if (rightBtn == LOW && leftBtn == LOW) {
      // Wait for release to avoid immediate input in game
      while (digitalRead(RIGHT_BUTTON) == LOW || digitalRead(LEFT_BUTTON) == LOW) {
        delay(10);
      }
      // Clear display before game starts
      for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
      updateDisplay();
      delay(200);
      return;
    }

    // RIGHT button - increment level (on press, not hold)
    if (rightBtn == LOW && lastRight == HIGH) {
      if (startLevel < 10) startLevel++;
    }

    // LEFT button - decrement level (on press, not hold)
    if (leftBtn == LOW && lastLeft == HIGH) {
      if (startLevel > 1) startLevel--;
    }

    lastRight = rightBtn;
    lastLeft = leftBtn;
  }
}

// Scroll score digits across the display at game over
void displayScore(uint16_t value) {
  uint8_t digits[5];
  uint8_t numDigits = 0;

  // Extract digits (handle zero)
  if (value == 0) {
    digits[numDigits++] = 0;
  } else {
    uint16_t temp = value;
    while (temp > 0 && numDigits < 5) {
      digits[numDigits++] = temp % 10;
      temp /= 10;
    }
    // Reverse to get correct order
    for (uint8_t i = 0; i < numDigits / 2; i++) {
      uint8_t tmp = digits[i];
      digits[i] = digits[numDigits - 1 - i];
      digits[numDigits - 1 - i] = tmp;
    }
  }

  // Scroll each digit across the screen
  for (uint8_t d = 0; d < numDigits; d++) {
    uint8_t digit = digits[d];

    // Scroll from right to center, pause, then scroll left
    // Scroll in from right
    for (int8_t x = -3; x <= 2; x++) {
      for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
      for (uint8_t row = 0; row < 5; row++) {
        uint8_t glyph = pgm_read_byte(&font3x5[digit][row]);
        if (x >= 0) {
          matrix[row + 1] = glyph << (5 - x);
        } else {
          matrix[row + 1] = glyph << (5 + (-x));
        }
      }
      updateDisplay();
      delay(60);
    }

    // Pause in center
    delay(300);

    // Scroll out to left
    for (int8_t x = 2; x >= -3; x--) {
      for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
      for (uint8_t row = 0; row < 5; row++) {
        uint8_t glyph = pgm_read_byte(&font3x5[digit][row]);
        if (x >= 0) {
          matrix[row + 1] = glyph << x;
        }
      }
      updateDisplay();
      delay(60);
    }
  }
}

void setup() {
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);

  sendCmd(MAX7219_REG_DISPLAY_TEST, 0x00);
  sendCmd(MAX7219_REG_DECODE_MODE, 0x00);
  sendCmd(MAX7219_REG_SCAN_LIMIT, 0x07);
  sendCmd(MAX7219_REG_INTENSITY, 0x08);  //brightness
  sendCmd(MAX7219_REG_SHUTDOWN, 0x01);

  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
  updateDisplay();

  randomSeed(analogRead(3));

  // Show level select screen at startup
  selectStartLevel();
}

uint16_t tick = 0;
uint8_t speed = 1;
uint16_t rightHoldTimer = 0;
uint16_t leftHoldTimer = 0;

void loop() {
  uint8_t col = random(0, 8);
  uint8_t row = 0;
  uint8_t bit = (1 << col);

  // Reset hold timers for new piece
  rightHoldTimer = 0;
  leftHoldTimer = 0;

  tick = tick + 1;
  if (tick % 10 == 0) {
    speed++;
    if (speed > 30) speed = 30;
  }

  // Apply starting level to speed (level 1 = normal, level 10 = max speed)
  uint8_t effectiveSpeed = speed + (startLevel - 1) * 6;
  if (effectiveSpeed > 30) effectiveSpeed = 30;

  uint16_t dropInterval = 800 - (effectiveSpeed * 20);  // gets smaller as speed increases
  uint8_t moveInterval = 30;                   // constant horizontal speed

  uint16_t dropCounter = 0;
  uint8_t moveCounter = 0;

  uint8_t moveDelay = 200;  // ms before repeat starts
  uint8_t moveRepeat = 80;  // ms between repeats

  // Falling block animation
  while (row < 7 && !(matrix[row + 1] & bit)) {


    // show falling pixel at current row/col
    sendCmd(row + 1, matrix[row] | bit);

    // tiny time slice
    delay(1);

    moveCounter++;
    if (moveCounter >= moveInterval) {
      moveCounter = 0;

      bool rightState = (digitalRead(RIGHT_BUTTON) == LOW);
      bool leftState = (digitalRead(LEFT_BUTTON) == LOW);

      bool softDrop = rightState && leftState;

      if (softDrop) {
        dropInterval = 20;  //fast
      } else {

        // check rightState
        if (rightState) {
          rightHoldTimer += moveInterval;

          if (rightHoldTimer == moveInterval || rightHoldTimer > moveDelay) {
            if (col > 0 && !(matrix[row] & (bit >> 1))) {
              sendCmd(row + 1, matrix[row]);
              // update position
              col--;
              bit >>= 1;
              // draw new
              sendCmd(row + 1, matrix[row] | bit);
            }
            if (rightHoldTimer > moveDelay) rightHoldTimer -= moveRepeat;
          }
        }

        // check leftState
        if (leftState) {
          leftHoldTimer += moveInterval;

          if (leftHoldTimer == moveInterval || leftHoldTimer > moveDelay) {
            if (col < 7 && !(matrix[row] & (bit << 1))) {
              sendCmd(row + 1, matrix[row]);
              // update position
              col++;
              bit <<= 1;
              // draw new
              sendCmd(row + 1, matrix[row] | bit);
            }
            if (leftHoldTimer > moveDelay) leftHoldTimer -= moveRepeat;
          }
        }
      }
    }

    // gravity timer
    dropCounter++;
    if (dropCounter >= dropInterval) {
      dropCounter = 0;
      // erase falling pixel before dropping
      sendCmd(row + 1, matrix[row]);
      row++;
    }
  }

  // land the block
  matrix[row] |= bit;
  updateDisplay();
  delay(80);

  //if row fill, clear row and drop lower rows
  if (matrix[row] == 0xFF) {
    score += startLevel;  // Score multiplied by starting level

    //flash row twice
    delay(60);
    sendCmd(row + 1, 0x00);
    delay(60);
    sendCmd(row + 1, 0xFF);
    delay(60);
    sendCmd(row + 1, 0x00);
    delay(60);
    sendCmd(row + 1, 0xFF);
    delay(60);

    //copy lower rows over
    for (int r = row; r > 0; r--) {
      matrix[r] = matrix[r - 1];
    }

    //blank out row zero
    matrix[0] = 0x00;

    //draw
    updateDisplay();
  }

  // If top row is filled, reset
  if (matrix[0] != 0) {

    //flash board twice
    sendCmd(MAX7219_REG_INTENSITY, 0x0D);
    delay(60);
    sendCmd(MAX7219_REG_INTENSITY, 0x08);
    delay(60);
    sendCmd(MAX7219_REG_INTENSITY, 0x0D);
    delay(60);
    sendCmd(MAX7219_REG_INTENSITY, 0x08);

    //hold and clear board
    delay(1000);
    for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
    updateDisplay();
    delay(300);

    // Display final score
    displayScore(score);
    delay(500);

    // Reset game state
    score = 0;
    speed = 1;
    tick = 0;

    // Let player select level for next game
    selectStartLevel();
  }
}