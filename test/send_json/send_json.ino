/*
   https://randomnerdtutorials.com/decoding-and-encoding-json-with-arduino-or-esp8266/
*/
#include <ArduinoJson.h> // download ArduinoJson v6 from Arduino Library Manager (go to https://arduinojson.org/v6/doc/)

#include <ESP8266WiFi.h>/
#include <WiFiClient.h>

#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

WiFiClient client;

// replace with your channel’s thingspeak API key and your SSID and password
const char* ssid = "Get Sensync";
const char* password = "makanminggu12";


// Replace with your server
const char* server = "ppkl.menlhk.go.id";

// Replace with your server port number frequently port 80 - with Node-RED you need to use port 1880
int portNumber = 80;

// Replace with your unique URL resource
const char* resource = "/onlimo/uji/connect/uji_data_onlimo";

const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server
const size_t MAX_CONTENT_SIZE = 512;       // max size of the HTTP response

int data_uid = 0;                          // id from server


// ARDUINO entry point #1: runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to initialize
  }
  Serial.println("Serial ready");
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
  delay(1000);

  if (connect(server, portNumber)) {
    if (sendRequest(server, resource) && skipResponseHeaders()) {
      Serial.print("HTTP POST request finished.");
    }
    getResponse();
  }
  disconnect();
  wait();
}

// ARDUINO entry point #2: runs over and over again forever
void loop() {
  //  if (connect(server, portNumber)) {
  //    if (sendRequest(server, resource) && skipResponseHeaders()) {
  //      Serial.print("HTTP POST request finished.");
  //    }
  //  }
  //  disconnect();
  //  wait();
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
bool sendRequest(const char* host, const char* resource) {
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
  data["Tanggal"] = "2019-05-12";
  data["Jam"] = "17:21:27";
  data["Suhu" ] = 32;
  data["DHL"] = 1.3;
  data["TDS"] = 1.2;
  data["Salinitas"] = 21.2;
  data["DO"] = 1.86;
  data["PH"] = 7.6;
  data["Turbidity"] = 17;
  data["Kedalaman"] = 18;
  data["SwSG"] = 19;
  data["Nitrat"] = 20;
  data["Nitrit"] = 21;
  data["Amonia"] = 22;
  data["ORP"] = 23;
  data["COD"] = 24;
  data["BOD"] = 25;

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
  //    "Jam": "14:00:00",
  //    "Suhu": 30,
  //    "DHL": 12,
  //    "TDS": 13,
  //    "Salinitas": 14.12,
  //    "DO": 1.86,
  //    "PH": 7.6,
  //    "Turbidity": 17,
  //    "Kedalaman": 18,
  //    "SwSG": 19,
  //    "Nitrat": 20,
  //    "Nitrit": 21,
  //    "Amonia": 22,
  //    "ORP": 23,
  //    "COD": 24,
  //    "BOD": 25
  //  },
  //  "apikey": "uji@sensorteknologiindonesia",
  //  "apisecret": "fa02c613-c5e9-4534-b53e-259e83c58441"
  //}
  //===========================================================


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
  //  Serial.println(response);

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
