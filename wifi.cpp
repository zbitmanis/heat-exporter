// example of arrudino wifi
// https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples/#connect-with-wpa

#include "wifi.h"

int status = WL_IDLE_STATUS;          // the WiFi radio's status

bool getWifiStatus(){
  return WiFi.RSSI() != 0 && WiFi.localIP();
}

bool getWifiStatus(HardwareSerial * serial){
  serial->print("wi status: ");
  serial->println(WiFi.status());
  return getWifiStatus();
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");

  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printWifiData(LiquidCrystal_I2C *lcd, byte col) {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  lcd->setCursor(0, 0);
  lcd->print("WF:");
  lcd->setCursor(3, 0);
  lcd->print(WiFi.SSID());
  lcd->setCursor(col -3, 0);
  
  lcd->print(WiFi.RSSI());  
  lcd->setCursor(0, 1);
  lcd->print("IP:");
  lcd->setCursor(3, 1);
  lcd->print(ip);

  // print your MAC address:
  //byte mac[6];
  //WiFi.macAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}

void connectToWifi(int *status, const char *ssid, const char *pass, byte ledPin) {
  // blink led to identify status connecting to Wifi
  for (int i = 0; i<=3;i++){
    digitalWrite(ledPin,HIGH);
    delay(100);
    digitalWrite(ledPin,LOW);
    delay(100);
  }
  
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
   } else {

      String fv = WiFi.firmwareVersion();
      if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
      }

      while (*status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network:
        *status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        digitalWrite(ledPin,HIGH);
        delay(5000);

        digitalWrite(ledPin,LOW);
        // if WiFi is disconnected blink led before the next loop
        if(!getWifiStatus()){ 
          delay(500);
        }
      }

      Serial.print("You're connected to the network");
      printCurrentNet();
      printWifiData();

      byte mac[6];
      WiFi.macAddress(mac);

      Serial.print("MAC address: ");
      printMacAddress(mac);
   }
}
