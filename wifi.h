// example of arrudino wifi 
#include <WiFiS3.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#ifndef WIFI_H
#define WIFI_H


extern int status;     // the WiFi radio's status



void printWifiData();
void printWifiData(LiquidCrystal_I2C *lcd, byte col);
void printCurrentNet();
void printMacAddress(byte mac[]);
void connectToWifi(int *status, const char * ssid, const char * pass, byte ledPin);
bool getWifiStatus(HardwareSerial * serial);
bool getWifiStatus();

#endif
