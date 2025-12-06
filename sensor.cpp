#include "sensor.h"


Sensor::Sensor(byte p,
               const char* n,
               const char* h,
               uint8_t t)
  : pin(p), name(n), header(h), type(t) {
  dht = nullptr;
  last_temperature = DHT_ERR;
  err = true;
  
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

void initSensors(HardwareSerial * serial, byte connected, std::vector<Sensor>& sensors ){

  for (byte i = 0; i < connected; i++) {
    sensor_t sensor;

    sensors[i].dht = new DHT_Unified(sensors[i].pin, sensors[i].type );

    sensors[i].dht->begin();

    sensors[i].dht->temperature().getSensor(&sensor);
    printSensorInfo(&sensor, DHT_SITTYPE1, sensors[i].name);

#ifdef DHT_USE_HUMIDITY
    sensors[i].dht->humidity().getSensor(&sensor);
    printSensorInfo(&sensor, DHT_SITTYPE2, sensors[i].name);
#endif
  }

};

void collectSensorData( std::vector<Sensor>& sensors ){
  size_t size = sensors.size();
  for (byte i = 0; i < size; i++) {
      sensors_event_t  event;     
      sensors[i].dht->temperature().getEvent(&event);

      if (isnan(event.temperature)) {
        sensors[i].last_temperature = DHT_ERR;
      } else {
        sensors[i].last_temperature = event.temperature;
      }
    }
};

void printSensorData( LiquidCrystal_I2C *lcd, HardwareSerial * serial, std::vector<Sensor>& sensors ){
  char buf[64];
  byte tlen = 3;
  int size = sensors.size();
  for (byte i = 0; i < size; i++) {

      lcd->setCursor(i * tlen, 1);

      if ( sensors[i].last_temperature == DHT_ERR ) {
        strcpy(buf, "error reading temperature for ");
        strcat(buf, sensors[i].name);
        serial->println(buf);
        sensors[i].last_temperature = DHT_ERR;
        lcd->print("Er");
      } else {
      
        lcd->print(int(sensors[i].last_temperature));
        strcpy(buf, DHT_SITYPE1_NAME);
        strcat(buf, " for ");
        strcat(buf, sensors[i].name);
        strcat(buf, ": ");
        serial->print(buf);
        serial->print(sensors[i].last_temperature);
        serial->println(DHT_SITMEASURE1);
      }
    }
};

void printSensorData( WiFiClient *client, HardwareSerial * serial, std::vector<Sensor>& sensors ){
    IPAddress ip = client->remoteIP();
    const char * metrics_path = "/metrics";
    String headers[]={
      "HTTP/1.1 200 OK",
      "Content-Type: text/plain",
      "Connection: close"
    };

    String metric_headers[]={
      "# HELP boiler_flow_temperature Temperatures of the flow in degree celsius.",
      "# TYPE boiler_flow_temperature gauge"
    };
    serial->print("new client request from: ");
    serial->println(ip.toString());

    String line = client->readStringUntil('\r');
    client->readStringUntil('\n');
    client->readStringUntil('\n');
    
    char buff[64];
    strcpy (buff,"GET ");
    strcat (buff,metrics_path);

    if (line.startsWith(buff)) {
      serial->println("serving request from /metrics ");
        
      for (byte i = 0; i <(sizeof(headers) / sizeof(headers[0])); i++) {
        client->println(headers[i]);
      }
      
      client->println();

      for (byte i = 0; i <(sizeof(metric_headers) / sizeof(metric_headers[0])); i++) {
        const char * line = metric_headers[i].c_str();
        serial->println(line);
        client->println(line);
      }
      
      int size = sensors.size();
      for (byte i = 0; i < size; i++) {
        if (sensors[i].last_temperature != DHT_ERR) {
          client->print("boiler_flow_temperature{sensor=\"");
          client->print(sensors[i].name);
          client->print("\"} ");
          client->println(sensors[i].last_temperature);
        }
      }
      
    }


};
