#include <ModbusMaster.h>

/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/
#define MAX485_DE      3
#define MAX485_RE_NEG  2

// instantiate ModbusMaster object
ModbusMaster node;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  // Modbus communication runs at 115200 baud
  Serial.begin(19200);
  Serial1.begin(19200);

  // Modbus slave ID 1
  node.begin(10, Serial1);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

bool state = true;

void loop()
{
  uint8_t result;
  uint16_t data[6];

  // Toggle the coil at address 0x0002 (Manual Load Control)
  //  result = node.writeSingleCoil(0x0002, state);
  //  state = !state;

  //100011000010001


  // Read 16 registers starting at 0x3100)
  result = node.readHoldingRegisters(0x02, 2);
  if (result == node.ku8MBSuccess)
  {


    uint32_t AB = node.getResponseBuffer(0x00);
    uint32_t CD = node.getResponseBuffer(0x01);
    Serial.print(AB); Serial.print("       "); Serial.println(AB, BIN);
    Serial.print(CD); Serial.print("       "); Serial.println(CD, BIN);
    uint32_t x = (((0xffff&CD) << 16) | (0xffff & AB));

    Serial.print(x); Serial.print("       "); Serial.println(x, HEX);
    Serial.print(float(x)); Serial.print("       "); Serial.println(float(x), HEX);
    float t = x;
    byte* a;
    a = (byte*) &t;
    Serial.print(a[3], HEX);
    Serial.print(a[2], HEX);
    Serial.print(a[1], HEX);
    Serial.print(a[0], HEX);
    Serial.println();
    Serial.println();

  }

  //  if (result == node.ku8MBSuccess)
  //  {
  //    Serial.print("Vbatt: ");
  //    Serial.println(node.getResponseBuffer(0x00));
  //    Serial.print("Vload: ");
  //    Serial.println(node.getResponseBuffer(0x01));
  //    Serial.print("Pload: ");
  //    Serial.println((node.getResponseBuffer(0x0D) +
  //                    node.getResponseBuffer(0x0E) << 16)/100.0f);
  //  }

  delay(1000);
}
