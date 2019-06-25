/*
   https://randomnerdtutorials.com/decoding-and-encoding-json-with-arduino-or-esp8266/
*/
#include <ArduinoJson.h> // download ArduinoJson v6 from Arduino Library Manager (go to https://arduinojson.org/v6/doc/)

#include <ESP8266WiFi.h>/
#include <WiFiClient.h>
#include <SoftwareSerial.h>

#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// =================== SERVER =======================
WiFiClient client;

// replace with your server and your SSID and password
const char* ssid = "Get Sensync";
const char* password = "makanminggu12";

// Replace with your server
const char* server = "ppkl.menlhk.go.id";

// Replace with your server port number frequently port 80 - with Node-RED you need to use port 1880
int portNumber = 80;

// Replace with your unique URL resource
const char* resource = "";

const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server
const size_t MAX_CONTENT_SIZE = 512;       // max size of the HTTP response
//=========================================================

//================= JSON DATA =============================
int data_uid = 0;                          // id from server

// data for JSON
typedef struct sparing {
  String ph;
  String cod;
  String tss;
  String nh3n;
  String debit;
} Sparing;

typedef struct onlimo {
  String suhu;
  String dhl;
  String tds;
  String salinitas;
  String DO;
  String ph;
  String turbidity;
  String kedalaman;
  String swsg;
  String nitrat;
  String nitrit;
  String amonia;
  String orp;
  String cod;
  String bod;
} Onlimo;
//=============================================

//================ Arduino MEGA ===============
//-- ESP_TX pin 14/D5 -  15/D8
//-- ESP_RX pin 12/D6 -  13/D7
SoftwareSerial ardSerial(14, 12, false, 256);
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

String rawData;

// FORMAT DATA : {"ph", "do", "cond", "tds", "salt", "gravity", "temp"}
String onlimo_param[7] = {"", "", "", "", "", "", ""};

// FORMAT DATA : {"ph", "tss", "flow"}
String sparing_param[3] = {"", "", ""};

bool isSparing;
/**======================================================= SETUP ==================================================
*/
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to initialize
  }
  Serial.println("Serial ready");
  ardSerial.begin(9600); // start port communication with arduino
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

/**==============================================================LOOP===========================================
*/
void loop() {
  while (!ardSerial.available()) {
    delay(100);
    state = idle;
    printState();
  }
  updateSerial();
}

// Open connection to the HTTP server (Node-RED running on Raspberry Pi)
bool connect(const char* hostName, int portNumber) {
  Serial.print("Connect to ");
  Serial.println(hostName);

  bool ok = client.connect(hostName, portNumber);

  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP POST request to the server
bool sendRequest(const char* host, bool IsSparing) {
  //================= ALLOCATE JSON DOCUMENT ======================
  // Inside the brackets, [capacity] is the RAM allocated to this document.
  // Don't forget to change this value to match your requirement.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  //  const int capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(18);
  const int capacity = 600; // edit this capacity
  StaticJsonDocument<capacity> doc;


  // Add the "location" object
  JsonObject data = doc.createNestedObject("data");
  data["IDStasiun"] = "UJI-14";
  data["Tanggal"] = "2019-05-13"; // must update tanggal
  data["Jam"] = String(random(0, 23)) + ":" + String(random(0, 59)) + ":" + String(random(0, 59));
  //  data["Jam"] = "17:21:31";       // must update jam

  if (IsSparing) { // for sparing
    resource = "/onlimo/uji/connect/uji_data_sparing";
    Sparing sparing = updateSparing();
    data["ph"] = sparing.ph;
    data["cod"] = sparing.cod;
    data["tss"] = sparing.tss;
    data["nh3n"] = sparing.nh3n;
    data["debit"] = sparing.debit;
  } else { // for onlimo
    resource = "/onlimo/uji/connect/uji_data_onlimo";
    Onlimo onlimo = updateOnlimo();
    data["Suhu" ] = onlimo.suhu;
    data["DHL"] = onlimo.dhl;
    data["TDS"] = onlimo.tds;
    data["Salinitas"] = onlimo.salinitas;
    data["DO"] = onlimo.DO;
    data["PH"] = onlimo.ph;
    data["Turbidity"] = onlimo.turbidity;
    data["Kedalaman"] = onlimo.kedalaman;
    data["SwSG"] = onlimo.swsg;
    data["Nitrat"] = onlimo.nitrat;
    data["Nitrit"] = onlimo.nitrit;
    data["Amonia"] = onlimo.amonia;
    data["ORP"] = onlimo.orp;
    data["COD"] = onlimo.cod;
    data["BOD"] = onlimo.bod;
  }
  // Add members
  doc["apikey"] = "uji@sensorteknologiindonesia";
  doc["apisecret"] = "fa02c613-c5e9-4534-b53e-259e83c58441";

  // Generate the minified JSON and send it to the Serial port.
  //
  serializeJson(doc, Serial);
  // The above line prints:
  // {"data":{"IDStasiun":"STASIUN-UJI","Tanggal":"2019-05-06","Jam":"14:00:00","Suhu":30,"DHL":12,"TDS":13,"Salinitas":14.12,"DO":1.86,"PH":7.6,"Turbidity":17,"Kedalaman":18,"SwSG":19,"Nitrat":20,"Nitrit":21,"Amonia":22,"ORP":23,"COD":24,"BOD":25},"apikey":"uji@sensorteknologiindonesia","apisecret":"fa02c613-c5e9-4534-b53e-259e83c58441"}

  // Start a new line
  Serial.println();

  // Generate the prettified JSON and send it to the Serial port.
  //
  serializeJsonPretty(doc, Serial);
  // The above line prints:
  //{
  //  "data": {
  //    "IDStasiun": "UJI-14",
  //    "Tanggal": "2019-05-06",
  //    .....
  //     ....
  //  },
  //  "apikey": "uji@sensorteknologiindonesia",
  //  "apisecret": "fa02c613-c5e9-4534-b53e-259e83c58441"
  //}
  //===========================================================
  Serial.println();

  Serial.print("POST ");
  Serial.println(resource);

  // Send "POST [resource] HTTP/1.1"
  client.print("POST ");
  client.print(resource);
  client.println(" HTTP/1.1");

  //Send the HTTP headers
  client.print("Host: ");
  client.println(host);
  //  client.println("Connection: close");/
  client.print("Content-Length: ");
  client.println(measureJson(doc));
  client.println("Content-Type: application/json");

  // Terminate headers with a blank line
  client.println();

  // Send JSON document in body
  serializeJson(doc, client);

  return true;
}

// Skip HTTP headers so that we are at the beginning of the response's body
bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
  }
  return ok;
}

