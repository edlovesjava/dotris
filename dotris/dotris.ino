/*
 * Micro DOTris - A Tetris-like game for ATtiny85 with 8x8 LED Matrix
 * 
 * Hardware:
 * - ATtiny85 microcontroller
 * - 8x8 LED Matrix (MAX7219 or similar)
 * - Push buttons for controls
 * 
 * Pin Configuration:
 * - See hardware.md for detailed pin mappings
 */

// Constants
const int MATRIX_SIZE = 8;
const int GAME_SPEED = 500; // milliseconds

// Pin definitions (adjust based on your hardware)
const int DATA_PIN = 0;   // DIN on MAX7219 (PB0)
const int CLK_PIN = 1;    // CLK on MAX7219 (PB1)
const int CS_PIN = 2;     // CS/LOAD on MAX7219 (PB2)
const int BTN_LEFT = 3;   // Left button (PB3)
const int BTN_RIGHT = 4;  // Right button (PB4)
// Note: ATtiny85 has only 5 usable I/O pins (PB0-PB4)
// For rotate button, use RESET pin with special config or external I/O expander
// Current config: rotate functionality is limited without additional hardware

// Game state
byte gameBoard[MATRIX_SIZE];
int currentPiece = 0;
int currentX = 3;
int currentY = 0;
int score = 0;

// Tetromino shapes (simplified for 8x8 matrix)
const byte pieces[7][4] = {
  {0x0F, 0x00, 0x00, 0x00}, // I
  {0x06, 0x06, 0x00, 0x00}, // O
  {0x0E, 0x04, 0x00, 0x00}, // T
  {0x0C, 0x06, 0x00, 0x00}, // S
  {0x06, 0x0C, 0x00, 0x00}, // Z
  {0x0E, 0x02, 0x00, 0x00}, // L
  {0x0E, 0x08, 0x00, 0x00}  // J
};

void setup() {
  // Initialize pins
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  // Note: Rotate button requires RESET pin config or I/O expander
  
  // Initialize display
  initDisplay();
  
  // Initialize game
  resetGame();
}

void loop() {
  // Read input
  if (digitalRead(BTN_LEFT) == LOW) {
    movePiece(-1);
    delay(200); // Debounce
  }
  if (digitalRead(BTN_RIGHT) == LOW) {
    movePiece(1);
    delay(200); // Debounce
  }
  // Rotate button disabled - requires RESET pin config or I/O expander
  // if (digitalRead(BTN_ROTATE) == LOW) {
  //   rotatePiece();
  //   delay(200); // Debounce
  // }
  
  // Game tick
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > GAME_SPEED) {
    lastUpdate = millis();
    updateGame();
  }
  
  // Update display
  updateDisplay();
}

void initDisplay() {
  // Initialize MAX7219 or LED matrix controller
  // This is a placeholder - actual initialization depends on your hardware
  sendCommand(0x0C, 0x01); // Normal operation
  sendCommand(0x0B, 0x07); // Scan all digits
  sendCommand(0x0A, 0x08); // Medium intensity
  sendCommand(0x09, 0x00); // No decode mode
  sendCommand(0x0F, 0x00); // Display test off
  
  // Clear display
  for (int i = 1; i <= 8; i++) {
    sendCommand(i, 0x00);
  }
}

void sendCommand(byte address, byte data) {
  digitalWrite(CS_PIN, LOW);
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, address);
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, data);
  digitalWrite(CS_PIN, HIGH);
}

void resetGame() {
  // Clear game board
  for (int i = 0; i < MATRIX_SIZE; i++) {
    gameBoard[i] = 0;
  }
  
  // Spawn new piece
  currentPiece = random(7);
  currentX = 3;
  currentY = 0;
  score = 0;
}

void updateGame() {
  // Move piece down
  currentY++;
  
  // Check collision
  if (checkCollision()) {
    currentY--; // Move back up
    lockPiece(); // Lock piece to board
    checkLines(); // Check for completed lines
    
    // Spawn new piece
    currentPiece = random(7);
    currentX = 3;
    currentY = 0;
    
    // Check game over
    if (checkCollision()) {
      resetGame();
    }
  }
}

bool checkCollision() {
  // Simplified collision detection
  // This is a placeholder - implement based on actual piece representation
  if (currentY >= MATRIX_SIZE - 1) {
    return true;
  }
  return false;
}

void lockPiece() {
  // Lock current piece to game board
  // This is a placeholder - implement based on actual piece representation
  gameBoard[currentY] |= pieces[currentPiece][0];
}

void checkLines() {
  // Check for completed lines and clear them
  for (int y = 0; y < MATRIX_SIZE; y++) {
    if (gameBoard[y] == 0xFF) {
      // Line is complete
      score++;
      
      // Shift down
      for (int i = y; i > 0; i--) {
        gameBoard[i] = gameBoard[i - 1];
      }
      gameBoard[0] = 0;
    }
  }
}

void movePiece(int dx) {
  currentX += dx;
  
  // Check boundaries
  if (currentX < 0) currentX = 0;
  if (currentX > MATRIX_SIZE - 1) currentX = MATRIX_SIZE - 1;
  
  // Check collision
  if (checkCollision()) {
    currentX -= dx; // Move back
  }
}

void rotatePiece() {
  // Rotate current piece
  // This is a placeholder - implement rotation logic
}

void updateDisplay() {
  // Update LED matrix display
  byte displayBuffer[MATRIX_SIZE];
  
  // Copy game board to display buffer
  for (int i = 0; i < MATRIX_SIZE; i++) {
    displayBuffer[i] = gameBoard[i];
  }
  
  // Add current piece to display buffer
  // This is a placeholder - implement based on actual piece representation
  if (currentY >= 0 && currentY < MATRIX_SIZE) {
    displayBuffer[currentY] |= pieces[currentPiece][0];
  }
  
  // Send to display
  for (int i = 0; i < MATRIX_SIZE; i++) {
    sendCommand(i + 1, displayBuffer[i]);
  }
}
