//--------------------------------------------------- SET RTC TIME 
void setTimeRTC() {
  //PRINTLN(F("Set Real Time Clock")); //separator(50);
  year = readByte();  //Serial.flush(); //clear serial buffer
  Serial.print(F("Enter the current Year (00-99): "));
  year = readByte();
  Serial.println(year);
  Serial.print(F("Enter the current Month (1-12): "));
  month = readByte();
  Serial.println(month);//Serial.println(months[month-1]);
  Serial.print(F("Enter the current day of the month (1-31): "));
  day = readByte();
  Serial.println(day);
  Serial.println(F("Enter the current day of the week (1-7): "));
  //Serial.print(F("1 Sun | 2 Mon | 3 Tues | 4 Weds | 5 Thu | 6 Fri | 7 Sat - "));
  Serial.print(F("[1 Minggu | 2 Senin | 3 Selasa | 4 Rabu | 5 Kamis | 6 Jumat | 7 Sabtu ]: "));
  weekday = readByte();
  Serial.println(days[weekday-1]);
  Serial.print(F("Enter the current hour (0-23): "));
  hour = readByte();
  Serial.println(hour);
  Serial.print(F("Enter the current minute (0-59) "));
  minute = readByte();
  Serial.println(minute);
  second = 0;
  Serial.println(F("The data has been entered."));
 
  // The following codes transmits the data to the RTC
  Wire.beginTransmission(RTC_DS3231);
  Wire.write(byte(0));
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekday));
  Wire.write(decToBcd(day));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.write(byte(0));
  Wire.endTransmission();
  // Ends transmission of data

  Done();  
}

byte readByte() {
  while (!Serial.available()) delay(10);
  byte reading = 0;
  byte incomingByte = Serial.read();
  while (incomingByte != '\n') {
    if (incomingByte >= '0' && incomingByte <= '9')
      reading = reading * 10 + (incomingByte - '0');
    else;
    incomingByte = Serial.read();
  }
  Serial.flush();
  return reading;
}

//--------------------------------------------------- PRINT DATE TIME 
void printTime() {
  char buffer[3];
  const char* AMPM = 0;
  
  readTime();
  Serial.print(days[weekday-1]);
  Serial.print(F(" "));
  Serial.print(day); //Serial.print(monthday);
  Serial.print(F("/"));
  //Serial.print(months[month-1]);
  Serial.print(month);
  Serial.print(F("/20"));
  Serial.print(year);
  Serial.print(F(" "));
  //if (hour > 12) {
  //  hour -= 12;
  //  AMPM = " PM";
  //}
  //else AMPM = " AM";
  Serial.print(hour);
  Serial.print(F(":"));
  sprintf(buffer, "%02d", minute);
  Serial.println(buffer);
  //Serial.println(AMPM);
}

String TimeStamp() {
  char buffer[3];
  String Tanggal;
  
  readTime();
  sprintf(buffer, "20%02d-%02d-%02d %02d:%02d", year, month, day, hour, minute );
  Tanggal = buffer;
  return Tanggal;
}

String SMStimeStamp() {
  char buffer[3];
  String Tanggal;
  
  readTime();
  sprintf(buffer, "20%02d-%02d-%02d,%02d:%02d", year, month, day, hour, minute );
  Tanggal = buffer;
  return Tanggal;
}

void readTime() {
  if (rtc_isrunning()){  
    Wire.beginTransmission(RTC_DS3231);
    Wire.write(byte(0));
    Wire.endTransmission();
    Wire.requestFrom(RTC_DS3231, 7);
    second = bcdToDec(Wire.read());
    minute = bcdToDec(Wire.read());
    hour = bcdToDec(Wire.read());
    weekday = bcdToDec(Wire.read());
    //monthday = bcdToDec(Wire.read());
    day = bcdToDec(Wire.read());
    month = bcdToDec(Wire.read());
    year = bcdToDec(Wire.read());
  }
  //else
    //Serial.println(F("Unable to sync with the RTC"));
}

//--------------------------------------------------- Test if RTC running 
uint8_t rtc_isrunning(void) {
  Wire.beginTransmission(RTC_DS3231);
  Wire.write(byte(0));
  Wire.endTransmission();

  Wire.requestFrom(RTC_DS3231, 1);
  uint8_t ss = Wire.read();
  //Serial.println(ss);
  return !(ss>>7);
}

byte decToBcd(byte val) {
  return ((val/10*16) + (val%10));
}
byte bcdToDec(byte val) {
  return ((val/16*10) + (val%16));
}
