#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal.h>
// Konfiguracja pinów dla przycisków w macierzy 8x8
const byte numRows = 8;
const byte numCols = 8;

byte rowPins[numRows] = {22, 23, 24, 25, 26, 27, 28, 29}; // Pinowe przyporządkowanie wierszy
byte colPins[numCols] = {30, 31, 32, 33, 34, 35, 36, 37}; // Pinowe przyporządkowanie kolumn

// Pozostałe zmienne i stałe
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 51, 52}; // Piny dla LED i przycisków
const byte saveButtonPin = 51; // Przycisk zapisu
const byte loadButtonPins[] = {53, 54, 55, 56, 57, 58, 59, 60}; // Przyciski odczytu
const byte crescButtonPin = 61; // Przycisk cresc
const byte plusButtonPin = 62; // Przycisk plus
const byte minusButtonPin = 63; // Przycisk minus
const byte crescLedPin = 64; // LED cresc
const byte SDchipSelectPin = 65; // Pin wyboru karty SD

// Zmienne
byte ledState[numCols][numRows]; // Tablica przechowująca stan LED
byte buttonState[numCols][numRows]; // Tablica przechowująca stan przycisków

int bank = 0; // Zmienna bank
int crescendo = 0; // Zmienna crescendo

void setup() {
  // Inicjalizacja pinów przycisków
  for (byte row = 0; row < numRows; row++) {
    pinMode(rowPins[row], INPUT_PULLUP);
  }

  for (byte col = 0; col < numCols; col++) {
    pinMode(colPins[col], OUTPUT);
    digitalWrite(colPins[col], HIGH);
  }

  // Inicjalizacja pinów LED
  for (int i = 0; i < 32; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Inicjalizacja pinów przycisków zapisu i odczytu
  pinMode(saveButtonPin, INPUT_PULLUP);
  for (int i = 0; i < 8; i++) {
    pinMode(loadButtonPins[i], INPUT_PULLUP);
  }

  // Inicjalizacja pinów przycisków cresc, plus, minus i LED cresc
  pinMode(crescButtonPin, INPUT_PULLUP);
  pinMode(plusButtonPin, INPUT_PULLUP);
  pinMode(minusButtonPin, INPUT_PULLUP);
  pinMode(crescLedPin, OUTPUT);
  digitalWrite(crescLedPin, LOW);

  // Inicjalizacja karty SD
  Serial.begin(9600);
  while (!Serial) {
    ; // Czekaj na połączenie z monitorem szeregowym
  }

  if (!SD.begin(SDchipSelectPin)) {
    Serial.println("Inicjalizacja karty SD nieudana!");
    return;
  }

  // Wyzeruj tablice stanów LED i przycisków
  clearStates();

  // Ustawienie początkowego stanu bank i crescendo
  bank = 0;
  crescendo = 0;

  // Wyświetlanie na LCD
  // ...
}

void loop() {
  // Odczyt stanu przycisków w macierzy
  readButtonStates();

  // Obsługa przycisku cresc
  if (buttonState[0][7] == HIGH && buttonState[7][7] == HIGH) {
    digitalWrite(crescLedPin, HIGH);
  } else {
    digitalWrite(crescLedPin, LOW);
  }

  // Obsługa przycisku zapisu
  if (digitalRead(saveButtonPin) == LOW && digitalRead(crescButtonPin) == LOW) {
    saveLEDState("cresc" + String(crescendo) + "file.txt");
  }

  // Obsługa przycisków odczytu
  for (int i = 0; i < 8; i++) {
    if (digitalRead(loadButtonPins[i]) == LOW && digitalRead(crescButtonPin) == LOW) {
      loadLEDState("bank" + String(bank) + "file" + String(i + 1) + ".txt");
    }
  }

  // Obsługa przycisków plus i minus
  if (digitalRead(plusButtonPin) == LOW && digitalRead(crescButtonPin) == LOW) {
    if (bank < 8) {
      bank++;
    } else {
      bank = 1;
    }
  }

  if (digitalRead(minusButtonPin) == LOW && digitalRead(crescButtonPin) == LOW) {
    if (bank > 1) {
      bank--;
    } else {
      bank = 8;
    }
  }

  // Aktualizacja wartości crescendo na wyświetlaczu LCD
  // ...

  // Aktualizacja stanu LED na podstawie macierzy ledState
  updateLEDs();
}

void readButtonStates() {
  for (byte col = 0; col < numCols; col++) {
    // Ustawienie pinu kolumny jako OUTPUT LOW
    pinMode(colPins[col], OUTPUT);
    digitalWrite(colPins[col], LOW);

    // Odczyt stanów przycisków w wierszu
    for (byte row = 0; row < numRows; row++) {
      buttonState[col][row] = digitalRead(rowPins[row]);
    }

    // Przywrócenie pinu kolumny jako INPUT_PULLUP
    pinMode(colPins[col], INPUT_PULLUP);
  }
}

void clearStates() {
  for (byte col = 0; col < numCols; col++) {
    for (byte row = 0; row < numRows; row++) {
      ledState[col][row] = LOW;
      buttonState[col][row] = HIGH;
    }
  }
}

void updateLEDs() {
  for (byte col = 0; col < numCols; col++) {
    for (byte row = 0; row < numRows; row++) {
      digitalWrite(ledPins[row * 8 + col], ledState[col][row]);
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
    Serial.println("Błąd podczas otwierania pliku do zapisu!");
  }
}

void loadLEDState(String fileName) {
  File dataFile = SD.open(fileName);
  if (dataFile) {
    byte index = 0;
    while (dataFile.available()) {
      ledState[index / numRows][index % numRows] = dataFile.read() - '0';
      index++;
    }
    dataFile.close();
  } else {
    Serial.println("Błąd podczas otwierania pliku do odczytu!");
  }
}
