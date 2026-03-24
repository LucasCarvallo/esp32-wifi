/*

Sketch uses 885764 bytes (67%) of program storage space. Maximum is 1310720 bytes.
Global variables use 45240 bytes (13%) of dynamic memory, leaving 282440 bytes for local variables. Maximum is 327680 bytes.

Sketch uses 885956 bytes (67%) of program storage space. Maximum is 1310720 bytes.
Global variables use 45240 bytes (13%) of dynamic memory, leaving 282440 bytes for local variables. Maximum is 327680 bytes.
*/

#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // WiFi.mode(WIFI_OFF);
  WiFi.disconnect();
  delay(2000);
}

void loop() {
  int n = WiFi.scanNetworks(false, true); // incluir redes ocultas
  for (int i = 0; i < n; ++i) {
    // int rssi = WiFi.RSSI(i);
    // if (rssi < -85) continue; // ignorar redes lejanas

    Serial.print("DATA:");
    Serial.print(WiFi.BSSIDstr(i));
    Serial.print(",");
    Serial.print(WiFi.RSSI(i));
    Serial.print(",");
    Serial.print(WiFi.channel(i));
    Serial.print(",");
    // Serial.println(WiFi.SSID(i));

    String ssid = WiFi.SSID(i);
    ssid = ssid.substring(0, 32); // maximo 32 caracteres
    if (ssid == "") ssid = "HIDDEN";
    Serial.println(ssid);
  }

  WiFi.scanDelete();
  delay(5000);
}
