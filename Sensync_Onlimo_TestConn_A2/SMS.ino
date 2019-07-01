void SendTextMessage(String pesan){
  Serial.println(F("Sending Text..."));
  Serial.println(sendCommand("AT+IPR=9600","OK",1));
  Serial.println(sendCommand("AT+CSQ","OK",1));
  Serial.println(sendCommand("AT+COPS=?","+COPS:",15));
  Serial.println(sendCommand("AT+CMGF=1","OK",2));
  Serial.println(sendCommand("AT+CMGS=\"+6282213346308\"",">",5));
  Serial.println(sendCommand(pesan,">",1));
  Serial.println(sendChar26("+CMGS:",10));
  Serial.println(waitResponse("OK",10));

  Serial.println(sendCommand("AT+CMGS=\"+6285222999512\"",">",5));
  Serial.println(sendCommand(pesan,">",1));
  Serial.println(sendChar26("+CMGS:",10));
  Serial.println(waitResponse("OK",10));

  Serial.println(F("Text Sent."));
}

void SendTextMessage2(String pesan){
  Serial.println(F("Sending Text..."));
  SIM800port.print("AT\r");
  ResponseGSM();
  SIM800port.print("AT+IPR=9600\r");
  ResponseGSM();
  SIM800port.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  ResponseGSM();
  //SIM800port.println("AT+CMGS=\"+6282130310254\"");
  SIM800port.println("AT+CMGS=\"+6285222999512\"");
  ResponseGSM();
  SIM800port.println(pesan); //the content of the message
  ResponseGSM();
  SIM800port.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  ResponseGSM();
  SIM800port.println();
  ResponseGSM();
  Serial.println(F("Text Sent."));
}

String sendCommand(String cmd, String resp, int timeout){
  String gsmString;
  bool first=true;
  Serial.print("Send Command > ");Serial.println(cmd);
  SIM800port.println(cmd);
  SIM800port.setTimeout(500);
  for (int i=0 ; i<(timeout*2) ; i++){
    gsmString = SIM800port.readString();
    if (gsmString !=""){
      Serial.print("> ");Serial.println(gsmString);
      if (gsmString.indexOf(resp) > 0){
        Serial.println("True");
        return gsmString;
        break;
      } else {
        Serial.println("False");
      }
    } else {
      if(first){Serial.print("> ");Serial.print("Empty ");}
      first=false;
      Serial.print(".");
    }
  }
  Serial.println();
  return "null";
}

String sendChar26(String resp, int timeout){
  Serial.println("Wait Response !");
  String gsmString;
  bool first=true;
  SIM800port.println((char)26);  //-- sending
  SIM800port.setTimeout(500);
  for (int i=0 ; i<(timeout*2) ; i++){
    gsmString = SIM800port.readString();
    if (gsmString !=""){
      Serial.print("> ");Serial.println(gsmString);
      if (gsmString.indexOf(resp) > 0){
        Serial.println("True");
        return gsmString;
        break;
      } else {
        Serial.println("False");
      }
    } else {
      if(first){Serial.print("> ");Serial.print("Empty ");}
      first=false;
      Serial.print(".");
    }
  }
  Serial.println();
  return "null";
}

String waitResponse(String resp, int timeout){
  String gsmString;
  bool first=true;
  for (int i=0 ; i<(timeout*2) ; i++){
    gsmString = SIM800port.readString();
    if (gsmString !=""){
      Serial.print("> ");Serial.println(gsmString);
      if (gsmString.indexOf(resp) > 0){
        Serial.println("True");
        return gsmString;
        break;
      } else {
        Serial.println("False");
      }
    } else {
      if(first){Serial.print("> ");Serial.print("Empty");}
      first=false;
      Serial.print(".");
    }
  }
  Serial.println();
  return "null";
}

String ResponseGSM(){
  String response="";
  long int nextime = millis();
  while(espSerial.available()){
    if((millis()-nextime) > 5000){ break;}
      //response = espSerial.readStringUntil("\n");
      response = espSerial.readString();
  }
  return response;
}

void gprsResponse(){
  delay(500);
  while (Serial.available()){
    SIM800port.write(Serial.read());
  }
  while (SIM800port.available()){
    Serial.write(SIM800port.read());
  }
}
