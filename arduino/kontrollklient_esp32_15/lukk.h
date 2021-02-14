//************* lukker *****************************

void lukk(String modus) {
  //       Serial.println("lukk.. " + String(digitalRead(bryter_lukket)) + " : " + String(port_lukket));

  if (digitalRead(bryter_lukket) == port_lukket)
  {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("Lukket");
    client.publish("honsegard/status", "Porten er lukket");
    client.publish("honsegard/led", "lukket");
    status_var = "Lukket";
    return;
  }
  if (feil == true) return;
  stopp();
  char payl[10];
  //(digitalRead(bryter_apen) == HIGH)
  //      Serial.println();
  //      Serial.println("lukk(), brytere:  " + String(digitalRead(bryter_lukket) ) + " " + String(digitalRead(bryter) ) + " Modus: " + modus);
  //       Serial.println("Knapper: " + String(digitalRead(knapp_apne) == LOW) + " : " + String(digitalRead(knapp_lukk) == LOW) + " : " + String(digitalRead(knapp_apne) == LOW));
  //      Serial.println("maxtid: " + String(maxtid));
  maxtid = EEPROM.readUShort(46);
 timeout = EEPROM.readUShort(42);
  //       Serial.println("maxtid: " + String(maxtid));
  display.clearDisplay();
  // text display tests
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  if ( modus == "close")
  {
    client.publish("honsegard/status", "Porten er lukket");
    display.println("Lukket");
    status_var = "Lukket";
    String tidstr = String(timegrense);
    tidstr.toCharArray(payl, tidstr.length() + 1);
    client.publish("honsegard/viser", payl);
    return;
  }
  else
  {
    //        Serial.println("Lukker..");
    client.publish("honsegard/status", "Lukker..");
    display.println("Lukker");
    status_var = "Lukker..";
  };

  display.display();
  digitalWrite(motor_enable, LOW);
  ledcWrite(0, pwr);   // 0 - 255
  //      Serial.println("fart: " + String(pwr));
  String str1 = String(pwr);
  str1.toCharArray(charBuf, 20);
  client.publish("honsegard/fart", charBuf);

  maxtid = EEPROM.readUShort(46);
  if (maxtid == 0) {
    maxtid = timeout;
    EEPROM.writeUShort(46, maxtid);
    EEPROM.commit();
    Serial.println("korrigert maxtid til timeout");
  }
  // prev=0;
  // timeout=timegrense;
  unsigned long timeout4 = millis();
  Serial.println("maxtid: " + String(maxtid) + " timeout: " + String(timeout));

  //      Serial.println("loop1 " + String(digitalRead(bryter_lukket) == !bryter) + " :" + String((millis() - timeout4) < timeout * 1000) + " : " + String(!feil) + " : " + String(status_var != "Stoppet") + " : " + String(prev) + " : " + String(timeout));
  //       Serial.println("Prev start lukk: " + String(prev));
  //       Serial.println("maxtid: " + String(maxtid));
  //       Serial.println("bryter_lukket: " + String(digitalRead(bryter_lukket)) + "  port_lukket: " + String(port_lukket));
  //   boolean port_lukket=false;  // bryter når tilkoblet
  // while ((digitalRead(bryter_lukket) == !bryter) && ((millis() - timeout4) < timeout * 1000) && (!feil) && (digitalRead(knapp_stopp) == HIGH) && ((status_var != "Stoppet")||(status_var != "Apen")))
  //     Serial.println("loop " + String(digitalRead(bryter_lukket) == !port_lukket) + " :" + String((millis() - timeout4) < maxtid * 100) + " : " + String(!feil) + " : " + String(status_var != "Stoppet"));
  while ((digitalRead(bryter_lukket) == !port_lukket) && ((millis() - timeout4) < timeout * 100) && (!feil)  && (status_var == "Lukker..")) //||(status_var != "Apen")))
  {
    client.loop();
    //Serial.println("loop "+String(digitalRead(bryter_lukket) == !bryter)+" :" +String((millis() - timeout4) < maxtid * 1000)+" : "+String(!feil)+" : "+String(status_var != "Stoppet"));
    if (((millis() - timeout4) % 100) == 0) // sjekker hver 100ms
    {
      if ((millis() - timeout4) != prev)
      {
        //       int tid=(millis() - timeout4)*maxtid/timeout*1000;
        //     Serial.println("prev: " + String(prev));
        tid = millis() - timeout4;// < maxtid * 1000
        int tid1 = pos * timegrense / (maxtid);
        //      tid = timeout - ((millis() - timeout6) / 100);
        //             Serial.print("pos: " + String(pos));
        Serial.println(" tid: " + String(tid) + " pos: " + String(pos) + " value: " + String(tid1)
                       + " maxtid: " + String(maxtid) + " timeout: " + String(timeout) + " status_var " + String(status_var) + " : " + String(((status_var != "Stoppet") || (status_var != "Apen"))));
        String tidstr = String(tid1);
        tidstr.toCharArray(payl, tidstr.length() + 1);
        client.publish("honsegard/viser", payl);
        tidstr = String(pos);
        tidstr.toCharArray(payl, tidstr.length() + 1);
        client.publish("honsegard/viser1", payl);
        prev = millis() - timeout4;
        if (pos < timegrense) pos++;
        sjekk_taster();
      }
    };

  };
  //     Serial.println("loop " + String(digitalRead(bryter_lukket) == !port_lukket) + " :" + String((millis() - timeout4) < maxtid * 100) + " : " + String(!feil) + " : " + String(status_var != "Stoppet"));
  //     Serial.println("pos slutt lukket: " + String(pos));

  if ((status_var == "Stoppet") || (feil)) return;
  //    Serial.println("Tid etter while: " + String(tid));
  //      Serial.print("Time lukket: "); Serial.print((millis() - timeout4)) ; Serial.print(":"); Serial.println(timeout * 1000);
  //  if ((millis() - timeout4) > timeout * 1000)
  if ((digitalRead(bryter_lukket) == !port_lukket))
  {
    stopp();
    //      Serial.println(" Timeout1 !");
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(13, 20);
    display.println("Timeout");
    client.publish("honsegard/status", "Timeout - porten lukket ikke");
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    digitalWrite(motor_enable, LOW);
    feil = true;
    status_var = "Deaktivert";
    prev = 0;
    delay(2000);
    return;
  }
  else //if ( digitalRead(bryter_lukket) == !port_lukket)
  {
    Serial.println("Lagret maxtid: " + String(pos));
    maxtid = millis() - timeout4;
    if (pos > 0)
    {
      maxtid = pos;
      //         Serial.println("Lagret maxtid: " + String(maxtid));
      EEPROM.writeUShort(46, maxtid);  //lagre tid
      EEPROM.commit();
      String str = String(maxtid);
      str.toCharArray(charBuf, 20);
      client.publish("honsegard/maxtime", charBuf);
      client.publish("honsegard/viser", "200");
      client.publish("honsegard/viser1", charBuf);
      EEPROM.writeUShort(46, maxtid);
      EEPROM.commit();

    };

    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.print("Lukket");
    client.publish("honsegard/status", "Porten er lukket");
    display.display();
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    digitalWrite(motor_enable, HIGH);
    status_var = "Lukket";
    /*  String tidstr = String(maxtid);
      tidstr.toCharArray(payl, tidstr.length() + 1);
      client.publish("honsegard/viser", payl);
      tidstr = String(tid / 1000);
      tidstr.toCharArray(payl, tidstr.length() + 1);
      //  client.publish("honsegard/maxtime", payl);
    */
    prev = 0;
  }
  //      Serial.println("Prev slutt lukket: " + String(prev));

}
