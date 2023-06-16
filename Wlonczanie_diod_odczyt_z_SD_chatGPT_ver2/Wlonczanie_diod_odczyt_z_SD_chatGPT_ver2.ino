#include <LiquidCrystal.h>
#include <SD.h>
#include <SPI.h>

// Definicje pinów
const int buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3, A4, A5, A6, A7};
const int ledPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41};
const int loadButtonPins[] = {42, 43, 44, 45, 46, 47, 48, 49};
const int saveButtonPin = 51;
const int crescButtonPin = 52;
const int crescLedPin = 53;

// Zmienne
bool buttonState[20] = {0};
bool ledState[20] = {0};
int bank = 0;
int crescendo = 0;
const int debounceDelay = 50;
unsigned long lastDebounceTime[20] = {0};

// Inicjalizacja wyświetlacza LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  // Inicjalizacja pinów przycisków i LED-ów
  for (int i = 0; i < 20; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
  }
  
  pinMode(saveButtonPin, INPUT_PULLUP);
  pinMode(crescButtonPin, INPUT_PULLUP);
  pinMode(crescLedPin, OUTPUT);

  // Inicjalizacja wyświetlacza LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Bank:");
  
  // Inicjalizacja karty SD
  if (!SD.begin(10)) {
    Serial.println("Błąd inicjalizacji karty SD");
    while (1);
  }
}

void loop() {
  handleButtons();
  handleEncoder();
  handlePotentiometer();
}

void handleButtons() {
  for (int i = 0; i < 20; i++) {
    if (debounceButton(i)) {
      toggleLED(i);

      if (digitalRead(crescButtonPin) == HIGH) {
        digitalWrite(crescLedPin, ledState[i] ? HIGH : LOW);
      }
    }
  }

  if (digitalRead(saveButtonPin) == LOW && digitalRead(crescButtonPin) == HIGH) {
    saveStateToSD();
  }

  for (int i = 0; i < 8; i++) {
    if (digitalRead(loadButtonPins[i]) == LOW && digitalRead(crescButtonPin) == HIGH) {
      loadStateFromSD(i + 1);
      digitalWrite(crescLedPin, LOW);
    }
  }
}

bool debounceButton(int button) {
  bool currentState = digitalRead(buttonPins[button]);

  if (currentState != buttonState[button]) {
    lastDebounceTime[button] = millis();
  }

  if (millis() - lastDebounceTime[button] > debounceDelay) {
    if (currentState != buttonState[button]) {
      buttonState[button] = currentState;
      return currentState == LOW;
    }
  }

  return false;
}

void toggleLED(int led) {
  ledState[led] = !ledState[led];
  digitalWrite(ledPins[led], ledState[led] ? HIGH : LOW);
}

void handleEncoder() {
  static int encoderPos = 0;
  static int lastEncoderPos = 0;

  int encoderDelta = readEncoder();
  if (encoderDelta != 0) {
    encoderPos += encoderDelta;

    if (encoderPos > 20) {
      encoderPos = 20;
    } else if (encoderPos < 1) {
      encoderPos = 1;
    }

    if (encoderPos != lastEncoderPos) {
      bank = encoderPos;
      updateLCD();
      lastEncoderPos = encoderPos;
    }
  }
}

int readEncoder() {
  static int oldAB = 0;
  static const int enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

  oldAB <<= 2;
  oldAB |= (digitalRead(crescButtonPin) << 1) | digitalRead(crescButtonPin - 1);
  return enc_states[(oldAB & 0x0F)];
}

void handlePotentiometer() {
  int potValue = analogRead(A0);
  crescendo = map(potValue, 0, 1023, 1, 20);
  updateLCD();
}

void updateLCD() {
  lcd.setCursor(7, 1);
  lcd.print("       ");
  lcd.setCursor(7, 1);
  lcd.print("Cresc: ");
  lcd.print(crescendo);
}

void saveStateToSD() {
  String fileName = "bank" + String(bank) + "file.txt";
  File file = SD.open(fileName, FILE_WRITE);

  if (file) {
    for (int i = 0; i < 20; i++) {
      file.println(ledState[i] ? "1" : "0");
    }

    file.close();
  } else {
    Serial.println("Błąd otwarcia pliku");
  }
}

void loadStateFromSD(int fileIndex) {
  String fileName = "bank" + String(bank) + "file" + String(fileIndex) + ".txt";
  File file = SD.open(fileName);

  if (file) {
    for (int i = 0; i < 20; i++) {
      if (file.available()) {
        String line = file.readStringUntil('\n');
        ledState[i] = line.toInt() == 1;
        digitalWrite(ledPins[i], ledState[i] ? HIGH : LOW);
      }
    }

    file.close();
  } else {
    Serial.println("Błąd otwarcia pliku");
  }
}
