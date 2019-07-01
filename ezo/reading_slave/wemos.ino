void echo(String data) {
  while (espSerial.available()) { // empty the rx buffer
    espSerial.read();
  }
  printState();
  updateTglJam();
  String test = tanggal + "," + jam + "," + data;
  Serial.println(test);
  espSerial.print(test);
  espSerial.flush();
  state = sending;
  printState();

  while (!espSerial.available()) {
    state = waiting;
    printState();
  }

  state = receiving;
  printState();

  String resp = "";
  while (espSerial.available()) {
    char in = espSerial.read();
    resp += in;
    delay(10);
  }
  Serial.println(resp);
  state = idle;
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
