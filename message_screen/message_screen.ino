#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>

#define SDA 18
#define SCL 19
#define CHANNEL 0

const char *ssid_Router = ""; // Put WiFi network name here
const char *password_Router = ""; // Put WiFi password here

String address = "/colors?values="; // Put full URL for server code
String address2 = "/status"; // Put full URL for server code

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}  
};

byte rowPins[4] = {14, 27, 26, 25};
byte colPins[4] = {13, 21, 22, 23};
String displaying = "";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  //  port to listen on
char incomingPacket[255];  // buffer for incoming packets

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  Wire.begin(SDA, SCL);
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  Serial.println("ESP32 is starting...");
  WiFi.begin(ssid_Router, password_Router);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  // Tests that a connection has been established
  bool readPacket = false;
  while (!readPacket) {
    int packetSize = Udp.parsePacket();
    if (packetSize)
     {
      // receive incoming UDP packets
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
      int len = Udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      readPacket = true;
    } 
  }

}

void loop() {
  char keyPressed = myKeypad.getKey();
  if (keyPressed) {
    Serial.println(keyPressed);
    if (keyPressed == '#') {
      submitRequest();
      displaying = "";
    }
    else if (keyPressed == '*') {
      listenMode();
      displaying = "";
    }
    else if (displaying.length() > 0 && keyPressed == 'D') {
      displaying.remove(displaying.length() - 1);
      lcd.clear();
    }
    else if (displaying.length() < 12 && keyPressed != 'D'){
      displaying += String(keyPressed);
    }
    lcd.setCursor(0, 0);
    lcd.print(displaying);
  }
  delay(100);
}

void submitRequest() {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin(address + displaying); //HTTP
  Serial.print("[HTTP] GET...\n"); // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode > 0) {
    displayLED(http.getString());
  }
  else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

}

void listenMode() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Listening...");
  while (true) {
    char keyPressed = myKeypad.getKey();
    if (keyPressed) {
      Serial.println(keyPressed);
      if (keyPressed == '*') {
        lcd.clear();
        break;
      }
    }
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin(address2); //HTTP
    Serial.print("[HTTP] GET...\n"); // start connection and send HTTP header
    int httpCode = http.GET();
    String res = http.getString();
    if(httpCode > 0) {
      if (!res.equals("stop")) {
        displayLED(res);
        break;
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  delay(1000);
  }
}

void displayLED(String s) {
  Serial.println("Displaying");
  Serial.println(s);
  Serial.println("Displaying 2");
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print(s);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.printf("%s", s);
  Udp.endPacket();
  delay(10000);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.printf("%s", "0,0,0");
  Udp.endPacket();
  lcd.clear();
  lcd.setCursor(0, 0);
}
