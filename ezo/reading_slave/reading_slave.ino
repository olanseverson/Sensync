#include "config.h"

void setup() {                                        //set up the hardware
  // == SENSOR SETTING ==
  pinMode(s1, OUTPUT);                   //Set the digital pin as output.
  pinMode(s0, OUTPUT);                   //Set the digital pin as output.
  pinMode(enable_1, OUTPUT);             //Set the digital pin as output.
  pinMode(enable_2, OUTPUT);             //Set the digital pin as output.

  //debugging purposes
  Serial.begin(115200);                                 //set baud rate for the hardware serial port_0 to 9600

  // Tentacle Shield comm (ATLAS SENSOR)
  sensor.begin(9600);                               //set baud rate for the software serial port to 9600

  // WeMos communication
  espSerial.begin(9600);                                //set baud rate for the hardware serial port_1 to 9600
  countSec = 0;

  // GSM communication
  gsm.begin(9600);                                //set baud rate for the hardware serial port_1 to 9600

  //  //WAITING FOR WEMOS TO CONNECT TO INTERNET
  //  while (!espSerial.available()) { // empty the rx buffer
  //    Serial.print(".");
  //    delay(500);
  //  }
  //  Serial.println("Connect to Wifi");

  Serial.println(F("= SENSYNC PROJECT - ONLIMO TEST-CONN ="));
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
  pinMode(LED_PIN, OUTPUT);
  Serial.print(F("Initialized... "));

  //------------------------------------------------------ TFT
  //-- Initialize HalloWing-oriented screen using a 1.8" TFT screen:
  tft.initR(INITR_HALLOWING);
  //-- Init ST7735S chip, black tab
  tft.initR(INITR_BLACKTAB);
  //-- tft rotation function
  tft.setRotation(1); //rotate 270 degree
  //------------------------------------------------------ RTC
  if (!rtc_isrunning()) {
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

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {                                         //here we go...
  if (millis() - timer > TICK) {
    sec++; //increase every TICK millisecond
    timer = millis();
    NextLoop();
    printTime();
    if (sec == 60) {
      sec = 0;
      min++;
      Serial.println(min);
      isUpload = false;
      isUpdate = false;
      displayTime();
    }

    if ((min % UPDATE_TIME == 0) && min != 0 && !isUpdate) { // update data
      //      espSerial.print("-"); // to ensure that communication is active
      //      espSerial.flush();
      //      isUpdate = true;
    }

    if ((min % UPLOAD_TIME == 0) && min != 0 && !isUpload) {
      //      echo(rawData); //send to WeMos for uploading
      displayTFT(); //-- Display TFT
      displayStat("Sampling...");
      rawData = getAllData(); // get rawData in CSV format (e.g. "1,2,3,4,5,6,7,")
      parseData(rawData);
      displayValue2TFT();
      displayTFT(); //-- Display TFT
      displayStat("Done");
      Serial.println(rawData);

      SendTextMessage(number);
      isUpload = true;
      min = 0;

    }
    Serial.println(sec);
  }
}

void displayTFT() { //-- Display TFT
  displayClear();
  displayTime();
  displayPV();
}

void Done() {
  displayTime();
  Serial.print(F("\n$"));
  idleTime = 0;     // reset Idle Time
}


void NextLoop() {
  tft.fillRoundRect(0, 113, tft.width(), 123, 10, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 115);  tft.println("NextLoop:");
  
  tft.setTextColor(ST77XX_MAGENTA);
  tft.fillRoundRect(70, 113, 120, 123, 10, ST77XX_BLACK);
  tft.setCursor(75, 115);  tft.println(sec + min * 60);
  tft.setCursor(88, 115);  tft.println("/");
  tft.setCursor(95, 115);  tft.println(UPLOAD_TIME * 60);

}
