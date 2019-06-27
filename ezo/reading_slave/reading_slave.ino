
//This code was written to be easy to understand.
//Modify this code as you see fit.
//This code will output data to the Arduino serial monitor.
//Type commands into the Arduino serial monitor to control the D.O. circuit.
//This code was written in the Arduino 1.8.3 IDE
//An Arduino UNO was used to test this code.
//This code was last tested 6/2017

//https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/


#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
// include TFT and SPI libraries
#include <TFT.h>
#include <SPI.h>

#define DEBUG_PORT Serial
//=========== TFT ====================
// pin : 3.3 52 51 49 48 53 gnd vcc
// tft 1.8 inch 128x160 pixel
// pin definition for Arduino MEGA
// MOSI 51   11(UNO)
// SCK 52    13(UNO)
#define cs   53  // 10 IN UNO
#define dc   49
#define rst  48
// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
int tftXPos;
unsigned long tftTime = 0;

//========== TENTACLE SHIELD ==========
#define rx 11                                          //define what pin rx is going to be
#define tx 10                                          //define what pin tx is going to be
SoftwareSerial sensor(rx, tx);                      //define how the soft serial port is going to work
const int s0 = 7;                        //Arduino pin 7 to control pin S0
const int s1 = 6;                        //Arduino pin 6 to control pin S1
const int enable_1 = 5;                   //Arduino pin 5 to control pin E on shield 1
const int enable_2 = 4;                  //Arduino pin 4 to control pin E on shield 2
static int countSec;                       //delay(s) for sending data to WeMOS (uploading to inet)
unsigned long serverTime = 0 ;
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
int channel;                             //INT pointer for channel switching - 0-7 serial, 8-127 I2C addresses
String rawData = "";

// store sensorValue in format "pH", "DO", "COND", "TDS", "SALT", "SWSG", "TEMP"
char sensorValue[7][10] = {"", "", "", "", "", "", ""};
//=====================================

//============== TIMING ===============
#define TICK 1000 // in ms
#define UPLOAD_TIME   15
#define UPDATE_TIME   1
int second = 0;        // second counter
int minute = 0;        // second counter
long timer = 0;  // storage for timer to upload
bool isUpdate = false;
bool isUpload = false;
//======================================

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

//=============== SIM800 ================
//Create software serial object to communicate with SIM800L
SoftwareSerial gsm(12, 13); //SIM800L Tx & Rx is connected to Arduino #3 & #2
String number = "\"+6282130310254\"";
String GSMData = "";
boolean gprsConnectionSetup = false;

//helper variables for waitUntilResponse() function
static long maxResponseTime = 5000;
unsigned long lastTimeGsm = 0;
String responseGsm = "";
//======================================

