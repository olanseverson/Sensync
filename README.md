# Sensync
Develop SPARING and ONLIMO sensor

EZO: sensor that consist of EZO Sensor
    - Uploader (for Wimos)
    - reading (for MEGA)
    
INIT: initial program (for uploading to thingspeak)

SPARING: sensor that consist of TSS and ph sensor 
        - Uploader (for Wimos)
        - reading (for MEGA)

TENTACLE_SETUP: setup program for tentacleShield

TEST: many program to test modular sensor
    - gprs: try to connect SIM800C module to send a message via SMS
    - send_json: uploading data to server using json format (using WeMos)
