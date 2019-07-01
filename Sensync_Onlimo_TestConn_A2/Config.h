//------------------------------------------------------------ Debug Message
#define ERROR(x)      Serial.print("\nERROR: "); Serial.println(x);
#define PRINT(x)      Serial.print(x);
#define PRINTLN(x)    Serial.println(x);
#define On            HIGH
#define Off           LOW
#define menit         60
#define BUZ_PIN       10
#define LED_PIN       13

int idleTime = 0;
int nxtlup = 5*menit;

//------------------------------------------------------------ SENSOR SIM800
#define SIM800port Serial2
const String SMScenter = "082213346308";
const String IDSTASIUN = "UJI-14";

//------------------------------------------------------------ ESP8266 Config
#include <SoftwareSerial.h>
//-- Serial for ESP8266 Wemos D1 mini
//-- Use Software Serial
#define ESP_TX  10
#define ESP_RX  11
#define ESP_RST 6
#define ESP_INT 12
//SoftwareSerial espSerial(ESP_TX, ESP_RX);
//-- Use Hardware Serial
#define espSerial Serial3

//-- ESP Buffer
char bufferRX[200];
int indexBufferRX;
bool statConnect;

//-- WiFi credentials
//const String wifiSSID = "Get Sensync";
const String wifiSSID = "SENSYNCBASECRBN";
const String wifiPASS = "makanminggu12";

//ESPAT ESP8266(wifiSSID, wifiPASS); 

//-- Server link
const String id         = "xx-bplhd-jabar-dpk";
const String url_server = "http://server.getsensync.com/proc/bplhd_jabar/base/process.php?id=";
const String url_login  = "http://server.getsensync.com/proc/bplhd_jabar/base/start.php?id=";
const String dasboard   = "http://server.getsensync.com/proc/bplhd_jabar/base/tesprocess.php";
const String url_start  = "http://server.getsensync.com/proc/bplhd_jabar/base/start.php?id=";
const String server     = "http://server.getsensync.com";
const String url_upload = "/proc/bplhd_jabar/base/process.php?id=";
 
//------------------------------------------------------------ RTC
#include <DS3231.h>
#include <Wire.h>

String timeStamp;
const int RTC_DS3231 = 0x68;
bool Century=false;
bool h12, PM, ADy, A12h, Apm;
byte ADay, AHour, AMinute, ASecond, ABits;
byte second,minute,hour,weekday,day,month,year = 0;
const char* days[] =
{"SUN", "MON", "THU", "WED", "Kamis", "Jumat", "Sabtu"};
//{"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
DS3231 rtc;

//------------------------------------------------------------ TFT
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS        53 // Chip select
#define TFT_RST       48 // Display reset
#define TFT_DC        49 // Display data/command select

#define TFT_SCK       52
#define TFT_SDA       51
#define TFT_BACKLIGHT  22 // Display backlight pin

//-- 1.8" TFT with ST7735
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//------------------------------------------------------------ SENSOR ATLAS
#include <SoftwareSerial.h>

#define atlas_rx 11                   //define what pin rx is going to be
#define atlas_tx 10                   //define what pin tx is going to be
SoftwareSerial SensorPort(atlas_rx, atlas_tx);  //define how the soft serial port is going to work

const int s0 = 7;               //Arduino pin 7 to control pin S0
const int s1 = 6;               //Arduino pin 6 to control pin S1
const int enable_1 = 5;         //Arduino pin 5 to control pin E on shield 1
const int enable_2 = 4;         //Arduino pin 4 to control pin E on shield 2
static int countSec;            //delay(s) for sending data to WeMOS (uploading to inet)
unsigned long serverTime = 0 ;
boolean sensor_string_complete = false;   //have we received all the data from the Atlas Scientific product
int channel;                    //INT pointer for channel switching - 0-7 serial, 8-127 I2C addresses
String rawData="", Datas="", GSMData="";

String sensorValue[7];//sensorValue in format ["pH", "DO", "COND", "TDS", "SALT", "SWSG", "TEMP"]
float suhu_, dhl_, tds_, sal_, do_, ph_, tur_, cod_;
float dlm_, swsg_, nitrit_, nitrat_, amon_, orp_, bod_;  

//------------------------------------------------------------- SWITCH
#define SWITCH_PIN 2
bool isFull = false;
