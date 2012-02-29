/*
 * Four Letter Word Generator
 * (C) 2011-12 Per Johan Groland
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 */

#include <SD.h>
#include <Wire.h>
#include <stdint.h>

// default address for an I2C EEPROM
#define eeprom_addr 0b1010000

// change this to match your SD shield or module;
// Arduino UNO or earlier / Adafruit SD shield: pin 10
// Arduino Mega: 53 (marked as SS)
// Arduino Ethernet shield: pin 4
// Sparkfun SD shield: pin 8
const int chipSelect = 53;    

File myFile;

void i2c_eeprom_write_byte(int device, unsigned int addr, byte data) {
  int rdata = data;
  Wire.beginTransmission(device);
  Wire.write((int)(addr >> 8)); // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.write(rdata);
  Wire.endTransmission();
  delay(10);
}

byte i2c_eeprom_read_byte(int device, unsigned int addr) {
  byte rdata = 0xFF;
  Wire.beginTransmission(device);
  Wire.write((int)(addr >> 8)); // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(device,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

bool st = HIGH;

bool writeDataWithStatus(byte data, uint32_t idx)
{
  if (idx % 10 == 0) {
    st = !st;
    digitalWrite(13, st);
  }
  
  if (idx % 4096 == 0) {
    Serial.print("Checkpoint ");
    Serial.println(idx);
  }
  
  i2c_eeprom_write_byte(eeprom_addr, idx, data);
  
  // read back data for verification
  byte result = i2c_eeprom_read_byte(eeprom_addr, idx);
  
  /*
  Serial.print("Wrote: ");
  Serial.print(data);
  Serial.print(" ");
  Serial.print("Read: ");
  Serial.println(result);
  */
  
  if (result != data)
    return false;
  return true;
}

void test()
{
  byte v;
  
  Serial.println("Writing test data");
  
  i2c_eeprom_write_byte(eeprom_addr, 0x0, 0x2a);
  i2c_eeprom_write_byte(eeprom_addr, 0x1, 0x3a);
  i2c_eeprom_write_byte(eeprom_addr, 0x2, 0x4a);
  i2c_eeprom_write_byte(eeprom_addr, 0x3, 0x5a);
  i2c_eeprom_write_byte(eeprom_addr, 0x4, 0x6a);
  i2c_eeprom_write_byte(eeprom_addr, 0x5, 0x7a);
  i2c_eeprom_write_byte(eeprom_addr, 0x6, 0x8a);
  
  Serial.println("Verifying data");
  
  v = i2c_eeprom_read_byte(eeprom_addr, 0x0);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(eeprom_addr, 0x1);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(eeprom_addr, 0x2);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(eeprom_addr, 0x3);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(eeprom_addr, 0x4);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(eeprom_addr, 0x5);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(eeprom_addr, 0x6);
  Serial.println(v, HEX);

}

void setup()
{
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
   
  if (!SD.begin(chipSelect)) {
    Serial.println(">>> Initialization failed!");
    return;
  }

  myFile = SD.open("output.dat", FILE_READ);
  
  if (!myFile) {
    Serial.println("Error opening output.dat");
    return;
  }
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);  
  Serial.println("Programming EEPROM");
  
  int b = 0;
  unsigned int offset = 0;  
  
  while ((b = myFile.read()) != -1) {
    writeDataWithStatus(b, offset++);
  }
  
  Serial.print("Wrote and verified ");
  Serial.print(offset);
  Serial.println(" bytes to SD card");
  
  myFile.close();
}


void loop()
{
  
}

