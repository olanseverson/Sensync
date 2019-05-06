# https://www.waveshare.com/wiki/Raspberry_Pi_Tutorial_Series:_I2C
# https://pydigger.com/pypi/smbus2
# https://smbus2.readthedocs.io/en/latest/

import time
import numpy as np
from smbus2 import SMBusWrapper, i2c_msg

# This is the address we setup in the Arduino Program
address1 = 0x10

# Delay time
delay = 10

def writeNumber(address, value):
    with SMBusWrapper(1) as bus:        
        # Write some bytes to address 
        msg = i2c_msg.write(address, value)
        bus.i2c_rdwr(msg)
    return -1

def readNumber(address, length):
    with SMBusWrapper(1) as bus:
        # Read 'length' bytes from 'address'
        msg = i2c_msg.read(address, length)
        bus.i2c_rdwr(msg)
        i = 0;
        data = [] 
        for value in msg:
            data.append(value)
    return data

def toWord(msb, lsb):
    num = (msb<<8)|lsb
    return num

while True:
    var = input("Enter 1 to 9: ")
    if not var:
        continue
    
    writeNumber(address1, var)
    print "RPI: Hi Arduino, I sent you ", var
    # sleep one second
    time.sleep(1)

    number = readNumber(address1, 8)
    print "Arduino: Hey RPI, I received a digit ", number
    for i in range(0, 4):
        print toWord(number[i*2], number[(i*2) + 1])

