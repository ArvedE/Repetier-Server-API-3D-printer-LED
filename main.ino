//LIBS YOU NEED
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>


#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    4

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 30

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* ssid = ""; // wifi Name
const char* password = ""; // wifi Password
// If you need some Info for the lines below you will find it here:
// http://<printerIP>:3344/printer/info
String printerUrl     = "http://<ip>/"; //Repetier-Sever IP from Printer
String printerApiKey  = "<api-keyY"; //API Key from Reptier-Server 
String printerSlug    = "<slug>"; // Slug
int    arraySize      = 2;
String printerActions[] = {"stateList", "listPrinter"}; // command

//Vars for the Info we get from the API
const char* job = "";
float extruderTempRead = 0.0;
float extruderTempSet = 0.0;
float heatedBedTempRead = 0.0;
float heatedBedTempSet = 0.0;
int lightOn = 0;
//Size of JSON we get make it higher if you get an error 
StaticJsonDocument<1000> doc;

void setup () {

  Serial.begin(9600);
  //Connect to Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting..");
  }
  //LED
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
    // END of Trinket-specific code.
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;  //Declare an object of class HTTPClient
    for (int i = 0; i < arraySize; i++) {
      //API URL
      String apiUrl = printerUrl + "printer/api/" + printerSlug + "?apiKey=" + printerApiKey + "&a=" + printerActions[i] + "&data={}";

      http.begin(apiUrl);
      int httpCode = http.GET();
      if (httpCode > 0) {
        String json = http.getString();
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
          Serial.print(F("deserializeJson() failed"));
        } else {
          if (printerActions[i] == "stateList") {
            lightOn = int(doc["Ender3"]["lights"]);
            extruderTempRead = doc["Ender3"]["extruder"][0]["tempRead"];
            extruderTempSet = doc["Ender3"]["extruder"][0]["tempSet"];
            heatedBedTempRead = doc["Ender3"]["heatedBeds"][0]["tempRead"];
            heatedBedTempSet = doc["Ender3"]["heatedBeds"][0]["tempSet"];
          } else if (printerActions[i] == "listPrinter") {
            job = doc[0]["job"];
          }
        }

      }

    }
    http.end();   //Close connection
  }
  if (lightOn > 0) {
    if (String(job) == "none" && extruderTempSet == 0.0000000000000000 && heatedBedTempSet == 0.0000000000000000) {

      if (extruderTempRead > 50 || heatedBedTempRead > 40) {
        //cooldown
        rgbFade(20,  "blue");
      } else {
        //nothing
        rgbFade(20,  "");
      }
    } else if (extruderTempSet != 0.0000000000000000 || heatedBedTempSet != 0.0000000000000000) {
      if (heatedBedTempRead < (heatedBedTempSet - 1) || extruderTempRead < (extruderTempSet - 1)) {
        //heating
        rgbFade(20,  "red");
      } else if (heatedBedTempSet != 0.0000000000000000 || extruderTempSet != 0.0000000000000000) {
        if (String(job) == "none") {
          //finished heating
          rgbFade(20,  "green");
        } else {
          //printing
          rgbFade(20,  "white");
        }
      }
    }

  } else {
    colorWipe(strip.Color(0,   0,   0), 50); // Light off
  }
  delay(500);

}

////////////////////////
//LED Methods we can use 
////////////////////////
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rgbFade(uint8_t wait, String color) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      if (String(color) == String("")) {
        strip.setPixelColor(i, Wheel((i + j) & 255));
      } else if (String(color) == String("red")) {
        strip.setPixelColor(i, WheelRed((i + j) & 255));
      } else if (String(color) == String("green")) {
        strip.setPixelColor(i, WheelGreen((i + j) & 255));
      } else if (String(color) == String("blue")) {
        strip.setPixelColor(i, WheelBlue((i + j) & 255));
      } else if (String(color) == String("white")) {
        strip.setPixelColor(i, WheelWhite((i + j) & 255));
      }

    }
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t WheelRed(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 0, 0);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, 0);
  }
  WheelPos -= 170;
  return strip.Color(0, 0, 0);
}

uint32_t WheelGreen(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(0, WheelPos * 3, 0);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, 255 - WheelPos * 3, 0);
  }
  WheelPos -= 170;
  return strip.Color(0, 0, 0);
}

uint32_t WheelBlue(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(0, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, 0, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(0, 0, 0);
}

uint32_t WheelWhite(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, WheelPos * 3, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 255 - WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(0, 0, 0);
}
