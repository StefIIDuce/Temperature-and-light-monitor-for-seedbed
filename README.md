The purpose of this project was to design a monitoring system for a seedbed, which would measure the temperature and light intensity. 
Depending on the measurements, it will power a ventilator for cooling and/or a servo for shade. 
Both the servo and ventilator can also be controlled through the Blynk app. 
De measured data will also be displayed on the Blynk app and an OLED screen.
De project is also able to extract the time from the internet.
To do this I made use of a site called Thingspeak.com and the time is extracted from https://www.worldtimeserver.com/current_time_in_BE.aspx?city=Brussels

The components used are: 
ESP32_DEV microcontroller 
0.96 OLED screen 
BMP280 temperature sensor 
BH1750 Lumen sensor 
x1 BC547B transistor 
x1 3K6 resistor 
Servo motor 
Small ventilator

The pins used from the ESP32 are: 
3.3V 
GND 
SDA(D21) 
SCL(D22) 
D18 
D15

The Blynk app project setting are:
Two Value Display's:
1) Title: Temperatuur (*C)
   Input: V2
   Reading rate: PUSH
2) Title: Lichtsterkte in Lux
   Input: V5
   Reading rate: PUSH
Two Button's:
1) Title: Shade
   Output: V1
   Mode: switch
2) Title: Ventilator
   Output: V3
   Mode: switch

Link to demonstration video:
https://youtu.be/-_TPJ3D3Hsk
