/*
 * Four Letter Word Generator
 * (C) 2011 Per Johan Groland, Sangwhan Moon (J2SE)
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.    See the GNU General Public License for more details.
 *
 */
 
/*
 * Reads the following files:
 *    rs_concise, sr_concise
 *
 *    Generates the following file:
 *
 *    output.dat:
 *    <WORD>[<idx0...idxn]00
 *    where <WORD> is 4 bytes and each idx is 2 bytes
 *
 * After generating the file, which should be suitable as a data file for
 * a standalone Four Letter Word Generator, a demo is shown.
 */

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

import java.util.HashMap;
import java.util.Random;
import java.util.Vector;

public class fourletterword
{ 
    // FIXME: This is a quick and dirty hack to make it work, not shine
    
    public static final class FourLetterWordEmulator
    {
        public class WordEntry {
            private String m_word;
            private int m_index;
            private int m_offset;
            private Vector m_associations;

            WordEntry(String word, int index) {
                m_word = word;
                m_index = index;
                m_associations = new Vector();
            }

            void AddAssociation(int index) {
                // FIXME: Check return value, although Collections.add(Object o) always returns true
                m_associations.add(index);
            }

            public int size() { return m_associations.size(); }
            public int elementAt(int e) { return (Integer)m_associations.elementAt(e); }
            public void setOffset(int offset) { m_offset = offset; }
            public int getOffset() { return m_offset; }

            public int getByteSize() { return 4 + 1 + m_associations.size()*2 + 2; }
            public String getWord() { return m_word; }
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

        Random random;

        public void setRandomIndex(int idx)
        {
            rand_idx = idx;
        }

        public int getRandomIndex()
        {
            return rand_idx;
        }

    	private Boolean is_alpha(String s) {
    	    for (int i = 0; i < s.length(); i++) {
    	        if (s.charAt(i) >= 'A' && s.charAt(i) <= 'Z');
    	        else return false;
    	    }
    	    return true;
    	}

    	private Boolean setup() {
    	    random = new Random(System.currentTimeMillis());

            try {
        	    rs_file = new BufferedReader(new InputStreamReader(new FileInputStream("rs_concise")));
        	    sr_file = new BufferedReader(new InputStreamReader(new FileInputStream("sr_concise")));            
            }
            catch (FileNotFoundException e){
                e.printStackTrace();
                return false;
            }
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
    	            return false;
    	        }

    	        if (word != null && associations != null && word.length() == 4 && is_alpha(word)) {
    	            String[] ass = associations.split("\\|");

    	            Vector temp = new Vector();
    	            for (int i = 0; i < ass.length; i+= 2) {
    	                if (ass[i].length() == 4    && is_alpha(ass[i]))
    	                {
    	                    // FIXME: Check return value, although Collections.add(Object o) always returns true
    	                    temp.add(ass[i]);
    	                }
    	            }

    	            if (temp.size() > 0) {
    	                // FIXME: Check return value for nulls, HashMap returns do mean something
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
    	            return false;
    	        }

    	        if (word != null && associations != null && word.length() == 4 && is_alpha(word)) {
    	            String[] ass = associations.split("\\|");

    	            Vector current_word = (Vector)words.get(word);

    	            if (current_word != null) { // has word already
    	                for (int i = 0; i < ass.length; i+= 2) {
    	                    if (ass[i].length() == 4    && is_alpha(ass[i]))
    	                        if (!current_word.contains(ass[i]))
    	                        {
    	                            // FIXME: Check return value, although Collections.add(Object o) always returns true
    	                            current_word.add(ass[i]);
    	                        }
    	                }

                        // FIXME: Check return value for nulls, HashMap returns do mean something
    	                words.put(word, current_word);
    	            }
    	            else { // new word
    	                Vector temp = new Vector();
    	                for (int i = 0; i < ass.length; i+= 2) {
    	                    if (ass[i].length() == 4    && is_alpha(ass[i]))
    	                    {
    	                        // FIXME: Check return value, although Collections.add(Object o) always returns true
    	                        temp.add(ass[i]);
    	                    }
    	                        
    	                }

    	                if (temp.size() > 0) {
    	                    // FIXME: Check return value for nulls, HashMap returns do mean something
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
    	    //    array of WordEntry
    	    //     - index in array represents index of word
    	    // 

    	    FileOutputStream out = null;

    	    try {
    	        out = new FileOutputStream("output.dat");
    	    }
    	    catch (IOException e) {
    	        e.printStackTrace();
    	        return false;
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

    	        if (ass.size() == 0)
    	            System.out.println("damn");

    	        for (int x = 0; x < ass.size(); x++) {
    	             String s = (String)ass.elementAt(x);
    	             int key2 = (Integer)word_index.get(s);
    	             arr[key].AddAssociation(key2);
    	        }

    	        next_offset = offset + arr[key].getByteSize();
    	        key++;
    	    }

    	    rand_idx = random.nextInt(arr.length); 
    	    //rand_idx = 0;
    	    Boolean generateResult = generateOutFile();
	    
    	    return generateResult;
    	}

    	private Boolean generateOutFile() {
    	    try {
    	        FileOutputStream out = new FileOutputStream("output.dat");

    	        for (int i = 0; i < arr.length; i++) {
    	            WordEntry we = arr[i];

    	            out.write(we.m_word.getBytes());

    	            if (we.size() > 255)
    	                System.out.println("uh-oh");

    	            byte s = (byte)we.size();
    	            out.write(s);

    	            for (int j = 0; j < we.size(); j++) {
    	                int array_index = we.elementAt(j);
    	                int byte_offset = arr[array_index].m_offset;

    	                if (byte_offset > 0xFFFF)
    	                    System.out.println("uh-oh");

    	                if (byte_offset > 255) {
    	                    byte high = (byte)(byte_offset >> 8);
    	                    byte low    = (byte)(byte_offset & 0xFF);
    	                    out.write(high);
    	                    out.write(low);
    	                }
    	                else {
    	                    byte high = 0;
    	                    byte low    = (byte)byte_offset;
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
    	        return false;
    	    }
	    
    	    return true;
    	}
    }
    
	public static void main(String argv[]) {
	    
	    Random random = new Random();
	    FourLetterWordEmulator flwe = new FourLetterWordEmulator();
	    
	    Boolean setupResult = flwe.setup();
	    
	    if (setupResult != false)
		{
	        while(true)
	        {
	            FourLetterWordEmulator.WordEntry we = flwe.arr[flwe.getRandomIndex()];
	            
	            int foo = (we.size()-1 <= 0) ? 0 : random.nextInt(we.size()-1);
                flwe.setRandomIndex(we.elementAt(foo));

                System.out.print("\r" + we.m_word);

                try {
                    Thread.sleep(500);
                }
                catch (InterruptedException e) {
                    // Let's assume this won't happen.
                }
	        }
		}

        else
        {
            System.out.println("Error while initializing");
        }

	}	
}