#include <Adafruit_NeoPixel.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SDA 18
#define SCL 19
#define LEDS_COUNT 8
#define LEDS_PIN 2
#define CHANNEL 0

const char *ssid_Router = "";
const char *password_Router = "";

//Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
Adafruit_NeoPixel strip(LEDS_COUNT, LEDS_PIN, NEO_GRB + NEO_KHZ800);
String address = "";

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}  
};

byte rowPins[4] = {14, 27, 26, 25};
byte colPins[4] = {13, 21, 22, 23};
String displaying = "";

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  strip.begin();
  //strip.setBrightness(10);
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

}

void loop() {
  char keyPressed = myKeypad.getKey();
  if (keyPressed) {
    Serial.println(keyPressed);
    if (keyPressed == '#') {
      submitRequest();
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
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print(s);
  for (int i = 0; i < LEDS_COUNT; i++) {
    strip.setPixelColor(i, r, g, b, 50);
  }
  strip.show();

  delay(5000);
  strip.clear();
  strip.show();
  
  lcd.clear();
  lcd.setCursor(0, 0);
}
