


#include <EEPROM.h>

// Piny przycisków
const int buttonPins[] = {47, 46, 44, 45, 42, 43, 40, 41, 38, 39, 36, 37, 30, 31, 28, 29, 26, 27, 24, 25, 22, 23, 20, 21, 18, 19, 16};
const int programmingButtonPin = 2;
const int ledPin = 13;

// Stany przycisków
int buttonStates[sizeof(buttonPins) / sizeof(buttonPins[0])];
int programmingButtonState = 1;
int lastProgrammingButtonState;

// Zmienne MIDI
byte midiChannel = 4; // Domyślny numer kanału MIDI (od 1 do 16)
const byte noteOnCommand = 0x90; // Polecenie Note On
const byte noteOffCommand = 0x80; // Polecenie Note Off

// Adres w pamięci EEPROM, gdzie zapisany jest numer kanału
int eepromAddress = 50;

// Zmienne dla obsługi błysku diody LED
bool expectancy=true;
bool isFlashing = false;
unsigned long flashStartTime = 0;
const unsigned long flashDuration = 50; // Czas trwania błysku (50 ms)

void setup() {
  // Inicjalizacja pinów przycisków
  for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttonStates[i] = HIGH;
  }
 
  pinMode(programmingButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
 
  // Odczyt numeru kanału z pamięci EEPROM
  midiChannel = EEPROM.read(eepromAddress);
 // if (midiChannel < 1 || midiChannel > 15) {
 //   midiChannel = 4; // Domyślny numer kanału, jeśli nieprawidłowy
//  }
 
  Serial.begin(31250); // Inicjalizacja komunikacji szeregowej z prędkością 31250 bps (standardowa dla MIDI)
 
  
delay(100);

programmingButtonState = digitalRead(programmingButtonPin);
if(programmingButtonState == LOW){
  digitalWrite(ledPin, HIGH); // Zapal diodę LED
  flashStartTime = millis();
  isFlashing = true;
  do {
     if (digitalRead(buttonPins[0])==LOW){
       midiChannel=1;
       EEPROM.write(eepromAddress, midiChannel); 
       expectancy=false;   
     }
     if (digitalRead(buttonPins[1])==LOW){
       midiChannel=2;
       EEPROM.write(eepromAddress, midiChannel);
       expectancy=false; 
     }
     if (digitalRead(buttonPins[2])==LOW){
       midiChannel=4;
       EEPROM.write(eepromAddress, midiChannel);
       expectancy=false;
     }
     if (digitalRead(buttonPins[3])==LOW){
       midiChannel=4;
       EEPROM.write(eepromAddress, midiChannel);
       expectancy=false;
     } 
     if (digitalRead(buttonPins[4])==LOW){
       midiChannel=5;
       EEPROM.write(eepromAddress, midiChannel);
       expectancy=false;
     } 
     if (digitalRead(buttonPins[5])==LOW){
       midiChannel=6;
       EEPROM.write(eepromAddress, midiChannel);
       expectancy=false;
     } 
       if (digitalRead(buttonPins[6])==LOW){
       midiChannel=7;
       EEPROM.write(eepromAddress, midiChannel);
       expectancy=false;
     } 
    } while (expectancy);
}
}
void loop() {
  

  // Odczyt stanów przycisków
  for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++) {
    int buttonState = digitalRead(buttonPins[i]);

    // Sprawdź, czy stan przycisku się zmienił
    if (buttonState != buttonStates[i]) {
      buttonStates[i] = buttonState;

      // Sprawdź, czy przycisk został naciśnięty (stan niski)
      if (buttonState == LOW) {
        // Wyślij polecenie MIDI "Note On"
        sendMIDICommand(noteOnCommand + (midiChannel -1), i + 36, 127);
        startLEDFlash();
      } else {
        // Wyślij polecenie MIDI "Note Off"
        sendMIDICommand(noteOffCommand+ (midiChannel -1), i + 36, 0);
        startLEDFlash();
      }
    }
  }

  // Obsługa błysku diody LED
  if (isFlashing) {
    if (millis() - flashStartTime >= flashDuration) {
      digitalWrite(ledPin, LOW); // Zgaś diodę LED
      isFlashing = false;
    }
  }
}

// Funkcja do wysyłania komend MIDI
void sendMIDICommand(byte command, byte note, byte velocity) {
  Serial.write(command);
  Serial.write(note);
  Serial.write(velocity);
}

// Rozpoczęcie błysku diody LED
void startLEDFlash() {
  digitalWrite(ledPin, HIGH); // Zapal diodę LED
  flashStartTime = millis();
  isFlashing = true;
}

// Wejście w tryb programowania
void enterProgrammingMode() {
  startLEDFlash();
 
  // Odczytaj numer kanału z przycisków i zapisz do EEPROM
  midiChannel = readMIDIChannelFromButtons();
  saveMIDIChannelToEEPROM();
 
  exitProgrammingMode();
}

// Wyjście z trybu programowania
void exitProgrammingMode() {
  readMIDIChannelFromEEPROM();
}

// Odczyt numeru kanału MIDI z przycisków
byte readMIDIChannelFromButtons() {
  while(expectancy){
  for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
     
      expectancy=false;
      return i + 1;
       break; // Dodana instrukcja break
    }
  }
  }
 // return 1; // Domyślny numer kanału, jeśli żaden przycisk nie jest wciśnięty
}

// Odczyt numeru kanału MIDI z pamięci EEPROM
void readMIDIChannelFromEEPROM() {
  midiChannel = EEPROM.read(eepromAddress);
  if (midiChannel < 1 || midiChannel > 16) {
    midiChannel = 1; // Domyślny numer kanału, jeśli nieprawidłowy
  }
}

// Zapisz numer kanału MIDI do pamięci EEPROM
void saveMIDIChannelToEEPROM() {
  EEPROM.write(eepromAddress, midiChannel);
}
