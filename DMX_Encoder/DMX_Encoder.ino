/*!
 */
#include <Wire.h>
// Adafruit_BusIO is needed by latest Adafruit_MCP23017 library (v2.1.0), both are needed by the MCP23017Encoders library
// Note: small syntax changes needed since original MCP23017Encoders library release
// Note: I2C address now passed to MPC23017.begin function, needed if alternative hardware used.
#include <MCP23017Encoders.h>
#include <ArduinoRS485.h> // the ArduinoDMX library depends on ArduinoRS485
#include <ArduinoDMX.h>
#include <ColorConverterLib.h>

MCP23017Encoders myMCP23017Encoders(3);         // Parameter is arduino interrupt pin that the MCP23017 is connected to

//#define DEBUG 

const int universeSize = 4;
long t = 0L;
int rgb_mode = 0;
boolean update = true;

double hue=0.0;
double saturation=1.0;
double lighting=0.5;
double value=0.5;
uint8_t _red;
uint8_t _green;
uint8_t _blue;

int red = 50;
int green = 100;
int blue = 150;

//Prepare: connect a button to a digital pin of the IO expansion board(eg: eGPA0)
void setup() {
  Serial.begin(115200);
  myMCP23017Encoders.begin(0x27);

  myMCP23017Encoders.setAccel(1, 10.0);
  myMCP23017Encoders.setAccel(2, 10.0);
  myMCP23017Encoders.setAccel(3, 10.0);

  myMCP23017Encoders.write(3,red);
  myMCP23017Encoders.write(2,green);
  myMCP23017Encoders.write(1,blue);

#ifndef DEBUG  
  // initialize the DMX library with the universe size
  if (!DMX.begin(universeSize)) 
  {
    Serial.println("Failed to initialize DMX!");
    while (1); // wait for ever
  }
#endif

}



void mode_proc(int mode)
{
    switch(mode)
    {
      case 0:
      red = myMCP23017Encoders.read(3);
      green = myMCP23017Encoders.read(2);
      blue = myMCP23017Encoders.read(1);
      
      if(red<0)myMCP23017Encoders.write(3,0);
      if(green<0)myMCP23017Encoders.write(2,0);
      if(blue<0)myMCP23017Encoders.write(1,0);
      
      if(red<0)red=0;
      if(green<0)green=0;
      if(blue<0)blue=0;

      if(red>255)myMCP23017Encoders.write(3,255);
      if(green>255)myMCP23017Encoders.write(2,255);
      if(blue>255)myMCP23017Encoders.write(1,255);

      if(red>255)red=255;
      if(green>255)green=255;
      if(blue>255)blue=255;

      break;
        
      case 1:      
      RGBConverter::HsvToRgb(hue, saturation, value,&_red, &_green,&_blue);
      red=_red;
      green=_green;
      blue=_blue;
      break;
      
      case 2:
      RGBConverter::HslToRgb(hue, saturation, lighting, &_red, &_green, &_blue);
      red=_red;
      green=_green;
      blue=_blue;
      break;
      
      default:
      break;
    }
}

void loop() {
    

 
  t = millis();
  if ( (t % 100L == 0) || update)
  {
  mode_proc(rgb_mode);
#ifndef DEBUG
    DMX.beginTransmission();
    DMX.write(1, red);
    DMX.write(2, green);
    DMX.write(3, blue);
    DMX.endTransmission();
#else
    //Serial.print("Lighting==");Serial.print(lighting);
//    Serial.print("Value==");Serial.print(value);
//    Serial.print("\tSaturation=");Serial.print(saturation);
//    Serial.print("\tHue=");Serial.print(hue);
    Serial.print("\t Red=");Serial.print(red);
    Serial.print("\t Green=");Serial.print(green);
    Serial.print("\t Blue=");Serial.println(blue);
#endif
    update = false;
  } 
 
}
