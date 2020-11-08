# Repetier-Server-API-3D-printer-LED
> LED for your 3D Printer Controlled with an ESP8266 and the Repetier-Server API.

For this Project you need: 

* ESP8266 Board
* 5 Volt DIN LED

![](header.png)

## Installation
Opne the main.ino in the Arduino IDE and add all Libs we Included.
Configure your Wifi Settings and Repetier Server API Information like API key and IP.
Upload the main.ino to the ESP8266 Board with the Arduino IDE.

Connect the EPS8266 5v PIN to the LED 5volt PIN.
Connect the EPS8266 Ground PIN to the LED Ground PIN.
Connect the EPS8266 Data (You can take the pin you want just configure it in the main.ino file) PIN to the LED DIN PIN.

Plug the ESP8266 in and now it should light

