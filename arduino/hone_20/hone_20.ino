/* Port 1165  posessor: Wemos D1 mini esp32
  alternativ: kobler seg til programmodule i Delphi. Da m� ip-adresse i klient peke mot ip-adresse for maskin som kj�rer programmet

  EEPROM:  0:
        1:SSID
        20:pswd
        40:fart
        42:timeout
        46:maxtime (tid lukk)
        50: tid open
        54: bryter åpen
        55: bryter lukket
        60:SSID2
        80:pswd2
        100:SSID3
        120:pswd3
        140:ssid4
        160:pswd4
        180:sted1


  Xperia L1_c8b8
  8c338a9be106

  OTA port: esp32
*/

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "EEPROM.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include "apne.h"

//char* ssid = "COMFAST";
const char* password = "12345abc";
//char* mqtt_server = "skule.sormo.no:1980";
char* mqtt_server = "192.168.10.17";

#define tries 4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64// OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String status_var = "";
//#include <SeeedOLED.h>
//#include <pgmspace.h>

#include <SPI.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define motorA 25
#define motorB 27
#define motor_enable 32
#define bryter_lukket 26
#define bryter_apen  18
//#define bryter_stopp  34
#define knapp_apne  16  //35
#define knapp_lukk  17 //34
#define knapp_stopp  23 //33
#define EEPROM_SIZE 300
#define lukket false
#define laseralarm 33
#define sirene 34
boolean bryter = false; // bryter når tilkoblet
boolean port_lukket = false; // bryter når tilkoblet
boolean port_apen = false; // bryter når tilkoblet
boolean prev_knapp_apne = false;
boolean prev_knapp_lukk = false;
boolean prev_knapp_stopp = false;

IPAddress local_IP(192, 168, 10, 95);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 10, 113); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional


const char* host = "192.168.10.16";
boolean feil = false;
byte pwr = 250;
uint16_t timeout, timegrense = 600, pos = 0;
int prev = 0;
int maxtid;
char charBuf[50];
char charBuf1[50];
String filnavn;

String   ssid1 = "xxx";
String  password1 = "xxx";
boolean prev_lukket = false;
boolean prev_apen = false;
boolean prev_stopp = false;
int pubmod = 0;
boolean knapp_trykket = false;
boolean published = false;
boolean mod_bryter_apen = false;
boolean mod_bryter_lukket = false;
boolean pub_bryter_apen = false;
boolean pub_bryter_lukket = false;
boolean published1 = false;
int pubmod1 = 0;
int prevmode;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
int freq = 5000;
byte resolution = 8;
int tid = 0;
int md = 0;
boolean published2 = false;
int i = 0;


//char msg1[];

WiFiServer server(80);
#include "html.h"
//#include "lukk.h"


