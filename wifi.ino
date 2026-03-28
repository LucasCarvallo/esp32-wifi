/*
# SIN OLED:
Sketch uses 885956 bytes (67%) of program storage space. Maximum is 1310720 bytes.
Global variables use 45240 bytes (13%) of dynamic memory, leaving 282440 bytes for local variables. Maximum is 327680 bytes.
# CON OLED y menu de options:
Sketch uses 929328 bytes (70%) of program storage space. Maximum is 1310720 bytes.
Global variables use 46832 bytes (14%) of dynamic memory, leaving 280848 bytes for local variables. Maximum is 327680 bytes.
*/

#include "WiFi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BTN_NEXT 18
#define BTN_OK 19

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int option = 0;
bool ejecutando = false;
unsigned long lastPressNext = 0;
unsigned long lastPressOk = 0;

// Prototipos de funciones
void showMenu(int option);
void executeOption(int option);
void displayMessage(String message);

void setup() {
  // ########## lo principal
  Serial.begin(115200);

  // ########## wifi
  WiFi.mode(WIFI_STA);
  // WiFi.mode(WIFI_OFF); // revisar
  WiFi.disconnect();

  // ########## display
  // Inicializar I2C
  Wire.begin(21, 22); // SDA, SCL(SCK)

  // Inicializar pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error OLED");
    for(;;);
  }

  // display.clearDisplay(); // limpiar pantalla
  display.setTextSize(1); // tamaño de texto
  display.setTextColor(WHITE); // color de tecto
  // display.setCursor(10, 20); // posicion del cursor
  // display.println("OK"); // mensaje
  // display.display(); // mostrar
  showMenu(option);

  // Configurar botones con INPUT_PULLUP: Presionado = LOW, No presionado = HIGH
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);

  // ########## lo final
  delay(2000);
}

void loop() {
  // Cambiar option
  if (digitalRead(BTN_NEXT) == LOW && millis() - lastPressNext > 200) {
    option = !option; // alterna 0 y 1
    showMenu(option);
    lastPressNext = millis();
  }

  // Confirmar option
  if (digitalRead(BTN_OK) == LOW && millis() - lastPressOk > 200) {
    executeOption(option);
    lastPressOk = millis();
  }
}

void showMenu(int option=0) {
  display.clearDisplay(); // limpiar pantalla
  display.setCursor(0, 0); // resetear cursor al inicio
  if (option == 0) {
    display.println("> Scan APs");
    display.println("Scan monitor");
  }
  else {
    display.println("Scan APs");
    display.println("> Scan Monitor");
  }
  display.display(); // mostrar
}

void executeOption(int option) {
  displayMessage("Escaneando...");
  if (option == 0) { // Scan APs
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
      ssid = ssid.substring(0, 32); // maximo ssid 32 caracteres
      if (ssid == "") ssid = "HIDDEN";
      Serial.println(ssid);
    }
    WiFi.scanDelete();
    // delay(5000);
    showMenu(option);
  }
  else { // Test
    displayMessage("Falta configurar");
  }
}

void displayMessage(String message = "") {
  display.clearDisplay(); // limpiar pantalla
  display.setCursor(0, 0); // resetear cursor al inicio
  display.println(message);
  display.display(); // mostrar
}


