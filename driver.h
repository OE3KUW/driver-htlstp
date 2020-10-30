//*****************************************************************************
//
//                               d r i v e r . h
//
//
// Elektronik und Technische Informatik      dezember 2020 Wolfgang Uriel KURAN
//*****************************************************************************
// C o n f i g u r a t i o n   L i n e s :                   © htl st.pölten EL

//-----------------------------------------------------------------------------
// target:
//-----------------------------------------------------------------------------
#define EL_TEST_BOARD                  0    // Testboardonly
#define DIS_TEST                       1    // 1-line Display conncted on PortB
#define DIS2_TEST                      2    // as DIS_TEST but with two lines
#define EL_ROBOT                       3    // Roboter i2c-disply two lines
//-----------------------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------------------

#define TRUE                           1
#define FALSE                          0
#define HIGH                           1
#define LOW                            0
#define RELEASED                       1
#define PRESSED                        0

//-----------------------------------------------------------------------------
// LEDs und BEEPER:
//-----------------------------------------------------------------------------

#define FLIP                           0x80    // on board led

#ifdef  EL_ROBOT

#define RIGHT_FRONT                    0x01  // used for blinker
#define RIGHT_REAR                     0x02
#define LEFT_FRONT                     0x04
#define LEFT_REAR                      0x08

#define DUAL_GREEN                     0x10	// PB4
#define DUAL_RED                       0x20 // PB5
#define DUAL_YELLOW                    0x30 // PB4 + PB5

#define BEEPER_CLICK                   0x80  // used for the beepeer


#endif // EL_ROBOT

//-----------------------------------------------------------------------------
// ADC:
//-----------------------------------------------------------------------------

#define ADC0                           0x0
#define ADC1                           0x01
#define ADC4                           0x04
#define ADC5                           0x05
#define ADC6                           0x06
#define ADC7                           0x07

#define ADC_VOLTAGE                    28.6  // depends on settings


// braucht das wer? - falls jka - wer?
#define NO_DISPLAY_AVAILABLE           0
#define DISPLAY_AVAILABLE              1
#define DISPLAY_WITH_2_LINES           2
#define DISPLAY_I2C_CONNECTED          3
#define DISPLAY_2LINES_I2C_CONNECTED   4


//-----------------------------------------------------------------------------
//
//                   c o m p o u n d   c o m p o n e n t s :
//
//-----------------------------------------------------------------------------


typedef struct struct_led LED_TYPE;  // this is used for Testboards and for the Flip
struct struct_led
{
    void (*on)         (unsigned char);
    void (*off)        (unsigned char);
    void (*barMeterLin)(unsigned char);
    void (*number)     (unsigned char);

};
LED_TYPE led;

//-----------------------------------------------------------------------------
// General Precondition for all led functions:
// use them only, if the targe is EL_ROBOT !  except FLIP can be used always
// on the other hand:
// the function barMeterLin can only be used for the target EL_TEST_BOARD
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// led.on(x);
//-----------------------------------------------------------------------------
// What 4: this function turns leds on
// IN: x can be RIGHT:FRONT for e.g. there is a list of defines preparated
// POST: the selected led is turned on
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// led.off(x);
//-----------------------------------------------------------------------------
// What 4: this function turns leds off
// IN: x can be DUAL_YELLOW for e.g.
// POST: the selected led is switched off
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// led.barMeterLin(x);
//-----------------------------------------------------------------------------
// What 4: this function turns a led-chain on. For higher x - values more leds
//         are turned on. Maximal 8, minimal zero.
// PRE: EL_TEST_BOARD is used for target
// IN: x - a unsigned char value
// POST: the led-chain is turned on
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// led.number(i)
//-----------------------------------------------------------------------------
// What 4: this function turns leds on the edge of the robot on
// IN:  i an number between 0 and 3
// POST: for 0 the RIGHT_FRONT led is switched on
//       for 1 the RIGHT_REAR led is switched on
//       for 2 the LEFT_REAR led is switched on
//       for 3 the LEFT_FRONT led is switched on
// RETURN: nothing
//-----------------------------------------------------------------------------


typedef struct struct_beeper BEEPER_TYPE;  // this is used for Testboards and for the Flip
struct struct_beeper
{
    void (*click)         (void);
};
BEEPER_TYPE beeper;

//-----------------------------------------------------------------------------
// beeper.click();
//-----------------------------------------------------------------------------
// What 4:  this functin produce a click on the beeper of the robot
// PRE: use this function only for the robot (target = EL_ROBOT)
// IN: nothing
// POST: 'click'
// RETURN: nothing
//-----------------------------------------------------------------------------

typedef struct struct_adc ADC_TYPE;
struct struct_adc
{
    void (*use)(unsigned char);
    unsigned char (*get)(void);
};
ADC_TYPE adc;

// usage:
//-----------------------------------------------------------------------------
// adc.use(ADC0);
//-----------------------------------------------------------------------------
// What4:  this function selects an ADC-Pin, for e.g. ADC0 ---> PORTF0
// IN:     ADC0 - choose one of the switchs:
//         ADC0. ADC1, ADC4, ADC5, ADC6 or ADC7 (all are defines ...)
//         attention for the EL roboter: PORTF6, and PORTF7
//         is used for the infra red system - so dont use: ADC6 and ADC7
// POST:   the multiplexer is switched
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// x = adc.get();
//-----------------------------------------------------------------------------
// What 4:  get the measured adc - level
// IN:      nothing
// POST:    nothing
// RETURN:  x  from type unsigned char - 0 up to 255
//-----------------------------------------------------------------------------