void setup_wifiXXX() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid1);

  // WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void display_vis(String mld)
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.print(mld);
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("callback");
  String str = "";
  str = String(topic);
  String topicstr = "";
  int i = 0;
  while ((topic[i] != (byte)0 && (i < 25)))
  {
    topicstr += (char) topic[i];
    i++;
  };
  topicstr += (char) topic[i];
  i = topicstr.indexOf('/'); //topicstr
  //
  Serial.print("topicstr: " + topicstr + "\t");

  topicstr = topicstr.substring(i + 1);

  str = "";
  for ( i = 0; i < length; i++) {
    msg[i] = payload[i];
    str += (char)payload[i];
  }
  msg[i] = (char)0;
  Serial.println("Payload:|" + str + "| length: " + String(length));
  Serial.println("Status_var callback:\t" + String(status_var) + "|");
  //} void dagg(){  String str = "";   String topicstr = "";



  //if  ( !((status_var == "Deaktivert"))||(status_var == "Aktivert")))
  {
    if (str.substring(0, 7) == "opening")
      apne("opening");
    else if  (str.substring(0, 4) == "open")
      apne("open");
    else if  (str == "close")
      lukk("close");
    else    if  (str == "closing")
    {
      Serial.println("lukk(closing)");
      lukk("closing");
    }
    else  if  (str == "reset")
    {
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(5, 20);
      display.println("Resetter");
      display.display();
      Serial.println("Resetter..");
      //     ssid1 = EEPROM.readString(2);
      //    ssid1.toCharArray(ssid, 20);
      //     password1.toCharArray(charBuf, 20);
      //    WiFi.begin(charBuf, charBuf1);

      ESP.restart();
    }
    else if  (topicstr == "ssid1")
    {
      EEPROM.writeString(2, str);
      EEPROM.writeByte(1, str.length());
      EEPROM.commit();
      Serial.println("ssid1 oppdatert.." + str);
    }
    else if  (topicstr == "pswd1")
    {
      EEPROM.writeString(21, str);
      EEPROM.writeByte(20, str.length());
      EEPROM.commit();
    }
    else if (topicstr == "ssid3")
    {
      EEPROM.writeString(61, str);
      EEPROM.writeByte(60, str.length());
      EEPROM.commit();
      Serial.println("ssid1 oppdatert.." + str);
    }
    else if (topicstr == "pswd3")
    {
      EEPROM.writeString(81, str);
      EEPROM.writeByte(80, str.length());
      EEPROM.commit();
    }

    else if  (topicstr == "ssid5")
    {
      EEPROM.writeString(101, str);
      EEPROM.writeByte(100, str.length() );
      EEPROM.commit();
    }
    else if  (topicstr == "pswd5")
    {
      EEPROM.writeString(121, str);
      EEPROM.writeByte(120, str.length());
      EEPROM.commit();
    }
    else if  (topicstr == "ssid7")
    {
      EEPROM.writeString(141, str);
      EEPROM.writeByte(140, str.length());
      EEPROM.commit();
    }
    else if  (topicstr == "pswd7")
    {
      EEPROM.writeString(161, str);
      EEPROM.writeByte(160, str.length());
      EEPROM.commit();
    }
    else if (topicstr == "sted1")
    {
      EEPROM.writeString(181, str);
      EEPROM.writeByte(180, str.length());
      EEPROM.commit();
      str.toCharArray(charBuf, 20);
      client.publish("honsegard22/sted", charBuf);
      Serial.println("Oppdatert sted: " + str);
    }
    else if  (topicstr == "timeout1")
    { display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(10, 20);
      display.println("Timeout,");
      EEPROM.writeUShort(42, str.toInt());
      EEPROM.commit();
    }
    else if  (topicstr == "fart1")
    {
      //    Serial.println("Lagre fart " + str);
      EEPROM.writeUShort(40, str.toInt());
      EEPROM.commit();
      pwr = str.toInt();
      str.toCharArray(charBuf, 20);
      client.publish("honsegard22/fart3", charBuf);

    }
    else if  (topicstr == "maxtime1")
    {
      EEPROM.writeUShort(46, str.toInt());
      EEPROM.commit();
      maxtid = EEPROM.readUShort(46);
    }
    /*   else if  (topicstr == "apen_mode1")
       {
         if (str == "true") EEPROM.write(54, 1);
         else EEPROM.write(54, 0);
         EEPROM.commit();
         port_apen = EEPROM.read(54);
         //     Serial.println("apen_mode: " + String(port_apen));
       }
       else if  (topicstr == "lukket_mode1")
       {
         if (str == "true")
         {
           EEPROM.write(55, 1);
         }
         else {
           EEPROM.write(55, 0);
         }
         EEPROM.commit();
         port_lukket = EEPROM.read(55);
       }*/
    else if  (str.substring(0, 6) == "stopp1")
    {
      status_var = "Stoppet";
      stopp();
    }
    else if  (str.substring(0, 10) == "deactivate")
      deaktiver();
    else if  (str.substring(0, 8) == "activate")
      aktiver();
  }
}
void oppdater_connect()
{

  //     if (client.connect(clientId.c_str()))
  {
    //      connected1 = true;
    // Serial.println(" oppdater connected");deactivate
    // Once connected, publish an announcement...
    client.publish("honsegard22/mqtt1", mqtt_server);
    ssid1.toCharArray(charBuf, 20);
    client.publish("honsegard2/wifi1", charBuf);
    String(filnavn).toCharArray(charBuf, 20);
    client.publish("honsegard2/fil", charBuf);
    //client.publish("honsegard2", "status1");
    //client.publish("honsegard2", "status2");
    char charBuf[50];
    String str1;
    str1 = String(EEPROM.readString(2));
    Serial.println("Publisher ssd1.." + str1);
    str1 = str1.substring(0, EEPROM.readByte(1));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/ssid", charBuf);
    str1 = String(EEPROM.readString(21));
    str1 = str1.substring(0, EEPROM.readByte(20));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/pswd", charBuf);
    str1 = String(EEPROM.readString(61));
    str1 = str1.substring(0, EEPROM.readByte(60));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/ssid2", charBuf);
    str1 = String(EEPROM.readString(81));
    str1 = str1.substring(0, EEPROM.readByte(80));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/pswd2", charBuf);
    str1 = String(EEPROM.readString(101));
    str1 = str1.substring(0, EEPROM.readByte(100));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/ssid4", charBuf);
    str1 = String(EEPROM.readString(121));
    str1 = str1.substring(0, EEPROM.readByte(120));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/pswd4", charBuf);
    str1 = String(EEPROM.readString(141));
    str1 = str1.substring(0, EEPROM.readByte(140));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/ssid6", charBuf);
    str1 = String(EEPROM.readString(161));
    str1 = str1.substring(0, EEPROM.readByte(160));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/pswd6", charBuf);
    str1 = String(EEPROM.readString(161));
    str1 = str1.substring(0, EEPROM.readByte(160));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/fart", charBuf);
    str1 = String(EEPROM.readString(181));
    str1 = str1.substring(0, EEPROM.readByte(180));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/sted", charBuf);
    str1 = String(EEPROM.readUShort(42));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/timeout", charBuf);
    str1 = String(EEPROM.readUShort(40));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/fart", charBuf);
    //      Serial.println("fart: " + str1);
    str1 = String(EEPROM.readUShort(46));
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/maxtime", charBuf);

    str1 =  String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
    Serial.println("IP adresse: " + str1);
    str1.toCharArray(charBuf, 20);
    client.publish("honsegard2/ip", charBuf);

    if (EEPROM.read(54) == 0)
      client.publish("honsegard2/apen_mode", "false");
    else if (EEPROM.read(55) == 0)
      client.publish("honsegard2/lukket_mode", "false");
    else
      client.publish("honsegard2/lukket_mode", "true");

    // ... and resubscribe
    client.subscribe("honsegard2Ut/#");
  }
}
boolean reconnect() {
  int teller = 0;
  char buf[50];
  Serial.println("reonnect");
  //  char charBuf1[50];
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println();



  // ssid1 = EEPROM.readString(2);
  // password1 = EEPROM.readString(21);
  // ssid1 = ssid1.substring(0, EEPROM.readByte(1));
  //  password1 = password1.substring(0, EEPROM.readByte(20));
  ssid1 = "Telenor-KH-2,5GHz";
  password1 = "Flaktveit69";
  // ssid1 = "Xperia45";
  //  password1 = "12345asdf";
  EEPROM.writeString(2, ssid1);
  EEPROM.writeByte(1, ssid1.length());
  EEPROM.commit();
  EEPROM.writeString(21, password1);
  EEPROM.writeByte(20, password1.length());
  EEPROM.commit();
  Serial.print("SSID1: "); Serial.print(ssid1); Serial.print(" PSWD: "); Serial.print(password1);
  //  Serial.print("Prøver " + ssid1 + "..");
  display.println("Kobler til WiFi:");
  display.print(ssid1);
  display.display();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  ssid1.toCharArray(charBuf, 20);
  password1.toCharArray(charBuf1, 20);
  //  Serial.println("charBuf:\t" + String(charBuf) + "\tSSID:\t" + String(charBuf1));
  WiFi.begin(charBuf, charBuf1);
  while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (teller++ < tries))
  {
    Serial.print(".");
    display.print(".");
    display.display();
    delay(10);
  };


  Serial.println("\tteller:\t" + String(teller) + "\ttries:\t" + String(tries));

  if (teller > tries)
  {
    //    Serial.println("2");
    display.println();
    //   Serial.println();
    teller = 0;
    //    ssid1 = EEPROM.readString(61);
    //    password1 = EEPROM.readString(81);
    ssid1 = "Dalaneset_Repeater";
    password1 = "Dalaneset";
    EEPROM.writeString(61, ssid1);
    EEPROM.writeByte(60, ssid1.length());
    EEPROM.commit();
    EEPROM.writeString(81, password1);
    EEPROM.writeByte(80, password1.length());
    EEPROM.commit();
    //   ssid1 = ssid1.substring(0, EEPROM.readByte(60));
    //        password1 = password1.substring(0, EEPROM.readByte(80));
    Serial.print("SSID2: "); Serial.print(ssid1); Serial.print(" PSWD: "); Serial.print(password1);
    //  Serial.print("Prøver " + ssid1 + "..");
    display.print(ssid1);
    display.display();
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_STA);
    ssid1.toCharArray(charBuf, 20);
    password1.toCharArray(charBuf1, 20);
    WiFi.begin(charBuf, charBuf1);

    while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (teller++ < tries))
    {
      Serial.print(".");
      display.print(".");
      display.display();
      delay(10);
    };
    Serial.println(" Teller:\t" + String(tries));

    //  Serial.print("\nSSID2: "); Serial.print(charBuf); Serial.print(" PSWD: "); Serial.println(charBuf1);
  };

  if (teller > tries)
  {
    teller = 0;
    ssid1 = EEPROM.readString(101);
    password1 = EEPROM.readString(121);
    ssid1 = ssid1.substring(0, EEPROM.readByte(100));
    password1 = password1.substring(0, EEPROM.readByte(120));
    Serial.print("SSID3: "); Serial.print(ssid1); Serial.print(" PSWD: "); Serial.print(password1);
    //    Serial.print("Prøver " + ssid1 + "..");
    display.print(ssid1);
    display.display();
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_STA);
    ssid1.toCharArray(charBuf, 20);
    password1.toCharArray(charBuf1, 20);
    WiFi.begin(charBuf, charBuf1);
    while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (teller++ < tries))
    {
      Serial.print(".");
      display.print(".");
      display.display();
      delay(10);
    };
    //  Serial.print("\nSSID3: "); Serial.print(charBuf); Serial.print(" PSWD: "); Serial.print(charBuf1);
    Serial.println("\tTeller:\t" + String(teller));

  };

  if (teller > tries)
  {
    //   display.println();
    //  Serial.println();
    WiFi.disconnect();
    teller = 0;
    ssid1 = EEPROM.readString(141);
    password1 = EEPROM.readString(161);
    ssid1 = ssid1.substring(0, EEPROM.readByte(140));
    password1 = password1.substring(0, EEPROM.readByte(160));
    Serial.print("SSID4: "); Serial.print(ssid1); Serial.print(" PSWD: "); Serial.print(password1);
    //   Serial.print("Prøver " + ssid1 + "..");
    display.print(ssid1);
    display.display();
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_STA);
    ssid1.toCharArray(charBuf, 20);
    password1.toCharArray(charBuf1, 20);
    WiFi.begin(charBuf, charBuf1);
    while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (teller++ < tries))
    {
      Serial.print(".");
      display.print(".");
      display.display();
      delay(10);
    };
  }
  Serial.println("Teller:\t" + String(teller));
  if (teller > tries)
  {
    EEPROM.write(0, 0);
    EEPROM.commit();
    Serial.println("restart, setter 0");
    ESP.restart();
  }

  EEPROM.write(0, 0xff);

  timeout = EEPROM.readUShort(42);
  maxtid = EEPROM.readUShort(46);

  Serial.println("..Ok");
  display.println("..Ok");
  display.println("Prover MQTT server:");
  display.print(mqtt_server);
  display.display();
  client.setServer(mqtt_server, 1883); // 1883 er default lister port.
  client.setCallback(callback);
  Serial.print("Prover MQTT server:");
  teller = 0;
  boolean connected1 = false;
  while (!client.connected() && (teller++ < 4) && (!connected1)) {
    Serial.print(".");
    display.print(".");
    display.display();
    // Create a random client ID
    String clientId = "1234";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    Serial.println("clientid: " + clientId);
    if (client.connect(clientId.c_str()))
    {
      connected1 = true;
      Serial.println(" connected");
      // Once connected, publish an announcement...
      client.publish("honsegard2/mqtt1", mqtt_server);
      //   String(__FILE__).toCharArray(charBuf, 50);
      //   client.publish("honsegard2/file", charBuf);
      ssid1.toCharArray(charBuf, 20);
      client.publish("honsegard2/wifi1", charBuf);
      client.publish("honsegard2/Subscribe", "honsegard2");
      //client.publish("honsegard2", "status1");
      //client.publish("honsegard2", "status2");
      String filnavn = String(__FILE__);
      char k = char(92);
      int j;
      i = 1;
      while ( i > 0) {
        j = i;
        i = filnavn.indexOf(k, i + 1);
      }
      char charBuf[50];
      //    filnavn = String(__FILE__);
      filnavn = filnavn.substring(j + 1);
      filnavn.toCharArray(charBuf, 20);
      client.publish("honsegard2/fil", charBuf);
      Serial.println("filnavn: " + filnavn + "\t" + String(__FILE__));
      String str1;
      str1 = String(EEPROM.readString(2));
      str1 = str1.substring(0, EEPROM.readByte(1));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/ssid", charBuf);
      str1 = String(EEPROM.readString(21));
      str1 = str1.substring(0, EEPROM.readByte(20));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/pswd", charBuf);
      str1 = String(EEPROM.readString(61));
      str1 = str1.substring(0, EEPROM.readByte(60));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/ssid2", charBuf);
      str1 = String(EEPROM.readString(81));
      str1 = str1.substring(0, EEPROM.readByte(80));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/pswd2", charBuf);
      str1 = String(EEPROM.readString(101));
      str1 = str1.substring(0, EEPROM.readByte(100));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/ssid4", charBuf);
      str1 = String(EEPROM.readString(121));
      str1 = str1.substring(0, EEPROM.readByte(120));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/pswd4", charBuf);
      str1 = String(EEPROM.readString(141));
      str1 = str1.substring(0, EEPROM.readByte(140));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/ssid6", charBuf);
      str1 = String(EEPROM.readString(161));
      str1 = str1.substring(0, EEPROM.readByte(160));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/pswd6", charBuf);
      //    str1 = String(EEPROM.readString(161));
      //   str1 = str1.substring(0, EEPROM.readByte(160));
      //   str1.toCharArray(charBuf, 20);
      //  client.publish("honsegard2/fart", charBuf);
      str1 = String(EEPROM.readString(181));
      str1 = str1.substring(0, EEPROM.readByte(180));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/sted", charBuf);
      str1 = String(EEPROM.readUShort(42));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/timeout", charBuf);
      str1 = String(EEPROM.readUShort(40));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/fart", charBuf);
      //      Serial.println("fart: " + str1);
      str1 = String(EEPROM.readUShort(46));
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/maxtime", charBuf);

      str1 =  String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
      Serial.println("IP adresse: " + str1);
      str1.toCharArray(charBuf, 20);
      client.publish("honsegard2/ip", charBuf);

      if (EEPROM.read(54) == 0)
        client.publish("honsegard2/apen_mode", "false");
      else if (EEPROM.read(55) == 0)
        client.publish("honsegard2/lukket_mode", "false");
      else
        client.publish("honsegard2/lukket_mode", "true");

      // ... and resubscribe
      client.subscribe("honsegard2Ut/#");
    }
  };
  if (teller > 4)
  {
    display.print("\nIkke kontakt MQTT serv");
    Serial.println("\nIkke kontakt  med MQTT server");
    display.display();
  }
  else
  {
    display.println(".Ok");
    display.display();
    Serial.println(".Ok");
    //  client.subscribe("honsegard2Ut/#");
  }
  if (client.connected())
    return true;
  else
    return false;
  delay(5000);
}
boolean rc()
{
  int count = 0;
  boolean connected1 = false;
  // Loop until we're reconnected
  Serial.println("Kobler til MQTT server:");
  //  display.println("MQTT server:");
  //  display.display("skule.sormo.no");

  while (!client.connected() && (count < 9) && (!connected1)) {
    Serial.print(".");
    display.print(".");
    display.display();
    count++;
    // Create a random client ID
    String clientId = "1234";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      connected1 = true;
      Serial.println("connected");
      display.println();
      // Once connected, publish an announcement...
      // client.publish("honsegard2", "hello world");
      // ... and resubscribe
      client.subscribe("honsegard2Ut/#");
      display.println("Tilkoblet MQTT server:");
      display.println(String(mqtt_server));
      display.display();
      return true;
    }
    if (count > 9)
    {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      display.println("Tilkoblet MQTT feilet");
      display.display();
      return false;
    }
    delay(500);
  }
}
void nytt_oppsett() {
  const char *ssid2 = "hane";
  const char *password2 = "honemor";
  WiFiServer server(80);
  Serial.println("nytt oppsett..");
  WiFi.softAP(ssid_w, password_w);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(1000);
  server.begin();
  Serial.println("Server started");
  Serial.println("SSID: " + String(ssid_w));
  Serial.println("Pswd: " + String(password_w));
  // EEPROM.write(0, 0);

  // ESP.restart();
}
void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  delay(10);
  Serial.flush();
  Serial.println("\n********************************");
  filnavn = String(__FILE__);

  Serial.println("Fil: " + String(__FILE__));
  Serial.print("Kompilert: "); Serial.println(__TIMESTAMP__);





  char c = char(92);
  int i = 0;
  int j = 0;
  while (i != -1)
  {
    i = filnavn.indexOf(c, i + 1);
    if (i != -1) j = i;
  };
  filnavn = filnavn.substring(j + 1);
  Serial.println("Filnavn:\t" + filnavn);
  pinMode(motor_enable, INPUT_PULLUP);
  pinMode(bryter_lukket, INPUT_PULLUP); //LOW: Lukket
  pinMode(bryter_apen, INPUT_PULLUP); //LOW: Lukket
  //  pinMode(bryter_stopp, INPUT_PULLUP); //LOW: Lukket
  pinMode(knapp_apne, INPUT_PULLUP); //LOW: Lukket
  pinMode(knapp_lukk, INPUT_PULLUP); //LOW: Lukket
  pinMode(knapp_stopp, INPUT_PULLUP); //LOW: Lukket
  pinMode(laseralarm, INPUT_PULLUP); //LOW: Lukket
  pinMode(sirene, INPUT_PULLUP); //LOW: Lukket

  Serial.println("setup  knapp_apne\t" + String(digitalRead(knapp_apne)) + "  knapp_stopp\t" + String(digitalRead(knapp_stopp)) + "  knapp_lukk\t" + String(digitalRead(knapp_lukk)));
  /*
    if (digitalRead(bryter_lukket) == lukket) pos = timegrense;
    else if  (digitalRead(bryter_apen) == lukket) pos = 0;
    /*  if ((bryter_lukket == lukket) && (port_lukket)==lukket)) pos = timegrense;
      else if (bryter_lukket == port_lukket) pos = 0;
  */
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
    delay(1000000);
  }   ;

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  };
  Serial.println("EEPROM Switch:\t" + String(EEPROM.read(0)));

  if (EEPROM.read(0) == 0)
  {
    Serial.println("Remote setup");
    setup_wifi1();
    return;
  };

  reconnect();
  // aktiver();
  // client.publish("honsegard2", "startet");
  port_apen = EEPROM.read(54);
  port_lukket = EEPROM.read(55);
  maxtid = EEPROM.readUShort(46);
  ledcSetup(0, freq, resolution);
  ledcAttachPin(motorA, 0);
  ledcWrite(0, 0);   // 0 - 255
  ledcSetup(1, freq, resolution);
  ledcAttachPin(motorB, 1);
  ledcWrite(1, 0);   // 0 - 255



  pubmod = 0;


  digitalWrite(motor_enable, LOW);

  char charBuf[50];
  i = 0;
  /* while (i < 20 && ssid[i] != (byte)0)
    {
     charBuf[i] = ssid[i];
     i++;
    }
    charBuf[i] = (byte)0;
    client.publish("honsegard2/ssid", charBuf);
  */
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Boot vector: " + String(EEPROM.read(0)));
  // status_var = "Deaktiver";
  // client.subscribe("honsegard2Ut/#");
  aktiver();
}
void rest()
{

  if (digitalRead(bryter_lukket) == lukket)
  {
    client.publish("honsegard2/bryter_lukket", "true");
    client.publish("honsegard2/led", "lukket");
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("Lukket");
    display.display();
    status_var = "Lukket";
  }
  else/*
  {
    client.publish("honsegard2/bryter_lukket", "false");
    client.publish("honsegard2/led", "deaktiver");
  };

*/ if (digitalRead(bryter_apen) == port_apen)
    {
      client.publish("honsegard2/bryter_apen", "true");
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(20, 20);
      display.print("Apen");
      display.display();
      status_var = "Apen";
    }
    else /*
    client.publish("honsegard2/bryter_apen", "false");

  if ((digitalRead(bryter_lukket) != lukket) && (digitalRead(bryter_apen) != lukket))
  {
    aktiver();
  }
*/
      status_var = "Deaktiver";
  display.clearDisplay();
  display.setTextSize(2);
  //    display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("Utkoblet");
  display.display();

  ledcWrite(0, 0);   // 0 - 255
  ledcWrite(1, 0);   // 0 - 255
  status_var = "Deaktiver";
}
void les_knapper()   // - brukes bare i loop
{
  if ( digitalRead(knapp_apne))   client.publish("honsegard2/knapp", "knapp_apne"); else  client.publish("honsegard2/knapp", "NOT knapp_apne");
  if ( digitalRead(knapp_lukk))     client.publish("honsegard2/knapp1", "knapp_lukk"); else  client.publish("honsegard2/knapp1", "NOT knapp_lukk");;
  if ( digitalRead(knapp_stopp))    client.publish("honsegard2/knapp2", "knapp_stopp"); else  client.publish("honsegard2/knapp2", "NOT knapp_stopp");;

  if (!published2)
  {
    /*  if (digitalRead(laseralarm) == lukket)  {
          digitalWrite(sirene,lukket);
          client.publish("honsegad/alarm", "true");
      }
      else
      {

          digitalWrite(sirene,!lukket);
          client.publish("honsegad/alarm", "false");
      }
    */
    if (digitalRead(knapp_stopp) == 0)
    {
      delay(200);
      if  ((digitalRead(knapp_apne) == 0) && (md != 3))
      {
        md = 3;
        published2 = true;
        aktiver();
      }
      else if  ((digitalRead(knapp_lukk) == 0) && (md != 4))
      {
        md = 4;
        published2 = true;
        deaktiver();
      }
      else if (md != 5)
      {
        md = 5;
        published2 = true;
        stopp();
      }
    }
    else if ((digitalRead(knapp_apne) == 0) && (md != 1)) {
      md = 1;
      published2 = true;
      apne("opening");
    }
    else if ((digitalRead(knapp_lukk) == 0) && (md != 2)) {
      md = 2;
      published1 = true;
      //   maxtid = EEPROM.readUShort(46);
      //   pos = maxtid;
      lukk("closing ");
    }
  }
  else if ((digitalRead(knapp_apne) != 0) && (digitalRead(knapp_stopp) != 0) && (digitalRead(knapp_lukk) != 0))
  {
    if (md != 0)
    {
      published2 = false;
      md = 0;
      delay(100);
    }
  }

}
//void loop(){client.loop();}

