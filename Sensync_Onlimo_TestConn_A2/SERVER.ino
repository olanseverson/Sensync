//--------------------------------------------------- Send to Server
void post2Server(){
  //String Datas=" ";
  //String dateTime = SMStimeStamp();
  Serial.println(F("$Upload Data..."));
  displayStat("Upload Data...");
  delay(2000);  
  Serial.println(F("$Post to Server..."));
  displayStat("Post to Server"); 
  //Serial.println(Datas); 
  //espSerial.println(Datas);
  //String response = ResponseESP("OK",5000);
  //PRINTLN(response);
  delay(1000);
}

void sms2Server(){
  //String Datas=" ";
  //String dateTime = TimeStamp();
  Serial.println(F("$SMS to Server..."));
  displayStat("SMS to Server");
  delay(2000);  
  //-- Collect Datas
  //Serial.print(F("SMS: "));  Serial.println(Datas);  
  //SendTextMessage(Datas);
  displayStat("Done");  
  delay(2000);  
}

//-- Upload data to New GetSensync Server (http://128.199.194.1)
bool send2NeoGetSensync(){    
  displayStat("Upload...");
  PRINTLN("Upload... ");
  delay(1000);
  separator(65);    
  PRINTLN("Collect all datas... ");
  //-- send data to cloud
  printTime();
  displayStat("Send Data...   ");
  Serial.println(F("Send Data ...   "));
  delay(1000);
  Serial.println(Datas);
  Serial.println();
  Datas.trim(); // remove space char
  espSerial.println(Datas);
  
  String response = ResponseESP("OK",5000);
  PRINTLN(response);
  if(response.indexOf("received") != -1) { 
    displayStat("Upload OK");
    delay(2000);
  }       
  else { 
    displayStat("Upload Done");
    ERROR(F("Upload Fail!"));
    delay(2000);
    Buzzer();
    Done();
    return 0;
  } 
  Buzzer();
  return 1;  
}

String SendDataSensor(){  
}

String ResponseESP(String resp, int timeout){
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

void startUp(){
  PRINTLN(F("Get Startup"));  
  espSerial.println("startup");
  String payload = ResponseESP("ready",5000);
  Serial.println(payload);
  Done();
}

void chkESPstat(){
  espSerial.println("chkstat");
  String payload = ResponseESP("ready",5000);
  Serial.println(payload);
  Done();
}

void getIPaddrs(){
  espSerial.println("getIP");
  String payload = ResponseESP("OK",5000);
  Serial.println(payload);
  Done();
}
