# https://www.waveshare.com/wiki/Raspberry_Pi_Tutorial_Series:_I2C
# https://pydigger.com/pypi/smbus2
# https://smbus2.readthedocs.io/en/latest/
# https://electronicshobbyists.com/raspberry-pi-sending-data-to-thingspeak-simplest-raspberry-pi-iot-project/
# https://www.hackster.io/adamgarbo/raspberry-pi-2-iot-thingspeak-dht22-sensor-b208f4

import time
import numpy as np
from smbus2 import SMBusWrapper, i2c_msg
import sys 
import urllib2 

myAPI = '3JEXAHO11O1HUNA3'

# This is the address we setup in the Arduino Program
address1 = 0x10

# Delay time
periods = 5

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

def main():
    # Setup here
    print 'Initializing...' 
    baseURL = 'https://api.thingspeak.com/update?api_key=%s' % myAPI 
    
    while True:
        #~ var = input("Enter 1 to 9: ")
        #~ if not var:
            #~ continue
        try: 
            # Send dummy value, to trigger arduino to capture data
            dummy = [0]
            writeNumber(address1, dummy) 
            print "RPI: Hi Arduino, I sent you ", dummy
            # sleep one second
            time.sleep(1)
            
            # Get data from Arduino
            data = []
            raw = readNumber(address1, 8)
            print "Arduino: Hey RPI, I received a digit ", raw
            for i in range(0, 4):
                data.append(toWord(raw[i*2], raw[(i*2) + 1]))
            print data 
             
            # 'data' will be sent to internet
            f = urllib2.urlopen(baseURL + "&field1=%s&field2=%s&field3=%s&field4=%s" % (data[0], data[1], data[2], data[3]))
            print f.read() 
            f.close() 
            time.sleep(periods) #uploads values every 'periods' minutes 
        except: 
            print 'exiting.' 
            break 
        

if __name__ == '__main__':
    main()