void loop() {
  ArduinoOTA.handle();
  //Serial.println("EEPROM(0):\t"+String(EEPROM.read(0)));
  if (EEPROM.read(0) == 0)
    // if (false)
  {
    client.publish("honsegard2/debug", "EEPROM.read(0) == 0");

    //  Serial.println("Booter i config modsu..");
    while (digitalRead(knapp_lukk) == lukket)
    {
      Serial.println("lukker boot");
      ledcWrite(0, pwr);  // 0 - 255
      pinMode(motorA, OUTPUT);
      pinMode(motorB, OUTPUT);
      digitalWrite(motorA, LOW);
      digitalWrite(motorB, HIGH);
    }
    while (digitalRead(knapp_apne) == lukket)
    {
      ledcWrite(1, pwr);  // 0 - 255
      Serial.println("åpner boot");
      pinMode(motorA, OUTPUT);
      pinMode(motorB, OUTPUT);
      digitalWrite(motorA, HIGH);
      digitalWrite(motorB, LOW);
    };
    //  ledcWrite(0, 0);   // 0 - 255
    //  ledcWrite(1, 0);   // 0 - 255
    //     digitalWrite(motorA, LOW);
    //     digitalWrite(motorB, LOW);

    loop_wifi();

    //sjekk_taster();
    // boot_loop() ;
  }
  else
  {
    //     Serial.println("Standard loop: Status_var:\t"+String(status_var));

    /*
        if (!client.connected()) {
          Serial.println("loop reconnect");
          if (!reconnect())
            Serial.println("Ingen kontakt med MQTT server");
          display.println("Ikke kontakt MQTT");
          display.display();
        };

        if ((millis() % 5000) == 0) {
          Serial.println("loop " + String(i++));
          client.publish("honsegard2en/status", "aktiv");
          oppdater_connect();
        };
    */
    //  Serial.print("Porter: " + String( digitalRead(bryter_lukket)) + "\t" + String(lukket) + "\t" + String ( digitalRead(bryter_apen)) + "\t" + String(lukket) + "\t");
    //   Serial.println("brytere sluttet: " + String( digitalRead(bryter_lukket) == lukket) + " : " + String ( digitalRead(bryter_apen) == lukket));

  }
  //if (digitalRead(bryter_lukket) != lukket)  client.loop();//
  client.loop();
  //} void dd() {

  if ( digitalRead(bryter_apen))   client.publish("honsegard2/debug", "bryter_apen"); else  client.publish("honsegard2/debug", "NOT bryter_apen");
  if ( digitalRead(bryter_lukket))     client.publish("honsegard2/debug1", "bryter_lukket"); else  client.publish("honsegard2/debug1", "NOT bryter_lukket");;
  //  if ( digitalRead(knapp_stopp))    client.publish("honsegard2/debug2", "knapp_stopp"); else  client.publish("honsegard2/debug2", "NOT knapp_stopp");;
  les_knapper();
  // }  void d() {
  {
    if (( digitalRead(bryter_lukket) == lukket) ||  ( digitalRead(bryter_apen) == lukket))
    {
      //      Serial.println("en bryter lukket");
      client.publish("honsegard2/debug", "en bryter lukket");
      published1 = 0;
      if (pubmod == 1) {
        published = false;
      };
      if (( digitalRead(bryter_lukket) == lukket) &&  ( digitalRead(bryter_apen) == lukket))
      {
        client.publish("honsegard2/bryter_lukket", "true");
        client.publish("honsegard2/bryter_apen", "true");
        Serial.println("Begge portposisjonene er meldt aktive");
        client.publish("honsegard2", "Begge portposisjonene er meldt aktive");
        feilrutine("FEIL: Begge portpos aktive??");
      }
      else
      {
        if (!published)
        {
          Serial.println("status_var:\t" + status_var);
          //   if (status_var != "Lukker..")
          {
            if ((digitalRead(bryter_lukket) == lukket) && ((status_var != "opening")))// || ((status_var != "Lukker.."))))
            {
              published = true;
              //  pos = timegrense;
              if (pubmod != 2) {
                Serial.println("LUKK I LOOP\tpos:\t" + String(pos) + "\tstatus_var:\t" + status_var );
                prevmode = pubmod;
                pubmod = 2;
                //        maxtid = EEPROM.readUShort(46);
                //     pos = maxtid;

                lukk("closed");
                status_var = "Lukket";
              }
            }
            else if ((digitalRead(bryter_apen) == lukket))// && (status_var != "Lukker.."))
            {
              if (pubmod != 3) {
                published = true;
                prevmode = pubmod;
                pubmod = 3;
                Serial.println("APNE I LOOP");
                apne("open");
              };
            }
          };
        }
      };
      les_knapper();
    }
    else  // her er ingen av porten sluttet.
    {
      //     Serial.println("bryter_apen:\t" + String(digitalRead(bryter_apen)));
      prevmode = pubmod;
      pubmod = 1;
      pubmod1 = 0;
      if (!published1)
      {
        if (pubmod != 4) {
          client.publish("honsegard2/led", "disabled");
          if (prevmode == 2) client.publish("honsegard2/bryter_lukket", "false");
          if (prevmode == 3) client.publish("honsegard2/bryter_apen", "false");
          pubmod1 = 4;
          published1 = true;
        }
      }
    };
    //   les_knapper();

  }

}
void vis_display()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Honsehus");

  display.setTextSize(1);
  display.setCursor(0, 24);
  display.println("skule.sormo.no");
  display.display();
  delay(2000);

  display.clearDisplay();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi SSID: " + ssid1);
  display.setCursor(0, 8);
  display.println("Kontakter server..");
  display.setCursor(0, 16);
  display.println("Kontakter server..");
  display.setCursor(0, 24);
  display.println("Kontakter server..");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Honesgrind");
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.println("Klient: Hakon");
  display.display();
  delay(100);


}
void sjekk_led()
{
  if (digitalRead(bryter_lukket) == lukket)
    client.publish("honsegard2/led", "lukket");
  else if (digitalRead(bryter_apen) == lukket)
    client.publish("honsegard2/led", "apen");
  else
    client.publish("honsegard2/led", "disabled");

}
void lukk(String modus) {
  Serial.println("lukk.. " + String(digitalRead(bryter_lukket)) + " : " + String(lukket) + " Modus:\t" + modus + "\tstatus_var:\t" + status_var + "\tpos:\t" + String(pos));

  if (((digitalRead(bryter_lukket) == lukket) || (modus == "closed")))
  {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("Lukket");
    display.display();
    client.publish("honsegard2/status", "Porten er lukket");
    client.publish("honsegard2/led", "lukket");
    client.publish("honsegard2/viser", "600");
    client.publish("honsegard2/bryter_lukket", "true");
    //   String str = String( EEPROM.readUShort(46));
    //   str.toCharArray(charBuf, 20);
    //   client.publish("honsegard2/viser1", charBuf);
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    modus = "closed";
    Serial.println("Bryter Lukket. status_var:\t" + String(status_var) + "\tTimeout:\t" + String(pos) + "\t" + String(maxtid));
    if (status_var == "Lukker..") {
      Serial.println("set maxtid..");
      maxtid = pos;
      Serial.println("Lagret maxtid: " + String(maxtid));
      EEPROM.writeUShort(46, maxtid);  //lagre tid
      EEPROM.commit();
      String str = String(maxtid);
      str.toCharArray(charBuf, 20);
      client.publish("honsegard2/maxtime", charBuf);
      client.publish("honsegard2/viser", "600");
      client.publish("honsegard2/viser1", charBuf);
      EEPROM.writeUShort(46, maxtid);
      EEPROM.commit();
      status_var = "Lukket";
    }
    else
    {
      String str = String(maxtid);
      str.toCharArray(charBuf, 20);
      client.publish("honsegard2/viser", "600");
      client.publish("honsegard2/viser1", charBuf);
      pos = maxtid;
    }
    return;
  }
  else
    client.publish("honsegard2/bryter_lukket", "false");

  if (feil == true) return;
  char payl[10];
  maxtid = EEPROM.readUShort(46);
  timeout = EEPROM.readUShort(42);
  display.clearDisplay();
  // text display tests
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  //        Serial.println("Lukker..");
  client.publish("honsegard2/status", "Lukker..");
  display.println("Lukker");
  status_var = "Lukker..";
  display.display();
  digitalWrite(motor_enable, LOW);
  ledcWrite(0, pwr);   // 0 - 255
  String str1 = String(pwr);
  str1.toCharArray(charBuf, 20);
  client.publish("honsegard2/fart", charBuf);

  maxtid = EEPROM.readUShort(46);
  if (maxtid == 0) {
    maxtid = 100;
    EEPROM.writeUShort(46, maxtid);
    EEPROM.commit();
    Serial.println("korrigert1 maxtid til timeout");
  }
  unsigned long timeout4 = millis();
  Serial.println("maxtid: \t" + String(maxtid) + " timeout: \t" + String(timeout) + " pos:\t" + String(pos));
  Serial.println ("bryter_lukket:\t" + String(digitalRead(bryter_lukket) != lukket) + "  timeout:\t" + String((millis() - timeout4) < timeout * 100) + " feil:\t" + String(!feil)
                  + "  status_var:\t" + status_var + "\t" + String(status_var == "Lukker..") + "  while: \t" + String((digitalRead(bryter_lukket) != lukket) && ((millis() - timeout4) < timeout * 100) && (!feil)  && (status_var == "Lukker..")));
  while ((digitalRead(bryter_lukket) != lukket) && ((millis() - timeout4) < timeout * 100) && (!feil)  && (status_var == "Lukker..")) //||(status_var != "Apen")))
    //  while ((digitalRead(bryter_lukket) != lukket) && ((millis() - timeout4) < timeout * 100))// && (!feil) )// && (status_var == "Lukker.."))// && (status_var != "opening")))
    //  while ( ((millis() - timeout4) < timeout * 100))
  {
    //  Serial.println("loop\t" + String((millis() - timeout4) % 100) + "\while:\t" + String(((digitalRead(bryter_lukket) != lukket) && ((millis() - timeout4) < timeout * 100))));
    //  if (digitalRead(bryter_apen) != lukket) client.loop();
    //  Serial.println("åpen bryter: \t" + String(digitalRead(bryter_apen) != lukket));
    if (((millis() - timeout4) % 100) == 0) // sjekker hver 100ms
    {
      Serial.println("bryter_apne\t" + String(digitalRead(bryter_apen))  + "  bryter_lukk\t" + String(digitalRead(bryter_lukket)));
      sjekk_led();
      if (digitalRead(bryter_apen) != lukket)  client.loop();

      if ((millis() - timeout4) != prev)
      {
        Serial.println("inner loop lukk\t" + String(pos) + "\ttime:\t" + String((millis() - timeout4)) + "\tTimeout:\t" + String(((millis() - timeout4) < timeout * 100)) + "\tfeil:\t" + String(feil));
        tid = millis() - timeout4;// < maxtid * 1000
        int tid1 = pos * timegrense / (maxtid);
        String tidstr = String(tid1);
        tidstr.toCharArray(payl, tidstr.length() + 1);
        client.publish("honsegard2/viser", payl);
        tidstr = String(pos);
        tidstr.toCharArray(payl, tidstr.length() + 1);
        client.publish("honsegard2/viser1", payl);
        prev = millis() - timeout4;
        if (pos < timegrense) pos++;
        sjekk_taster();
      }
    };

  };
  Serial.println ("bryter_lukket passed:\t" + String(digitalRead(bryter_lukket) != lukket) + "  timeout:\t" + String((millis() - timeout4) < timeout * 100) + " feil:\t" + String(!feil)
                  + "  status_var:\t" + status_var + "\t" + String(status_var == "Lukker..") + "  while: \t" + String((digitalRead(bryter_lukket) != lukket) && ((millis() - timeout4) < timeout * 100) && (!feil)  && (status_var == "Lukker..")));
  Serial.println("passed loop tid:\t" + String(tid));
  Serial.println("pos:\t" + String(pos) + "\ttimegrense:\t" + String(timegrense) + "\tstatus_var:\t" + status_var + "\tFeil:\t" + String(feil));
  Serial.println("Bryter lukket:\t" + String(digitalRead(bryter_lukket) != lukket) + "\tBryter:\t" + String(digitalRead(bryter_lukket)) + "\tLukket:\t" + String(lukket));
  if ((status_var == "Stoppet") || (feil)) return;
  if (((millis() - timeout4) > timeout * 100)) {  // timeout
    //  if (tid > timeout * 100) {  // timeout
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(12, 20);
    display.println("Timeout!");
    display.display();
    Serial.println("timout..");
    client.publish("honsegard2/status", "Timeout - porten lukket ikke");
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    //  digitalWrite(motor_enable, LOW);
    feil = true;
    status_var = "Deaktivert";
    prev = 0;
    //   delay(2000);
    return;
  }
  else if ( digitalRead(bryter_lukket) != lukket) // lukket
  {
    Serial.println("lukket..");
    //   client.publish("honsegard2/led", "lukket");
    Serial.println("Lagret maxtid: " + String(pos));
    maxtid = millis() - timeout4;
    if (pos > 0)
    {
      Serial.println("set maxtid..");
      maxtid = pos;
      Serial.println("Lagret maxtid: " + String(maxtid));
      EEPROM.writeUShort(46, maxtid);  //lagre tid
      EEPROM.commit();
      String str = String(maxtid);
      str.toCharArray(charBuf, 20);
      client.publish("honsegard2/maxtime", charBuf);
      client.publish("honsegard2/viser", "600");
      client.publish("honsegard2/viser1", charBuf);
      EEPROM.writeUShort(46, maxtid);
      EEPROM.commit();
    };
    Serial.println("Lukker...");
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("Lukket");
    client.publish("honsegard2/status", "Porten er lukket");
    display.display();
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    // digitalWrite(motor_enable, HIGH);
    status_var = "Lukket";
    prev = 0;
  }
}
void sjekk_taster()
{
  Serial.println("knapp_apne\t" + String(digitalRead(knapp_apne)) + "  knapp_stopp\t" + String(digitalRead(knapp_stopp)) + "  knapp_lukk\t" + String(digitalRead(knapp_lukk)));
  if  ((digitalRead(knapp_apne) == lukket) &&  (digitalRead(knapp_stopp) == lukket)) {
    client.publish("honsegard2/status", "sjekk - aktiver");
    //  display.println("Apen");
    // status_var = "Apen";
    aktiver();
  }

  else if  ((digitalRead(knapp_lukk) == lukket) &&  (digitalRead(knapp_stopp) == lukket))  {
    client.publish("honsegard2/status", "sjekk - deaktiver");
    //  display.println("Lukket");
    //   status_var = "Lukket";
    deaktiver();
  }
  /*  else if (digitalRead(knapp_lukk) == lukket) {
      // client.publish("honsegard2/status", "sjekk - åpner");
     Serial.println("honsegard2/status, sjekk - åpner");
      apne("closed");
    }
  */
  /* else if (digitalRead(knapp_apne) == lukket) {
     // client.publish("honsegard2/status", "sjekk - åpner");
     apne("open");
    }
  */
  else if  (digitalRead(knapp_apne) == lukket) {
    client.publish("honsegard2/status", "sjekk - åpner");
    /*  digitalWrite(motor_enable, LOW);
      ledcWrite(0, pwr);   // 0 - 255
      ledcWrite(1, 0);   // 0 - 255
    */
    apne("opening");
  }
  else if  (digitalRead(knapp_lukk) == lukket)  {
    client.publish("honsegard2/status", "sjekk - lukker ");
    /*   digitalWrite(motor_enable, LOW);
       ledcWrite(1, pwr);   // 0 - 255
       ledcWrite(0, 0);   // 0 - 255

    */
    lukk("closing");
  }
  if  (digitalRead(knapp_stopp) == lukket) {
    client.publish("honsegard2/status", "sjekk - stopp ");
    stopp();
  }

  if ((digitalRead(knapp_stopp) == 0) && !prev_knapp_stopp)
  {
    stopp();
    prev_knapp_stopp = true;
  }
  else
    prev_knapp_stopp = false;


}
void apne(String modus) {
  Serial.println("apne:\t bryter_apen:\t" + String(digitalRead(bryter_apen)) + "\tModus:\t" + modus + "\tPos:\t" + String(pos));
  if (feil == true) return;
  if (((digitalRead(bryter_apen) == lukket) || (modus == "open")) && (status_var != "closing"))
  {
    Serial.println("apen - nullstilt");
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.print("Apen");
    display.display();
    client.publish("honsegard2/status", "Porten er åpen");
    client.publish("honsegard2/led", "apen");
    client.publish("honsegard2/bryter_apen", "true");
    client.publish("honsegard2/viser", "0");
    client.publish("honsegard2/viser1", "0");
    status_var = "Apen";
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    modus = "open";
    status_var = "Stoppet";
    pos = 0;
    Serial.println("bryter apen:\t" + String(digitalRead(bryter_apen) == lukket) + "\tModus:\t" + String(modus));
    return;
  }
  else
  {
    client.publish("honsegard2/bryter_apen", "false");
    client.publish("honsegard2/led", "disabled");
  }


  char payl[10];
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  if ( modus == "opening")
  {
    client.publish("honsegard2/status", "Porten åpner..");
    display.println("Apner");
    display.display();
    status_var = "Apner..";
    ledcWrite(1, pwr);   // 0 - 255
  };
  if ((pos == 0) && (digitalRead(bryter_apen) == lukket))
  {
    Serial.println("pos=0");
    client.publish("honsegard2/status", "Porten er åpen");
    client.publish("honsegard2/led", "apen");
    client.publish("honsegard2/viser", "0");
    client.publish("honsegard2/viser1", "0");
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.println("Apen");
    display.display();
    status_var = "Apen";
    return;
  };
  display.display();
  digitalWrite(motor_enable, LOW);
  ledcWrite(1, pwr);   // 0 - 255
  Serial.println("pwr:\t" + String(pwr));
  //  ledcWrite(1, 150);   // 0 - 255
  maxtid = EEPROM.readUShort(46);
  timeout = EEPROM.readUShort(42);
  Serial.println("maxtid:\t" + String(maxtid) + "\ttimeout:\t" + String(timeout) + "\tpos:\t" + String(pos));
  unsigned long timeout6 = millis();
  prev = 0;
  while ((digitalRead(bryter_apen) != lukket) && ((millis() - timeout6) < timeout * 100) && (!feil) && (status_var == "Apner.."))
  {
    //if (digitalRead(bryter_lukket) != lukket)
    client.loop();
    if (((millis() - timeout6) % 100) == 0) //lesing av hver tidels sekund
    {
      //     sjekk_led();
      if ((millis() - timeout6) != prev)
      {
        Serial.println("Bryter_apne\t" + String(digitalRead(bryter_apen))  + "  bryter_lukk\t" + String(digitalRead(bryter_lukket)));
        Serial.println("inner loop apne\t" + String(pos) + "\ttime:\t" + String((millis() - timeout6)) + "\tTimeout:\t" + String(timeout * 100));
        //    Serial.println("inner loop\t"+String(pos));
        tid = millis() - timeout6;// < maxtid * 1000
        int tid1 = pos * timegrense / (maxtid);
        String tidstr = String(tid1);
        tidstr.toCharArray(payl, tidstr.length() + 1);
        client.publish("honsegard2/viser", payl);
        tidstr = String(pos);
        tidstr.toCharArray(payl, tidstr.length() + 1);
        client.publish("honsegard2/viser1", payl);
        prev = millis() - timeout6;
        if (pos > 0) pos--;
        sjekk_taster();
      }
    };
  };
  Serial.println("passed loop tid:\t" + String(tid));
  Serial.println("pos:\t" + String(pos) + "\ttimegrense:\t" + String(timegrense));

  if ((status_var == "Stoppet") || (feil)) return;
  if (((millis() - timeout6) > timeout * 100)) {  // timeout
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(12, 20);
    display.println("Timeout!");
    display.display();
    client.publish("honsegard2/status", "Timeout - porten åpnet ikke");
    feil = true;
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    //  digitalWrite(motor_enable, LOW);
    feil = true;
    status_var = "Deaktivert";
    prev = 0;
    //    delay(2000);
    return;
  }
  else //if ( digitalRead(bryter_apen) != lukket) // lukket
  {
    client.publish("honsegard2/led", "apen");
    client.publish("honsegard2/viser", "0");
    client.publish("honsegard2/viser1", "0");
    client.publish("honsegard2/status", "Porten er åpen");
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.println("Apen");
    display.display();
    status_var = "Apen";
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    prev = 0;
  }
}
void aktiver()
{
  Serial.println("aktivert");
  digitalWrite(motor_enable, LOW);
  status_var = "Aktivert";
  //  client.publish("honsegard2/status", "Aktivert");
  feil = false;
  client.publish("honsegard2/status", "Velg Åpne eller Steng");
  display.clearDisplay();
  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(13, 20);
  display.println("Aktivert");
  display.display();
}
void stopp()
{
  Serial.println("stoppet");
  ledcWrite(0, 0);   // 0 - 255
  ledcWrite(1, 0);   // 0 - 255
  Serial.println("stopp " + status_var);
  if (feil) return;
  if ((digitalRead(bryter_lukket) == lukket) || (digitalRead(bryter_apen) == lukket))
    return;

  status_var = "Stoppet";
  // feil=true;
  client.publish("honsegard2/status", "Stoppet");
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.println("Stoppet");
  display.display();
}
void deaktiver()
{
  client.publish("honsegard2/status", "Utkoblet");
  feil = true;
  display.clearDisplay();
  display.setTextSize(2);
  //    display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("Utkoblet");
  display.display();
  digitalWrite(motor_enable, HIGH);
  ledcWrite(0, 0);   // 0 - 255
  ledcWrite(1, 0);   // 0 - 255
}
void feilrutine(String melding)
{
  char payl[50];
  melding.toCharArray(payl, melding.length() + 1);
  client.publish("honsegard2/status", payl);
  feil = true;
  display.clearDisplay();
  display.setTextSize(3);
  //    display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("Feil");
  display.display();
}
void status()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(status_var);
  // client.publish("honsegard2/status", "deactivate");
  display.display();
}
void boot() {

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            String htmlstr = "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'><html xmlns='http://www.w3.org/1999/xhtml'><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' /><title>Untitled Document</title></head>";
            htmlstr = htmlstr
                      + "<body><p align='center'>&nbsp;</p><form id='form1' name='form1' method='get'> <h2 align='center'>  <table width='328' border='1'><tr><td colspan='2'>Innstillinger port til hønsegard</td>"
                      + "</tr><tr><td width='52'>SSID:</td><td width='260'><input type='text' name='ssid' id='ssid' /></td></tr><tr><td>Passord:</td><td><input type='text' name='passwd' id='passwd' /></td></tr> <tr><td colspan='2'>"
                      + "<input type='submit' name='submit' id='submit' value='Oppdater' /></td></tr></table></h2> <p align='center'></p></form><p align='center'>&nbsp;</p><p>&nbsp;</p></body></html>";
            client.println(htmlstr);
            client.println();
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        };

        // lese nytt ssid og pswd`:
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
          EEPROM.writeString(1, ssid1);
          EEPROM.writeString(20, pswd1);
          Serial.println("Length: " + String(ssid1.length()));
          EEPROM.commit();
          Serial.println("les eeprom");
          ssid1 = "";

          ssid1 = EEPROM.readString(1);
          Serial.println("EEPROM ssid: " + ssid1);
          Serial.println(EEPROM.read(0));
          EEPROM.write(0, 0xff);
          EEPROM.commit();
          ESP.restart();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
