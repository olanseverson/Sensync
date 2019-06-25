#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(12, 13); //SIM800L Tx & Rx is connected to Arduino #3 & #2
String number = "\"+6282130310254\"";

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(2000);
  updateSerial();
//  mySerial.println("AT+CMGS=\"+6282130310254\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms +6282130310254  +6285270817607
  SendTextMessage(number);
//  updateSerial();
//  mySerial.print("huting"); //text content
//  updateSerial();
//  mySerial.write(26);
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (mySerial.available())
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void SendTextMessage(String number)
{
  Serial.println("Sending Text...");
  mySerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  updateSerial();
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  String temp = "AT+CMGS=" + number;
  mySerial.println(temp);
  updateSerial();
  mySerial.println("How are you today???"); //the content of the message
  updateSerial();
  mySerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  updateSerial();
  mySerial.println();
  Serial.println("Text Sent.");
}
