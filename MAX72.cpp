/*
  MAX7219/MAX7221
  Maxim MAX7219/MAX7221 Class for Arduino
  Created by Cort Buffington
  July 2012

This class implements several methods for using the MAX72xx family of LED controllers. All methods for display writing (turning LEDs on and off) store the display state in an 8x8 RAM matrix. Some of the methods will update (and display) the entire 8x8 matrix, while some will only "update" parts of it. Hardware SPI is used. The focus of this class is on speed and efficiency. It is limited in that it ONLY addresses the chip(s) in matrix mode, i.e. does not use the onboard 7 segment decoding features.

NOTE: Reducing the Scan Limit value below maximum will mess up the intensity control. The number of items the chip scans is linked to how PWM dimming is implemented. Unless you're really strapped for resources, or don't care about dimming, you'll get the best results by leaving it at 8 (maximum).

*/

#include <SPI.h>                 // Arduino IDE SPI library - uses AVR hardware SPI features
#include "MAX72.h"               // Header files for this class

// Defines to keep logical information symbolic go here

#define    HIGH          (1)
#define    LOW           (0)
#define    ON            (1)
#define    OFF           (0)
#define    OUTPUT        (1)
#define    INPUT         (0)

// Here we have things for the SPI bus configuration

#define    CLOCK_DIVIDER (2)           // SPI bus speed to be 1/2 of the processor clock speed - 8MHz on most Arduinos


// Constructor to instantiate an instance of MAX72 to a specific chipSelect digital pin and with the number of digits specified to scan

MAX72::MAX72(uint8_t chipSelect, uint8_t numdigits) {
  _chipSelect = chipSelect;
  _numDigits = numdigits;     // Register values 0-7 for 1-8 digits

  pinMode(_chipSelect, OUTPUT);
  digitalWrite(_chipSelect, HIGH);

  SPI.begin();                          // Start up the SPI bus… crank'er up Charlie!
  SPI.setClockDivider(CLOCK_DIVIDER);   // Sets the SPI bus speed
  SPI.setBitOrder(MSBFIRST);            // Sets SPI bus bit order (this is the default, setting it for good form!)
  SPI.setDataMode(SPI_MODE0);           // Sets the SPI bus timing mode (this is the default, setting it for good form!)

  writeRegister(SHUTDOWN, 0x0);         // Shut down the display while initializing things...
  dispClear();				                  // Blank the display
  writeRegister(SCANLIMIT, _numDigits - 1);	// Set the scan limit (Register values 0-7 for 1-8 digits) for the chip to scan
  writeRegister(DECODEMODE, 0x0);	      // Set no BCD segment decoding
  writeRegister(INTENSITY, 0xf);	      // Start at maximum brightness
  writeRegister(SHUTDOWN, 0x1);		      // Turn the display back on
}

// Clears the entire display… or at least as much as was defined to scan when instantiating the object.

void MAX72::dispClear() {
  for (uint8_t i = 0; i < _numDigits; i++) {
    _displayCache[i] = 0x0;
    writeRegister(DIGITREGISTER(i), _displayCache[i]);
  }
}

// Turns on every LED on the display… or at least as much as was defined to scan when instantiating the object.

void MAX72::dispAll() {
  for (uint8_t i = 0; i < _numDigits; i++) {
    _displayCache[i] = 0xff;
    writeRegister(DIGITREGISTER(i), _displayCache[i]);
  }
}

// Causes the RAM buffer to be re-written to the chip. Handy if something goes wrong, or you just become unsure of the state.

void MAX72::dispRefresh() {
  for (uint8_t i = 0; i < _numDigits; i++) {
    writeRegister(DIGITREGISTER(i), _displayCache[i]);
  }
}

// Sets the Intensity of the output (PWM), valid values are 0-15.

void MAX72::setIntensity(uint8_t intensity) {
  if (intensity <= 0xf) {
    writeRegister(INTENSITY, intensity);
  }
}

// Sets the digit (columns) specified with the segment values specified.

void MAX72::setDigit(uint8_t digit, uint8_t segments) {
  if (0 <= digit < _numDigits) {
    _displayCache[digit] = segments;
    writeRegister(DIGITREGISTER(digit), _displayCache[digit]);
  }
}

// Sets the "matrix" combination of digits (columns) and segments (rows) supplied.

void MAX72::setMatrix(uint8_t digits, uint8_t segments) {
  //uint8_t colmask = 0b00000001;
  for (uint8_t i = 0, colmask = 0b00000001; i < _numDigits; i++, colmask <<= 1) {
    if (colmask & digits) {
      _displayCache[i] = segments;
      writeRegister(DIGITREGISTER(i), _displayCache[i]);
    } else {
      _displayCache[i] = 0x0;
      writeRegister(DIGITREGISTER(i), _displayCache[i]);
    }
    //colmask <<= 1;
  }
}

// Special method - assumes all digits are enabled, and sets the segments (rows) specified on every digit (column)

void MAX72::setRows(uint8_t segments) {
  for (uint8_t i = 0; i < _numDigits; i++) {
    _displayCache[i] = segments;
    writeRegister(DIGITREGISTER(i), _displayCache[i]);
  }
}

// Special method - assumes all segments (rows) are enabled and sets the digits (columns) specified on every segment (row)

void MAX72::setColumns(uint8_t digits) {
  uint8_t colmask = 1;
  for (uint8_t i = 0; i < _numDigits; i++) {
    if (colmask & digits) {
      _displayCache[i] = 0xff;
      writeRegister(DIGITREGISTER(i), _displayCache[i]);
    } else {
      _displayCache[i] = 0x0;
      writeRegister(DIGITREGISTER(i), _displayCache[i]);
    }
    colmask = colmask << 1;
  }
}

// Sets an individual pixel located at digit (column), segment (row), to state "state"  (ON/OFF, HIGH/LOW, 1/0).

void MAX72::setPixel(uint8_t digit, uint8_t segment, boolean state) {
  if (! (0 <= digit < _numDigits)) {
    return;
  }
  
  if (state) {
    _displayCache[digit] |= (HIGH << segment);
    writeRegister(DIGITREGISTER(digit), _displayCache[digit]);
  } else {
    _displayCache[digit] &= ~ (HIGH << segment);
    writeRegister(DIGITREGISTER(digit), _displayCache[digit]);
  }
}

// Internal method for actually writing to the MAX7219/21 chip. Not for public use.

void MAX72::writeRegister(uint8_t chipRegister, uint8_t chipValue) {
//  PORTD &= ~ (1 << _chipSelect);
  digitalWrite(_chipSelect, LOW);
  SPI.transfer(chipRegister);
  SPI.transfer(chipValue);
//  PORTD |= (1 << _chipSelect);
  digitalWrite(_chipSelect, HIGH);
}
