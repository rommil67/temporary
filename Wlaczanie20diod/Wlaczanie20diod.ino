//włącza 20 diod 
const int buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
const int ledPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41};

bool ledState[20] = {false}; // Tablica do przechowywania stanu diod
bool buttonState[20] = {false}; // Tablica do przechowywania stanu przycisków

void setup() {
  for (int i = 0; i < 20; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 20; i++) {
    buttonState[i] = digitalRead(buttonPins[i]); // Odczytaj stan przycisku

    if (buttonState[i] == LOW && !ledState[i]) { // Jeśli przycisk jest wciśnięty i dioda jest wyłączona
      digitalWrite(ledPins[i], HIGH); // Włącz diodę
      ledState[i] = true; // Zapisz stan diody jako włączony
    } else if (buttonState[i] == LOW && ledState[i]) { // Jeśli przycisk jest wciśnięty i dioda jest włączona
      digitalWrite(ledPins[i], LOW); // Wyłącz diodę
      ledState[i] = false; // Zapisz stan diody jako wyłączony
    }
  }
}
