// Włączanie diod i zapis stanu na kartę sd
#include <SPI.h>
#include <SD.h>

const int buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
const int ledPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41};

bool ledState[20] = {false}; // Tablica do przechowywania stanu diod
bool buttonState[20] = {false}; // Tablica do przechowywania stanu przycisków



const int debounceDelay = 50; // Opóźnienie debouncingu w milisekundach
unsigned long lastDebounceTime[20] = {0}; // Tablica do przechowywania czasu ostatniego debouncingu przycisków

const int chipSelect = 53; // Pin CS dla karty SD
// const char* fileName = "stany_diod.txt"; // Nazwa pliku na karcie SD


void setup() {
  for (int i = 0; i < 20; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(buttonPins[20], INPUT_PULLUP);
  pinMode(buttonPins[21], INPUT_PULLUP);

  Serial.begin(9600);

  if (!SD.begin(chipSelect)) {
    Serial.println("Błąd inicjalizacji karty SD!");
    while (1);
  }
}
bool debounceButton(int buttonIndex) {
  int buttonPin = buttonPins[buttonIndex];
  bool currentState = digitalRead(buttonPin);

  if (currentState != buttonState[buttonIndex]) {
    lastDebounceTime[buttonIndex] = millis();
  }

  if (millis() - lastDebounceTime[buttonIndex] > debounceDelay) {
    if (currentState != buttonState[buttonIndex]) {
      buttonState[buttonIndex] = currentState;
      return true;
    }
  }

  return false;
}

void loop() {
  for (int i = 0; i < 20; i++) {
    if (debounceButton(i)) { // Wywołaj funkcję debounceButton() dla każdego przycisku
      if (buttonState[i] == LOW && !ledState[i]) { // Jeśli przycisk jest wciśnięty i dioda jest wyłączona
        digitalWrite(ledPins[i], HIGH); // Włącz diodę
        ledState[i] = true; // Zapisz stan diody jako włączony
      } else if (buttonState[i] == LOW && ledState[i]) { // Jeśli przycisk jest wciśnięty i dioda jest włączona
        digitalWrite(ledPins[i], LOW); // Wyłącz diodę
        ledState[i] = false; // Zapisz stan diody jako wyłączony
      }
    }
  }

  if (debounceButton(20)) { // Wywołaj funkcję debounceButton() dla przycisku zapisu na kartę SD
    if (buttonState[20] == LOW) { // Jeśli przycisk zapisu jest wciśnięty
      zapiszStanyNaKarcieSD(); // Zapisz stany diod na kartę SD
    }
  }

  if (debounceButton(21)) { // Wywołaj funkcję debounceButton() dla przycisku odczytu z karty SD
    if (buttonState[21] == LOW) { // Jeśli przycisk odczytu jest wciśnięty
      odczytajStanyZKartySD(); // Odczytaj stany diod z karty SD
    }
  }
}

void zapiszStanyNaKarcieSD() {
  File dataFile = SD.open("stany_diod.txt", FILE_WRITE);

  if (dataFile) {
    for (int i = 0; i < 20; i++) {
      dataFile.print(ledState[i]);
      dataFile.print(" ");
    }
    dataFile.println();
    dataFile.close();
    Serial.println("Zapisano stany diod na karcie SD.");
  } else {
    Serial.println("Błąd zapisu na kartę SD.");
  }
}

void odczytajStanyZKartySD() {
  if (SD.exists("stany_diod.txt")) {
    File dataFile = SD.open("stany_diod.txt");

    if (dataFile) {
      String line = dataFile.readStringUntil('\n');
      dataFile.close();

      Serial.println("Odczytane stany diod z karty SD:");
      Serial.println(line);

      int index = 0;
      for (int i = 0; i < line.length(); i++) {
        if (line[i] == '0' || line[i] == '1') {
          ledState[index] = line[i] - '0';
          digitalWrite(ledPins[index], ledState[index] ? HIGH : LOW);
          index++;
        }
      }

      Serial.println("Aktualizowano stany diod na podstawie odczytanych danych z karty SD.");
    } else {
      Serial.println("Błąd odczytu z karty SD.");
    }
  } else {
    utworzPlikNaKarcieSD(); // Utwórz nowy plik na karcie SD
  }
}

void utworzPlikNaKarcieSD() {
  File dataFile = SD.open("stany_diod.txt", FILE_WRITE);

  if (dataFile) {
    String defaultStates = "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
    dataFile.println(defaultStates);
    dataFile.close();
    Serial.println("Utworzono nowy plik na karcie SD.");
  } else {
    Serial.println("Błąd tworzenia pliku na karcie SD.");
  }
}

