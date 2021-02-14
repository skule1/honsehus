/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)

   EEPROM:  0: length ssid
          1-19: ssid
          20:length pswd
          21-39: pswd
          40:fart
          42:timeout

*/

//#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
//#include "EEPROM.h"
#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const char *ssid_w = "hane";
const char *password_w = "12345678";
int addr = 0;
//#define EEPROM_SIZE 64

//WiFiServer server(80);


void setup_wifi1() {
  // Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  WiFi.softAP(ssid_w, password_w);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("WiFi adresse: ");
  Serial.println(myIP);
  display.println("WiFi adresse:");
  display.println(myIP);
  display.display();
  delay(1000);
  server.begin();
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }

  Serial.println(" bytes read from Flash . Values are:");
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(char(EEPROM.read(i))); //Serial.print(" ");
  }
  Serial.println();
  Serial.println("writing random n. in memory");

  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.

  server.begin();

  Serial.println("Server started");
  Serial.println("SSID: " + String(ssid_w));
  Serial.println("Pswd: " + String(password_w));
  display.println("Server started");
  display.println("SSID: " + String(ssid_w));
  display.println("Pswd: " + String(password_w));
  display.display();

}

int find_text(String needle, String haystack) {
  int foundpos = -1;
  for (int i = 0; i <= haystack.length() - needle.length(); i++) {
    if (haystack.substring(i, needle.length() + i) == needle) {
      foundpos = i;
    }
  }
  return foundpos;
}
boolean gg = true;
void loop_wifi() {

  if (gg) {
    Serial.println("loop_wifi");
    gg = false;
  };
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //     Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {

            String htmlstr = "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'><html xmlns='http://www.w3.org/1999/xhtml'><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' /><title>Untitled Document</title></head><body style='background-color: #0000000;' ><form  align='center' action='a.html' style='background-color: #0000000; width: 400px; padding-top: 20px; padding-bottom: 50px' method='get'> <h2 align='center'><table width='200' cellpadding='5' border='0' align='center' style='background-color: #097479;'><tr><td colspan='2'; align='center'>Innstillinger port<br> til honsegard</td></tr><tr><td><font color='white'>SSID:</font></td>><td width='260'><input type='text' name='ssid' id='ssid' /></tr><td><font color='white'>PSWD:</font></td><td><input type='text' name='passwd' id='passwd' /></td></tr><tr><td colspan='2'><p align='center'><input type='submit' style='height: 40px; width: 150px; font-size: 20px' name='submit' id='submit' value=Oppdater></p></td></tr></table></h2> <p align='center'></p></form><p align='center'>&nbsp;</p><p>&nbsp;</p></body></html>";
            /*

              "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>";
              +"<html xmlns='http://www.w3.org/1999/xhtml'><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' /><title>Untitled Document</title></head>";
              +"<body style='background-color: #0000000;' ><p align='center'>&nbsp;</p>";
              +"<form  action='a.html' style='background-color: #0000000; width: 300px; padding-top: 50px; padding-bottom: 50px' method='get'>";
              +"<h2 align='center'><table width='200' cellpadding='5' border='0' align='center' style='background-color: #097479;'><tr>";
              +"<td colspan='2' align='center'>Innstillinger port<br> til hønsegard</td></tr><tr><td><font color='white'>SSID:</font></td>>";
              +"<td width='260'><input type='text' name='ssid' id='ssid' /></tr><td><font color='white'>PSWD:</font></td>";
              +"<td><input type='text' name='passwd' id='passwd' /></td></tr><tr>";
              +"<td colspan='2'><p align='center'><input type='submit' name='submit' id='submit' value=Oppdater></p></td></tr></table></h2> <p align='center'></p></form><p align='center'>&nbsp;</p><p>&nbsp;</p></body></html>";

              /*     String htmlstr = "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'><html xmlns='http://www.w3.org/1999/xhtml'><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' /><title>Untitled Document</title></head>";
              htmlstr = htmlstr
                     + "<body style='background-color: #0000000;' ><p align='center'>&nbsp;</p><form  action='a.html' style='background-color: #0000000; width: 300px; padding-top: 50px; padding-bottom: 50px' method='get'> <h2 align='center'> ";
                     + "<table width='200' cellpadding='5' border='0' align='center' style='background-color: #097479;'><tr><td colspan='2'>Innstillinger port til hønsegard</td>";
                     + "</tr><tr><td><font color='white'>SSID</font></td>><td width='260'><input type='text' name='ssid' id='ssid' /><td><font color='white'>PSWD:</font></td><td><input type='text' name='passwd' id='passwd' /></td></tr> <tr><td colspan='2'>";
                     + "<p align='center'><input type='submit' name='submit' id='submit' value=Oppdater'></p></td></tr></table></h2> <p align='center'></p></form><p align='center'>&nbsp;</p><p>&nbsp;</p></body></html>";

            */client.println(htmlstr);
            client.println();
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        };

        // lese nytt ssid og pswd`:
        //   Serial.println();
        if (currentLine.endsWith("Oppdater"))
        {
          int i = currentLine.indexOf('=');
          int j = currentLine.indexOf('&');
          int k = currentLine.indexOf('=', i + 1);
          int l = currentLine.indexOf('&', j + 1);
          String ssid1 = currentLine.substring(i + 1, j);
          String pswd1 = currentLine.substring(k + 1, l);
          Serial.println("ny SSID: " + ssid1);
          Serial.println("nytt pswd: " + pswd1);
          display.println("Ny SSID: " + ssid1);
          display.println("Nytt pswd: " + pswd1);
          display.display();
          EEPROM.writeString(2, ssid1);
          EEPROM.writeString(21, pswd1);
          EEPROM.writeByte(1, ssid1.length());
          EEPROM.writeByte(20, pswd1.length());
          Serial.println("Length: " + String(ssid1.length()));
          EEPROM.commit();
          Serial.println("les eeprom");
          ssid1 = "";

          ssid1 = EEPROM.readString(2);
          Serial.println("EEPROM ssid: " + ssid1);
          ssid1 = ssid1.substring(0, EEPROM.readByte(1));
          Serial.println("EEPROM ssid: " + ssid1);
          pswd1 = EEPROM.readString(21);
          Serial.println("EEPROM pswd1: " + pswd1);
          pswd1 = pswd1.substring(0, EEPROM.readByte(20));
          Serial.println("EEPROM pswd1: " + pswd1);
          Serial.print("EEPROM #pswd1: " );
          Serial.println(EEPROM.readByte(20));
          EEPROM.write(0, 0xff);
          EEPROM.commit();
          Serial.println(EEPROM.read(0));
          delay(5000);
          ESP.restart();
        }


      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
