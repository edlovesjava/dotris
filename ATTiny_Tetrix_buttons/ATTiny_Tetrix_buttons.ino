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

  uint16_t dropInterval = 800 - (speed * 20);  // gets smaller as speed increases
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
    delay(500);

    // Reset game state
    speed = 1;
    tick = 0;
  }
}