/*
 * Four Letter Word Generator
 * (C) 2011 Per Johan Groland
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
 
/*
 * Reads the following files:
 *  rs_concise, sr_concise
 *
 *  Generates the following file:
 *
 *  output.dat:
 *  <WORD>[<idx0...idxn]00
 *  where <WORD> is 4 bytes and each idx is 2 bytes
 *
 * After generating the file, which should be suitable as a data file for
 * a standalone Four Letter Word Generator, a demo is shown.
 */
 
class WordEntry {
  String m_word;
  int m_index;
  int m_offset;
  Vector m_associations;
  
  WordEntry(String word, int index) {
    m_word = word;
    m_index = index;
    m_associations = new Vector();
  }
  
  void AddAssociation(int index) {
    m_associations.add(index);
  }
  
  int size() { return m_associations.size(); }
  int elementAt(int e) { return (Integer)m_associations.elementAt(e); }
  void setOffset(int offset) { m_offset = offset; }
  
  int getByteSize() { return 4 + 1 + m_associations.size()*2 + 2; }
};

WordEntry[] arr;
int rand_idx = 0;

HashMap words;
HashMap word_index;
HashMap index_word;

BufferedReader rs_file;
BufferedReader sr_file;
String word;
String associations;

PFont fontA;

Boolean is_alpha(String s) {
  for (int i = 0; i < s.length(); i++) {
    if (s.charAt(i) >= 'A' && s.charAt(i) <= 'Z');
    else return false;
  }
  return true;
}

void setup() {
  size(300, 200);
  background(0);
  smooth();
  randomSeed(millis());

  fontA = loadFont("CourierNew36.vlw");
  textAlign(CENTER);

  // Set the font and its size (in units of pixels)
  textFont(fontA, 32);
  
  rs_file = createReader("rs_concise");
  sr_file = createReader("sr_concise");
  words = new HashMap(); // <String, Vector of String>
  word_index = new HashMap(); // <String, int>
  index_word = new HashMap(); // <int, String>

  int count = 0;
  int idx = 0;

  // read sr_concise
  do {
    try {
      word = sr_file.readLine();
      associations = sr_file.readLine();
    }
    catch (IOException e) {
      e.printStackTrace();
      break;
    }
    
    if (word != null && associations != null && word.length() == 4 && is_alpha(word)) {
      String[] ass = split(associations, "|");
        
      Vector temp = new Vector();
      for (int i = 0; i < ass.length; i+= 2) {
        if (ass[i].length() == 4  && is_alpha(ass[i]))
          temp.add(ass[i]);
      }
      
      if (temp.size() > 0) {
        words.put(word, temp);
        word_index.put(word, idx);
        index_word.put(idx++, word);
      }
    }
  } while (word != null);

  // read rs_concise
  do {
    try {
      word = rs_file.readLine();
      associations = rs_file.readLine();
    }
    catch (IOException e) {
      e.printStackTrace();
      break;
    }
    
    if (word != null && associations != null && word.length() == 4 && is_alpha(word)) {
      String[] ass = split(associations, "|");
        
      Vector current_word = (Vector)words.get(word);
        
      if (current_word != null) { // has word already
        for (int i = 0; i < ass.length; i+= 2) {
          if (ass[i].length() == 4  && is_alpha(ass[i]))
            if (!current_word.contains(ass[i]))
              current_word.add(ass[i]);
        }

        words.put(word, current_word);
      }
      else { // new word
        Vector temp = new Vector();
        for (int i = 0; i < ass.length; i+= 2) {
          if (ass[i].length() == 4  && is_alpha(ass[i]))
            temp.add(ass[i]);
        }
        
        if (temp.size() > 0) {
          words.put(word, temp);
          word_index.put(word, idx);
          index_word.put(idx++, word);
        }
      }
    }
  } while (word != null);

  // We now have:
  // words: Hash table of all words with a vector of associations
  // index_word: index to word mapping
  // word_index: word to index mapping
  //
  // Next: create the final data:
  //  array of WordEntry
  //   - index in array represents index of word
  // 

  FileOutputStream out = null;
  
  try {
    out = new FileOutputStream("output.dat");
  }
  catch (IOException e) {
    e.printStackTrace();
  }

  arr = new WordEntry[idx];
  
  int offset = 0;
  int next_offset = 0;
  
  int key = 0;
  
  // loop over words by index
  while (key < idx) {
    offset = next_offset;

    String word = (String)index_word.get(key);       
    Vector ass = (Vector)words.get(word);

    arr[key] = new WordEntry(word, key);
    arr[key].setOffset(offset);
    
    if (ass.size() == 0) println("damn");
    
    for (int x = 0; x < ass.size(); x++) {
       String s = (String)ass.elementAt(x);
       int key2 = (Integer)word_index.get(s);
       arr[key].AddAssociation(key2);
    }
    
    next_offset = offset + arr[key].getByteSize();
    key++;
  }

  rand_idx = (int)random(0, arr.length); 
  //rand_idx = 0;
  generateOutFile();
  generateOutFileArray();
}

