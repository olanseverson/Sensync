/*
  ||
  || @file init.ino
  || @version 1.0
  || @author Yoland Nababan
  ||
  || @description
  || | Data logging project.
  || | Act as a slave to a Raspberry using I2C Communication. Arduino Send 4 value when request from Master (Raspberry)
  || | The data is Analog Value from 4 different potentiometer. Data is uploaded to ThingsSpeak.com to store the data.
  || | 4 analog data store in 1 Channel, 4 field data. To store data to internet, see main program in python (.py).
  || #
  ||
*/

/* I2C FOR SERIAL COMMUNICATION
 * Connection:
 * ARDUINO ============ RASPI (PI 3 MODEL B V1.2)
 * SDA(A4) ===========> SDA(BCM 2)
 * SCL(A5) ===========> SCL(BCM 3)
 * GND     ===========> GND
*/

/** POTENTIOMETER
 * using analog(pin 14) A0 to A3(pin 17)
 */

//for I2C library 
#include <Wire.h>

#define SLAVE_ADDRESS 0x10 //address of Arduino seen by Raspi

int number = 0; // variable dummy to store data sent by Raspi 

int sensorValue[4] = {0, 0, 0, 0}; // Raw ADC data
word sensorPin[4] = {14, 15, 16, 17}; // pin ADC
void setup() {
  
  pinMode(13, OUTPUT);
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);s

  Serial.println("Ready!");
}

void loop() {
  delay(100);
}


void receiveData(int byteCount) 
/** callback for received data
 * update the value of adc every times Raspi (MASTER) request to arduino
 */
 
{
  while (Wire.available()) { // get the data from raspi that has been stored in buffer
    number = Wire.read();
    Serial.print("data received: ");
    Serial.println(number);
  }
  
  for (int i = 0; i < 4; i++) { // update the value of adc
    sensorValue[i] = analogRead(sensorPin[i]);
    Serial.println(sensorValue[i]);
  }
  Serial.println("--");
}


void sendData() 
/** callback for sending data
 * sending the updated ADC value back to Raspi(MASTER)
 * ADC is 10 bit, but Byte data is 8 bit, so data must be separated to 2 variabel (BYTES) before sending to raspi
 * in main program (RASPI), the separated data must be combine again.
 */
{
  byte x[8];
  for (int i = 0; i < 4; i++) { //separating 10 bit ADC to 2-8 bit for transfering data
    x[2*i] = sensorValue[i]>>8; // msb
    x[(2*i) + 1] = sensorValue[i]&0x00ff; // lsb
  }
  
  Wire.write(x, 8); // write 8 bytes of data from 4 ADC value
}

/*
  || @changelog
  || | 1.0 2019-05-08 - Yoland Nababan : Initial Release
  || #
*/
