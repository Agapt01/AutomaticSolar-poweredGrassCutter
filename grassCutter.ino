#include <Stepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// ----- Stepper motor (knife drop) -----
const int stepsPerRevolution = 200;
Stepper mower_stepper(stepsPerRevolution, A0, A1, A2, A3); // IN1-IN4 for stepper

// ----- Gear motors via L298N driver -----
const int IN1 = 2; // Motor A
const int IN2 = 3;
const int IN3 = 4; // Motor B
const int IN4 = 5;

// ----- Cutter motor -----
const int DCMOTOR_PIN = 13; 

// ----- Keypad setup -----
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {9, 10, 11, 12}; 
byte colPins[COLS] = {6, 7, 8};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ----- LCD (I2C, 0x27 address) -----
LiquidCrystal_I2C lcd(0x27, 16, 2);


int grass_height = 0;

void setup() {
  // Initialize stepper
  mower_stepper.setSpeed(60);

  // Motor pin setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(DCMOTOR_PIN, OUTPUT);
  digitalWrite(DCMOTOR_PIN, LOW); // Off initially

  // LCD initialization
  lcd.init();
  lcd.backlight();

  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Solar-powered");
  lcd.setCursor(0, 1);
  lcd.print("grass cutter");
  delay(2000);

  // Prompt user for height
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter height:");
  lcd.setCursor(0, 1);

  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key >= '0' && key <= '9') {
        if (input.length() < 2) {
          input += key;
          lcd.print(key);
        }
      } else if (key == '#') {
        break; // Confirm
      } else if (key == '*') {
        input = "";
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
      }
    }
  }

  grass_height = input.toInt();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Grass height:");
  lcd.setCursor(0, 1);
  lcd.print(grass_height);
  lcd.print(" cm");
  delay(2000);

  // Drop cutter using stepper motor
  int steps = map(grass_height, 0, 30, 0, stepsPerRevolution); // adjust 30 to your real max
  mower_stepper.step(steps);

  // Start cutter motor
  digitalWrite(DCMOTOR_PIN, HIGH);

  // Start moving forward
  moveForward();
}

void loop() {
// ----- Movement Functions -----
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
}


