/* four letter word generator
 *
 * SDA -> analog 4 (bottom) 
 * SCL -> analog 5 (top)
 */

#include <Wire.h>
#include <TWIDisplay.h>

#define SLAVE_ADDR 0x12
TWIDisplay disp(SLAVE_ADDR);

#define chipAddress 0b1010000
unsigned long offset = 0;
char current_word[6];

byte read_byte(int device, unsigned int addr) {
  byte rdata = 0xFF;
  Wire.beginTransmission(device);
  Wire.write((int)(addr >> 8)); // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(device,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void read_buffer(int device, unsigned int addr, byte *buffer, int length) {
  Wire.beginTransmission(device);
  Wire.write((int)(addr >> 8)); // MSB
  Wire.write((int)(addr & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(device,length);
  int c = 0;
  for ( c = 0; c < length; c++ )
    if (Wire.available()) buffer[c] = Wire.read();
}

// check if there is an eeprom installed with the fourletterword database
bool has_eeprom()
{
   byte b1 = read_byte(chipAddress, 0); 
   byte b2 = read_byte(chipAddress, 0); 
   
   if (b1 == 65 && b2 == 65)
     return true;
   return false;
}

// gets word at offset and picks a random one to go next
unsigned long get_word(unsigned long offset, char* word) {
	unsigned char low = 0xFF, high = 0xFF;
	unsigned int next_offset;
	unsigned char count = 0;
	int next = 0;

        //Serial.print("Reading word at offset ");
        //Serial.println(offset, DEC);
        read_buffer(chipAddress, offset, (byte*)word, 5);
        count = word[4];
        word[4] = NULL;

	next = random(0, count-1);
        //Serial.print("Next word at ");
        //Serial.print(next, DEC);

        offset += 5 + next * 2;

	high = read_byte(chipAddress, offset++);
	low  = read_byte(chipAddress, offset++);

	next_offset = (high << 8) | low;

	return next_offset;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  randomSeed(analogRead(0));

  if (has_eeprom())
    Serial.println("Found EEPROM!");
  else
    Serial.println("No EEPROM found, this won't work");

  offset = 0;
  
  // randomize starting point
  for (int i = 0; i < 10; i++) {
    offset = get_word(offset, current_word);
  }
}

void loop() {
  offset = get_word(offset, current_word);
  Serial.println(current_word);
  
  disp.clear();
  disp.print(current_word);
  
  delay(500);
}

