#include <Wire.h>

#define SLAVE_ADDRESS 0x10
int number = 0;
int state = 0;
int sensorValue[4] = {0, 0, 0, 0};
word sensorPin[4] = {14, 15, 16, 17};
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  Serial.println("Ready!");
}

void loop() {
  delay(100);
}

// callback for received data
void receiveData(int byteCount) {

  while (Wire.available()) {
    number = Wire.read();
    Serial.print("data received: ");
    Serial.println(number);
  }
  for (int i = 0; i < 4; i++) {
    sensorValue[i] = analogRead(sensorPin[i]);
    Serial.println(sensorValue[i]);
  }
  Serial.println("--");
}

// callback for sending data
void sendData() {
  
  byte x[8];
  for (int i = 0; i < 4; i++) {
    x[2*i] = sensorValue[i]>>8; // msb
    x[(2*i) + 1] = sensorValue[i]&0x00ff; // lsb
  }
  
  Wire.write(x, 8);
}
