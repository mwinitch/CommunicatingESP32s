/**********************************************************************
  Filename    : Drive LiquidCrystal I2C to display characters
  Description : I2C is used to control the display characters of LCD1602.
  Auther      : www.freenove.com
  Modification: 2020/07/11
**********************************************************************/
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <Key.h>
#include <Keypad.h>

#define LED_PIN 2
#define LEDS_COUNT 8
#define SDA 18                    //Define SDA pins
#define SCL 19                    //Define SCL pins

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LEDS_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char *ssid_Router     = ""; // Wifi ID
const char *password_Router = ""; // Wifi password

String address = "/colors?values="; // Put full URL for server code
String address2 = "/status"; // Put full URL for server code

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}  
};

byte rowPins[4] = {23, 22, 21, 13};
byte colPins[4] = {14, 27, 26, 25};

String displaying = "";

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

/*
 * note:If lcd1602 uses PCF8574T, IIC's address is 0x27,
 *      or lcd1602 uses PCF8574AT, IIC's address is 0x3F.
*/
LiquidCrystal_I2C lcd(0x27,16,2); 

void setup() {
  strip.begin();
  //strip.setBrightness(10);
  Wire.begin(SDA, SCL);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial.println("ESP32 is starting...");
  WiFi.begin(ssid_Router, password_Router);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");

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
  int index = s.indexOf(",");
  String token1 = s.substring(0, index);
  String next = s.substring(index + 1);
  int nextSplit = next.indexOf(",");
  String token2 = next.substring(0, nextSplit);
  String token3 = next.substring(nextSplit + 1);
  int r = token1.toInt();
  int g = token2.toInt();
  int b = token3.toInt();
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print(s);
  for (int i = 0; i < LEDS_COUNT; i+=2) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();

  delay(10000);
  strip.clear();
  strip.show();
  
  lcd.clear();
  lcd.setCursor(0, 0);
}
