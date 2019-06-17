
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

//==== TFT ====
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

//==== TENTACLE SHIELD ====
#define VCC_PIN  13                                     // VCC alternatif to WiMos VCC 5 V
#define rx 11                                          //define what pin rx is going to be
#define tx 10                                          //define what pin tx is going to be
#define UPLOAD_TIME 15                                  // period for uploading to server (in minute(s))

SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work

const int s0 = 7;                        //Arduino pin 7 to control pin S0
const int s1 = 6;                        //Arduino pin 6 to control pin S1
const int enable_1 = 5;                   //Arduino pin 5 to control pin E on shield 1
const int enable_2 = 4;                  //Arduino pin 4 to control pin E on shield 2

static int countSec;                       //delay(s) for sending data to WeMOS (uploading to inet)
unsigned long serverTime = 0 ;

String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float DO;                                             //used to hold a floating point number that is the DO
int channel;                             //INT pointer for channel switching - 0-7 serial, 8-127 I2C addresses

char sensordata[32];                     //A 30 byte character array to hold incoming data from the sensors
String stamp_type;                       // hold the name / type of the stamp
char stamp_version[4];                   // hold the version of the stamp
boolean answerReceived;                  // com-functions store here if a connection-attempt was successful

void setup() {                                        //set up the hardware
  // == SENSOR SETTING ==
  pinMode(s1, OUTPUT);                   //Set the digital pin as output.
  pinMode(s0, OUTPUT);                   //Set the digital pin as output.
  pinMode(enable_1, OUTPUT);             //Set the digital pin as output.
  pinMode(enable_2, OUTPUT);             //Set the digital pin as output.
  pinMode(VCC_PIN, OUTPUT);             //Set the digital pin as output.

  digitalWrite(VCC_PIN, HIGH);
  Serial.begin(9600);                                 //set baud rate for the hardware serial port_0 to 9600
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  Serial1.begin(9600);                                //set baud rate for the hardware serial port_1 to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  countSec = 0;

  // ==TFT SETTING ==
  //initialize the library
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.setTextSize(1);
  TFTscreen.setRotation(2);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text("Starting .... ", 0, 0);
}
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {                                         //here we go...
  if ((millis() - serverTime) > 1000) { // increase every 1 second
    if (countSec == UPLOAD_TIME * 60 ) {
      countSec = 0;
      String allData = getAllData();
      Serial1.println(allData);
      Serial.println(allData);
      delay(1000);
      printToTFT(allData);
    }
    serverTime = millis();
    countSec++;
    Serial.println(countSec);
  }
}
//=======================================================================


String getAllData() {
  String allData = "";
  TFTscreen.background(0, 0, 0); // clear the screen
  char val[7][10] = {"", "", "", "", "", "", ""}; // save the  sensor reading
  TFTscreen.text("DATA :", 0, 0);
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

void printToTFT(String data)
//print all raw data to more readable format to TFT Display
{
  const char param[7][6] = {"pH", "DO", "COND", "TDS", "SALT", "SWSG", "TEMP"};
  char val[7][10] = {"", "", "", "", "", "", ""};
  String temp = data;
  for (int i = 0; i < 7; i++) {
    int idx = temp.indexOf(',');
    temp.toCharArray(val[i], idx + 1);
    if (idx != -1) {
      temp.remove(0, idx + 1);
    }
  }
  // print to tft display
  for (int i = 0; i < 7; i++) {
    TFTscreen.text(param[i], 0, (i + 1) * 10);
    TFTscreen.text(val[i], 35, (i + 1) * 10);
  }
}


String requestToSensor(String inputstring) {
  sensorstring = "";                                //clear the string
  myserial.print(inputstring);                      //send that string to the Atlas Scientific product
  myserial.print('\r');                             //add a <CR> to the end of the string
  delay(1000);

  while (myserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)myserial.read();              //get the char we just received
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
