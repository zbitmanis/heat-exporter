#include "lcd.h"
#include "heat-exporter.h"


byte wifiSymbol[] = {
  B11111,
  B00000,
  B11111,
  B00000,
  B11111,
  B01110,
  B00100,
  B00100
  
};

byte wifiNoSymbol[] = {
  
  B11100,
  B00110,
  B11001,
  B01100,
  B10011,
  B00110,
  B11100,
  B11100
  
};

void clearLCD( LiquidCrystal_I2C * lcd, byte col, byte lines) {
  lcd->clear();
  for (byte i = 0 ; i <lines; i++){
      for (byte j = 0 ; j < col; j++){
        lcd->setCursor( j, i);
        lcd->print(" ");
      }
  } 
}

void setupLCDOutput(
  LiquidCrystal_I2C * lcd,
  byte dht_count,
  std::vector<Sensor>& sensors,
  byte sitype,
  bool blink,
  byte hlen,
  bool backlight, 
  bool wifi ) {
  byte maxpos = 0;

  if (blink) {
    for (byte i = 0; i < 2; i++) {
      lcd->backlight();
      delay(250);
      lcd->noBacklight();
      delay(250);
    }
  }
 
  lcd->createChar(0, wifiSymbol);
  lcd->setCursor(LCD_COLUMNS - 1, 0);
  if(wifi){
   lcd->write(0);  
  }else{
   lcd->print("X");
  };

  for (byte i = 0; i < dht_count; i++) {
    byte pos = i * hlen;
    maxpos = pos;
    lcd->setCursor(i * hlen, 0);
    lcd->print(sensors[i].header);
  }

  if (maxpos < LCD_COLUMNS - 2) {
    switch (sitype) {
      case DHT_SITTYPE1:
        lcd->setCursor(LCD_COLUMNS - 2, 1);
        lcd->print(char(LCD_DEG_CHAR));
        lcd->setCursor(LCD_COLUMNS - 1, 1);
        lcd->print("C");
        break;
      case DHT_SITTYPE2:
        lcd->setCursor(LCD_COLUMNS - 1, 1);
        lcd->print(DHT_SITMEASURE2);
        break;
    }
  }

  if (backlight) {
    lcd->backlight();
  } else {
    lcd->noBacklight();
  }
}