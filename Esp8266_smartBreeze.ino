#include <DHT.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <pgmspace.h>
#include <ArduinoJson.h>

#define DHTPIN D2
#define DHTTYPE DHT11
#define IR_PIN D1
#define PIR_PIN D0

const char* ssid = "Redmi Note 9 Pro";
const char* password = "dj@426426";

DHT dht(DHTPIN, DHTTYPE);
IRsend irsend(IR_PIN);
ESP8266WebServer server(80);

enum OperationMode { AUTO, MANUAL };
OperationMode currentMode = AUTO;

// Keep all your original raw IR codes exactly as they were
const uint16_t rawOFF[] PROGMEM = { 4400, 4500, 450, 1700, 400, 650, 450, 1700, 450, 1700, 400, 700, 400, 700, 400, 1700, 450, 650, 450, 650, 450, 1700, 400, 650, 450, 700, 400, 1700, 450, 1700, 400, 700, 400, 1700, 450, 650, 450, 1700, 400, 1700, 450, 1700, 450, 1700, 400, 700, 400, 1700, 450, 1700, 400, 1700, 450, 650, 450, 650, 450, 650, 450, 650, 450, 1700, 400, 700, 400, 700, 400, 1700, 450, 1700, 400, 1700, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 400, 700, 400, 1700, 450, 1700, 450, 1700, 400, 1700, 450, 1700, 400, 5300, 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1700, 450, 650, 450, 650, 400, 1750, 400, 700, 400, 700, 400, 1750, 400, 650, 450, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 450, 650, 400, 1750, 400, 1700, 450, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1700, 450, 650, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 1700, 450, 650, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 1800, 350, 1750, 350, 1800, 350, 1750, 400 };
const uint16_t raw30C[] PROGMEM = { 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1750, 400, 700, 350, 700, 400, 1750, 400, 700, 400, 700, 400, 1750, 400, 700, 350, 700, 400, 1750, 400, 1750, 400, 700, 350, 1750, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 350, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 5250, 4450, 4500, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 350, 1750, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1750, 350, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 350, 1750, 400, 1750, 400, 700, 400, 700, 350, 750, 350, 750, 350, 750, 350, 1750, 400, 750, 300, 800, 300, 1800, 350, 1800, 350, 1750, 350, 1800, 350 };
const uint16_t raw29C[] PROGMEM = { 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1750, 400, 700, 350, 700, 400, 1750, 400, 700, 400, 700, 400, 1750, 350, 700, 400, 700, 400, 1750, 400, 1750, 400, 700, 350, 1750, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350, 1750, 400, 700, 400, 1750, 400, 700, 400, 700, 350, 750, 350, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 5250, 4450, 4500, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 350, 1800, 350, 750, 350, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 750, 350, 1750, 400, 1750, 350, 1750, 400, 1750, 400, 1750, 350 };
const uint16_t raw28C[] PROGMEM = { 4400, 4500, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1750, 350, 1750, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 5300, 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 350, 750, 350, 750, 350, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350, 1800, 350, 1750, 400, 1750, 400 };
const uint16_t raw27C[] PROGMEM = { 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1750, 400, 700, 350, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 700, 400, 1750, 350, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 5300, 4450, 4500, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1750, 350, 1750, 400, 1750, 400, 1700, 400, 750, 350, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 750, 350, 750, 350, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 1800, 350, 700, 350, 1800, 350, 1800, 300, 1800, 350, 1800, 350 };
const uint16_t raw26C[] PROGMEM = { 4450, 4450, 400, 1750, 400, 700, 400, 1750, 350, 1750, 400, 700, 400, 700, 400, 1750, 400, 700, 350, 700, 400, 1750, 400, 700, 400, 700, 400, 1750, 350, 1750, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350, 1750, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1750, 400, 700, 350, 750, 350, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 5300, 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1750, 400, 650, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 700, 400, 1750, 350, 700, 400, 700, 400, 700, 400, 750, 400, 700, 350, 750, 350, 1750, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 750, 350, 1750, 400, 1750, 400, 1700, 400, 1750, 400 };
const uint16_t raw25C[] PROGMEM = { 4450, 4450, 450, 1750, 400, 700, 350, 1750, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 350, 1750, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1750, 350, 1750, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 350, 750, 350, 1750, 400, 1750, 400, 700, 400, 700, 400, 700, 350, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 5300, 4400, 4500, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1750, 350, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1750, 350, 1750, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350, 750, 350, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350 };
const uint16_t raw24C[] PROGMEM = { 4450, 4500, 400, 1700, 450, 650, 450, 1700, 400, 1700, 450, 650, 450, 650, 450, 1700, 400, 650, 450, 650, 450, 1700, 450, 650, 450, 650, 450, 1700, 400, 1700, 450, 650, 450, 1700, 400, 1750, 400, 650, 450, 1700, 450, 1700, 400, 1700, 450, 1700, 400, 1700, 450, 1700, 400, 700, 400, 1750, 400, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 650, 450, 650, 450, 1700, 450, 650, 400, 1750, 400, 1700, 450, 1700, 400, 1700, 450, 1700, 400, 1750, 400, 5300, 4400, 4500, 450, 1700, 400, 700, 400, 1700, 450, 1700, 400, 700, 400, 700, 400, 1700, 450, 650, 450, 650, 450, 1700, 400, 700, 400, 700, 400, 1700, 450, 1700, 400, 700, 400, 1700, 450, 1700, 400, 700, 400, 1700, 450, 1700, 400, 1750, 400, 1700, 450, 1700, 400, 1700, 450, 650, 450, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 350, 750, 400, 700, 400, 700, 350, 700, 450, 650, 400, 1750, 400, 700, 400, 1750, 400, 1700, 400, 1750, 350, 1750, 400, 1750, 400, 1700, 450 };
const uint16_t raw23C[] PROGMEM = { 4450, 4450, 400, 1750, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 350, 1750, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 5300, 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1750, 350, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1750, 350, 750, 350, 1750, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 350, 1750, 400, 1750, 400, 700, 400, 1750, 400, 700, 400, 650, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 1750, 350, 750, 350, 1750, 400, 1750, 400, 1700, 400, 1750, 400 };
const uint16_t raw22C[] PROGMEM = { 4400, 4500, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 5300, 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 350, 1750, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 1750, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 750, 350, 750, 350, 700, 400, 1750, 400, 1750, 350, 1750, 400, 1750, 400 };
const uint16_t raw21C[] PROGMEM = { 4400, 4500, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1750, 350, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 700, 350, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 5300, 4450, 4450, 450, 1700, 400, 700, 400, 1750, 350, 1750, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1750, 350, 1750, 400, 1750, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 350, 750, 350, 750, 350, 750, 350, 1750, 400, 1750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 1750, 350, 750, 350, 750, 350, 1800, 350, 1800, 300, 1800, 350, 1800, 350, 1750, 350 };
const uint16_t raw20C[] PROGMEM = { 4450, 4450, 400, 1750, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1750, 350, 1750, 400, 1750, 400, 1700, 400, 5300, 4450, 4450, 450, 1700, 400, 700, 400, 1750, 400, 1750, 400, 700, 350, 700, 400, 1750, 400, 700, 400, 700, 400, 1750, 400, 650, 400, 700, 400, 1750, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 350, 1750, 400, 700, 400, 1750, 350, 750, 350, 750, 350, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 350, 750, 350, 1750, 400, 1750, 400, 700, 400, 1750, 350, 1750, 400, 1750, 350, 1800, 350, 1750, 400 };
const uint16_t raw19C[] PROGMEM = { 4450, 4450, 400, 1750, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 700, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 350, 1750, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 400, 5300, 4450, 4450, 400, 1750, 400, 700, 400, 1750, 400, 1750, 400, 700, 400, 700, 350, 1750, 400, 700, 400, 700, 400, 1750, 400, 700, 350, 700, 400, 1750, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 700, 400, 1700, 400, 1750, 400, 1700, 400, 1750, 400, 1750, 400, 1700, 400, 700, 400, 1750, 400, 700, 400, 700, 350, 750, 350, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1750, 400, 1700, 400, 700, 400, 700, 400, 1750, 400, 1750, 350, 1750, 400, 1750, 350 };
const uint16_t raw18C[] PROGMEM = { 4450, 4500, 400, 1700, 450, 650, 450, 1700, 400, 1700, 450, 650, 450, 650, 450, 1700, 400, 650, 450, 650, 450, 1700, 450, 650, 450, 650, 450, 1700, 400, 1700, 450, 650, 450, 1700, 400, 1750, 400, 650, 450, 1700, 450, 1700, 400, 1700, 450, 1700, 400, 1700, 450, 1700, 400, 700, 400, 1750, 400, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 650, 450, 1700, 400, 650, 450, 650, 450, 700, 400, 650, 450, 1700, 450, 1700, 400, 1700, 450, 650, 450, 1700, 400, 1700, 450, 1700, 400, 1700, 450, 5300, 4400, 4500, 450, 1700, 400, 700, 400, 1700, 450, 1700, 400, 650, 450, 700, 400, 1700, 450, 650, 450, 650, 450, 1700, 400, 700, 400, 700, 400, 1700, 450, 1700, 400, 700, 400, 1700, 450, 1700, 400, 700, 450, 1650, 450, 1700, 450, 1700, 400, 1700, 450, 1700, 450, 1650, 450, 650, 450, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 650, 450, 650, 450, 1700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 1700, 450, 1700, 400, 1750, 400, 700, 400, 1700, 450, 1700, 400, 1700, 450, 1700, 400 };
const uint16_t raw17C[] PROGMEM = { 4450, 4500, 450, 1700, 400, 700, 400, 1750, 400, 1750, 350, 750, 350, 750, 350, 1750, 400, 700, 400, 700, 400, 1750, 350, 750, 350, 750, 350, 1800, 350, 1800, 300, 750, 350, 1800, 350, 1800, 300, 800, 300, 1800, 350, 1800, 300, 1800, 350, 1800, 350, 1750, 350, 1800, 350, 750, 350, 1800, 300, 800, 300, 800, 300, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 750, 350, 700, 400, 1750, 350, 1800, 350, 1750, 350, 1800, 350, 1750, 350, 1800, 350, 1750, 400, 1750, 350, 5250, 4500, 4500, 400, 1750, 350, 750, 350, 1750, 350, 1800, 400, 700, 350, 750, 350, 1750, 400, 700, 400, 700, 400, 1750, 400, 700, 350, 750, 350, 1750, 400, 1750, 400, 650, 400, 1750, 400, 1750, 350, 700, 400, 1750, 400, 1750, 400, 1700, 400, 1750, 400, 1700, 450, 1700, 400, 700, 400, 1700, 450, 650, 450, 650, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 700, 400, 650, 450, 650, 400, 1750, 400, 1700, 450, 1700, 400, 1700, 450, 1700, 400, 1750, 400, 1700, 450, 1700, 450 };


