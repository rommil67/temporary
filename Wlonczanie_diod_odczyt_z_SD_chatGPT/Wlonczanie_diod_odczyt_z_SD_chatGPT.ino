#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

#define SD_CS_PIN 10
#define encoderPinA 51
#define encoderPinB 52
#define bankIncreasePin 53
#define bankDecreasePin 54
#define saveButtonPin 55

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int numButtons = 20;
const int numLEDs = 20;

int buttonPins[numButtons] = {2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
int ledPins[numLEDs] = {23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42};
int loadButtonPins[numLEDs] = {43, 44, 45, 46, 47, 48, 49, 50};
int buttonState[numButtons] = {LOW};
int ledState[numLEDs] = {LOW};
unsigned long lastDebounceTime[numButtons] = {0};
const unsigned long debounceDelay = 50;
volatile int encoderValue = 0;
volatile bool encoderChanged = false;
int bank = 0;

void setup() {
  Serial.begin(9600);
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(bankIncreasePin, INPUT_PULLUP);
  pinMode(bankDecreasePin, INPUT_PULLUP);
  pinMode(saveButtonPin, INPUT_PULLUP);

  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < numLEDs; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  for (int i = 0; i < numLEDs; i++) {
    pinMode(loadButtonPins[i], INPUT_PULLUP);
  }

  attachInterrupt(digitalPinToInterrupt(encoderPinA), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), handleEncoder, CHANGE);

  lcd.begin(16, 2);
  lcd.print("Bank: ");
  lcd.setCursor(0, 1);
  lcd.print("Plik: ");
  lcd.print(getLastDigit());

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Błąd inicjalizacji karty SD!");
    while (true);
  }

  Serial.println("Inicjalizacja zakończona.");
}

void loop() {
  for (int i = 0; i < numButtons; i++) {
    if (debounceButton(i)) {
      if (buttonState[i] == LOW) {
        toggleLED(i);
      }
    }
  }

  for (int i = 0; i < numLEDs; i++) {
    if (debounceButton(loadButtonPins[i])) {
      if (buttonState[loadButtonPins[i]] == LOW) {
        loadLEDStates(i);
      }
    }
  }

  if (debounceButton(saveButtonPin)) {
    if (buttonState[saveButtonPin] == LOW) {
      saveLEDStates();
    }
  }

  if (encoderChanged) {
    bank += encoderValue;
    if (bank > 256) {
      bank = 0;
    } else if (bank < 0) {
      bank = 256;
    }
    lcd.setCursor(6, 0);
    lcd.print("    ");
    lcd.setCursor(6, 0);
    lcd.print(bank);
    encoderChanged = false;
  }
}

bool debounceButton(int buttonIndex) {
  bool currentState = digitalRead(buttonPins[buttonIndex]);
  if (currentState != buttonState[buttonIndex]) {
    lastDebounceTime[buttonIndex] = millis();
  }
  if (millis() - lastDebounceTime[buttonIndex] >= debounceDelay) {
    buttonState[buttonIndex] = currentState;
    return true;
  }
  return false;
}

void toggleLED(int ledIndex) {
  ledState[ledIndex] = !ledState[ledIndex];
  digitalWrite(ledPins[ledIndex], ledState[ledIndex]);
}

void handleEncoder() {
  static byte previousA = 0;
  static byte previousB = 0;
  byte currentA = digitalRead(encoderPinA);
  byte currentB = digitalRead(encoderPinB);
  if (previousA == 0 && previousB == 1 && currentA == 1 && currentB == 1) {
    encoderValue++;
    encoderChanged = true;
  } else if (previousA == 1 && previousB == 1 && currentA == 1 && currentB == 0) {
    encoderValue--;
    encoderChanged = true;
  }
  previousA = currentA;
  previousB = currentB;
}

void saveLEDStates() {
  String fileName = "bank" + String(bank) + "file1.txt";
  File file = SD.open(fileName, FILE_WRITE);
  if (file) {
    for (int i = 0; i < numLEDs; i++) {
      file.println(ledState[i]);
    }
    file.close();
    Serial.println("Zapisano stan LED do pliku: " + fileName);
  } else {
    Serial.println("Błąd podczas zapisu do pliku: " + fileName);
  }
}

void loadLEDStates(int fileIndex) {
  String fileName = "bank" + String(bank) + "file" + String(fileIndex + 1) + ".txt";
  File file = SD.open(fileName);
  if (file) {
    int i = 0;
    while (file.available() && i < numLEDs) {
      int state = file.parseInt();
      ledState[i] = state;
      digitalWrite(ledPins[i], state);
      i++;
    }
    file.close();
    Serial.println("Wczytano stan LED z pliku: " + fileName);
  } else {
    Serial.println("Błąd podczas odczytu pliku: " + fileName);
  }
}

int getLastDigit() {
  String fileName = "bank" + String(bank) + "file1.txt";
  int lastDigit = 0;
  if (SD.exists(fileName)) {
    int length = fileName.length();
    if (length > 5) {
      lastDigit = fileName[length - 5] - '0';
    }
  }
  return lastDigit;
}
