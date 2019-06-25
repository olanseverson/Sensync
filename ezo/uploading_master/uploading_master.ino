//http://www.esp8266learning.com/dht11-sensor-data-to-thingspeak-using-a-wemos-d1.php
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#define LED_PIN LED_BUILTIN   //D4

//-- ESP_TX pin 14/D5 -  15/D8
//-- ESP_RX pin 12/D6 -  13/D7
SoftwareSerial ardSerial(14, 12, false, 256);

// replace with your channel’s thingspeak API key and your SSID and password
String apiKey = "3JEXAHO11O1HUNA3";
const char* ssid = "Get Sensync";
const char* password = "makanminggu12";
const char* serverThingspeak = "api.thingspeak.com";

const String url_upload   = "http://server.getsensync.com/proc/onlimo/process.php?";
const String param[7] = {"ph", "do", "cond", "tds", "salt", "gravity", "temp"};

WiFiClient client;

String rawData;
int ledState = LOW;
bool isUpdated = false;

//================ WeMos ===============
#define Idle 0
#define SENDING 1
#define WAITING 2
#define RECEIVING 3

enum upload_status {
  idle = Idle,
  sending = SENDING,
  waiting = WAITING,
  receiving = RECEIVING,
};
enum upload_status state = idle;
//======================================

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  ardSerial.begin(9600);
  delay(500);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  ardSerial.print('\n'); // send a char to start program in Arduino
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  while (!ardSerial.available()) {
    delay(100);
    state = idle;
    printState();
  }
  updateSerial();
}
//=======================================================================

void updateSerial()
{
  delay(100);
  if (ardSerial.available())
  {
    rawData = "";
    state = receiving;
    printState();
    while (ardSerial.available()) {
      char in = ardSerial.read();
      rawData += in;
    }
    state = waiting;
    printState();
    Serial.println(rawData);//Forward what Software Serial received to Serial Port

    if (rawData != "0") {
      rawData.remove(0,1);
      String data = parseData(rawData, ",", false);
      state = sending;
      printState();
      Serial.println(data);
      uploadToServer(data);
    }
  }
}

String parseData(String rawData, String delimiter, bool isToThingspeak) {
  String temp = rawData; //copy rawData so rawData is not disappear
  char count = '1';
  int param_cnt = 0;
  String outputData = ""; // data to be returned
  int pos = 0;
  String token;
  while ( (pos = temp.indexOf(delimiter)) != -1) {
    token = temp.substring(0, pos);  // copy string from index 0 to pos
    if (isToThingspeak) {
      outputData = outputData + "&field" + count + "=" + token;
    }
    else {
      outputData = outputData + "&" + param[param_cnt] + "=" + token;
    }
    param_cnt++;
    count ++;
    temp.remove(0, pos + delimiter.length()); // remove string from index 0 to pos
  }
  return outputData;
}

void uploadToThingspeak(String data) {
  if (client.connect(serverThingspeak, 80)) {
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

void uploadToServer(String data) {

  String datana = url_upload + data;

  HTTPClient http;  //Declare object of class HTTPClient
  http.begin( datana ); //Specify request destination
  int httpCode    = http.GET(); //Send the request
  String payload  = http.getString(); //Get the response payload
  payload.trim();

  Serial.println(datana);
  Serial.print("HTTP return code: ");
  Serial.println(httpCode);   //Print HTTP return code
  Serial.print(F("Response: "));
  Serial.println(payload);    //Print request response payload
  yield();
  http.end();     //Close connection

  delay(1000);
  ardSerial.print(httpCode);
  ardSerial.flush();
}

void printState() {
  switch (state) {
    case Idle:
      Serial.println("IDLE");
      break;
    case SENDING:
      Serial.println("SENDING");
      break;
    case WAITING:
      Serial.println("WAITING");
      break;
    case RECEIVING:
      Serial.println("RECEIVING");
      break;
  }
}
