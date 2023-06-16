
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BACKLIGHT_PIN 3

LiquidCrystal_I2C	lcd(0x27,2,1,0,4,5,6,7);

void setup()
{
  lcd.begin (16,2);
 lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
 lcd.setBacklight(HIGH);

lcd.home ();
lcd.print("Kocham Cie Basiu");  
}

void loop()
{
 lcd.setCursor(0, 1);
 lcd.print(millis()/1000);
 }
