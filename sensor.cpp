#include "sensor.h"


Sensor::Sensor(byte p,
               const char* n,
               const char* h)
  : pin(p), name(n), header(h) {
  dht = nullptr;
  last_temperature = DHT_ERR;
#ifdef DHT_USE_HUMIDITY
  last_humidity = DHT_ERR;
#endif
};

//Sensor * sensors [] ;
std::vector<Sensor> sensors;

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

};

const char* getSensorName(byte pin) {
  // find sensor name from
  char sname [24];
  bool found = false;


  for (byte i = 0 ; i < sensors.size() && !found; i++) {
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
};