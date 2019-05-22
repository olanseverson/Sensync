//http://www.esp8266learning.com/dht11-sensor-data-to-thingspeak-using-a-wemos-d1.php
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#define LED_PIN LED_BUILTIN   //D4

//-- ESP_TX pin 14/D5 -  15/D8
//-- ESP_RX pin 12/D6 -  13/D7
SoftwareSerial ardSerial(14, 12, false, 256);

// replace with your channel’s thingspeak API key and your SSID and password
String apiKey = "3JEXAHO11O1HUNA3";
const char* ssid = "Get Sensync";
const char* password = "makanminggu12";
const char* server = "api.thingspeak.com";
WiFiClient client;

String rawData;
int ledState = LOW;
bool isUpdated = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  ardSerial.begin(9600);
  delay(1000);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  updateSerial();
  if (isUpdated) {
    String data = parseData(rawData, ",");
    Serial.println(data); //edit the rawData before sending to Thingspeak
    uploadToServer(data);
    isUpdated = false;
  }

}
//=======================================================================

void updateSerial()
{
  delay(1000);
  while (Serial.available())
  {
    ardSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  if (ardSerial.available())
  {
    isUpdated = true;
    rawData = "";
    while (ardSerial.available()) {
      char in = ardSerial.read();
      rawData += in;
    }
    Serial.println(rawData);//Forward what Software Serial received to Serial Port
  }
}

String parseData(String rawData, String delimiter) {
  String temp = rawData; //copy rawData so rawData is not disappear
  char count = '1';
  String outputData = ""; // data to be returned
  int pos = 0;
  String token;
  while ( (pos = temp.indexOf(delimiter)) != -1) {
    token = temp.substring(0, pos);  // copy string from index 0 to pos
    outputData = outputData + "&field" + count + "=" + token;
    count ++;
    temp.remove(0, pos + delimiter.length()); // remove string from index 0 to pos
  }
  return outputData;
}

void uploadToServer(String data) {
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += data;
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.println("Sending data to Thingspeak");
  }
  client.stop();
}
