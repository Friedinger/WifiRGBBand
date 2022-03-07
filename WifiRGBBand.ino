#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "config.h" //Include configuration file: Rename "sampleconfig.h" to "config.h" and ajust sample values

const char* ssid     = ConfigSsid;
const char* password = ConfigPassword;
int pin = ConfigPin;

int light = 0;
int lightpre = 0;
String weblink;
String webrequest;
String webparameter;
String webparametervalue;

const char html[] = R"(
<head>
  <title>WifiRGBBand</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      color: #ffffff;
      background: #000000;
      font-family: Verdana, sans-serif;
    }
    input[type=submit] {
       font-size: 16px;
      background: #000000;
      border: none;
      color: white;
      text-decoration: none;
      cursor: pointer;
    }
    input[type=color] {
      border: none;
      background: #000000;
      height: 22px;
    }
    a {
      color: white;
      text-decoration: none;
    }
  </style>
</head>
<body>
  <h1><a href="/">WifiRGBBand</a></h1>
  <a href="/led/off">Off</a><br><br>
  <h3>Static Colour: </h3>
  <form action="/led/static">
    <input type="color" id="colour" name="colour" value="#ffffff">
    <input type="submit" value="Submit">
  </form><br>
  <h3>Effects: </h3>
  <a href="/led/rainbow">Rainbow</a><br>
  <a href="/led/rainbowcycle">RainbowCycle</a><br>
  <a href="/led/colourwipe">ColourWipe</a><br>
</body>
)";

Adafruit_NeoPixel strip = Adafruit_NeoPixel(100, pin, NEO_GRB + NEO_KHZ800);

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  strip.begin();
  strip.setBrightness(50);
  strip.show();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(html);
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        if (currentLine.startsWith("GET /") && currentLine.endsWith(" HTTP/1.1") && !currentLine.startsWith("GET /favicon.ico")) {
          webrequest = currentLine;
          webrequest.remove(0, 4);
          webrequest.remove((webrequest.length() - 9), 9);
          weblink = webrequest;
          weblink.remove(weblink.indexOf("?"), weblink.length());
          webparameter = webrequest;
          webparameter.remove(0, webparameter.indexOf("?") + 1);
          webparameter.remove(webparameter.indexOf("="), (webparameter.length() - webparameter.indexOf("=")));
          webparametervalue = webrequest;
          webparametervalue.remove(0, webparametervalue.indexOf("=") + 1);
          webparametervalue.remove(webparametervalue.indexOf("&"), (webparametervalue.length() - webparametervalue.indexOf("&")));
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
  if (weblink == "/led/off") {
    strip.clear();
    strip.show();
  }
  if (weblink == "/led/static") {
    if (webparameter == "colour" && webparametervalue) {
      String colour1 = webparametervalue;
      colour1.remove(0, 3);
      uint32_t colour2 = hstol(colour1);
      strip.fill(colour2);
      strip.show();
      Serial.println(colour2);  
      weblink = "";
    }
  }
  if (weblink == "/led/rainbow") {
    rainbow(20);
  }
  if (weblink == "/led/rainbowcycle") {
    rainbowCycle(20);
  }
  if (weblink == "/led/colourwipe") {
    colorWipe(strip.Color(255, 0, 0), 50);
    colorWipe(strip.Color(0, 255, 0), 50);
    colorWipe(strip.Color(0, 0, 255), 50);
  }
}

uint32_t hstol(String recv){
  char c[recv.length() + 1];
  recv.toCharArray(c, recv.length() + 1);
  return strtol(c, NULL, 16);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);
      }
    }
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);
      }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
