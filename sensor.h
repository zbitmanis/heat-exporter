#ifndef SENSOR_H
#define SENSOR_H

#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT_U.h>
#include <vector>

#define DHT_ERR -127
#define DHT_SITTYPE1 1
#define DHT_SITTYPE2 2

#define DHT_SITYPE1_NAME "temperature"
#define DHT_SITYPE2_NAME "humidity"
#define DHT_SITMEASURE1 "°C"
#define DHT_SITMEASURE2 "%"

#define DEBUG_HEATER 1

void printSensorInfo(sensor_t* sensor, byte sitype, const char * name);
const char* getSensorName(byte pin);

struct Sensor {
  byte last_temperature;
#ifdef DHT_USE_HUMIDITY
  byte last_humidity;
#endif

  byte pin;
  const char* name;
  const char* header;

  DHT_Unified* dht;
  Sensor(byte p,
         const char* n,
         const char* h);
};

extern std::vector<Sensor> sensors;

#endif
