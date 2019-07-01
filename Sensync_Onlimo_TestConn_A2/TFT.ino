//--------------------------------------------------- Display Time
void displayTime() {
  tft.drawLine(0, 0, tft.width(), 0, ST77XX_WHITE);  
  tft.fillRoundRect(0, 3, 99, 40, 10, ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_WHITE);
  
  readTime();
  //-- display TIME  
  tft.setCursor(5,5);
  if(hour<10){tft.print(F("0"));}
  tft.print(hour);
  tft.print(F(":"));
  if(minute<10){tft.print(F("0"));}
  tft.println(minute);
  
  //-- display DATE  
  tft.setTextSize(0);
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(5,35);
  tft.print(days[weekday-1]);
  tft.print(F(" "));
  if(day<10){tft.print(F("0"));}
  tft.print(day);
  tft.print(F("/"));
  if(month<10){tft.print(F("0"));}
  tft.print(month);
  tft.print(F("/20"));
  tft.print(year);
}

//------------------------------------------------------------ Display Temperature
void displayPV(){
  tft.drawLine(0, 45, tft.width(), 45, ST77XX_WHITE);  
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_BLUE);
  tft.setCursor(100, 5);    
  tft.println("PV: 12.1V");
  tft.setCursor(100, 15);    
  tft.println("I : 0.56A");
  tft.setCursor(100, 25);    
  tft.println("P : 6.78W");
  drawStringCenter(80, 80,"SENSYNC",3,ST77XX_CYAN);
}

void displayValue2TFT(){
  displaySensorVal("TEMP ", suhu_);
  displaySensorVal("pH ", ph_);
  displaySensorVal("DO ", do_);
  displaySensorVal("COND ", dhl_);
  displaySensorVal("TDS ", tds_);
  displaySensorVal("SALT ", sal_);
  displaySensorVal("SWSG ", swsg_);
}

//------------------------------------------------------------ Display Sensor Value
void displaySensorVal(String labelna, float valuena){
  int posX=80, posY1=57, posY2=85;
  String nilaina = String(valuena); //float to string
  char clabel[labelna.length()];    //char array for label
  char cnilai[nilaina.length()];    //char array for nilai

  labelna.toCharArray(clabel,labelna.length()); //string label to char array
  nilaina.toCharArray(cnilai,nilaina.length()); //string nilai to char array
  
  tft.fillRoundRect(0, posY1-10, tft.width(), 110, 10, ST77XX_BLACK);
  drawStringCenter(posX, posY1,clabel,2,ST77XX_MAGENTA);
  drawStringCenter(posX, posY2,cnilai,3,ST77XX_CYAN);
  displayStat("Display Value");
  delay(2000);
}

//------------------------------------------------------------ Display Status
void displayStat(String statuz){
  tft.drawLine(0,110, tft.width(),110, ST77XX_WHITE);  
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(5, 115);  tft.println("Status:");
  tft.setTextColor(ST77XX_WHITE);
  tft.fillRoundRect(50, 113, 120, 123, 10, ST77XX_BLACK);
  tft.setCursor(55,115);  tft.println(statuz);
  //Serial.println(statuz);
}

//------------------------------------------------------------ Display Starup
void TFTprintStartUp(){
  tft.setTextWrap(false);
  displayClear();
  drawStringCenter(80, 20,"SENSYNC",3,ST77XX_BLUE);
  drawStringCenter(80, 60,"PROJECT",2,ST77XX_GREEN);
  drawStringCenter(80, 100,"ONLIMO",2,ST77XX_YELLOW);
  //drawStringCenter(80, 110,"XSENSE-V3-134159C",0,ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 115);  tft.println("XSENSE-V3-134159C");
  delay(3000);
}

void displayClear(){
  tft.fillScreen(ST77XX_BLACK);
}

void invertColor(){
  for(int i=0; i<3; i++){
    tft.invertDisplay(true);
    delay(4000);
    tft.invertDisplay(false);
    delay(4000);
  } 
}

void drawtextline(char *text, uint16_t color, int txtwait) {
  tft.setTextColor(color);
  tft.setTextWrap(false);
  tft.println(text);
  delay(txtwait);
}

void drawtextWrap(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

// Draw string centered
void drawStringCenter(int16_t x, int16_t y, char *text, int8_t textsize, uint16_t Color){
  if(text!=NULL){
    tft.setTextColor(Color);
    tft.setTextSize(textsize);
    tft.setCursor(x - (strlen(text)*3*textsize), y-(4*textsize));
    while(*text)
      tft.write(*text++);
  }
}
