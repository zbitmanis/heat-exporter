#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "sensor.h"
#include "wifi.h"
#include "secrets.h"

#define DHTPIN1 2
#define DHTPIN2 3
#define DHTPIN3 4
#define DHTPIN4 5
#define DHTPIN5 6
#define DHT_CONECTED 3
#define DHT_DELAY_SENSOR 0
//#define DHT_USE_HUMIDITY






#define DHTTYPE DHT22


#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2
#define LCD_DEG_CHAR 223


uint32_t genDelayMS = 1000;


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

void setupLCDOutput(
  byte dht_count,
  std::vector<Sensor>&,
  byte sitype,
  bool blink = false,
  byte hlen = 3,
  bool backlight = true) {
  byte maxpos = 0;

  if (blink) {
    for (byte i = 0; i < 2; i++) {
      lcd.backlight();
      delay(250);
      lcd.noBacklight();
      delay(250);
    }
  }

  for (byte i = 0; i < dht_count; i++) {
    byte pos = i * hlen;
    maxpos = pos;
    lcd.setCursor(i * hlen, 0);
    lcd.print(sensors[i].header);
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
  while (!Serial) {
    ;  // wait for serial port to connect.
  }

  sensors.push_back(Sensor(DHTPIN1, "boiler_out", "bo"));
  sensors.push_back(Sensor(DHTPIN2, "floor_in", "fi"));
  sensors.push_back(Sensor(DHTPIN3, "floor_out", "fo"));
  sensors.push_back(Sensor(DHTPIN4, "radiators_in", "ri"));
  sensors.push_back(Sensor(DHTPIN5, "radiators_out", "ro"));

  //iniitialize lcd
  lcd.init();
  char ubuf[3] = "";
  ubuf[0] = (char)223;
  ubuf[1] = '\0';

  strcat(ubuf, "C");

  char ssid[] = SECRET_SSID;  // your network SSID (name)
  char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

  connectToWifi(&status, ssid, pass);


  setupLCDOutput(DHT_CONECTED, sensors, DHT_SITTYPE2, true);

  // initilaize sensors

  for (byte i = 0; i < DHT_CONECTED; i++) {
    sensor_t sensor;

    sensors[i].dht = new DHT_Unified(sensors[i].pin, DHTTYPE);

    sensors[i].dht->begin();

    sensors[i].dht->temperature().getSensor(&sensor);
    if (i == DHT_DELAY_SENSOR) {
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
  for (byte i = 0; i < DHT_CONECTED; i++) {
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