const int rawOFF_Size = sizeof(rawOFF) / sizeof(rawOFF[0]);
const int raw30C_Size = sizeof(raw30C) / sizeof(raw30C[0]);
const int raw29C_Size = sizeof(raw29C) / sizeof(raw29C[0]);
const int raw28C_Size = sizeof(raw28C) / sizeof(raw28C[0]);
const int raw27C_Size = sizeof(raw27C) / sizeof(raw27C[0]);
const int raw26C_Size = sizeof(raw26C) / sizeof(raw26C[0]);
const int raw25C_Size = sizeof(raw25C) / sizeof(raw25C[0]);
const int raw24C_Size = sizeof(raw24C) / sizeof(raw24C[0]);
const int raw23C_Size = sizeof(raw23C) / sizeof(raw23C[0]);
const int raw22C_Size = sizeof(raw22C) / sizeof(raw22C[0]);
const int raw21C_Size = sizeof(raw21C) / sizeof(raw21C[0]);
const int raw20C_Size = sizeof(raw20C) / sizeof(raw20C[0]);
const int raw19C_Size = sizeof(raw19C) / sizeof(raw19C[0]);
const int raw18C_Size = sizeof(raw18C) / sizeof(raw18C[0]);
const int raw17C_Size = sizeof(raw17C) / sizeof(raw17C[0]);

