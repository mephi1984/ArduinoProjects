#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

void setup() {
  ESP.wdtEnable(10000); 
  Serial.begin(115200); //Baudrate
  Serial.println("ESP starts");
  WiFi.begin("Panfilova_85","70544500z");
  Serial.print("Connecting...");
  while(WiFi.status()!=WL_CONNECTED){ //Loop which makes a point every 500ms until the connection process has finished
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP-Address: ");
  Serial.println(WiFi.localIP()); //Displaying the IP Address

  server.onNotFound([](){ 
    server.send(404, "text/plain", "Link was not found!");  
  });
 
  server.on("/", []() {
    server.send(200, "text/plain", "Landing page!");
  });
 
  server.on("/custom", []() {
    server.send(200, "text/plain", "Just a custom route!");
    ownFunction();
  });
  server.begin();
}
void loop() {
   //ESP.wdtDisable(); 
   ESP.wdtFeed();
  server.handleClient();
  //ESP.wdtEnable(10000);
}
void ownFunction(){ //go to "IP-Adress/custom" to call this function
  Serial.println("Own function was called");
}