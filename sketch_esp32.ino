#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>

const char* ssid = "Lukes iPhone 12";                                     //ssid und Passwort des Wlans oder Hotspots, mit dem sich der ESP verbindet
const char* password = "1234567a";

float v;
float a;
float p;

AsyncWebServer server(80);                                                //erstellt Website Objekt auf Port 80

String readVoltage() {                                                    //Funktion, die Spannung ausliest und zurücksendet
  v = analogRead(34) * 3.3 / 4096;                                        //in Spannung umrechnen
  return String(v);
  }


String readamp() {                                                        //Funktion, die Stromstärke ausrechnet und zurücksendet
  a = v / 10;                                                             //fester widerstand von 10 Ohm; ausrechnen der Stromstärke mit der URI-Formel
  return String(a*1000);                                                  //mal 1000 für mA
  }


String readpower() {                                                      //Funktion, die Leistung ausrechnet und zurücksendet 
  p = v * a;                                                              //ausrechnen der Leistung mit der PUI-Formel
  return String(p*1000);                                                  //mal 1000 für mW
  }

void setup(){
  pinMode(34, INPUT);                                                     //Pin die die anliegende Spannung ausrechnet
  Serial.begin(115200);                                                   //debugging zwecke und zum IP anzeigen lassen

  if(!SPIFFS.begin()){                                                    //SPIFFS initialisieren
    Serial.println("Fehler mit SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);                                             //Verbindung zum Wlan herstellen mit oben gespeicherten Informationen
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbindet mit dem Wlan...");
  }

  Serial.println(WiFi.localIP());                                         //IP Adresse am seriellen Monitor ausdrucken

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){            //Route für root / website
    request->send(SPIFFS, "/index.html");
  });
  server.on("/voltage", HTTP_GET, [](AsyncWebServerRequest *request){     //Informationen an die Website schicken (Häufigkeit der Anfrage wird im html-Code festgelegt)
    request->send_P(200, "text/plain", readVoltage().c_str());
  });
  server.on("/amp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readamp().c_str());
  });
  server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readpower().c_str());
  });

  server.begin();                                                         //Server starten
}
 
void loop(){
  
}
