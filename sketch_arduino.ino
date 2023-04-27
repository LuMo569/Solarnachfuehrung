//   _____       _                             _      __ _   _ _                            
//  / ____|     | |                           | |    / _(_) (_) |                           
// | (___   ___ | | __ _ _ __ _ __   __ _  ___| |__ | |_ _   _| |__  _ __ _   _ _ __   __ _ 
//  \___ \ / _ \| |/ _` | '__| '_ \ / _` |/ __| '_ \|  _| | | | '_ \| '__| | | | '_ \ / _` |
//  ____) | (_) | | (_| | |  | | | | (_| | (__| | | | | | |_| | | | | |  | |_| | | | | (_| |
// |_____/ \___/|_|\__,_|_|  |_| |_|\__,_|\___|_| |_|_|  \__,_|_| |_|_|   \__,_|_| |_|\__, |
//                                                                                     __/ |
//                                                                                    |___/
// - von Mathis, Enfa und Luke
// - Code von Luke
// - Version 1.4.2
//
// Anordnung der LDRs:
//      
//  ldr1 | ldr2
//  ------------
//  ldr3 | ldr4
//

#include <Servo.h>
#include <Stepper.h>

int SPU = 2048;                                   //SPU = Steps per Revolution
Stepper motor(SPU, 4, 6, 5, 7);                   //Einbinden des Schrittmotors

Servo myservo;                                    //Einbinden des Servos
int servopos = 30;                                //Variable, die den Drehwinkel des Servos speichert (0° - 180°)

int ldr1;                                         //Hier werden die Ausgelsenen Werte der LDRs gespeichert
int ldr2;
int ldr3;
int ldr4;

int unten;                                        //Mittelwerte von oben, unten, rechts, und links werden hier gespeichert
int oben;                                         //also z. B. unten ist der Mittelwert aus ldr3 und ldr4
int links;
int rechts;

int difhor;                                       //Die Differenz von oben und unten bzw. links und rechts wird in diesen beiden Variablen gespeichert
int difver;                                       //diese wird benötigt, um eine Hysterese zu bilden und somit das Zittern des Motors zu verhindern

int tol = 50;                                     //Toleranzwert für die Hysterese

void setup() {
  myservo.attach(9);                              //Servo an Pin 9                 
  motor.setSpeed(8);                              //Geschwindigkeit des Schrittmotors
  Serial.begin(9600);
}

void loop() {
  ldr1 = analogRead(A0);                          //Einlesen der Sensorwerte
  ldr2 = analogRead(A1);
  ldr3 = analogRead(A2);
  ldr4 = analogRead(A3);

  unten = (ldr3 + ldr4)/2;                        //Bilden der Mittelwerte
  oben = (ldr1 + ldr2)/2;
  links = (ldr1 + ldr3)/2;
  rechts = (ldr2 + ldr4)/2;

  difhor = links - rechts;                        //Bilden der Differenzen
  difver = oben - unten;
  
  if(tol < -1*difhor || tol < difhor){            //Diese Zeile stellt die Hysterese dar, positiv und negativ, da die Differenz positv oder (||) negativ sein kann
    if(links < rechts){                           //wenn der wert rechts größer ist (ergo rechts ist es heller), dann drehe nach rechts (Rotation am Schrittmotor) 
      motor.step(50);
      }else if(links > rechts){                   //andernfalls drehe nach links
        motor.step(-50);      
        }
    } 
  if(tol < -1*difver || tol < difver){            //s. o.
    if(oben < unten && servopos < 160){           //wenn der wert unten größer ist (ergo unten ist es heller) dann neige die Solarplatte nach unten (mit dem Servo) + nicht größer als 160
      servopos += 10;                             //servopos um 20 erhöhen       
      myservo.write(servopos);
      }else if(oben > unten && servopos > 30){    //andernfalls nach oben + nicht kleiner als 30
        servopos -= 10;                           //servopos um 20 veringern 
        myservo.write(servopos);      
        }
    } 
  delay(1000);                                    //Pause für eine Sekunde, dass die Motoren genügend Zeit zum Bewegen haben
}
