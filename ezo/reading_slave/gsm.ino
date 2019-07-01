void updateGSMSerial()
{
  delay(500);
  while (Serial.available())
  {
    gsm.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (gsm.available())
  {
    Serial.write(gsm.read());//Forward what Software Serial received to Serial Port
  }
}


void updateGSMData(String tanggal, String jam) {
  /** fixedData is written like the format as follows
     ONLIMO KLHK-1 2018-04-19 08:00 10.2 20.2 300.4 40 50 6.8
     70 80 90 100 110 120 130 140 150
     sensorValue in format ["pH", "DO", "COND", "TDS", "SALT", "SWSG", "TEMP"]
  */
  String fixedData = "ONLIMO UJI-14";
  fixedData = fixedData + " " + tanggal;
  fixedData = fixedData + " " + jam;
  fixedData = fixedData + " " + sensorValue[6];
  fixedData = fixedData + " " + sensorValue[2];
  fixedData = fixedData + " " + sensorValue[3];
  fixedData = fixedData + " " + sensorValue[4];
  fixedData = fixedData + " " + sensorValue[1];
  fixedData = fixedData + " " + sensorValue[0];
  fixedData = fixedData + " " + "-";
  fixedData = fixedData + " " + "-";
  fixedData = fixedData + " " + sensorValue[5];
  fixedData += " - - - - - - ";
  GSMData = fixedData;
  Serial.println(fixedData);
}

void SendTextMessage(String number)
{
  gsm.listen();// "gsm" serial port is active
  //WAITING FOR GSM TO CONNECTED
  //  ConnectGSM();
  Serial.println("GSM Connected");
  Serial.println("Sending Text...");
  gsm.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  updateGSMSerial();
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  String temp = "AT+CMGS=" + number;
  gsm.println(temp);
  updateGSMSerial();
  updateTglJam();
  updateGSMData(tanggal, jam);
  gsm.println(GSMData); //the content of the message
  updateGSMSerial();
  gsm.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  updateGSMSerial();
  gsm.println();
  Serial.println("Text Sent.");
  displayStat("SMS Sent.");
}

void ConnectGSM() {
  gsm.listen();
  gsm.println("AT"); //Once the handshake test is successful, it will back to OK
  waitUntilResponse("OK");
  char isConnect;
  do
  {
    gsm.println("AT+CREG?"); //Check whether it has registered in the network
    gsm.flush();
    delay(10);
    String x;
    if (gsm.available() > 0)
    {
      x = gsm.readString();
      Serial.print("I received: ");
      Serial.println(x);
      isConnect = x.charAt(20);
      Serial.println(isConnect);
    }
  } while (isConnect != '1' && isConnect != '5');
  gsm.println("AT+CMGF=1"); // Configuring TEXT mode
  waitUntilResponse("OK");

  gprsConnectionSetup = true;
} // setupGPRSConnection

void readResponse(){
  responseGsm = "";
  while (responseGsm.length() <= 0 || !responseGsm.endsWith("\n"))
  {
    tryToRead();
    if (millis() - lastTimeGsm > maxResponseTime)
    {
      return;
    }
  }
} // readResponse

void tryToRead() {
  while (gsm.available()) {
    char c = gsm.read();  //gets one byte from serial buffer
    responseGsm += c; //makes the string readString
  }
} // tryToRead

void waitUntilResponse(String resp) {
  lastTimeGsm = millis();
  String response = "";
  String totalResponse = "";
  while (response.indexOf(resp) < 0 && millis() - lastTimeGsm < maxResponseTime)
  {
    readResponse();
    totalResponse = totalResponse + response;
    DEBUG_PORT.println(response);
  }

  if (totalResponse.length() <= 0)
  {
    DEBUG_PORT.println("NO RESPONSE");
    if (gprsConnectionSetup == true) {
      DEBUG_PORT.println("error");
    }
  }
  else if (response.indexOf(resp) < 0)
  {
    if (gprsConnectionSetup == true) {
      DEBUG_PORT.println("error");
    }
    DEBUG_PORT.println("UNEXPECTED RESPONSE");
    DEBUG_PORT.println(totalResponse);
  } else {
    DEBUG_PORT.println("SUCCESSFUL");
  }

} // waitUntilResponse
