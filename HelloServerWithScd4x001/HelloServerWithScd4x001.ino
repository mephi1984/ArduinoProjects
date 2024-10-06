#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Arduino.h>
#include <SensirionI2CScd4x.h>
#include <Wire.h>

SensirionI2CScd4x scd4x;

ESP8266WebServer server(80);

uint16_t global_co2 = 0;


void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}

IRAM_ATTR void SendData()
{
    char cstr[16];
    sprintf(cstr, "%d", global_co2);
    server.send(200, "text/plain", cstr);
}

void setupSensirion()
{
Wire.begin();

    uint16_t error;
    char errorMessage[256];

    scd4x.begin(Wire);

    // stop potentially previously started measurement
    error = scd4x.stopPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        printSerialNumber(serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    Serial.println("Waiting for first measurement... (5 sec)");
}


void setup() {
  ESP.wdtEnable(65000); 
      Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    setupSensirion();
  Serial.println("ESP starts");

  WiFi.mode(WIFI_STA);
  //WiFi.begin("Panfilova_85","70544500z");

  WiFi.begin("Galaxy A12 Vlad","london3231");
  Serial.print("Connecting...");
  while(WiFi.status()!=WL_CONNECTED){ //Loop which makes a point every 500ms until the connection process has finished
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP-Address: ");
  Serial.println(WiFi.localIP()); //Displaying the IP Address

  server.keepAlive(false);

  server.onNotFound([](){ 
    server.send(404, "text/plain", "Link was not found!");  
  });
 
  server.on("/", SendData);

  server.begin();
}

void sensirionLoop() {
    uint16_t error;
    char errorMessage[256];

    delay(100);

    // Read Measurement
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool isDataReady = false;
    error = scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return;
    }
    if (!isDataReady) {
        return;
    }
    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else if (co2 == 0) {
        Serial.println("Invalid sample detected, skipping.");
    } else {
        Serial.print("Co2:");
        Serial.print(co2);
        Serial.print("\t");
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("\t");
        Serial.print("Humidity:");
        Serial.println(humidity);
    }

    global_co2 = co2;
}

IRAM_ATTR void loop() {
   //ESP.wdtDisable(); 
   ESP.wdtFeed();
  server.handleClient();
ESP.wdtFeed();
  sensirionLoop();
  //ESP.wdtEnable(10000);
}
