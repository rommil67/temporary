#include <SPI.h>

#include <SD.h>



String fileName ="diody";

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
odczytajStany Z Karty();
}
void odczytajStanyZKartySD() {
  if (SD.exists(fileName)) {
    File dataFile = SD.open(fileName);

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
