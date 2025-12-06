
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <vector>


#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#include "sensor.h"

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2
#define LCD_DEG_CHAR 223

extern byte wifiSymbol[];
extern byte wifiNoSymbol[];


void clearLCD( LiquidCrystal_I2C * lcd, byte col, byte lines);

void setupLCDOutput(
  LiquidCrystal_I2C * lcd,
  byte dht_count,
  std::vector<Sensor>&,
  byte sitype,
  bool blink,
  byte hlen = 3,
  bool backlight = true, 
  bool wifi = true);