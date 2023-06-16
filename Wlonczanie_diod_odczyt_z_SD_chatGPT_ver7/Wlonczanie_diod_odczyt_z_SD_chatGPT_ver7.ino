/* Ten kod obsługuje 32 LED i 32 przyciski w macierzy 8x8. 
Przyciski włączania/wyłączania LED są obsługiwane w pętli głównej loop(), 
a przyciski dodatkowe (cresc, save, plus, minus) są obsługiwane
w bloku if-else z warunkiem sprawdzającym stan przycisku cresc.

Proszę zauważyć, że kod ten zakłada istnienie odpowiednich 
pinów na Arduino Mega dla 32 LED, 32 przycisków, przycisków dodatkowych
 oraz diody crescLed. Upewnij się, że piny są poprawnie przypisane 
 i podłączone do odpowiednich elementów sprzętowych.

 WERSJA 0.6
*/

#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>

// Definicje pinów
const byte buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
const byte ledPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53};
const byte loadButtonPins[] = {54, 55, 56, 57, 58, 59, 60, 61};
const byte saveButtonPin = 51;
const byte crescButtonPin = 52;
const byte plusButtonPin = 62;
const byte minusButtonPin = 63;
const byte crescLedPin = 40;
const byte numRows = 8;
const byte numCols = 8;

// Tablice przechowujące stany przycisków i LED
byte buttonState[numCols][numRows];
byte ledState[numCols][numRows];

// Zmienne
int bank = 0;
int crescendo = 0;

// Obiekt do obsługi wyświetlacza LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
  // Inicjalizacja wyświetlacza LCD
  lcd.begin(16, 2);
  
  // Inicjalizacja przycisków i LED
  for (byte i = 0; i < numCols; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
  }
  for (byte i = 0; i < 8; i++) {
    pinMode(loadButtonPins[i], INPUT_PULLUP);
  }
  pinMode(saveButtonPin, INPUT_PULLUP);
  pinMode(crescButtonPin, INPUT_PULLUP);
  pinMode(plusButtonPin, INPUT_PULLUP);
  pinMode(minusButtonPin, INPUT_PULLUP);
  pinMode(crescLedPin, OUTPUT);
  
  // Inicjalizacja karty SD
  if (!SD.begin(53)) {
    Serial.println("Błąd inicjalizacji karty SD!");
    while (1);
  }

  clearStates();
}

void loop() {
  readButtons();

  // Obsługa przycisku cresc
  if (buttonState[7][7] == LOW) {
    digitalWrite(crescLedPin, HIGH);
    if (buttonState[bank][crescendo] == LOW) {
      saveLEDState("cresc" + String(crescendo) + ".txt");
    }
    if (buttonState[saveButtonPin - 40][crescButtonPin - 40] == LOW) {
      loadLEDState("cresc" + String(crescendo) + ".txt");
    }
  } else {
    digitalWrite(crescLedPin, LOW);
    if (buttonState[bank][crescendo] == LOW) {
      saveLEDState("file.txt");
    }
    if (buttonState[saveButtonPin - 40][crescButtonPin - 40] == LOW) {
      loadLEDState("file.txt");
    }
  }

  // Obsługa przycisku plus
  if (buttonState[plusButtonPin - 40][plusButtonPin - 40] == LOW) {
    bank++;
    if (bank > 7) {
      bank = 0;
    }
  }

  // Obsługa przycisku minus
  if (buttonState[minusButtonPin - 40][minusButtonPin - 40] == LOW) {
    bank--;
    if (bank < 0) {
      bank = 7;
    }
  }

  // Aktualizacja wyświetlacza LCD
  displayBank();
  displayCrescendo();

  // Obsługa LED
  for (byte col = 0; col < numCols; col++) {
    for (byte row = 0; row < numRows; row++) {
      if (buttonState[col][row] == LOW) {
        ledState[col][row] = !ledState[col][row];
        digitalWrite(ledPins[row * 8 + col], ledState[col][row]);
      }
    }
  }
}

void readButtons() {
  for (byte col = 0; col < numCols; col++) {
    for (byte row = 0; row < numRows; row++) {
      buttonState[col][row] = digitalRead(buttonPins[col * 8 + row]);
    }
  }
  for (byte i = 0; i < 8; i++) {
    buttonState[saveButtonPin - 40][i] = digitalRead(loadButtonPins[i]);
  }
  buttonState[saveButtonPin - 40][crescButtonPin - 40] = digitalRead(crescButtonPin);
  buttonState[plusButtonPin - 40][plusButtonPin - 40] = digitalRead(plusButtonPin);
  buttonState[minusButtonPin - 40][minusButtonPin - 40] = digitalRead(minusButtonPin);
}

void clearStates() {
  for (byte col = 0; col < numCols; col++) {
    for (byte row = 0; row < numRows; row++) {
      ledState[col][row] = 0;
      buttonState[col][row] = HIGH;
      digitalWrite(ledPins[row * 8 + col], LOW);
    }
  }
}

void saveLEDState(String fileName) {
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    for (byte col = 0; col < numCols; col++) {
      for (byte row = 0; row < numRows; row++) {
        dataFile.print(ledState[col][row]);
      }
    }
    dataFile.close();
  } else {
    Serial.println("Błąd podczas otwierania pliku: " + fileName);
  }
}

void loadLEDState(String fileName) {
  File dataFile = SD.open(fileName);
  if (dataFile) {
    clearStates();
    for (byte col = 0; col < numCols; col++) {
      for (byte row = 0; row < numRows; row++) {
        if (dataFile.available()) {
          char value = dataFile.read();
          if (value == '1') {
            ledState[col][row] = 1;
            digitalWrite(ledPins[row * 8 + col], HIGH);
          }
        }
      }
    }
    dataFile.close();
  } else {
    Serial.println("Błąd podczas otwierania pliku: " + fileName);
  }
}

void displayBank() {
  lcd.setCursor(0, 0);
  lcd.print("Bank: ");
  lcd.print(bank);
}

void displayCrescendo() {
  lcd.setCursor(0, 1);
  lcd.print("Cresc: ");
  lcd.print(crescendo);
}
