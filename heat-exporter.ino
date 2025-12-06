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
#define BUTTON_PIN 8
#define LED_PIN 7

#define DHT_INTERVAL 2000
#define BUTTON_INTERVAL 250
#define CLIENT_INTERVAL 50
#define MIN_BUTTON_INTERVAL 50


#define WIFI_SERVER_PORT 80


#define DHTTYPE DHT22


byte lastButtonState = HIGH;
byte buttonPressed = false;
unsigned long startSensorMillis = 0;
unsigned long startButtonMillis = 0;
unsigned long buttonPressMillis = 0;
unsigned long currentMillis = 0;

unsigned int buttonPressDuration = 0;

void processButtonEvent(unsigned long currentMillis, LiquidCrystal_I2C *lcd){

    int buttonState = digitalRead(BUTTON_PIN);
    if (DEBUG_HEATER) {
      Serial.print("button state ");
      Serial.println(buttonState);
    }
    if (buttonState == LOW && lastButtonState == HIGH) {
      lastButtonState = buttonState;
      buttonPressed = true;
      buttonPressDuration = 0;
      buttonPressMillis = currentMillis;

      digitalWrite(LED_PIN, HIGH);

      Serial.println("the button is pressed");
      
      clearLCD(lcd, LCD_LINES, LCD_COLUMNS);
      printWifiData(lcd, LCD_COLUMNS);

    } else if (buttonState == HIGH && lastButtonState == LOW) {
      lastButtonState = buttonState;
      buttonPressed = false;

      digitalWrite(LED_PIN, LOW);
      clearLCD(lcd, LCD_LINES, LCD_COLUMNS);
      
      bool ws = getWifiStatus(&Serial);
      
      setupLCDOutput(lcd, DHT_CONECTED, sensors, DHT_SITTYPE1, false, 3, true, ws);
      
      Serial.print("the button is released after ");
      Serial.println(buttonPressDuration);
      buttonPressDuration = 0;
    
    } else {
      buttonPressDuration = currentMillis - buttonPressMillis;
    }

    startButtonMillis = currentMillis;
}

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
WiFiServer server(WIFI_SERVER_PORT);  //WiFi WebServer

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect.
  }

  sensors.push_back(Sensor(DHTPIN1, "boiler_out", "bo",DHTTYPE));
  sensors.push_back(Sensor(DHTPIN2, "floor_in", "fi", DHTTYPE));
  sensors.push_back(Sensor(DHTPIN3, "floor_out", "fo",DHTTYPE));
  sensors.push_back(Sensor(DHTPIN4, "radiators_in", "ri",DHTTYPE));
  sensors.push_back(Sensor(DHTPIN5, "radiators_out", "ro",DHTTYPE));

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  //iniitialize lcd
  lcd.init();
  char ubuf[3] = "";
  ubuf[0] = (char)223;
  ubuf[1] = '\0';

  strcat(ubuf, "C");

  char ssid[] = SECRET_SSID;
  char pass[] = SECRET_PASS;

  connectToWifi(&status, ssid, pass, LED_PIN);
  server.begin();

  setupLCDOutput(&lcd, DHT_CONECTED, sensors, DHT_SITTYPE1, true);

  // initilaize sensors
   initSensors(&Serial ,DHT_CONECTED, sensors );
}



void loop() {
  sensors_event_t event;

  currentMillis = millis();

  if (currentMillis - startSensorMillis >= DHT_INTERVAL) {
    startSensorMillis = currentMillis;
    collectSensorData(sensors);
    if (!buttonPressed) {
      printSensorData(&lcd, &Serial, sensors);
    }
  }

  if (currentMillis - startButtonMillis >= BUTTON_INTERVAL) {
    processButtonEvent(currentMillis, &lcd);
  }

  if (currentMillis - startButtonMillis >= CLIENT_INTERVAL) {
    WiFiClient client = server.available();

    if (client) {
      digitalWrite(LED_PIN, HIGH);
      printSensorData( &client, &Serial, sensors ); 
      client.stop();
      digitalWrite(LED_PIN, LOW);
    }
  }


#ifdef DHT_USE_HUMIDITY
#endif
}
