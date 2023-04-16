#include <WiFi.h>                         

const char* ssid = "Lukes iPhone 12";                       //Name des Netzwerks
const char* password = "1234567a";                          //Passwort des Netzwerks

WiFiServer server(80);                                      //Setzt Web-Server Port auf 80

String header;                                              //Variable um HTTP request zu speichern

String power = "0";                                         //Variable um Leistung der Solarzelle zu speichern

const int Cell = 34;                                        //Variable die definiert an welchem Pin die Solarzelle angeschlossen ist

unsigned long currentTime = millis();                       //jetzige Zeit
unsigned long previousTime = 0;                             //vorherige Zeit
const long timeoutTime = 2000;                              //definiert timeout Zeit

void setup() {
  Serial.begin(115200);
  pinMode(Cell, INPUT);
  
  Serial.print("Verbindung zu ");                           //Verbindung zum Wlan herstellen
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.println("Verbindung hergestellt");
  
  Serial.println("IP Adresse: ");                           //IP-Adresse am serilellen Monitor anzeigen und den Server starten      
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();                   //nach einkommenden clients lauschen

  if (client) {                                             //wenn sich ein client verbindet...
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Neuer Client.");                        //Nachricht an seriallen Monitor 
    String currentLine = "";                                //Erschafft einen String der die Daten des neuen Clients enthält
    // loop während client verbunden ist
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {                             //wenn es Bytes vom Client zu lesen gibt,
        char c = client.read();                             //lese das Byte und dann
        Serial.write(c);                                    //druck es auf dem seriellen Monitor
        header += c;
        if (c == '\n') {                                    //wenn das Byte ein Zeilenumbruchszeichen ist
          //wenn die aktuelle Zeile leer ist, haben Sie zwei Zeilenumbrüche hintereinander.
          //Das ist das Ende der HTTP-Anfrage des Clients, also senden Sie eine Antwort:
          if (currentLine.length() == 0) {
            //HTTP-Header beginnen immer mit einem Antwortcode (z. B. HTTP/1.1 200 OK)
            //und einen Inhaltstyp, damit der Client weiß, was auf ihn zukommt, und dann eine Leerzeile:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            power = String(analogRead(Cell) * 3.3 / 4096);  //liest Wert und konvertiert zu Spannung
            
            //dieser code zeit die Leistung der solarzelle auf dem seriellen Moitor an 
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>" + power + " Volt" + "</p>");
            client.println("</body></html>");
            
            //Die HTTP-Antwort endet mit einer weiteren Leerzeile
            client.println();
            //break aus der while Schleife
            break;
          } else {                                          //wenn man einen zeilenumbruch erhalten hat, so lösche currentLine
            currentLine = "";
          }
        } else if (c != '\r') {                             // wenn du etwas anderes als ein Wagenrücklaufzeichen haben,
          currentLine += c;                                 // hänge es an das Ende der currentLine
        }
      }
    }

    header = "";                                            //Inhalt der header Variable löschen
    client.stop();                                          //Verbindung schließen
    Serial.println("Client getrennt.");
    Serial.println("");
  }
}
