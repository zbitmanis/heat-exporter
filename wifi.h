// example of arrudino wifi 
#include <WiFiS3.h>

#ifndef WIFI_H
#define WIFI_H

#define WIFI_SERVER_PORT  80

extern int status;     // the WiFi radio's status
extern WiFiServer server;           //WiFiServer



void printWifiData();
void printCurrentNet();
void printMacAddress(byte mac[]);
void connectToWifi(int *status, const char * ssid, const char * pass);

#endif
