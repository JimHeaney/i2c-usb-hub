//i2C USB Hub Prototyping
//Basic Functions required to make the i2C USB Hub function without a library
//By: Jim Heaney
//License: CC-BY-NC-SA 4.0
//More info: https://github.com/JimHeaney/i2c-usb-hub/wiki/Test-Code

#include "Wire.h"
bool startup(byte addr = 0x18); //Default address is 0x18 (0b0011000), can be changed with jumpers on the back

byte address;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if(startup()){
    //Returns 1 if successful
    Serial.println(F("USB Hub Initialized"));
    Serial.print(F("Address Set to: "));
    Serial.println(address);
  } else {
    Serial.println(F("Could not find device, please reset."));
    while(1);
  }

    setLEDs(1); //Turn on the LEDs
    setCurrent(1); //Set high current limit mode
}

void loop() {
  // put your main code here, to run repeatedly:

  //Cycle through all ports;
  setPort(1, 1);
  delay(1000);
  setPort(1, 0);
  setPort(2, 1);
  delay(1000);
  setPort(2, 0);
  setPort(3, 1);
  delay(1000);
  setPort(3, 0);
  setPort(4, 1);
  delay(1000);
  setPort(4, 0);
}

bool startup(byte addr){
  //Called to initialize the IC, optionally accepts address
  address = addr;
  Wire.begin();
  Wire.beginTransmission(address);
  Wire.write(0b00000011); //Configuration register address
  Wire.write(0b00000000); //Sets all pins as outputs 
  bool error = Wire.endTransmission();
  Wire.beginTransmission(address);
  Wire.write(0b00000010); //Polarity Inversion Register
  Wire.write(0b00000000); //Disable inversion on all pins (Why is this a feature?!)
  Wire.endTransmission();
  Wire.beginTransmission(address);
  Wire.write(0b00000001); //Output Control Register
  Wire.write(0b10000000); //Set current limit output to high for lower limit, all other inputs low
  Wire.endTransmission();
  setPort(4, 0); //For some reason the 4th port defaults to on, turn it off
  setLEDs(0); //Default the LEDs off
  return !error;
}

void setPort(byte port, bool mode){
  //Turns port X (1, 2, 3, 4) on (1) or off (0)
  if((port > 4) || (port == 0)){
    //Invalid port numbers
    return; 
  }
  Wire.beginTransmission(address);
  Wire.write(0b00000001); //Output port register
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  byte oldstate = Wire.read(); //The current state of the registers, just flip the one needed bit  
  bitWrite(oldstate, (port+3), mode);  
  Wire.beginTransmission(address);
  Wire.write(0b00000001);
  Wire.write(oldstate);
  Wire.endTransmission();
}

void setLEDs(bool mode){
  //Turns on (1) or off (0) the LEDs
  Wire.beginTransmission(address);
  Wire.write(0b00000001); //Output port register
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  byte oldstate = Wire.read();
  bitWrite(oldstate, 3, mode);
  Wire.beginTransmission(address);
  Wire.write(0b00000001);
  Wire.write(oldstate);
  Wire.endTransmission();
}

void setCurrent(bool mode){
  //Sets the current limit of the USB ports. 1 for high-power mode, 0 for low-power mode. 
  Wire.beginTransmission(address);
  Wire.write(0b00000001); //Output port register
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  byte oldstate = Wire.read();
  bitWrite(oldstate, 0, !mode);
  Wire.beginTransmission(address);
  Wire.write(0b00000001);
  Wire.write(oldstate);
  Wire.endTransmission();
}
