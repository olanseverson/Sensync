#~ /*
  #~ || @file init.py
  #~ || @version 1.0
  #~ || @author Yoland Nababan
  #~ ||
  #~ || @description
  #~ || | Data logging project.
  #~ || | Act as a master to a arduino using I2C Communication. Arduino Send 4 value when request from Master (Raspberry)
  #~ || | The data is Analog Value from 4 different potentiometer. Data is uploaded to ThingsSpeak.com to store the data.
  #~ || | 4 analog data store in 1 Channel, 4 field data. This program is the interface to internet.
  #~ || #
  #~ || 
  #~ || @documentation
  #~ || | https://www.waveshare.com/wiki/Raspberry_Pi_Tutorial_Series:_I2C
  #~ || | https://pydigger.com/pypi/smbus2
  #~ || | https://smbus2.readthedocs.io/en/latest/
  #~ || | https://electronicshobbyists.com/raspberry-pi-sending-data-to-thingspeak-simplest-raspberry-pi-iot-project/
  #~ || | https://www.hackster.io/adamgarbo/raspberry-pi-2-iot-thingspeak-dht22-sensor-b208f4
  #~ || #
#~ */

#~ /* I2C FOR SERIAL COMMUNICATION
 #~ * Connection:
 #~ * ARDUINO ============ RASPI (PI 3 MODEL B V1.2)
 #~ * SDA(A4) ===========> SDA(BCM 2)
 #~ * SCL(A5) ===========> SCL(BCM 3)
 #~ * GND     ===========> GND
#~ */

import time
import numpy as np
from smbus2 import SMBusWrapper, i2c_msg
import sys 
import urllib2 

myAPI = '3JEXAHO11O1HUNA3' # API for ThingSpeak  (setting the permission first)

# This is the address we setup in the Arduino Program
address1 = 0x10

# Delay time (in second)
periods = 60

def writeNumber(address, value):
# write <list> value to slave to the address
    with SMBusWrapper(1) as bus:        
        # Write some bytes to address 
        msg = i2c_msg.write(address, value)
        bus.i2c_rdwr(msg)
    return -1

def readNumber(address, length):
# read 'length' bytes from slave using address and store value to array
    with SMBusWrapper(1) as bus:
        # Read 'length' bytes from 'address'
        msg = i2c_msg.read(address, length)
        bus.i2c_rdwr(msg)
        data = [] 
        for value in msg: # store the requested data to array
            data.append(value)
    return data

def toWord(msb, lsb):
# combine msb(8bytes) and lsb(8 bytes) to num
    num = (msb<<8)|lsb
    return num

def main():
    # Setup here
    print 'Initializing...' 
    baseURL = 'https://api.thingspeak.com/update?api_key=%s' % myAPI 
    
    while True:
        try: 
            # Send dummy value, to trigger arduino to capture data
            dummy = [0]
            writeNumber(address1, dummy) # the arduino respond this by updating analogRead
            print "RPI: Hi Arduino, I sent you ", dummy
            # sleep one second
            time.sleep(1)
            
            # Get data from Arduino
            data = []
            raw = readNumber(address1, 8)  # receive 8 bytes data (each 2 bytes represent 1 sensorValue)
            print "Arduino: Hey RPI, I received a digit ", raw
            for i in range(0, 4):
                data.append(toWord(raw[i*2], raw[(i*2) + 1]))  # combine 8 bytes to 4 bytes (becomes original data)
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
    main() # main program

#~ /*
  #~ || @changelog
  #~ || | 1.0 2019-05-08 - Yoland Nababan : Initial Release
  #~ || #
#~ *
