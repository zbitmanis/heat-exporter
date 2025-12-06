#ifndef SENSOR_H
#define SENSOR_H

#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT_U.h>
#include <vector>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <WiFiS3.h>

#define DHT_ERR -127
#define DHT_SITTYPE1 1
#define DHT_SITTYPE2 2

#define DHT_SITYPE1_NAME "temperature"
#define DHT_SITYPE2_NAME "humidity"
#define DHT_SITMEASURE1 "°C"
#define DHT_SITMEASURE2 "%"

#define DEBUG_HEATER 0



struct Sensor {
  float last_temperature;
#ifdef DHT_USE_HUMIDITY
  byte last_humidity;
#endif

  byte pin;
  const char* name;
  const char* header;
  bool err;
  uint8_t type;  

  DHT_Unified* dht;
  Sensor(byte p,
         const char* n,
         const char* h,
         uint8_t t);
};

void printSensorInfo(sensor_t* sensor, byte sitype, const char * name);
const char* getSensorName(byte pin);
void collectSensorData( std::vector<Sensor>& sensors);
void printSensorData( LiquidCrystal_I2C *lcd, HardwareSerial * serial, std::vector<Sensor>& sensors );
void printSensorData( WiFiClient *client, HardwareSerial *serial, std::vector<Sensor>& sensors );
void initSensors(HardwareSerial * serial, byte connected, std::vector<Sensor>& sensors );


extern std::vector<Sensor> sensors;

#endif
