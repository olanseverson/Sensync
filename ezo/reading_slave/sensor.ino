
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