//======================= RTC 1307 =============
#include <Wire.h>
const int DS1307 = 0x68; // Address of DS1307 see data sheets
const char* days[] =
{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* months[] =
{"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
// Initializes all values:
byte sec = 0;
byte min = 0;
byte hour = 0;
byte weekday = 0;
byte monthday = 0;
byte month = 0;
byte year = 0;
String jam, tanggal;
//======================================
void setup() {                                        //set up the hardware
  // == SENSOR SETTING ==
  pinMode(s1, OUTPUT);                   //Set the digital pin as output.
  pinMode(s0, OUTPUT);                   //Set the digital pin as output.
  pinMode(enable_1, OUTPUT);             //Set the digital pin as output.
  pinMode(enable_2, OUTPUT);             //Set the digital pin as output.

  //debugging purposes
  Serial.begin(115200);                                 //set baud rate for the hardware serial port_0 to 9600

  // Tentacle Shield comm
  sensor.begin(9600);                               //set baud rate for the software serial port to 9600

  // WeMos communication
  Serial1.begin(9600);                                //set baud rate for the hardware serial port_1 to 9600

  countSec = 0;

  //==================== RTC setup ===================
  Wire.begin();
  if (! rtc_isrunning())
    Serial.println(F("Unable to sync with the RTC"));
  else
    Serial.println(F("RTC has set the system time"));
  //=================================================
  printTime();

  // GSM communication
  gsm.begin(9600);                                //set baud rate for the hardware serial port_1 to 9600

  // ===========TFT SETTING ============
  //initialize the library
  TFTscreen.begin();
  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.setTextSize(1);
  TFTscreen.setRotation(1);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text("Starting .... ", 0, 0);
  //====================================

  //WAITING FOR WEMOS TO CONNECT TO INTERNET
  while (!Serial1.available()) { // empty the rx buffer
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connect to Wifi");
  rawData.reserve(75);
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {                                         //here we go...
  if (millis() - timer > TICK) {
    second++; //increase every TICK millisecond
    timer = millis();
    if (second == 60) {
      second = 0;
      minute++;
      Serial.println(minute);
      isUpload = false;
      isUpdate = false;
    }

    if ((minute % UPDATE_TIME == 0) && minute != 0 && !isUpdate) { // update data
      rawData = getAllData(); // get rawData in CSV format (e.g. "1,2,3,4,5,6,7,")
      Serial1.print("-"); // to ensure that communication is active
      Serial1.flush();
      Serial.println(rawData);
      isUpdate = true;
    }

    if ((minute % UPLOAD_TIME == 0) && minute != 0 && !isUpload) { //send to WeMos for uploading
      echo(rawData);
      SendTextMessage(number);
      isUpload = true;
      minute = 0;
    }
    Serial.println(second);
  }
}
//=======================================================================
//void loop() {
//  String rawData = "";
//  bool isSparing = false;
//  if (isSparing) {
//    rawData = "0sp,1,2,3,";
//  } else {
//    rawData = "0ol,1,2,3,4,5,6,7,";
//  }
//  echo(rawData);
//  delay(60000);
//}

void echo(String data) {
  while (Serial1.available()) { // empty the rx buffer
    Serial1.read();
  }
  printState();
//  jam = String(random(0, 23)) + ":" + String(random(0, 59)) + ":" + String(random(0, 59));
//  tanggal = "2019-05-06";
  readTime();
  String test = tanggal + "," + jam + "," + data;
  Serial.println(test);
  Serial1.print(test);
  Serial1.flush();
  state = sending;
  printState();

  while (!Serial1.available()) {
    state = waiting;
    printState();
  }

  state = receiving;
  printState();

  String resp = "";
  while (Serial1.available()) {
    char in = Serial1.read();
    resp += in;
    delay(10);
  }
  Serial.println(resp);
  state = idle;
}

String getAllData()
//OUTPUT(e.g.): "1,2,3,4,5,6,7,"
{
  sensor.listen(); // "sensor" serial port is active
  String allData = "";
  char val[7][10] = {"", "", "", "", "", "", ""}; // save the  sensor reading
  int count = 0;
  for (int i = 0; i < 4; i++)
  {
    channel = i;
    change_serial_mux_channel();
    //    Serial.print(info(requestToSensor("i")) + " : ");     // FOR DEBUGGING
    String value = readUntil('*', requestToSensor("r"));
    value.trim();
    //    Serial.println(value);                                //FOR DEBUGGING
    allData += value + ",";
  }
  return allData;
}

String requestToSensor(String inputstring) {
  String sensorstring = "";                                //clear the string
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  sensor.print(inputstring);                      //send that string to the Atlas Scientific product
  sensor.print('\r');                             //add a <CR> to the end of the string
  delay(1000);

  while (sensor.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)sensor.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }


  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    //    Serial.println(sensorstring);                     //send that string to the PC's serial monitor
    /*                                                //uncomment this section to see how to convert the DO reading from a string to a float
      if (isdigit(sensorstring[0])) {                   //if the first character in the string is a digit
      DO = sensorstring.toFloat();                    //convert the string to a floating point number so it can be evaluated by the Arduino
      if (DO >= 6.0) {                                //if the DO is greater than or equal to 6.0
        Serial.println("high");                       //print "high" this is demonstrating that the Arduino is evaluating the DO as a number and not as a string
      }
      if (DO <= 5.99) {                               //if the DO is less than or equal to 5.99
        Serial.println("low");                        //print "low" this is demonstrating that the Arduino is evaluating the DO as a number and not as a string
      }
      }
    */

    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
    return sensorstring;
  } else {
    return "";
  }
}

void change_serial_mux_channel() {           // configures the serial muxers depending on channel.

  switch (channel) {                         //Looking to see what channel to open

    case 0:                                  //If channel==0 then we open channel 0
      digitalWrite(enable_1, LOW);           //Setting enable_1 to low activates primary channels: 0,1,2,3
      digitalWrite(enable_2, HIGH);          //Setting enable_2 to high deactivates secondary channels: 4,5,6,7
      digitalWrite(s0, LOW);                 //S0 and S1 control what channel opens
      digitalWrite(s1, LOW);                 //S0 and S1 control what channel opens
      break;                                 //Exit switch case

    case 1:
      digitalWrite(enable_1, LOW);
      digitalWrite(enable_2, HIGH);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, LOW);
      break;

    case 2:
      digitalWrite(enable_1, LOW);
      digitalWrite(enable_2, HIGH);
      digitalWrite(s0, LOW);
      digitalWrite(s1, HIGH);
      break;

    case 3:
      digitalWrite(enable_1, LOW);
      digitalWrite(enable_2, HIGH);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);
      break;

    case 4:
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, LOW);
      digitalWrite(s1, LOW);
      break;

    case 5:
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, LOW);
      break;

    case 6:
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, LOW);
      digitalWrite(s1, HIGH);
      break;

    case 7:
      digitalWrite(enable_1, HIGH);
      digitalWrite(enable_2, LOW);
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);
      break;

    default:
      digitalWrite(enable_1, HIGH);   //disable soft serial
      digitalWrite(enable_2, HIGH);   //disable soft serial
  }
}

