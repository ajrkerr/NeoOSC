#include <RGBConverter.h>
#include <Adafruit_NeoPixel.h>

#define DPIN   11 // Data Pin
#define PIXNUM 24 // Number of pixels

#define FRAME_LENGTH  40 // How many MS do we wait before updating
#define COLOR_STEPS  240 // How many steps in our color range
#define MINCOLOR      0 // In degrees, see HSV color space for more information
#define MAXCOLOR     360 // In degrees, see HSV color space for more information
#define BRIGHTNESS    16 // Brightness for the matrix  
#define SATURATION   1.0 // Saturation of color (See HSV colorspace)
#define VALUE        1.0 // Value of color      (See HSV colorspace)
#define OSCILLATE   false // Whether to restart the color band at the beginning, or start decremnting instead

#define CHANGE_HUE_COMMAND 1
#define CHANGE_BRIGHTNESS_COMMAND 2

#define COLOR_SPREAD (MAXCOLOR - MINCOLOR)

// Globals //
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXNUM, DPIN, NEO_GRB + NEO_KHZ800);
unsigned long frame = 0;
unsigned long currTime = 0;
float hueOverride = -1;
int brightness = BRIGHTNESS;
String commandBuffer;

// Helpers //
int smoothOscillate(unsigned long pos, int start, int finish) {
  int range  = finish - start;
  int offset = pos % (range * 2) - range;
  return start + abs(offset);
}

// Configure the pins that we're using //
void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
  commandBuffer = "";
}


void loop() {
  readFromSerial();
  
  // Incase of overflow
  if(currTime < millis()) {
    frame = 0;
  } 
  currTime = millis();
  
   // If we have hit the next frame
  if((currTime / FRAME_LENGTH) > frame) {
    frame = (currTime / FRAME_LENGTH); // Advance frame
    updateDisplay();
  }
}

void updateDisplay() {
  int spread = MAXCOLOR - MINCOLOR;
  
  double hue;
  double sat = SATURATION;
  double val = VALUE; 
  byte rgb[] = { 0, 0, 0 };
  RGBConverter conv; // Convert from HSV to RGB space
  
  for(int i = 0; i < PIXNUM; i++) {
    int colorPos = i + frame;
    // Whether or not we're going to oscilate between a color band or just run through linearly
    if(OSCILLATE) {
      hue = (double)smoothOscillate((colorPos) * (360.0 / COLOR_STEPS), MINCOLOR, MAXCOLOR) / 360;
    } else {
      hue = (((double)((colorPos) % COLOR_STEPS) / (COLOR_STEPS)) * COLOR_SPREAD + MINCOLOR) / 360;
    }
  
    if(hueOverride >= 0) {
      hue = hueOverride;
    }
    
    conv.hsvToRgb(hue, sat, val, rgb);
    
    int r = rgb[0];
    int g = rgb[1];
    int b = rgb[2];
    
    strip.setPixelColor(i, abs(r), abs(g), abs(b));
  }
  
  strip.setBrightness(brightness);
  strip.show();
}


boolean isTerminator(char ch) {
  return ch == ';';
}

int extractCommandType(String buffer) {
  switch(buffer[0]) { 
    case 'h':
      return CHANGE_HUE_COMMAND;
    case 'b':
      return CHANGE_BRIGHTNESS_COMMAND;
  }
}

int extractCommandData(String buffer) { 
  int stringLength = buffer.length();
  return buffer.substring(1, stringLength - 1).toInt();
}

void processCommand(String buffer) {
  int commandType = extractCommandType(buffer);
  int commandData = extractCommandData(buffer);
  
  switch(commandType) { 
    case CHANGE_HUE_COMMAND:
      hueOverride = (float)commandData / 360;
      break;
    case CHANGE_BRIGHTNESS_COMMAND:
      brightness = commandData * 255 / 100;
      break;
  }
  
  commandBuffer = "";
}

void readFromSerial() {
  while(Serial.available() > 0) {
    char readChar = Serial.read();
    commandBuffer += readChar;
    
    if(isTerminator(readChar)) {
      processCommand(commandBuffer); 
    }
  }
}

