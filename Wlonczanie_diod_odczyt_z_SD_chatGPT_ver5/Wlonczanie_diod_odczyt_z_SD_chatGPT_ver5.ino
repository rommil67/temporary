/* Ten kod obsługuje 32 LED i 32 przyciski w macierzy 8x8. 
Przyciski włączania/wyłączania LED są obsługiwane w pętli głównej loop(), 
a przyciski dodatkowe (cresc, save, plus, minus) są obsługiwane
w bloku if-else z warunkiem sprawdzającym stan przycisku cresc.

Proszę zauważyć, że kod ten zakłada istnienie odpowiednich 
pinów na Arduino Mega dla 32 LED, 32 przycisków, przycisków dodatkowych
 oraz diody crescLed. Upewnij się, że piny są poprawnie przypisane 
 i podłączone do odpowiednich elementów sprzętowych.
*/

#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
// Obiekt do obsługi wyświetlacza LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int SDchipSelectPin = 10; // Pin CS dla karty SD

const byte numRows = 8;    // Liczba wierszy w macierzy przycisków
const byte numCols = 8;    // Liczba kolumn w macierzy przycisków

byte ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37}; // Piny LED
byte buttonPins[] = {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71}; // Piny przycisków

byte ledState[numCols][numRows];      // Stan LED (0 lub 1)
byte buttonState[numCols][numRows];   // Stan przycisków (HIGH lub LOW)

int bank = 0;           // Numer banku (od 0 do 7)
int crescendo = 0;      // Wartość crescendo (od 1 do 20)

const int crescButtonPin = 72;    // Przycisk cresc
const int saveButtonPin = 73;     // Przycisk zapisu
const int plusButtonPin = 74;     // Przycisk plus
const int minusButtonPin = 75;    // Przycisk minus
const int crescLedPin = 76;       // Dioda crescLed

void setup() {
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

  // Inicjalizacja pinów LED jako OUTPUT
  for (int i = 0; i < 32; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Inicjalizacja przycisków jako INPUT_PULLUP
  for (int i = 0; i < 32; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Inicjalizacja pinów dla dodatkowych przycisków
  pinMode(crescButtonPin, INPUT_PULLUP);
  pinMode(saveButtonPin, INPUT_PULLUP);
  pinMode(plusButtonPin, INPUT_PULLUP);
  pinMode(minusButtonPin, INPUT_PULLUP);
  pinMode(crescLedPin, OUTPUT);
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
    if (buttonState[saveButtonPin - 40][crescButtonPin - 40] == LOW) {
      saveLEDState("file" + String(bank) + ".txt");
    }
    if (buttonState[plusButtonPin - 40][crescButtonPin - 40] == LOW) {
      bank = (bank + 1) % 8;
      updateLEDs();
    }
    if (buttonState[minusButtonPin - 40][crescButtonPin - 40] == LOW) {
      bank = (bank - 1 + 8) % 8;
      updateLEDs();
    }
  }

  // Obsługa przycisków włączania/wyłączania LED
  for (byte col = 0; col < numCols; col++) {
    for (byte row = 0; row < numRows; row++) {
      if (buttonState[col][row] == LOW) {
        ledState[col][row] = !ledState[col][row];
        digitalWrite(ledPins[row * 8 + col], ledState[col][row]);
      }
    }
  }

  // Wyświetlanie wartości crescendo na LCD
  displayCrescendo();

  delay(50); // Opóźnienie dla stabilności odczytu przycisków
}

void clearStates() {
  for (byte col = 0; col < numCols; col++) {
    for (byte row = 0; row < numRows; row++) {
      ledState[col][row] = 0;
      buttonState[col][row] = HIGH;
    }
  }
}

void readButtons() {
  for (byte col = 0; col < numCols; col++) {
    pinMode(buttonPins[col], OUTPUT);
    digitalWrite(buttonPins[col], LOW);
    for (byte row = 0; row < numRows; row++) {
      buttonState[col][row] = digitalRead(buttonPins[row + numCols]);
    }
    pinMode(buttonPins[col], INPUT_PULLUP);
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
          } else {
            ledState[col][row] = 0;
            digitalWrite(ledPins[row * 8 + col], LOW);
          }
        }
      }
    }
    dataFile.close();
  } else {
    Serial.println("Błąd podczas otwierania pliku: " + fileName);
  }
}

void displayCrescendo() {
  lcd.setCursor(0, 1);
  lcd.print("Cresc: " + String(crescendo));
}
