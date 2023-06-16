/* Ten kod obsługuje 32 LED i 32 przyciski w macierzy 8x8. 
Przyciski włączania/wyłączania LED są obsługiwane w pętli głównej loop(), 
a przyciski dodatkowe (cresc, save, plus, minus) są obsługiwane
w bloku if-else z warunkiem sprawdzającym stan przycisku cresc.

Proszę zauważyć, że kod ten zakłada istnienie odpowiednich 
pinów na Arduino Mega dla 32 LED, 32 przycisków, przycisków dodatkowych
 oraz diody crescLed. Upewnij się, że piny są poprawnie przypisane 
 i podłączone do odpowiednich elementów sprzętowych.

 WERSJA 0.9
*/

#include <SPI.h>
#include <LiquidCrystal.h>
#include <SD.h>

const int numRows = 8; // liczba wierszy w macierzy przycisków
const int numCols = 8; // liczba kolumn w macierzy przycisków

// Definicje pinów dla macierzy przycisków
int rowPins[numRows] = {2, 3, 4, 5, 6, 7, 8, 9};
int colPins[numCols] = {10, 11, 12, 13, A1, A2, A3, A4};

// Definicje pinów dla przycisku 'Set' i LED 'crescLed'
const int setCrescButtonPin = 51;
const int crescLedPin = 22;

// Definicje pinów dla przycisków odczytu z karty SD
int loadButtonPins[8] = {30, 31, 32, 33, 34, 35, 36, 37};

// Definicje innych zmiennych globalnych
int bank = 0;
int crescendo = 0;
LiquidCrystal lcd(40, 41, 42, 43, 44, 45, 46);

void setup() {
  // Inicjalizacja wyświetlacza LCD
  lcd.begin(16, 2);
  lcd.print("Bank: ");
  lcd.setCursor(0, 1);
  lcd.print("Cresc: 0");

  // Inicjalizacja pinów macierzy przycisków
  for (int row = 0; row < numRows; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], HIGH);
  }
  for (int col = 0; col < numCols; col++) {
    pinMode(colPins[col], INPUT_PULLUP);
  }

  // Inicjalizacja pinów przycisków 'Set' i 'crescLed'
  pinMode(setCrescButtonPin, INPUT_PULLUP);
  pinMode(crescLedPin, OUTPUT);

  // Inicjalizacja pinów przycisków odczytu z karty SD
  for (int i = 0; i < 8; i++) {
    pinMode(loadButtonPins[i], INPUT_PULLUP);
  }

  // Inicjalizacja karty SD
  if (!SD.begin(53)) {
    lcd.setCursor(0, 1);
    lcd.print("SD Card Error");
    while (1);
  }
}

void loop() {
  // Odczyt wartości zmiennej crescendo z potencjometru
  int crescValue = analogRead(A1);
  crescendo = map(crescValue, 0,
