#include <ESP8266WiFi.h>

#include "src/GardenPLCAPIServer/GardenPLCAPIServer.h"

#include "WiFiConfig.h"
/* WiFiConfig.h must contain the credentials and configuration of the WiFi connection in the following format:
    #define WIFI_SSID     "WIFI CONNECTION NAME"
    #define WIFI_PASSWORD "WIFI PASSWORD"

    #define WIFI_IP_ADDRESS 192, 168, 1, 100
    #define WIFI_IP_GATEWAY 192, 168, 1, 1
    #define WIFI_IP_SUBNET  255, 255, 255, 0
*/
GardenPLCAPIServer APIServer;


void setup() {
    // Set up the WiFi Connection
    IPAddress local_IP(WIFI_IP_ADDRESS);
    IPAddress gateway(WIFI_IP_GATEWAY);
    IPAddress subnet(WIFI_IP_SUBNET);
    WiFi.config(local_IP, gateway, subnet);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) delay(100);

    APIServer.startServer();
}

void loop(void){
    APIServer.handleClient();
}