typedef struct struct_eeprom EEPROM_TYPE;
struct struct_eeprom
{
    void (*storeInt)(int, unsigned int);
    int (*getInt)(unsigned int);
};
EEPROM_TYPE eeprom;

//-----------------------------------------------------------------------------
// eeprom.storeInt(value, address);
//-----------------------------------------------------------------------------
// What 4: with this function an integer value can be stored in te e2prom
// PRE: the targe address must be between 0 and 0x100
// IN: value any integer, address the position where the value should be stored
// POST: the vlaue is stored permanently
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// i = eeprom.getInt(address);
//-----------------------------------------------------------------------------
// What 4: with this function an stored integer can be get from the e2prom
// PRE: the targe address must be between 0 and 0x100
// IN: the address where the value is stored.
// POST: nothing
// RETURN: i is an integer - get from the e2prom
//-----------------------------------------------------------------------------


typedef struct struct_i2c I2C_TYPE;
struct struct_i2c
{
    void (*write)(char addr, char data);

};
I2C_TYPE i2c;

// i2c wird erst später weiter entwickelt...


typedef struct display_struct DISPLAY;
struct display_struct
{
// public:
    void (*writeChar)(char a);
    void (*setCursor)(char x);
    void (*writeString)(char * s);
    void (*writeString2ndLine)(char * s);
    void (*writeInt)(int i);
    void (*writeFloat)(float x);  // wird erst entwicklet...
    void (*hideCursor)(void);
    void (*showCursor)(void);
    void (*storeSymbol)(char s[], char space);
    void (*clear)(void);
//  private:
    int shownCursorPosition;
    int Linelength;
};
DISPLAY display;

//-----------------------------------------------------------------------------
// General Precondition for all led - functions:
// 1) for all display functions - the target DIS_TEST, DIS2_TEST or EL_ROBOT
// must be used as parameter for initDriver!
// reaoson: EL_TESTBOARD uses the PORT B for the led chain
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.writeChar('X');
//-----------------------------------------------------------------------------
// What 4: to write characters on the display
// IN: 'X' any character
// NOTE: ü, ä, ö, Ü, Ä, Ö will be transformed automatically
// NOTE2: character numbers 0,1,2,3,4,5,6,7 are reservated for symbols
// POST: display shows the character at the position of the cursor
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.setCursor(position);
//-----------------------------------------------------------------------------
// What 4: tis function sets the cursor
// NOTE: allowed values are 0 to 15 - higher numbers will be corrected
// IN: the wanted cursor position
// POST: the cursor ist set correctly - if the number is between 0 and 15
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.writeString("text");
//-----------------------------------------------------------------------------
// What 4: this functin write out any string
// NOTE: it allways starts an cursor-positon 0
// IN: text any string
// POST: the text is shown on the display
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.writeString2ndLine("text");
//-----------------------------------------------------------------------------
// What 4: this function writes in the 2nd line any text
// PRE DIS2_TEST must be used as target od EL_ROBOT
// IN: text - any string
// POST: the text is shown in the 2nd line of the display
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.writeInt(i);
//-----------------------------------------------------------------------------
// What 4: an integer number will be written on the display
// IN: i any integer - even negativ values are allowed
// POST: the number is shown at the cursor position
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.writeFloat(x);
//-----------------------------------------------------------------------------
// What 4: an float value will be written on the display
// PRE: x is any float value - even negativ values are allowed
// IN: x any float value
// POST: the value is shown on the display
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.storeSymbol(a, n);
//-----------------------------------------------------------------------------
// What 4: a own symbol can be created and stored by this function
// IN: a is the adress of a array sith 8 binary-numbers,
//     n a number between 0 and 7   stands for the position, where the
//     special character should be stored
// for e.g.  char a[] ={0b11100100,
//                      0b00001010,
//                      0b00001010,
//                      0b00010001,
//                      0b00010001,
//                      0b00010001,
//                      0b00011111,
//                      0b00010001,
//                      0b00010001};  creates a symbol siilar to an 'A'
// POST: a later display.writeChar(n);  shows the stored special symbol
// RETURN:nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.clear();
//-----------------------------------------------------------------------------
// What 4: this functin clears the disply
// IN: nothing
// POST: the display is cleared
// RETURN: nothing
//-----------------------------------------------------------------------------







// Prototyps:

void initDriver(char target);



void delay(int msec); // use this function only if you have good reasons for!


#ifndef __OPTIMIZE__
# warning "SET [-Os]  in Project > Build options! schoene Gruesse von AV Kuran   "
# warning "                                                                      "​
# warning "             ___   ________  __________  ___   ________               "​
# warning "            /   | / ____/ / / /_  __/ / / / | / / ____/               "​
# warning "           / /| |/ /   / /_/ / / / / / / /  |/ / / __                 "​
# warning "          / ___ / /___/ __  / / / / /_/ / /|  / /_/ /                 "​
# warning "_______  /_/  |_\____/_/ /_/ /_/  \____/_/ |_/\____/ ___________._____"
# warning "======================================================================"​
#endif    // created by 3A and 3BHELS - thanx!