// Close the connection with the HTTP server
void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}

// Pause for a 1 minute
void wait() {
  Serial.println("Wait 60 seconds");
  delay(60000);
}

void getResponse() {
  long interval = 2000;
  unsigned long currentMillis = millis(), previousMillis = millis();

  while (!client.available()) {
    if ( (currentMillis - previousMillis) > interval ) {
      Serial.println("Timeout");
    }
    currentMillis = millis();
  }

  char status[100] = {0};
  client.readBytesUntil('\r', status, sizeof(status)); // skip a line
  client.readBytesUntil('\r', status, sizeof(status)); // get the response in json format
  String response = status;
  response.trim();
  Serial.println(response);

  if (response.indexOf("200") == -1) { // transmission didn`t succeed
    Serial.println("Uploading Error");
    return;
  }

  // Allocate the JSON document
  // Don't forget to change this value to match your JSON document.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<200> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, response);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  serializeJsonPretty(doc, Serial); // print json data in pretty format
  // extract the value;
  data_uid = doc["rows"]["data_uid"];
  const int statusCode = doc["status"]["statusCode"];
  const char* statusDesc = doc["status"]["statusDesc"];

  Serial.println(data_uid);
  Serial.println(statusCode);
  Serial.println(statusDesc);
}

Sparing updateSparing()
// FORMAT DATA : sparing_param = {"ph", "tss", "flow"}
{
  Sparing sparing; // Struct of string
  sparing.ph = sparing_param[0];
  sparing.cod = "-";
  sparing.tss = sparing_param[1];
  sparing.nh3n = "-";
  sparing.debit = sparing_param[2];
  return sparing;
}

Onlimo updateOnlimo()
// FORMAT DATA : onlimo_param{"ph", "do", "cond", "tds", "salt", "gravity", "temp"}
{
  Onlimo onlimo; // Struct of string

  onlimo.suhu = onlimo_param[6];
  onlimo.dhl = onlimo_param[2];
  onlimo.tds = onlimo_param[3];
  onlimo.salinitas = onlimo_param[4];;
  onlimo.DO = onlimo_param[1];
  onlimo.ph = onlimo_param[0];
  onlimo.turbidity = "-";
  onlimo.kedalaman = "-";
  onlimo.swsg = onlimo_param[5];;
  onlimo.nitrat = "-";
  onlimo.nitrit = "-";
  onlimo.amonia = "-";
  onlimo.orp = "-";
  onlimo.cod = "-";
  onlimo.bod = "-";

  return onlimo;
}

void uploadToMainServer() {
  if (connect(server, portNumber)) {
    if (sendRequest(server, isSparing) && skipResponseHeaders()) {
      Serial.print("HTTP POST request finished.");
    }
    getResponse();
  }
  disconnect();
  //  wait();
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
      rawData.remove(0, 1); // remove first char
      parseData(rawData, ",");
      state = sending;
      printState();
      uploadToMainServer();
    }
  }
}

void parseData(String rawData, String delimiter)
// e.g. of rawData : "0ol1,2,3,4,5,6,7,"
{
  String temp = rawData; //copy rawData so rawData is not corrupt
  int param_cnt = 0;
  int pos = 0;
  String token;
  pos = temp.indexOf(delimiter);
  token = temp.substring(0, pos);  // copy string from index [0..pos]
  temp.remove(0, pos + delimiter.length()); // remove string from index [0..pos]
  Serial.println(token);
  Serial.println(temp);
  if (token.indexOf("sp") != -1) { // if data come from sparing sensor
    isSparing = true;
    while ( (pos = temp.indexOf(delimiter)) != -1) {
      token = temp.substring(0, pos);  // copy string from index 0 to pos
      sparing_param[param_cnt] = token;
      Serial.println(sparing_param[param_cnt]);
      param_cnt++;
      temp.remove(0, pos + delimiter.length()); // remove string from index 0 to pos
    }
    updateSparing();
  } else if (token.indexOf("ol") != -1) {// if data come from onlimo sensor
    isSparing = false;
    while ( (pos = temp.indexOf(delimiter)) != -1) {
      token = temp.substring(0, pos);  // copy string from index 0 to pos
      onlimo_param[param_cnt] = token;
      Serial.println(onlimo_param[param_cnt]);
      param_cnt++;
      temp.remove(0, pos + delimiter.length()); // remove string from index 0 to pos
    }
    updateOnlimo();
  }
}