unsigned long lastMotionTime = 0;
const unsigned long motionTimeout = 600000;
bool motionActive = false;
String lastSentCommand = "OFF";

void setup() {
  Serial.begin(9600);
  dht.begin();
  irsend.begin();
  pinMode(PIR_PIN, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/auto", HTTP_GET, handleAutoMode);
  server.on("/manual", HTTP_GET, handleManualMode);
  server.on("/settemp", HTTP_GET, handleSetTemp);
  server.on("/resend", HTTP_GET, handleResendCommand);
  server.on("/voice", HTTP_GET, handleVoiceCommand);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 2000) {  // Update every 2 seconds
    lastUpdate = millis();
    float currentTemp = dht.readTemperature();
    Serial.print("Current Temperature: ");
    Serial.println(currentTemp);
  }

  if (currentMode == AUTO) {
    handleAutoModeLogic();
  }
  delay(10);
}

void handleApiStatus() {
  StaticJsonDocument<200> doc;
  doc["mode"] = currentMode == AUTO ? "auto" : "manual";
  doc["temperature"] = dht.readTemperature();
  doc["ac_setting"] = lastSentCommand;
  doc["motion"] = digitalRead(PIR_PIN) == HIGH ? "detected" : "no_motion";
  doc["ip"] = WiFi.localIP().toString();

  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  server.send(200, "application/json", jsonResponse);
}

