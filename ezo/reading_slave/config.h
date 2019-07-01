//------------------------------------------------------------ Debug Message
#define DEBUG_PORT    Serial
#define LED_PIN       13
#define ERROR(x)      Serial.print("\nERROR: "); Serial.println(x);
#define PRINT(x)      Serial.print(x);
#define PRINTLN(x)    Serial.println(x);
int idleTime = 0;

//============== TIMING ===============
#define TICK 1000 // in ms
#define UPLOAD_TIME   2
#define UPDATE_TIME   1
int sec = 0;        // second counter
int min = 0;        // second counter
long timer = 0;  // storage for timer to upload
bool isUpdate = false;
bool isUpload = false;
//======================================

//------------------------------------------------------------ TFT
//https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

// pin : 3.3 52 51 49 48 53 gnd vcc
#define TFT_CS        53 // Chip select
#define TFT_RST       48 // Display reset
#define TFT_DC        49 // Display data/command select

#define TFT_SCK       52
#define TFT_SDA       51
#define TFT_BACKLIGHT  22 // Display backlight pin

//-- 1.8" TFT with ST7735
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//========== TENTACLE SHIELD ==========
#include <SoftwareSerial.h>
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

//================ WeMos ===============
#define espSerial Serial1

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

String timeStamp;
const int RTC_DS3231 = 0x68;
bool Century=false;
bool h12, PM, ADy, A12h, Apm;
byte ADay, AHour, AMinute, ASecond, ABits;
byte second,minute,hour,weekday,day,month,year = 0;
const char* days[] =
{"SUN", "MON", "THU", "WED", "THU", "FRI", "SAT"};
//{"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
String tanggal, jam;
//======================================
