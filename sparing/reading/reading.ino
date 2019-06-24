#include <ModbusMaster.h>
#include <SoftwareSerial.h>
/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/

//============== TSS ===================
#define rx 11                                          //define what pin rx is going to be
#define tx 10                                          //define what pin tx is going to be
SoftwareSerial mySerial(rx, tx);


#define MAX485_DE      9
#define MAX485_RE_NEG  12

// instantiate ModbusMaster object
ModbusMaster node;
//======================================

//================ PH SENSOR ===========
#define Offset 0.00            //deviation compensate
#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
//======================================


//================ WeMos ===============
#define Idle 0
#define SENDING 1
#define WAITING 2
#define RECEIVING 3
//======================================


//================= TFT ================
#include <TFT.h>
#include <SPI.h>
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
//======================================

enum upload_status {
  idle = Idle,
  sending = SENDING,
  waiting = WAITING,
  receiving = RECEIVING,
};

enum upload_status state = idle;
//======================================


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
void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  //debugging purposes
  Serial.begin(19200);

  // Modbus communication runs at 19200 baud
  mySerial.begin(19200);

  // WeMos communication
  Serial1.begin(19200);

  // Modbus slave ID 1
  node.begin(10, mySerial);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  //======== TFT SETTING =======
  //initialize the library
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(1);
  TFTscreen.setRotation(0);

  //WAITING FOR WEMOS TO CONNECT TO INTERNET
  printLine("connecting....", 0, 0);
  while (!Serial1.available()) { // empty the rx buffer
    Serial.print(".");
    delay(500);
  }

  timer = millis();
  deleteLine(0, 10);
  printLine("CONNECTED", 0, 0);
  delay(1000);
  printLine("ph  : ", 0, 10);
  printLine("tss : ", 0, 20);
  printLine("0/"+ String(UPLOAD_TIME), 10, 150);
}

void loop()
{
  float tss, ph;
  String data = "";
  String temp = ""; // for display tft
  if (millis() - timer > TICK) {
    second++; //increase every TICK millisecond
    timer = millis();
    if (second == 60) {
      second = 0;
      minute++;
      Serial.println(minute);
      isUpload = false;
      isUpdate = false;
      deleteLine(150, 10);
      temp = String(minute) + "/" + String(UPLOAD_TIME);
      printLine(temp, 10, 150);
    }

    if ((minute % UPDATE_TIME == 0) && minute != 0 && !isUpdate) { // update data
      tss = getTSS(0x02);
      ph = getPH(10, SensorPin);
      data = String(ph) + ",";
      data = data + String(tss) + ",";
      Serial.println(data);
      Serial1.print("0"); // to ensure that communication is active
      isUpdate = true;
      deleteLine(10,20);
      temp = "ph  : " + String(ph); printLine(temp, 0, 10);
      temp = "tss : " + String(tss); printLine(temp, 0, 20);
    }

    if ((minute % UPLOAD_TIME == 0) && minute != 0 && !isUpload) { //send to WeMos for uploading
      deleteLine(10,20);
      temp = "ph  : " + String(ph); printLine(temp, 0, 10);
      temp = "tss : " + String(tss); printLine(temp, 0, 20);
      echo(data);
      isUpload = true;
      minute = 0;
    }
    Serial.println(second); 
  }
}

void deleteLine(int yStart, int height) {
  TFTscreen.stroke(0, 0, 0); //black
  TFTscreen.fill(0, 0, 0); //black
  TFTscreen.rect(0, yStart, 128, height);
}
void printLine(String text, int xPos, int yPos) {
  char buf[30] = "";
  text.toCharArray(buf, 30);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(buf, xPos, yPos);
}

void echo(String data) {
  while (Serial1.available()) { // empty the rx buffer
    Serial1.read();
  }
  printState();
  Serial1.print(data);
  Serial1.flush();
  state = sending;
  printState();

  while (!Serial1.available()) {
    state = waiting;
    printState();
  }

  state = receiving;
  printState();

  String rawData;
  while (Serial1.available()) {
    char in = Serial1.read();
    rawData += in;
  }
  Serial.println(rawData);
  state = idle;
}

float getTSS(byte reg) {
  uint8_t check;
  uint32_t result = 0;
  check = node.readHoldingRegisters(reg, 2);
  if (check == node.ku8MBSuccess)
  {
    //getting raw data
    uint32_t AB = node.getResponseBuffer(0x00);
    uint32_t CD = node.getResponseBuffer(0x01);

    //concat 2 x 16 bit to 32 bit;
    result = (((0xffff & CD) << 16) | (0xffff & AB));

    //debugging
    //    float t = result;
    //    byte* a;
    //    a = (byte*) &t;
    //    Serial.print(AB); Serial.print("       "); Serial.println(AB, BIN);
    //    Serial.print(CD); Serial.print("       "); Serial.println(CD, BIN);
    //    Serial.print(result); Serial.print("       "); Serial.println(result, HEX);
    //    Serial.print(a[3], HEX);
    //    Serial.print(a[2], HEX);
    //    Serial.print(a[1], HEX);
    //    Serial.print(a[0], HEX);
    //    Serial.println();
    //    Serial.println();
  }

  // convert to float
  float* temp = (float*) &result;
  return *temp;
}

float getPH(int arrLength, int analogPin) {
  float avg = 0;
  long amount = 0;
  for (int i = 0; i < arrLength; i++) {
    amount = amount + analogRead(analogPin);
  }
  avg = (float)amount / arrLength;
  float voltage = avg * 5.0 / 1024;
  float sensorValue = 3.5 * voltage + Offset;

  return sensorValue;
}


void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
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
