
#include <Wire.h>
#include <stdint.h>

#define chipAddress 0b1010000

extern const uint8_t flw_data_1[] PROGMEM;
extern const uint8_t flw_data_2[] PROGMEM;

extern const uint16_t flw_data_1_size;
extern const uint16_t flw_data_2_size;

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
  
  i2c_eeprom_write_byte(chipAddress, idx, data);
  
  // read back data for verification
  byte result = i2c_eeprom_read_byte(chipAddress, idx);
  
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

/*
bool verifyDataWithStatus(byte data, uint32_t idx)
{
  if (idx % 30 == 0) {
    st = !st;
    digitalWrite(13, st);
  }

  byte result = i2c_eeprom_read_byte(chipAddress, idx);
  
  return result == data;
}
*/

void test()
{
  byte v;
  
  Serial.println("Writing test data");
  
  i2c_eeprom_write_byte(chipAddress, 0x0, 0x2a);
  i2c_eeprom_write_byte(chipAddress, 0x1, 0x3a);
  i2c_eeprom_write_byte(chipAddress, 0x2, 0x4a);
  i2c_eeprom_write_byte(chipAddress, 0x3, 0x5a);
  i2c_eeprom_write_byte(chipAddress, 0x4, 0x6a);
  i2c_eeprom_write_byte(chipAddress, 0x5, 0x7a);
  i2c_eeprom_write_byte(chipAddress, 0x6, 0x8a);
  
  Serial.println("Verifying data");
  
  v = i2c_eeprom_read_byte(chipAddress, 0x0);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(chipAddress, 0x1);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(chipAddress, 0x2);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(chipAddress, 0x3);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(chipAddress, 0x4);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(chipAddress, 0x5);
  Serial.println(v, HEX);

  v = i2c_eeprom_read_byte(chipAddress, 0x6);
  Serial.println(v, HEX);

}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  unsigned int offset = 0;
  //byte v;
  bool st = true;
  
  Serial.println("Writing data chunk 1");
  
  // write first data chunk
  for (uint32_t i = 0; i < flw_data_1_size; i++) {
    st = writeDataWithStatus(pgm_read_byte(flw_data_1 + i), offset++);
    
    if (!st) {
      Serial.println("Verification failed");
    }
    
    if (i % 1000 == 0)
      Serial.println(i);
  }

  Serial.println("Writing data chunk 2");

  // write second data chunk
  for (uint32_t i = 0; i < flw_data_2_size; i++) {
    st = writeDataWithStatus(pgm_read_byte(flw_data_2 +i), offset++);

    if (!st) {
      Serial.println("Verification failed");
    }
    
    if (i % 1000 == 0)
      Serial.println(i);
  }

  /*
  offset = 0;
  bool ret = true;

  Serial.println("Verifying data chunk 1");

  // verify first data chunk
  for (uint32_t i = 0; i < flw_data_1_size; i++) {
    if (!verifyDataWithStatus(pgm_read_byte(flw_data_1 +i), offset++)) {
      Serial.print("Verification failed at offset");
      Serial.println(offset-1);
      return;
    }
  }

  Serial.println("Verifying data chunk 2");

  // verify first data chunk
  for (uint32_t i = 0; i < flw_data_2_size; i++) {
    if (!verifyDataWithStatus(pgm_read_byte(flw_data_2 +i), offset++)) {
      Serial.print("Verification failed at offset");
      Serial.println(offset-1);
      return;
    }
  }
  */

  Serial.println("Written and verified successfully");
}

void loop()
{
}

