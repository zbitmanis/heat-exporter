#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "heat-exporter.h"
#include "sensor.h"
#include "wifi.h"
#include "secrets.h"
#include "lcd.h"

#define DHTPIN1 2
#define DHTPIN2 3
#define DHTPIN3 4
#define DHTPIN4 5
#define DHTPIN5 6
#define DHT_CONECTED 5
#define DHT_DELAY_SENSOR 0
//#define DHT_USE_HUMIDITY
#define BUTTON_PIN 6
#define BUZZER_PIN 7

#define DHT_INTERVAL 2000
#define BUTTON_INTERVAL 250




#define DHTTYPE DHT22





uint32_t genDelayMS = 1000;
byte lastButtonState = HIGH;
byte buttonState = HIGH;

unsigned long startMillis = 0;
unsigned long currentMillis = 0;




LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);



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
   
  pinMode(BUTTON_PIN, INPUT);

  //iniitialize lcd
  lcd.init();
  char ubuf[3] = "";
  ubuf[0] = (char)223;
  ubuf[1] = '\0';

  strcat(ubuf, "C");

  char ssid[] = SECRET_SSID;  
  char pass[] = SECRET_PASS;  

  connectToWifi(&status, ssid, pass);


  setupLCDOutput(&lcd, DHT_CONECTED, sensors, DHT_SITTYPE1, true, true);

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
  currentMillis = millis();
  if (currentMillis - startMillis >= DHT_INTERVAL) {
    startMillis = currentMillis;
    for (byte i = 0; i < DHT_CONECTED; i++) {
      sensors[i].dht->temperature().getEvent(&event);

      lcd.setCursor(i * tlen, 1);

      if (isnan(event.temperature)) {
        strcpy(buf, "error reading temperature for ");
        strcat(buf, sensors[i].name);
        Serial.println(buf);
        sensors[i].last_temperature = DHT_ERR;
        lcd.print("Er");
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
