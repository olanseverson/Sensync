//This code was written to be easy to understand.
//Modify this code as you see fit.
//This code will output data to the Arduino serial monitor.
//Type commands into the Arduino serial monitor to control the D.O. circuit.
//This code was written in the Arduino 1.8.3 IDE
//An Arduino UNO was used to test this code.
//This code was last tested 6/2017


#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
#define rx 11                                          //define what pin rx is going to be
#define tx 10                                          //define what pin tx is going to be

SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work

const int s0 = 7;                        //Arduino pin 7 to control pin S0
const int s1 = 6;                        //Arduino pin 6 to control pin S1
const int enable_1 = 5;                   //Arduino pin 5 to control pin E on shield 1
const int enable_2 = 4;                  //Arduino pin 4 to control pin E on shield 2

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
  pinMode(s1, OUTPUT);                   //Set the digital pin as output.
  pinMode(s0, OUTPUT);                   //Set the digital pin as output.
  pinMode(enable_1, OUTPUT);             //Set the digital pin as output.
  pinMode(enable_2, OUTPUT);             //Set the digital pin as output.

  Serial.begin(9600);                                 //set baud rate for the hardware serial port_0 to 9600
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
}


void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}

void loop() {                                         //here we go...
  for (int i = 0; i < 4; i++)
  {
    channel = i;
    change_serial_mux_channel();
    
    Serial.print(i);
    Serial.print(" =>> ");
    requestToSensor("r");
    //    requestToSensor("i");Serial.println();
    delay(2000);
  }
  delay(1000);
}

void requestToSensor(String inputstring) {
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
    Serial.println(sensorstring);                     //send that string to the PC's serial monitor
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
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
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

boolean request_serial_info() {                        // helper to request info from a uart stamp and parse the answer into the global stamp_ variables

  clearIncomingBuffer();
  myserial.write("i");                                // send "i" which returns info on all versions of the stamps
  myserial.write("\r");

  delay(150);                             // give it some time to send an answer

  int sensor_bytes_received = myserial.readBytesUntil(13, sensordata, 9);  //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received

  if (sensor_bytes_received > 0) {                     // there's an answer
    answerReceived = true;                             // so we can globally know if there was an answer on this channel

    if ( parseInfo() ) {                               // try to parse the answer string
      delay(100);
      clearIncomingBuffer();                           // some stamps burp more info (*OK or something). we're not interested yet.
      return true;
    }
  }

  return false;                                        // it was not possible to get info from the stamp
}

boolean parseInfo() {                  // parses the answer to a "i" command. returns true if answer was parseable, false if not.

  // example:
  // PH EZO  -> '?I,pH,1.1'
  // ORP EZO -> '?I,OR,1.0'   (-> wrong in documentation 'OR' instead of 'ORP')
  // DO EZO  -> '?I,D.O.,1.0' || '?I,DO,1.7' (-> exists in D.O. and DO form)
  // EC EZO  -> '?I,EC,1.0 '
  // TEMP EZO-> '?I,RTD,1.2'


  // Legazy PH  -> 'P,V5.0,5/13'
  // Legazy ORP -> 'O,V4.4,2/13'
  // Legazy DO  -> 'D,V5.0,1/13'
  // Legazy EC  -> 'E,V3.1,5/13'

  if (sensordata[0] == '?' && sensordata[1] == 'I') {          // seems to be an EZO stamp

    // PH EZO
    if (sensordata[3] == 'p' && sensordata[4] == 'H') {
      stamp_type = F("EZO pH");
      stamp_version[0] = sensordata[6];
      stamp_version[1] = sensordata[7];
      stamp_version[2] = sensordata[8];

      return true;

      // ORP EZO
    }
    else if (sensordata[3] == 'O' && sensordata[4] == 'R') {
      stamp_type = F("EZO ORP");
      stamp_version[0] = sensordata[6];
      stamp_version[1] = sensordata[7];
      stamp_version[2] = sensordata[8];

      return true;

      // DO EZO
    }
    else if (sensordata[3] == 'D' && sensordata[4] == 'O') {
      stamp_type = F("EZO DO");
      stamp_version[0] = sensordata[6];
      stamp_version[1] = sensordata[7];
      stamp_version[2] = sensordata[8];

      return true;

      // D.O. EZO
    }
    else if (sensordata[3] == 'D' && sensordata[4] == '.' && sensordata[5] == 'O' && sensordata[6] == '.') {
      stamp_type = F("EZO DO");
      stamp_version[0] = sensordata[8];
      stamp_version[1] = sensordata[9];
      stamp_version[2] = sensordata[10];

      return true;

      // EC EZO
    }
    else if (sensordata[3] == 'E' && sensordata[4] == 'C') {
      stamp_type = F("EZO EC");
      stamp_version[0] = sensordata[6];
      stamp_version[1] = sensordata[7];
      stamp_version[2] = sensordata[8];

      return true;
      
      // RTD EZO
    }
    else if (sensordata[3] == 'R' && sensordata[4] == 'T' && sensordata[5] == 'D') {
      stamp_type = F("EZO RTD");
      stamp_version[0] = sensordata[7];
      stamp_version[1] = sensordata[8];
      stamp_version[2] = sensordata[9];

      return true;

      // unknown EZO stamp
    }
    else {
      stamp_type = F("unknown EZO stamp");
      return true;
    }

  }

  // it's a legacy stamp (non-EZO)
  else
  {
    // Legacy pH
    if ( sensordata[0] == 'P') {
      stamp_type = F("pH (legacy)");
      stamp_version[0] = sensordata[3];
      stamp_version[1] = sensordata[4];
      stamp_version[2] = sensordata[5];
      stamp_version[3] = 0;
      return true;

      // legacy ORP
    }
    else if ( sensordata[0] == 'O') {
      stamp_type = F("ORP (legacy)");
      stamp_version[0] = sensordata[3];
      stamp_version[1] = sensordata[4];
      stamp_version[2] = sensordata[5];
      stamp_version[3] = 0;
      return true;

      // Legacy D.O.
    }
    else if ( sensordata[0] == 'D') {
      stamp_type = F("D.O. (legacy)");
      stamp_version[0] = sensordata[3];
      stamp_version[1] = sensordata[4];
      stamp_version[2] = sensordata[5];
      stamp_version[3] = 0;
      return true;

      // Lecagy EC
    }
    else if ( sensordata[0] == 'E') {
      stamp_type = F("EC (legacy)");
      stamp_version[0] = sensordata[3];
      stamp_version[1] = sensordata[4];
      stamp_version[2] = sensordata[5];
      stamp_version[3] = 0;
      return true;
    }
  }

  /*
    Serial.println("can not parse data: ");
    Serial.print("'");
    Serial.print(sensordata);
    Serial.println("'");
  */
  return false;        // can not parse this info-string
}

void clearIncomingBuffer() {          // "clears" the incoming soft-serial buffer
  while (myserial.available() ) {
    //Serial.print((char)sSerial.read());
    myserial.read();
  }
}
