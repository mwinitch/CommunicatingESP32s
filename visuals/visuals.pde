import hypermedia.net.*;

// For connecting to the socket
int PORT = 4210;
String IP = ""; // Check what the ESP32 prints to the serial monitor to see IP
UDP udp;
String colors;
int r = 0;
int g = 0;
int b = 0;

void setup()
{
  udp = new UDP(this, PORT);
  udp.send("Hello ESP32", IP);

  udp.listen(true);
  fullScreen(); 
}

void draw()
{
  background(r, g, b); 
}

void receive(byte[] data) {
  colors = new String(data);
  println(colors);
  String[] values = split(colors, ',');
  r = int(values[0]);
  g = int(values[1]);
  b = int(values[2]);
}