String info(String sensorType) {
  if (sensorType.indexOf("pH") != -1) {
    return "EZO pH (pH)";
  } else if (sensorType.indexOf("DO") != -1) {
    return "EZO DO (mg/L)";
  } else if (sensorType.indexOf("EC") != -1) {
    return "EZO EC (EC(uS/cm);TDS(ppm);S(ppt);SG)";
  } else if (sensorType.indexOf("RTD") != -1) {
    return "EZO RTD (degC)";
  } else {
    return "undefined";
  }
}

String readUntil(char delimiter, String input) {
  String data = input;
  int i = data.indexOf(delimiter);
  if (i != -1) {
    data.remove(i);
  }
  return data;
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

void parseData(String data)
// data format : "1,2,3,4,5,6,7,"
// parsing data from CSV to array
{
  String temp = data;
  for (int i = 0; i < 7; i++) {
    int idx = temp.indexOf(',');
    temp.toCharArray(sensorValue[i], idx + 1);
    if (idx != -1) {
      temp.remove(0, idx + 1);
    }
  }
}

void updateGSMData(String tanggal, String jam) {
  parseData(rawData); // turn data from CSV to array format

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
  updateGSMData(tanggal, jam);
  gsm.println(GSMData); //the content of the message
  updateGSMSerial();
  gsm.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  updateGSMSerial();
  gsm.println();
  Serial.println("Text Sent.");
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

void readResponse() {
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


//--------------------------------------------------- Test if RTC running
uint8_t rtc_isrunning(void) {
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0));
  Wire.endTransmission();

  Wire.requestFrom(DS1307, 1);
  uint8_t ss = Wire.read();
  Serial.println(ss);
  return !(ss >> 7);
} //rtc_isrunning()


//--------------------------------------------------- BCD convertion
byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}
byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

void readTime() {
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0));
  Wire.endTransmission();
  Wire.requestFrom(DS1307, 7);
  sec = bcdToDec(Wire.read());
  min = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read());
  weekday = bcdToDec(Wire.read());
  monthday = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());

  tanggal = "";
  // add tanggal in yyyy-month-day  format( month and day must in 2 digit)
  tanggal = "20" + String(year) + "-";
  if (month <10){
    tanggal += "0";
  }
  tanggal = tanggal + month + "-";
  if (monthday <10){
    tanggal += "0";
  }
  tanggal +=monthday;

  // add tanggal in hh:mm:ss  format( minute and second must in 2 digit)
  jam = "";
  if (hour<10){
    jam+= "0";
  }
  jam = jam + hour + ":";
  if (min<10){
    jam += "0";
  }
  jam = jam + min + ":00";
} // readTime()

void printTime() {
  char buffer[3];
  const char* AMPM = 0;
  readTime();
  Serial.print(days[weekday - 1]);
  Serial.print(F(" "));
  Serial.print(monthday);
  Serial.print(F("/"));
  Serial.print(months[month - 1]);
  Serial.print(F("/20"));
  Serial.print(year);
  Serial.print(F(" "));
  if (hour > 12) {
    hour -= 12;
    AMPM = " PM";
  }
  else AMPM = " AM";
  Serial.print(hour);
  Serial.print(F(":"));
  sprintf(buffer, "%02d", minute);
  Serial.print(buffer);
  Serial.println(AMPM);
  Serial.println(tanggal);
  Serial.println(jam);
}