void generateOutFile() {
  try {
    FileOutputStream out = new FileOutputStream("output.dat");
   
    for (int i = 0; i < arr.length; i++) {
      WordEntry we = arr[i];
     
      out.write(we.m_word.getBytes());
      
      if (we.size() > 255) println("uh-oh");
      
      byte s = (byte)we.size();
      out.write(s);
      
      for (int j = 0; j < we.size(); j++) {
        int array_index = we.elementAt(j);
        int byte_offset = arr[array_index].m_offset;
        
        if (byte_offset > 0xFFFF) println("uh-oh");
        
        if (byte_offset > 255) {
          byte high = (byte)(byte_offset >> 8);
          byte low  = (byte)(byte_offset & 0xFF);
          out.write(high);
          out.write(low);
        }
        else {
          byte high = 0;
          byte low  = (byte)byte_offset;
          out.write(high);
          out.write(low); 
        }
      }
      
      byte endmarker = (byte)0xFF;
      out.write(endmarker);
      out.write(endmarker);
    }
  }
  catch (IOException e) {
    e.printStackTrace();
  }
}

void startNewArray(int cnt)
{
  
}

void writeBytesToArray(byte[]b, FileWriter out)
{
  for (int i = 0; i < b.length; i++) {
    writeByteToArray("'" + (char)b[i] + "'", out);
  } 
}

long bytecounter = 0;

void writeByteToArray(String s, FileWriter out)
{
  if (out != null) {
    try {
      //out.write("'");
      out.write(s);
      out.write(",\n");
      
      bytecounter++;
      
      if (bytecounter == 30000)
      {
        out.write("};\n\n");
        out.write("const uint16_t flw_data_1_size = " + bytecounter + ";\n");
        out.write("const uint8_t flw_data_2[] PROGMEM = { \n");
      }

    }
    catch (IOException e) {
      e.printStackTrace();
    }
  } 
}

void generateOutFileArray() {
  try {
    FileWriter out = new FileWriter("flw_db.c");
   
    out.write("// Generated Four Letter Word Database\n");
    out.write("// https://github.com/perjg/fourletterword\n");
    out.write("#include <avr/pgmspace.h>");
    out.write("\n");
    out.write("const uint8_t flw_data_1[] PROGMEM = { \n");    
    
    print("Array length: " + arr.length);
    
    for (int i = 0; i < arr.length; i++) {      
      WordEntry we = arr[i];
     
      writeBytesToArray(we.m_word.getBytes(), out);
      
      //out.write(we.m_word.getBytes());
      
      if (we.size() > 255) println("uh-oh");
      
      out.write(Integer.toString(we.size()) + ",\n");
      
      for (int j = 0; j < we.size(); j++) {
        int array_index = we.elementAt(j);
        int byte_offset = arr[array_index].m_offset;
        
        if (byte_offset > 0xFFFF) println("uh-oh");
        
        if (byte_offset > 255) {
          int high = (int)(byte_offset >> 8);
          int low  = (int)(byte_offset & 0xFF);
          
          
          writeByteToArray(Integer.toString(high), out);
          writeByteToArray(Integer.toString(low),  out);
        }
        else {
          int high = 0;
          int low  = (int)byte_offset;
          writeByteToArray(Integer.toString(high), out);
          writeByteToArray(Integer.toString(low),  out);
        }
      }
      
      int endmarker = 0xFF;
      writeByteToArray(Integer.toString(endmarker), out);
      writeByteToArray(Integer.toString(endmarker), out);
    }
    
    out.write("\n};\n");
    out.write("const uint16_t flw_data_2_size = " + (bytecounter - 30000) + ";\n");
    
    out.close();
  }
  catch (IOException e) {
    e.printStackTrace();
  }
}


void draw() {
  WordEntry we = arr[rand_idx];

  int foo = (int)random(0, we.size()-1);
  rand_idx = we.elementAt(foo);
  
  fill(0);
  rect(0, 0, 300, 200);
  fill(255);
  text(we.m_word, random(45, 255), random(25, 190));

  delay(500);
}

