# Communicating ESP32s

This project allows two ESP32 microcontrollers to communicate over a Flask web server. Each ESP32 uses a keypad to message three numbers which are used as RGB values for either setting colors for LEDs, or the background color for a screen monitor. Here is a [video](https://www.youtube.com/watch?v=a1yPacIG4h4&ab_channel=ChiannaLiCohen) of the project in action. 

The project structure is as follows:
* `message_screen` contains the Arduino file that changes the background color of a monitor
* `message_led` contains the Ardunio file that changes the LED colors for the ESP32
* `server` contains all code needed to deploy the Flask server
* `visuals` contains the Processing script

Below is a diagram of how the ESP32s communicate through the server
<br>
 <img src="https://github.com/mwinitch/CommunicatingESP32s/blob/master/imgs/SystemsDiagram.png" width="1000">
 <br>

## The Wiring and the Hardware
We used three pieces of hardware: the 4X4 Matrix Keypad, the Neopixel LED module, and the LCD1602 Display Module.
The 4X4 Matrix Keypad was wired according to this diagram here:
<br>
 <img src="https://github.com/mwinitch/CommunicatingESP32s/blob/master/imgs/keypad.png" width="500">
 <br>

The numbers and letters may not allign perfectly with every module even if you orient it the same way, so bear in mind that you may have to manually calibrate the code so that you are properly controlling the rows and columns. You will also have to install the keypad library from Mark Stanley and Alexander Brevig by going to Tools > Manage Libraries, and then searching for the library title.

The LCD1602 Display Module was wired similar to this diagram: 
<br>
 <img src="https://github.com/mwinitch/CommunicatingESP32s/blob/master/imgs/lcd.png" width="700">
 <br>

However, instead of using GPIO pinds 14 and 13, we used 19 and 18 respectively so as not to interfere with the keypad. In order to use the module with the Arduino IDE, you had to install the LiquidCrystal I2C library.

The Neopixel LED module was connected in a similar way as described [here](https://github.com/Cina10/EmbeddedSys_Generative), except to the ESP32 instead of a Raspbery Pi. The signal wire was connected to GPIO 2 here. We then used the Adafruit NeoPixel library. More information can be found [here](https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use) about how to use it. 


## The Flask Server
The ESP32s can communicate from anywhere. To faciliate this, there must be a server that allows for communication between the two devices. We use Flask to create this server. You can see the code for the server in the `server` directory. The `app.py` is the Flask application. The `requirements.txt` file contains all of the needed packages for the server to run. The `Procfile` is used in helping with deployment. 

One of the main routes of the server is `/status` which either returns stop of three numbers seperated by commas. The other route is `/colors` where you can post three numbers to the server that can then be accessed for other for 15 seconds until it resets. 

## The API
This project uses The Color API whose documentation can be found [here](http://www.thecolorapi.com/). We particularly used the `/scheme` endpoint to query for analogus colors. No API key was needed, but the code can be modified in the our server code in the `/colors` end point such that it chooses different color schemes, when it switches the color. 

A sample call to the API is as follows
~~~ 
http://www.thecolorapi.com/scheme?rgb=[your RBG value]&mode=analogic&format=json
~~~

## Deployment
While you can run the server locally, it will have to be deployed in order to properly be used. You can deploy to any cloud service you like such as Heroku, AWS, DigitalOcean, etc. For this project we deployed to Heroky. [This video](https://www.youtube.com/watch?v=Li0Abz-KT78&ab_channel=Codemy.com) can show you how to deploy a Flask server to Heroku. The `server` directory already provides all the correct files for deployment.

## ESP32 Connecting to WiFi
The ESP32 will have to connect to WiFi in order to communicate with the server. In `message_screen.ino` and `message_led.ino` the following lines will have to be changed:
~~~C++
const char *ssid_Router = ""; // Put WiFi network name here
const char *password_Router = ""; // Put WiFi password here
~~~
For both of these lines please provide the WiFi network name and password. 

## ESP32 Connecting to the Server
The ESP32 will have to connect with the server. In order to do this, change the following lines in `message_screen.ino` and `message_led.ino`:
~~~C++
String address = "/colors?values="; // Put full URL for server code
String address2 = "/status"; // Put full URL for server code
~~~
Before the initial `/` in `address` and `address2` please add the URL to the server. This way the ESP32 will be able to make HTTP requests to the correct URL. 

## Processing UDP Socket
The file `visuals.pde` uses a UDP socket to communicate with the ESP32. You will need to install the UDP library for Processing. To do this, open up Processing and go to Sketch -> Import Library -> Add Library -> and search for UDP and install. The following line in `visuals.pde` will have to be changed:
~~~Processing
String IP = ""; // Check what the ESP32 prints to the serial monitor to see IP
~~~
You will need the IP address of the ESP32. The file `message_screen.ino` will print its IP address, so that can be copied and placed into the `visuals.pde` file. 
