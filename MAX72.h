/*
  MAX7219/MAX7221
  Maxim MAX7219/MAX7221 Class for Arduino
  Created by Cort Buffington
  July 2012

This class implements several methods for using the MAX72xx family of LED controllers. All methods for display writing (turning LEDs on and off) store the display state in an 8x8 RAM matrix. Some of the methods will update (and display) the entire 8x8 matrix, while some will only "update" parts of it. Hardware SPI is used. The focus of this class is on speed and efficiency. It is limited in that it ONLY addresses the chip(s) in matrix mode, i.e. does not use the onboard 7 segment decoding features.

NOTE: Reducing the Scan Limit value below maximum will mess up the intensity control. The number of items the chip scans is linked to how PWM dimming is implemented. Unless you're really strapped for resources, or don't care about dimming, you'll get the best results by leaving it at 8 (maximum).

*/


#ifndef MAX72_h
#define MAX72_h

// REGISTER AND CONSTANT DEFINITIONS

// When referencing the digitregister in the chip, it's 1-8 instead of 0-7.
//                              Everything else is 0-7, go figure...
#define DIGITREGISTER(x) ((x) + 1)

#define DECODEMODE (0x9)
#define INTENSITY (0xA)
#define SCANLIMIT (0xB)
#define SHUTDOWN (0xC)
#define DISPLAYTEST (0xF)

#include <Arduino.h>

class MAX72 {
  public:
    MAX72(uint8_t, uint8_t);			// Constructor to instantiate a discrete IC as an object and set up the chip
    void dispClear();				// Clears the entire display
    void dispAll();				// Sets all LEDs to on
    void dispRefresh();				// Writes the RAM buffer to the chip… just in case you need to do this
    void setIntensity(uint8_t);			// Sets the on-chip PWM intensity, valid values are 0-15
    void setDigit(uint8_t, uint8_t);		// Sets a the specified digit with the specified segment values
    void setMatrix(uint8_t, uint8_t);		// Sets the entire display with matrix values digits, segments (columns, rows)
    void setColumns(uint8_t);			// Sets the digits (columns) as specified, turns all segments (rows) on
    void setRows(uint8_t);			// Sets the segments (rows) as specified, turns all digits (columns) on
    void setPixel(uint8_t, uint8_t, boolean);	// Sets or clears the pixel at the specified digit (column) and segment (row)

  private:
    uint8_t _chipSelect;			// digital pin for chip select, must be discrete per instantiation
    uint8_t _numDigits;
    uint8_t _displayCache[8];			// RAM matrix for display caching
    void writeRegister(uint8_t, uint8_t);
 };
		
#endif //MAX72
