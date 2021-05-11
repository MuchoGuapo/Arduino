#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define DHTPIN 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,20,4);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hi!");
  lcd.setCursor(0,1);
  lcd.print("I'm iot_de_mucho.");

  dht.begin();
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  lcd.setCursor(0,0);
  lcd.print("Humi[%]: ");
  lcd.print(h,1);
  lcd.print("   ");
  lcd.setCursor(0,1);
  lcd.print("Temp[C]: ");
  lcd.print(t,1);
  lcd.print("   ");  
}
