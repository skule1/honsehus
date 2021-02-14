
//***********************   ÅPNE  ***************************

void apne(String modus) {
  Serial.println("apne..");
  if (feil == true) return;
  if (digitalRead(bryter_apen) == port_apen)

  {
    Serial.println("apen brytere sluttet");
    Serial.print(digitalRead(bryter_apen)); Serial.print(" : "); Serial.println(port_apen);
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.print("Apen");
    display.display();
    client.publish("honsegard/status", "Porten er åpen");
    client.publish("honsegard/led", "apen");
    client.publish("honsegard/viser", "0");
    client.publish("honsegard/viser1", "0");
    status_var = "Apen";
    pos = 0;
    return;
  }
  Serial.println("pass.:");
  lukk("close");

  char payl[10];
  //  (digitalRead(bryter_apen) == HIGH)
  Serial.println("apne() " + String(digitalRead(knapp_apne) ) + " " + String(digitalRead(bryter_apen) ) + " Modus: " + modus);

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  if ( modus == "opening")
  {
    client.publish("honsegard/status", "Porten åpner..");
    display.println("Apner");
    display.display();
    status_var = "Apner..";
    ledcWrite(1, pwr);   // 0 - 255
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.println("Apen");
    client.publish("honsegard/status", "Porten er åpen");
    display.display();
    status_var = "Apen";
    client.publish("honsegard/viser", "0");
    client.publish("honsegard/viser1", "0");
    return;
  };
  display.display();
  digitalWrite(motor_enable, LOW);
  ledcWrite(1, pwr);   // 0 - 255
  //  ledcWrite(1, 150);   // 0 - 255
 // maxtid = EEPROM.readUShort(46);
   timeout = EEPROM.readUShort(42);
/*  if (maxtid == 0) {
    maxtid = timeout;
    EEPROM.writeUShort(46, maxtid);
    EEPROM.commit();
    Serial.println("korrigert maxtid til timeout");
  }
*/  // pos=maxtid;
  Serial.println("maxtid: " + String(maxtid) + " timeout: " + String(timeout));
  unsigned long timeout6 = millis();
  prev = 0;
  Serial.println("prev Apne: " + String(prev) + " timeout6: " + String(millis() - timeout6) + "  tidsbruk: " + String((maxtid - ((millis() - timeout6) / 100))));
  Serial.println("Bryter_åpen: " + String(digitalRead(bryter_apen)) + "  Bryter: " + String(digitalRead(port_apen)) + "  Logikk: " + String((digitalRead(bryter_apen) == port_apen)));
  Serial.println("port: " + String(digitalRead(bryter_apen) == !port_apen) + " tid: " + String(timeout * 100 - ((millis() - timeout6)) > 0) + " feil: " + String(!feil) + " Status: " + String(status_var == "Apner.."));
/*
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
*/




while ((digitalRead(bryter_apen) == !port_apen) && ((timeout * 100 - (millis() - timeout6)) > 0) && (!feil) && (status_var == "Apner.."))
    {
      if (((millis() - timeout6) % 100) == 0) //lesing av hver tidels sekund
      {
        if ((millis() - timeout6) != prev)
        {
          client.loop();
          //       int tid=(millis() - timeout4)*maxtid/timeout*1000;
          //     Serial.println("prev: " + String(prev));
          tid = millis() - timeout6;// < maxtid * 1000
          int tid1 = pos * timegrense / (maxtid);
          //      tid = maxtid - ((millis() - timeout6) / 100);
          //      Serial.println(" tid: " + String(timeout*100 - (millis() - timeout6)) + " pos: " + String(pos) + " value: " + String(tid1)
          //                          + " maxtid: " + String(maxtid) + " status_var " + String(status_var));
          Serial.println("Bryter: " + String(digitalRead(bryter_apen) == !port_apen) + " Tid: " + String(timeout * 100 - (millis() - timeout6)) + " Feil: " + String(!feil) + " Status: " + String(status_var));
          String tidstr = String(tid1);
          tidstr.toCharArray(payl, tidstr.length() + 1);
          client.publish("honsegard/viser", payl);
          tidstr = String(pos);
          tidstr.toCharArray(payl, tidstr.length() + 1);
          client.publish("honsegard/viser1", payl);
          prev = millis() - timeout6;
          if (pos > 0) pos--;
          sjekk_taster();
        }
      };
    };
*/
  ledcWrite(0, 0);   // 0 - 255
  ledcWrite(1, 0);   // 0 - 255
  Serial.println("Bryter: " + String(digitalRead(bryter_apen) == !port_apen) + " Tid: " + String(timeout * 100 - ((millis() - timeout6))) + " Feil: " + String(!feil) + " Status: " + String(status_var == "Apner.."));

  digitalWrite(motor_enable, LOW);
  Serial.println("pos slutt apne: " + String(pos));
  if ((status_var == "Stoppet") || (feil)) return;
  //      Serial.print("apne() " + String(digitalRead(knapp_apne) ) + " " + String(digitalRead(bryter_apen) ) + " Modus: ");
  // client.publish("honsegard/status", "Porten er åpen");
  //      Serial.print("Prev: " + String(prev));
  //      Serial.print("  Tidsbruk: "); Serial.print(millis() - timeout6);
  //      Serial.print(":"); Serial.print(timeout * 1000); Serial.print(":"); Serial.println((millis() - timeout6 > timeout * 1000));

  //  if (((millis() - timeout6)<=1) && (digitalRead(bryter_apen) == bryter))
  // boolean port_apen=HIGH;
  if  (digitalRead(bryter_apen) == !port_apen)
  {
    stopp();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(12, 20);
    display.println("Timeout!");
    display.display();
    //       Serial.println("Timeout!");
    client.publish("honsegard/status", "Timeout - porten åpnet ikke");
    feil = true;
    status_var = "Deaktivert";
    prev = 0;
    delay(2000);
    return;
  }
  else //if (digitalRead(bryter_apen) == LOW)
  {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.println("Apen");
    client.publish("honsegard/status", "Porten er åpen");
    display.display();
    status_var = "Apen";
    String tidstr = "0";
    tidstr.toCharArray(payl, tidstr.length() + 1);
    client.publish("honsegard/viser", payl);
    ledcWrite(0, 0);   // 0 - 255
    ledcWrite(1, 0);   // 0 - 255
    prev = 0;
  }
  //   Serial.println("Prev slutt apne: "+String(prev));

}
