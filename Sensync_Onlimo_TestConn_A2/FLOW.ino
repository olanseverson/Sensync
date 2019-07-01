float getFlow2(unsigned long timeout){
  int k = 0.5;
  long waktuLama;
  float frekuensi,periode,debit;
  unsigned long out = timeout * 1000;
  waktuLama = pulseIn(2,HIGH,out);
  periode = waktuLama / 1000000.0;
  frekuensi = (1.0 / periode) * 0.5;
  Serial.print("Frekuensi : ");Serial.print(frekuensi);Serial.println(" Hz");
  debit = frekuensi / 0.2;
  Serial.print("Debit : ");Serial.print(debit);Serial.println(" L/min");
  if (waktuLama == 0){
    return 0;
  } else {
    //Serial.println(frekuensi);
    return debit;
  }
}
