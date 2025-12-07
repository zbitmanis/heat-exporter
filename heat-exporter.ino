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



byte buttonPressed = false;
unsigned long startSensorMillis = 0;
unsigned long startButtonMillis = 0;
unsigned long startClientMillis = 0;
unsigned long currentMillis = 0;


bool processButtonEvent(unsigned long currentMillis, LiquidCrystal_I2C *lcd) {

  static uint8_t lastButtonState;
  static unsigned int buttonPressDuration;
  static unsigned long buttonPressMillis;
  bool buttonPressed = false;

  uint8_t buttonState = digitalRead(BUTTON_PIN);

  if (DEBUG_HEATER) {
    Serial.print("button state ");
    Serial.println(buttonState);
  }

  if (buttonState == LOW && lastButtonState == HIGH) {
    lastButtonState = buttonState;
    buttonPressed = 1;
    buttonPressMillis = currentMillis;
    Serial.print("the button is pressed after ");
    Serial.println(buttonPressDuration);
    
    buttonPressDuration = 0;

    digitalWrite(LED_PIN, HIGH);

    clearLCD(lcd, LCD_LINES, LCD_COLUMNS);
    printWifiData(lcd, LCD_COLUMNS);

  } else if (buttonState == HIGH && lastButtonState == LOW) {
    lastButtonState = buttonState;
    buttonPressed = 0;

    digitalWrite(LED_PIN, LOW);
    clearLCD(lcd, LCD_LINES, LCD_COLUMNS);

    bool ws = getWifiStatus(&Serial);

    setupLCDOutput(lcd, DHT_CONECTED, sensors, DHT_SITTYPE1, false, 3, true, ws);
    printSensorData(lcd, &Serial, sensors);

    Serial.print("the button is released after ");
    Serial.println(buttonPressDuration);
    buttonPressDuration = 0;

  } else {
    buttonPressDuration = currentMillis - buttonPressMillis;
    buttonPressed = (buttonState == LOW);    
  }

  return buttonPressed;
}

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
WiFiServer server(WIFI_SERVER_PORT);  //WiFi WebServer

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect.
  }

  sensors.push_back(Sensor(DHTPIN1, "boiler_out", "bo", DHTTYPE));
  sensors.push_back(Sensor(DHTPIN2, "floor_in", "fi", DHTTYPE));
  sensors.push_back(Sensor(DHTPIN3, "floor_out", "fo", DHTTYPE));
  sensors.push_back(Sensor(DHTPIN4, "radiators_in", "ri", DHT11));
  sensors.push_back(Sensor(DHTPIN5, "radiators_out", "ro", DHT11));

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  //iniitialize lcd
  lcd.init();

  char ssid[] = SECRET_SSID;
  char pass[] = SECRET_PASS;

  connectToWifi(&status, ssid, pass, LED_PIN);
  server.begin();

  setupLCDOutput(&lcd, DHT_CONECTED, sensors, DHT_SITTYPE1, true);

  // initilaize sensors
  initSensors(&Serial, DHT_CONECTED, sensors);
}



void loop() {

  currentMillis = millis();

  if (currentMillis - startSensorMillis >= DHT_INTERVAL) {
    startSensorMillis = currentMillis;
    collectSensorData(sensors);
    if (!buttonPressed) {
      printSensorData(&lcd, &Serial, sensors);
    }
  }

  if (currentMillis - startButtonMillis >= BUTTON_INTERVAL) {
    buttonPressed = processButtonEvent(currentMillis, &lcd);
    startButtonMillis = currentMillis;
  }

  if (currentMillis - startClientMillis >= CLIENT_INTERVAL) {
    startClientMillis = currentMillis;
    WiFiClient client = server.available();

    if (client) {
      digitalWrite(LED_PIN, HIGH);
      printSensorData(&client, &Serial, sensors);
      client.stop();
      digitalWrite(LED_PIN, LOW);
    }
  }


#ifdef DHT_USE_HUMIDITY
#endif
}
