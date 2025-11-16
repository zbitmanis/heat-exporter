#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "sensor.h"

#define DHTPIN1  2
#define DHTPIN2  3
#define DHTPIN3  4
#define DHTPIN4  5
#define DHTPIN5  6
#define DHT_CONECTED 3
#define DHT_DELAY_SENSOR 0
//#define DHT_USE_HUMIDITY

#define DEBUG_HEATER 1


#define DHT_SITTYPE1 1
#define DHT_SITTYPE2 2

#define DHT_SITYPE1_NAME "temperature"
#define DHT_SITYPE2_NAME "humidity"
#define DHT_SITMEASURE1 "°C"
#define DHT_SITMEASURE2 "%"



#define DHTTYPE DHT22


#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES   2
#define LCD_DEG_CHAR 223


uint32_t genDelayMS = 1000;



Sensor sensors [] = {
  {DHTPIN1, "boiler_out", "bo"},
  {DHTPIN2, "floor_in", "fi"},
  {DHTPIN3, "floor_out", "fo"},
  {DHTPIN4, "radiators_in", "ri"},
  {DHTPIN5, "radiators_out", "ro"}
};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

const char* getSensorName(byte pin) {
  // find sensor name from
  char sname [24];
  bool found = false;


  for (byte i = 0 ; i < sizeof(sensors) && !found; i++) {
    if (pin == sensors[i].pin) {
      strcpy(sname, sensors[i].name);
      if (DEBUG_HEATER) {
        Serial.print("sensor nr: ");
        Serial.print(i);
        Serial.print(" name:");
        Serial.print(sname);
        Serial.print(" pin:");
        Serial.print(sensors[i].pin);
        Serial.println();
      }
      found = true;
    }
  }
  return sname;
}

void printSensorInfo(sensor_t* sensor, byte sitype, const char * name) {
  char buf[64];
  char sitmeasure[8];

  switch (sitype) {
    case DHT_SITTYPE1:
      strcpy(buf, DHT_SITYPE1_NAME);
      strcat(buf, " for ");
      strcat(buf, name);
      strcpy(sitmeasure, DHT_SITMEASURE1);
      break;
    case DHT_SITTYPE2:
      strcpy (buf, DHT_SITYPE2_NAME);
      strcat (buf, " for ");
      strcat(buf, name);
      strcpy(sitmeasure, DHT_SITMEASURE2);
      break;
  }


  Serial.println(F("------------------------------------"));
  Serial.println(buf);
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor->name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor->version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor->sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor->max_value); Serial.println(sitmeasure);
  Serial.print  (F("Min Value:   ")); Serial.print(sensor->min_value); Serial.println(sitmeasure);
  Serial.print  (F("Resolution:  ")); Serial.print(sensor->resolution); Serial.println(sitmeasure);
  Serial.println(F("------------------------------------"));

}

void setupLCDOutput(
  byte dht_count,
  Sensor *sensors_array,
  byte sitype,
  bool blink = false,
  byte hlen = 3,
  bool backlight = false ) {
  byte maxpos = 0;

  if (blink) {
    for ( byte i = 0; i < 2; i++ ) {
      lcd.backlight();
      delay(250);
      lcd.noBacklight();
      delay(250);
    }
  }

  for ( byte i = 0; i < dht_count; i++ ) {
    byte pos = i * hlen;
    maxpos = pos;
    lcd.setCursor(i * hlen, 0);
    lcd.print(sensors_array[i].header);
  }

  if (maxpos < LCD_COLUMNS - 2) {
    switch (sitype) {
      case DHT_SITTYPE1:
        lcd.setCursor(LCD_COLUMNS - 2, 0);
        lcd.print(char(LCD_DEG_CHAR));
        lcd.setCursor(LCD_COLUMNS - 1, 0);
        lcd.print("C");
        break;
      case DHT_SITTYPE2:
        lcd.setCursor(LCD_COLUMNS - 1, 0);
        lcd.print(DHT_SITMEASURE2);
        break;
    }
  }

  if (backlight) {
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
}

void setup() {
  Serial.begin(9600);

  //iniitialize lcd
  lcd.init();
  char ubuf[3]="";
  ubuf[0]=(char)223;
  ubuf[1]='\0';
  
  strcat(ubuf, "C");
  setupLCDOutput(DHT_CONECTED, sensors, DHT_SITTYPE2, true);

  // initilaize sensors

  for ( byte i = 0 ; i < DHT_CONECTED; i++ ) {
    sensor_t sensor;

    sensors[i].dht = new DHT_Unified(sensors[i].pin, DHTTYPE);

    sensors[i].dht->begin();

    sensors[i].dht->temperature().getSensor(&sensor);
    if ( i == DHT_DELAY_SENSOR ) {
      genDelayMS = sensor.min_delay / 1000;
    }
    printSensorInfo(&sensor, DHT_SITTYPE1, sensors[i].name);

#ifdef DHT_USE_HUMIDITY
    sensors[i].dht->humidity().getSensor(&sensor);
    printSensorInfo(&sensor, DHT_SITTYPE2, sensors[i].name);
#endif
  }

}



void loop() {
  delay(genDelayMS);
  sensors_event_t event;
  char buf[64];
  byte tlen = 3;
  for ( byte i = 0; i < DHT_CONECTED; i++ ) {
    sensors[i].dht->temperature().getEvent(&event);

    lcd.setCursor(i * tlen, 1);

    if (isnan(event.temperature)) {
      strcpy(buf, "error reading temperature for ");
      strcat(buf, sensors[i].name);
      Serial.println(buf);
      sensors[i].last_temperature = DHT_ERR;
    } else {
      sensors[i].last_temperature = event.temperature;

      lcd.print(sensors[i].last_temperature);
      strcpy(buf, DHT_SITYPE1_NAME);
      strcat(buf, " for ");
      strcat(buf, sensors[i].name);
      strcat(buf, ": ");
      Serial.print(buf);
      Serial.print(event.temperature);
      Serial.println(DHT_SITMEASURE1);
    }
  }
  //delay(100);
  //lcd.setCursor(0, 0);
  //lcd.print("Heater");
  //lcd.setCursor(0, 1);
  //lcd.print(millis() / 1000);
  //delay(100);

#ifdef DHT_USE_HUMIDITY
#endif

}