// Add this function with your other handler functions
void handleVoiceCommand() {
  if (server.hasArg("command")) {
    String command = server.arg("command");
    command.toLowerCase();
    
    if (command == "turn on" || command == "switch on" || command == "on") {
      if (lastSentCommand == "OFF") {
        sendIRCommand("24C"); // Default temperature when turning on
      }
    } 
    else if (command == "turn off" || command == "switch off" || command == "off") {
      sendIRCommand("OFF");
    }
    else if (command.startsWith("set to") || command.startsWith("change to")) {
      // Extract temperature from command
      int spacePos = command.lastIndexOf(' ');
      String tempStr = command.substring(spacePos + 1);
      
      // Remove any non-numeric characters (like degrees or C)
      // Replace replaceAll with multiple replace calls
      tempStr.replace("°", "");
      tempStr.replace("c", "");
      tempStr.replace(" ", "");
      
      if (tempStr.length() > 0) {
        int temp = tempStr.toInt();
        if (temp >= 17 && temp <= 30) {
          sendIRCommand(String(temp) + "C");
        }
      }
    }
    else if (command == "increase temperature" || command == "temperature up") {
      if (lastSentCommand.endsWith("C")) {
        int currentTemp = lastSentCommand.substring(0, lastSentCommand.length() - 1).toInt();
        if (currentTemp < 30) {
          sendIRCommand(String(currentTemp + 1) + "C");
        }
      }
    }
    else if (command == "decrease temperature" || command == "temperature down") {
      if (lastSentCommand.endsWith("C")) {
        int currentTemp = lastSentCommand.substring(0, lastSentCommand.length() - 1).toInt();
        if (currentTemp > 17) {
          sendIRCommand(String(currentTemp - 1) + "C");
        }
      }
    }
    
    server.send(200, "text/plain", "Command processed");
  } else {
    server.send(400, "text/plain", "No command provided");
  }
}

void handleAutoModeLogic() {
  int motionState = digitalRead(PIR_PIN);

  if (motionState == HIGH) {
    lastMotionTime = millis();
    if (!motionActive) {
      motionActive = true;
      Serial.println("Motion detected - system active");
    }
  }

  if (motionActive && (millis() - lastMotionTime > motionTimeout)) {
    motionActive = false;
    sendIRCommand("OFF");
    lastSentCommand = "OFF";
    Serial.println("No motion - AC turned OFF");
  }

  if (motionActive) {
    static unsigned long lastTempCheck = 0;
    if (millis() - lastTempCheck >= 10000) {
      lastTempCheck = millis();
      float temp = dht.readTemperature();
      Serial.print("Temperature: ");
      Serial.println(temp);

      String command;
      if (temp < 22) command = "OFF";
      else if (temp <= 26) command = "24C";
      else command = "18C";

      if (command != lastSentCommand) {
        sendIRCommand(command);
        lastSentCommand = command;
      }
    }
  }
}

