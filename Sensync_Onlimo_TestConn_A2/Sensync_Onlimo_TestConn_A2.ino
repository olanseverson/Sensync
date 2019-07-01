/**************************************************************************
  Sensync Base - Xsense
  Board   : ArduinoMega Pro
  Display : Colour TFT 1.8" ST7735S
  Modem   : ESP8266
  Sensor  : BMP280, Sensirion, Alphasense
  
 **************************************************************************/
#include "Config.h"

void setup(void) {
  Wire.begin();
  Serial.begin(115200);
  espSerial.begin(9600);
  SIM800port.begin(9600);
  SensorPort.begin(9600);  // Modbus communication runs at 19200baud
  pinMode(s1, OUTPUT);                   //Set the digital pin as output.
  pinMode(s0, OUTPUT);                   //Set the digital pin as output.
  pinMode(enable_1, OUTPUT);             //Set the digital pin as output.
  pinMode(enable_2, OUTPUT);             //Set the digital pin as output.  
  delay(1000);
  Serial.println(F("= SENSYNC PROJECT - ONLIMO TEST-CONN ="));
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
  pinMode(LED_PIN, OUTPUT);
  Serial.print(F("Initialized... "));

  //----------------------------------------------------- SWITCH
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  
  //------------------------------------------------------ TFT
  //-- Initialize HalloWing-oriented screen using a 1.8" TFT screen:
  tft.initR(INITR_HALLOWING);   
  //-- Init ST7735S chip, black tab
  tft.initR(INITR_BLACKTAB);    
  //-- tft rotation function
  tft.setRotation(1); //rotate 270 degree
  //------------------------------------------------------ RTC
  if (!rtc_isrunning()){ 
    PRINTLN(F("\nPress [ENTER] to sync with the RTC..."));
    setTimeRTC();
  }

  //------------------------------------------------------ TFT
  TFTprintStartUp();
  displayTFT(); //-- Display TFT
  Serial.println("Done");
  printTime();

  //------------------------------------------------------ Sensor
  rawData.reserve(75);
}

//------------------------------------------------------------ Main Loop
void loop() {
  //-- Start Sampling
  separator(50);
  Serial.println(F("Start Sampling..."));
  displayTFT(); //-- Display TFT
  displayStat("Sampling...");
  Heartbeat();  //-- Blink LED
  readSensor();
  displayValue2TFT();
  displayTFT(); //-- Display TFT
  displayStat("Done");
  Heartbeat();  //-- Blink LED
  UploadTestConn();//-- Upload
  separator(50);
  NextLoop();
}

void displayTFT(){  //-- Display TFT
  displayClear();
  displayTime();
  displayPV();
}

void UploadTestConn(){
  post2Server();
  sms2Server();  
}

void NextLoop(){
  tft.fillRoundRect(0, 113, tft.width(), 123, 10, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  Serial.print(F("$NextLoop in ")); Serial.print(nxtlup); Serial.println(F(" seconds"));
  tft.setCursor(5, 115);  tft.println("NextLoop:");
  for(int nxtloop = nxtlup; nxtloop>0; nxtloop--){
    tft.setTextColor(ST77XX_MAGENTA);
    tft.fillRoundRect(70, 113, 120, 123, 10, ST77XX_BLACK);
    tft.setCursor(75,115);  tft.println(nxtloop);
    if(nxtloop%10==0){displayTime();}
    String inData = ""; 
    while(Serial.available()){
      inData = Serial.readStringUntil('\n');
    }
    if(inData.indexOf("start")!=-1)   nxtloop=0;
    if(inData.indexOf("upload")!=-1)  UploadTestConn();
    if(inData.indexOf("setrtc")!=-1)  setTimeRTC();
    if(inData.indexOf("help")!=-1)    Help();
    digitalWrite(LED_PIN, digitalRead(LED_PIN)^1);
    delay(1000);
    updateSwitchStatus();
  }
  Serial.println();
}

//--------------------------------------------------- Miscl.
void Help(){
  Serial.print(F("\nCommand:\nstart  upload  setrtc\n$"));  
}

void Buzzer(){
  
}

void Done() {
  displayTime();
  Serial.print(F("\n$"));
  idleTime = 0;     // reset Idle Time
}

void separator(int des) {
  for(int i=0; i<des; i++){PRINT(F("-"));} 
  PRINTLN();
}

//--------------------------------------------------- Blink LED
void Heartbeat(){
    digitalWrite(LED_PIN, LOW);
    delay(50) ;  
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(25) ;  
    digitalWrite(LED_PIN, HIGH); 
    delay(1200);
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
}

void updateSwitchStatus(){
  isFull =  digitalRead(SWITCH_PIN);
  Serial.println(isFull);
}
