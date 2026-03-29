/*
  Proyecto: WiFi Scanner para ESP32
  Descripción: Escanea redes WiFi cercanas y muestra información relevante.
  Autor: Lucas
  Fecha: Junio 2024

  Funcionalidades:
  - Escaneo de redes WiFi (SSID, BSSID, RSSI, canal)
  - Interfaz de usuario con pantalla OLED y botones
  - Posibilidad de agregar modo monitor (en desarrollo)

  Notas:
  - El botón "Next" alterna entre opciones del menú.
  - El botón "OK" confirma la selección y ejecuta la acción.

Particion por defecto: "Default 4MB with SPIFFS (1.2MB APP/1.5MB SPIFFS)"
Particion para mas espacio para APP: "Minimal SPIFFS (más espacio para programa)"

### PYTHON = 1
Sketch uses 929288 bytes (70%) of program storage space. Maximum is 1310720 bytes.
Global variables use 46840 bytes (14%) of dynamic memory, leaving 280840 bytes for local variables. Maximum is 327680 bytes.

### PYTHON = 0
Sketch uses 928952 bytes (70%) of program storage space. Maximum is 1310720 bytes.
Global variables use 46840 bytes (14%) of dynamic memory, leaving 280840 bytes for local variables. Maximum is 327680 bytes.
*/

#include "WiFi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BTN_NEXT 18
#define BTN_OK 19
#define PYTHON 0

int option = 0;
bool ejecutando = false;
unsigned long lastPressNext = 0;
unsigned long lastPressOk = 0;
bool scanning = false;

// Función para guardar APs encontrados (opcional pero útil)
// struct WiFiNetwork {
//     String ssid;
//     int rssi;
//     int channel;
// };
// WiFiNetwork networks[50]; // Guardar hasta 50 redes
// int networkCount = 0;

// Prototipos de funciones
void showMenu(int option);
void executeOption(int option);

// ##################################################################
void setup() {
  // ########## lo principal
  Serial.begin(115200);

  // ########## wifi
  WiFi.mode(WIFI_STA);
  // WiFi.mode(WIFI_OFF); // revisar
  WiFi.disconnect();

  // ########## Configurar botones con INPUT_PULLUP: Presionado = LOW, No presionado = HIGH
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);

  // ########## display
  // Inicializar Display Oled I2C
  Wire.begin(21, 22); // puertos SDA, SCL(SCK)

  // Inicializar pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error OLED");
    for(;;);
  }

  // display.clearDisplay(); // limpiar pantalla
  display.setTextSize(1); // tamaño de texto
  display.setTextColor(WHITE); // color de texto
  // display.setCursor(10, 20); // posicion del cursor
  // display.println("OK"); // mensaje
  // display.display(); // mostrar

  // ########## lo final
  showMenu(option);
  // delay(2000);
}

void loop() {
  if (!ejecutando) {
    // Cambiar option
    if (digitalRead(BTN_NEXT) == LOW && millis() - lastPressNext > 200) {
      option = !option;
      showMenu(option);
      lastPressNext = millis();
    }

    // Confirmar option
    if (digitalRead(BTN_OK) == LOW && millis() - lastPressOk > 200) {
      executeOption(option);
      lastPressOk = millis();
    }
  }
  else {
    // Salir de la ejecucion actual presionando OK nuevamente
    if (digitalRead(BTN_OK) == LOW && millis() - lastPressOk > 200) {
      ejecutando = false;
      showMenu(option);
      lastPressOk = millis();
      return;
    }

    if (ejecutando && option == 0 && scanning) {
      int n = WiFi.scanComplete();
      if (n >= 0) {
        display.clearDisplay();
        display.setCursor(0, 0);
        for (int i = 0; i < n; ++i) {
          // int rssi = WiFi.RSSI(i);
          // if (rssi < -85) continue; // ignorar redes lejanas

          String ssid = WiFi.SSID(i);
          if (ssid == "") ssid = "HIDDEN";

          #if PYTHON
            Serial.print("DATA:");
            Serial.print(WiFi.BSSIDstr(i));
            Serial.print(",");
            Serial.print(WiFi.RSSI(i));
            Serial.print(",");
            Serial.print(WiFi.channel(i));
            Serial.print(",");
            Serial.println(ssid);
          #endif

          if (ssid.length() > 22) ssid.remove(22); // maximo ssid X caracteres (en el display entran 21-22 (Claro-Fibra-2.4G-3523)
          display.println(ssid);
        }
        display.display();
        WiFi.scanDelete();
        WiFi.scanNetworks(true, true);
      }
    }
  }
}

void showMenu(int option=0) {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (option == 0) {
    display.println("> Scan APs");
    display.println("Scan monitor");
  }
  else {
    display.println("Scan APs");
    display.println("> Scan Monitor");
  }
  display.display();
}

void executeOption(int option) {
  ejecutando = true;

  display.clearDisplay();
  display.setCursor(0, 0);

  if (option == 0) { // Scan APs
    WiFi.scanNetworks(true, true); // primer parametro: async, segundo parametro: redes ocultas
    scanning = true;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Escaneando...");
    display.display();
  }
  else {
    display.println("Falta programar...");
  }
  display.display();
}
