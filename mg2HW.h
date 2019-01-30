/* 
	
mg2HW.cpp library
this library is to be used with standuino MicroGranny hardware
MOZZI library compatible
documentation of the library you find here:
mg2HW library

Created by Vaclav Pelousek 2013 www.pelousek.eu
for Standuino wwww.standuino.eu

*/

#ifndef mg2HW_h
#define mg2HW_h

#include "Arduino.h"
#include <avr/pgmspace.h>
#include "portManipulations.h"
#include "shiftRegisterFast.h"

#define DEFAULT 0
#define UNFREEZE_EXTERNALY 1

#define NUMBER_OF_ROWS 6

// the digital pins used to measure the button states
// used as INPUT_PULLUP
#define BUTTON_PIN 6
#define BUTTON_2_PIN 7

#define LED_PIN 6 
#define LED_2_PIN 7
#define LED_ROW 4
#define LED_2_ROW 5

#define DIGIT_1_PIN 2
#define DIGIT_2_PIN 3
#define DIGIT_3_PIN 4
#define DIGIT_4_PIN 5 //6


#define NUMBER_OF_BIG_BUTTONS 6
// a mapping from sample buttons to column pins
#define BIG_1 0
#define BIG_2 3
#define BIG_3 4
#define BIG_4 5
#define BIG_5 6
#define BIG_6 7


#define FN 11
#define PAGE 8

#define UP 10
#define DOWN 9

#define REC 2
#define HOLD 1

// a mapping from Column Shift Register pin to LED segment
#define SEG_A 5 //Display pin 11
#define SEG_B 7 //Display pin 7
#define SEG_C 3 //Display pin 4
#define SEG_D 1 //Display pin 2
#define SEG_E 2 //Display pin 1
#define SEG_F 6 //Display pin 10
#define SEG_G 4 //Display pin 15
#define SEG_DOT 0 //Display pin 3

#define LED_R_PIN 9
#define LED_G_PIN 8
#define LED_B_PIN 10


#define KNOB_LED_1_PIN 15 //5
#define KNOB_LED_2_PIN 14 //2
#define KNOB_LED_3_PIN 13 //3
#define KNOB_LED_4_PIN 12 //6

#define KNOB_LED_1 15
#define KNOB_LED_2 14
#define KNOB_LED_3 13
#define KNOB_LED_4 12

#define KNOB_PIN_1 1
#define KNOB_PIN_2 2
#define KNOB_PIN_3 3
#define KNOB_PIN_4 4

const unsigned char bigButton[NUMBER_OF_BIG_BUTTONS] = {BIG_1, BIG_2, BIG_3, BIG_4, BIG_5, BIG_6};

#define KNOB_TOLERANCE 2
#define KNOB_FREEZE_DISTANCE 128

#define NUMBER_OF_KNOBS 4

#define NUMBER_OF_BUTTONS 16

#define NUMBER_OF_DIGITS 4


#define XXX 9
#define IND 12
#define INT 15
#define PAR 18
#define DIR 21
#define PCH 24
#define CRS 27
#define STR 30
#define CTT 33
#define SHF 36
#define LOP 39
#define RTE 42
#define AMT 45
#define ONN 48
#define OFF 51
#define FWD 54
#define BCK 57
#define RD1 60
#define RD2 63
#define PRE 66
#define SEL 69

#define ND 72
#define ST 75
#define REP 78
#define DIR 81
#define LFO 84
#define VOL 87
#define COP 90
#define SVE 93
#define SVD 96
#define LPS 99
#define LPE 102
#define CPD 105
#define PST 108
#define CLR 111




#define FACTORY_CLEAR_PIN 2
#define FACTORY_CLEAR_SIGNAL_PIN 13


#define VOID 36
#define MINUS 37
#define LINES 38
#define SLASH 39

#define ZERO 0

const unsigned char knobLed[NUMBER_OF_KNOBS]={KNOB_LED_1_PIN,KNOB_LED_2_PIN,KNOB_LED_3_PIN,KNOB_LED_4_PIN};

#define LED_R 9
#define LED_G 12
#define LED_B 8
#define NUMBER_OF_COLORS 8




#define BLACK 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define MAGENTA 5
#define CIAN 6
#define WHITE 7

/*
Segments
 -  A
 F / / B
 -  G
 E / / C
 -  D
 */

#define CLOCK_PIN 8
#define LATCH_PIN 9
#define DATA_PIN 19

class mg2HW
{
  public: 
  
  
  	

    mg2HW(); // constructor
	void initialize();
	void update();
	//void updateButtons();
	void dimForRecord(unsigned char _BUTTON);

        void setLed(unsigned char _LED, boolean _STATE);
        void setColor(unsigned char _COLOR);
    
        bool buttonState(unsigned char _BUTTON); 
	bool justPressed(unsigned char _BUTTON);
	bool justReleased(unsigned char _BUTTON);
	
	bool knobFreezed(unsigned char _KNOB);
	bool knobMoved(unsigned char _KNOB);
	
	void freezeAllKnobs();
	void unfreezeAllKnobs();
	void freezeKnob(unsigned char _KNOB);
	void unfreezeKnob(unsigned char _KNOB);
	int knobValue(unsigned char _KNOB);
	int lastKnobValue(unsigned char _KNOB);
	void setLastKnobValue(unsigned char _KNOB,int _val);
//	void setFreezeType(unsigned char _TYPE);
	
	
	void flipSwitch(unsigned char _SWITCH);
	void setSwitch(unsigned char _SWITCH, boolean _STATE);
	bool switchState(unsigned char _SWITCH);
	void resetSwitches();
	
	void displayText(char *text);
	void displayChar(char whichChar,unsigned char _digit) ;
	void lightNumber(int numberToDisplay, unsigned char _digit);
	void displayNumber(int _number);
	void setDot(unsigned char _dot, boolean _state);

	unsigned char soundFromSwitches();
	unsigned char soundFromButtons();
	
//	bool factoryClear();
//	void factoryCleared();
	void updateKnobs();
	void updateButtons();
	void updateDisplay();
	
	int _row;
        // elements 0-3 contain bytes that represents the alphanumeric character to display on that digit.
        unsigned char displayBuffer[NUMBER_OF_ROWS];


  //private: 
  	
        // bit strings that represent the hw state and events
	int buttonStateHash;
	int lastButtonStateHash;
	int switchStateHash;
	int justPressedHash;
	int justReleasedHash;
	int ledStateHash;
	unsigned char knobFreezedHash = 0b11111111;
	unsigned char knobChangedHash;
	
        // both of these instantiate to all zeros
	int knobValues[NUMBER_OF_KNOBS];
	int lastKnobValues[NUMBER_OF_KNOBS];
	
	bool mozzi;
	bool unfreezeExternaly;

	
};

#endif
