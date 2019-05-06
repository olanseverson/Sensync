#~ from smbus2 import SMBusWrapper

#~ with SMBusWrapper(1) as bus:
    #~ b = bus.read_byte(0x10, 0)
    #~ print(b)
    
#~ from smbus2 import SMBusWrapper

#~ with SMBusWrapper(1) as bus:
    #~ # Read a block of 16 bytes from address 80, offset 0
    #~ block = bus.read_word_data(0x10, 1, 2)
    #~ # Returned value is a list of 16 bytes
    #~ print(block)

from smbus2 import SMBusWrapper, i2c_msg

with SMBusWrapper(1) as bus:
    # Read 64 bytes from address 80
    msg = i2c_msg.read(0x10, 6)
    bus.i2c_rdwr(msg)
    for value in msg:
        print(value)
        
    # show structure of object
    l = dir(msg) 
    print l
    
    # Write some bytes to address 80
    msg = i2c_msg.write(0x10, [65, 66, 67, 68])
    bus.i2c_rdwr(msg)

#~ from smbus2 import SMBusWrapper, i2c_msg

#~ # Single transaction writing two bytes then read two at address 80
#~ write = i2c_msg.write(0x10, [40, 50])
#~ read = i2c_msg.read(0x10, 2)
#~ with SMBusWrapper(1) as bus:
    #~ bus.i2c_rdwr(write, read)
