#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

#define PIN 13

const char* ssid     = "";
const char* password = "";

int light = 0;
int lightpre = 0;
String weblink;
String webrequest;
String webparameter;
String webparametervalue;

const char html[] = R"(
<a href="/led/off">Off</a><br>
<form action="/led/static">
  <label for="colour">Static Colour: </label>
  <input type="color" id="colour" name="colour" />
  <input type="submit" value="Submit" />
</form><br>
<a href="/led/rainbow">Rainbow</a><br>
<a href="/led/rainbowcycle">RainbowCycle</a><br>
<a href="/led/colourwipe">ColourWipe</a><br>
</form>
)";

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(100, PIN, NEO_GRB + NEO_KHZ800);

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

int value = 0;

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
        if (currentLine.startsWith("GET /") && currentLine.endsWith(" HTTP/1.1")) {
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
      Serial.println("Colour1: " + colour1);
      uint32_t colour2 = webparametervalue.toInt();
      Serial.println("Colour2: " + colour2);
      byte colour = 0xFF0000;
      //byte colour3 = colour1.toByte();
      Serial.println("Colour: " + colour);
      strip.fill(colour);
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

void switchlight() {
  if (light != lightpre) {
    lightpre = light;
    switch (light) {
      case 0:
        strip.clear();
        strip.show();
        break;
      case 1:
        strip.fill(strip.Color(255, 255, 255)); //white
        strip.show();
        break;
      case 11:
        rainbow(20);
        lightpre = -1;
        break;
      case 12:
        rainbowCycle(20);
        lightpre = -1;
        break;
      case 13:
        colorWipe(strip.Color(255, 0, 0), 50); //red
        colorWipe(strip.Color(0, 255, 0), 50); //green
        colorWipe(strip.Color(0, 0, 255), 50); //blue
        lightpre = -1;
        break;
      case 21:
        strip.fill(strip.Color(255, 0, 0)); //red
        strip.show();
        break;
      case 22:
        strip.fill(strip.Color(0, 255, 0)); //green
        strip.show();
        break;
      case 23:
        strip.fill(strip.Color(0, 0, 255)); //blue
        strip.show();
        break;
      case 24:
        strip.fill(strip.Color(255, 255, 0)); //yellow
        strip.show();
        break;
      case 25:
        strip.fill(strip.Color(0, 255, 255)); //cyan
        strip.show();
        break;
      case 26:
        strip.fill(strip.Color(160, 0, 255)); //purple
        strip.show();
        break;
      case 27:
        strip.fill(strip.Color(255, 0, 255)); //magenta
        strip.show();
        break;
    }
  }
}

// Some example procedures showing how to display to the pixels:
//colorWipe(strip.Color(255, 0, 0), 50); // Red
//colorWipe(strip.Color(0, 255, 0), 50); // Green
//colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
// Send a theater pixel chase in...
//theaterChase(strip.Color(127, 127, 127), 50); // White
//theaterChase(strip.Color(127, 0, 0), 50); // Red
//theaterChase(strip.Color(0, 0, 127), 50); // Blue

//rainbow(20);
//theaterChaseRainbow(50);

// Fill the dots one after the other with a color
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

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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
