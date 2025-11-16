
#define DHT_ERR -127

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
         const char* h
        ): pin(p), name(n), header(h) {
    dht = nullptr;
    last_temperature = DHT_ERR;
#ifdef DHT_USE_HUMIDITY
    last_humidity = DHT_ERR;
#endif
  };
};