void sendIRCommand(String command) {
  Serial.print("Sending: ");
  Serial.println(command);

  uint16_t buffer[200];

  if (command == "OFF") {
    memcpy_P(buffer, rawOFF, sizeof(rawOFF));
    irsend.sendRaw(buffer, sizeof(rawOFF) / sizeof(uint16_t), 38);
  } else if (command == "30C") {
    memcpy_P(buffer, raw30C, sizeof(raw30C));
    irsend.sendRaw(buffer, sizeof(raw30C) / sizeof(uint16_t), 38);
  } else if (command == "29C") {
    memcpy_P(buffer, raw29C, sizeof(raw29C));
    irsend.sendRaw(buffer, sizeof(raw29C) / sizeof(uint16_t), 38);
  } else if (command == "28C") {
    memcpy_P(buffer, raw28C, sizeof(raw28C));
    irsend.sendRaw(buffer, sizeof(raw28C) / sizeof(uint16_t), 38);
  } else if (command == "27C") {
    memcpy_P(buffer, raw27C, sizeof(raw27C));
    irsend.sendRaw(buffer, sizeof(raw27C) / sizeof(uint16_t), 38);
  } else if (command == "26C") {
    memcpy_P(buffer, raw26C, sizeof(raw26C));
    irsend.sendRaw(buffer, sizeof(raw26C) / sizeof(uint16_t), 38);
  } else if (command == "25C") {
    memcpy_P(buffer, raw25C, sizeof(raw25C));
    irsend.sendRaw(buffer, sizeof(raw25C) / sizeof(uint16_t), 38);
  } else if (command == "24C") {
    memcpy_P(buffer, raw24C, sizeof(raw24C));
    irsend.sendRaw(buffer, sizeof(raw24C) / sizeof(uint16_t), 38);
  } else if (command == "23C") {
    memcpy_P(buffer, raw23C, sizeof(raw23C));
    irsend.sendRaw(buffer, sizeof(raw23C) / sizeof(uint16_t), 38);
  } else if (command == "22C") {
    memcpy_P(buffer, raw22C, sizeof(raw22C));
    irsend.sendRaw(buffer, sizeof(raw22C) / sizeof(uint16_t), 38);
  } else if (command == "21C") {
    memcpy_P(buffer, raw21C, sizeof(raw21C));
    irsend.sendRaw(buffer, sizeof(raw21C) / sizeof(uint16_t), 38);
  } else if (command == "20C") {
    memcpy_P(buffer, raw20C, sizeof(raw20C));
    irsend.sendRaw(buffer, sizeof(raw20C) / sizeof(uint16_t), 38);
  } else if (command == "19C") {
    memcpy_P(buffer, raw19C, sizeof(raw19C));
    irsend.sendRaw(buffer, sizeof(raw19C) / sizeof(uint16_t), 38);
  } else if (command == "18C") {
    memcpy_P(buffer, raw18C, sizeof(raw18C));
    irsend.sendRaw(buffer, sizeof(raw18C) / sizeof(uint16_t), 38);
  } else if (command == "17C") {
    memcpy_P(buffer, raw17C, sizeof(raw17C));
    irsend.sendRaw(buffer, sizeof(raw17C) / sizeof(uint16_t), 38);
  }
  delay(100);
}



void handleAutoMode() {
  currentMode = AUTO;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleManualMode() {
  currentMode = MANUAL;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetTemp() {
  if (currentMode == MANUAL) {
    String temp = server.arg("temp");

    if (temp == "INC" || temp == "DEC") {
      if (lastSentCommand.endsWith("C")) {
        int currentTemp = lastSentCommand.substring(0, lastSentCommand.length() - 1).toInt();

        if (temp == "INC" && currentTemp < 30) {
          currentTemp++;
        } else if (temp == "DEC" && currentTemp > 17) {
          currentTemp--;
        }

        temp = String(currentTemp) + "C";
      } else if (lastSentCommand == "OFF") {
        temp = "24C";  // Default temperature when turning on from OFF
      }
    }

    sendIRCommand(temp);
    lastSentCommand = temp;
    server.sendHeader("Location", "/");
    server.send(303);
  } else {
    server.send(403, "text/plain", "Switch to manual mode first");
  }
}

void handleResendCommand() {
  if (lastSentCommand.length() > 0) {
    sendIRCommand(lastSentCommand);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleStatus() {
  if (server.header("Accept") == "application/json") {
    handleApiStatus();
    return;
  }

  String status = "Current Mode: " + String(currentMode == AUTO ? "AUTO" : "MANUAL") + "\n";
  status += "Room Temperature: " + String(dht.readTemperature()) + "°C\n";
  status += "AC Setting: " + (lastSentCommand == "OFF" ? "OFF" : lastSentCommand) + "\n";
  status += "Motion: " + String(digitalRead(PIR_PIN) == HIGH ? "Detected" : "No motion") + "\n";
  server.send(200, "text/plain", status);
}