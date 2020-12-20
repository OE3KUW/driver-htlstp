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
#define DIS_I2C                        3    // Display i2c connected
#define EL_ROBOT                       4    // Roboter i2c-disply two lines

//#define LOCK_DOWN

//-----------------------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------------------

#define TRUE                           1
#define FALSE                          0
#define HIGH                           1
#define LOW                            0

//-----------------------------------------------------------------------------
// LEDs und BEEPER and KEYs:
//-----------------------------------------------------------------------------

#define FLIP                           0x80    // on board led

#define RIGHT_FRONT                    0x01  // used for blinker
#define RIGHT_REAR                     0x02
#define LEFT_FRONT                     0x04
#define LEFT_REAR                      0x08

#define DUAL_GREEN                     0x10	// PB4
#define DUAL_RED                       0x20 // PB5
#define DUAL_YELLOW                    0x30 // PB4 + PB5
#define DUAL_LED                       0x30 // zuseful to switch the led off

#define BEEPER_CLICK                   0x80  // used for the beepeer

#define KEY0                           0x01
#define KEY1                           0x02
#define KEY2                           0x04
#define KEY3                           0x08

#define BOUD_RATE_9600                 102

//-----------------------------------------------------------------------------
// ADC:
//-----------------------------------------------------------------------------

#define ADC0                           0x0
#define ADC1                           0x01
#define ADC4                           0x04

//#define ADC5                         0x05  // dont use this! (infra red sensor)
//#define ADC6                         0x06  // dont use this! (infra red sensor)
//#define ADC7                         0x07  // dont use this! (wheel control)

// note: ADC5,ADC6 and ADC7 are not in use, 'cause ADC5 and ADC6 are used for
// the infra red sensor - they are using the same port pins
// and port pi 7 is used for the wheel control


#define ADC_VOLTAGE                    28.6  // depends on settings

//-----------------------------------------------------------------------------
// I-RED:
//-----------------------------------------------------------------------------

#define IRED_FRONT                     0x0
#define IRED_BACK                      0x40
#define IRED_LEFT                      0x0
#define IRED_RIGHT                     0x20

#define ON                             1
#define OFF                            0


// braucht das wer? - falls jka - wer?
//#define NO_DISPLAY_AVAILABLE           0
//#define DISPLAY_AVAILABLE              1
//#define DISPLAY_WITH_2_LINES           2
//#define DISPLAY_I2C_CONNECTED          3
//#define DISPLAY_2LINES_I2C_CONNECTED   4


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
//    void (*use)(unsigned char);
    unsigned char (*get)(unsigned char);
    unsigned char MeasuredValues[5];  //ADC0, ADC1, x, x, ADC4
};
ADC_TYPE adc;

// usage:
//-----------------------------------------------------------------------------
// adc.use(ADC0);
/** this function is not in use anymore !!! **/
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
// x = adc.get(ADC0);
//-----------------------------------------------------------------------------
// What 4:  get the measured adc - level
// IN:      ADC0, ADC1 or ADC4 - from which pin should the level be measured
// POST:    nothing
// RETURN:  x  from type unsigned char - 0 up to 255
// NOTE: the hardware provides only one analog digital converter
// a multiplexer selectes the port pin connection from the adc to the
// used port pin. This is handeld by the timer interrupt.
// means all 10 msec a next portpin will be connected to be measured
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

typedef struct struct_keys KEYS_TYPE;
struct struct_keys
{
// public:
    char (*pressed)(char key);
    char (*released)(char key);
    void (*acknowledge)(void);
    void (*quit)(void);  // will be deleted later!
    char (*stillPressed)(char key); // new
// private:
    volatile char last_keys;
    volatile char next_keys;
    volatile char flags;

};
KEYS_TYPE key;

//-----------------------------------------------------------------------------
// if (key.pressed(KEY0) == TRUE)
//-----------------------------------------------------------------------------
// What 4: this function can be used to get key-information
// PRE: keys can only be used for target = EL_TEST_BOARD, DIS_TEST and
//      DIS2_TEST
//      KEY2 and KEY3 can be used for DIS_I2C
// IN: KEY0 = a defined value for a key. see define - list
// POST: nothing
// RETURN: TRUE if KEY0 is pressed
// NOTE: after any key-event use key.acknowledge(); to be prepared for the next event
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// if (key.released(KEY0))
//-----------------------------------------------------------------------------
// What 4: this function can be used to get key-information
// PRE: keys can only be used for target = EL_TEST_BOARD, DIS_TEST and
//      DIS2_TEST
//      KEY2 and KEY3 can be used for DIS_I2C
// IN: KEY0 = a defined value for a key. see define - list
// POST: nothing
// RETURN:  FALSE if KEY0 is released
// NOTE: after any key-event use key.acknowledge(); to be prepared for the next event
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// key.acknowledge();     // former key.quit();
//-----------------------------------------------------------------------------
// What 4: use this function to acknowledge a key-event
// PRE: keys can only be used for target = EL_TEST_BOARD, DIS_TEST and
//      DIS2_TEST
//      KEY2 and KEY3 can be used for DIS_I2C
// IN: nothing
// POST: internal flags will be reseted.
// RETURN: nothing
// NOTE: the former name key.quit will be deleted later
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// if(key.stillPressed(KEY0))
//-----------------------------------------------------------------------------
// What 4: to find out - that a key wasn't relesed in between
// PRE: keys can only be used for target = EL_TEST_BOARD
// IN: KEY0 = a defined value for a key. see define - list
// POST: nothing
// RETURN: TRUE or FALSE
//-----------------------------------------------------------------------------

typedef struct struct_serial SERIAL_TYPE;
struct struct_serial
{
// public:
    void (*cb)(char);
    void (*send)(char);
    void (*storeMyCallBackFunction)(void (*cb)(char));
// private:
    volatile char received;
    volatile char flag;

};
SERIAL_TYPE serial;

//-----------------------------------------------------------------------------
// serial.send('A');
//-----------------------------------------------------------------------------
// What 4: this function sends a character via the UART
// PRE: hardware connected, PC programm receives - for EL_ROBOT only!
// IN: 'A' any character
// POST: the string is sent character by character - 9600 boud
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// serial.storeMyCallBackFunktion(receiveFunction);
//-----------------------------------------------------------------------------
// What 4: this fundction stores the address of the call-back-function
// PRE: hardware connected, PC programm receives
//      an own callback function from type: void f(char); must exist.
//      - for EL_ROBOT only!
// IN: the address of the own callbackfunction
// POST: the adress is stored.
// RETURN: nothing
//-----------------------------------------------------------------------------

typedef struct struct_motor MOTOR_TYPE;
struct struct_motor
{
// public:
    void (*setSpeed)(char);
    void (*setDiff)(char);
    void (*stop)(void);
//private:
    volatile unsigned char left;
    volatile unsigned char right;
//    unsigned char leftC;
//    unsigned char rightC;

    volatile char speed;
    volatile char diff;
};
MOTOR_TYPE motor;

//-----------------------------------------------------------------------------
// motor.setSpeed(s);
//-----------------------------------------------------------------------------
// What 4: this fuction defines how fast the motor drives forward
// PRE: target == EL_ROBOT
// IN:  s = integer value
// POST: motor drives forward with a speed set by s
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// motor.setDiff(d);
//-----------------------------------------------------------------------------
// What 4: this function defines how fast the motor turns around
// PRE: target == EL_ROBOT
// IN: d an integer value, it defines how fast the motor turns
// POST: motor turns around with a speed set by d
// RETURN: nothing
//-----------------------------------------------------------------------------

typedef struct struct_iRed IRED_TYPE;
struct struct_iRed
{
// public:
    void (*selectDirection)(char); // Front or Back
    void (*selectSide)(char);      // Left or Right
    void (*selectQuarter)(char);
    void (*switchTransmitter)(char);
    char (*receivedSignal)(void);
    void (*acknowledge)(void);
// private:
    volatile char flag;
    volatile char transmit;
};
IRED_TYPE iRed;

//-----------------------------------------------------------------------------
// iRed.selectDirection(IRED_FRONT);
//-----------------------------------------------------------------------------
// What 4: select receiving and transmitting direction from iRed signals
// PRE: hardware must have all iRed sensors :-)
// IN:  IRED_FRONT or IRED_BACK
// POST: the directionfor receiving and for transmitting is choosen
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// iRed.selectSide(IRED_LEFT);
//-----------------------------------------------------------------------------
// What 4: select the side to send iRed signals
// PRE: hardware must have all iRed sensors :-)
// IN: IRED_LEFT or IRED_RIGHT
// POST: the side is selected to send iRed signals
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// iRed.selectQuarter(n);
//-----------------------------------------------------------------------------
// What 4: selcts for receiving front or rear, and for sending the quarter
// PRE: same as above - hardware must work
// IN: n 0 FRONT_RIGHT, 1 BACK_RIGHT, 2 BACK_LEFT and 3 FRONT_LEFT
// POST:
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// iRed.switchTransmitter(ON);
//-----------------------------------------------------------------------------
// What 4: with this function, the iRed sending can be started or stopped
// IN: ON or OFF
// POST: the iRed signal will be sended on the choosen quarter - or stopped
// RETURN: nothing
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// if(iRed.receivedSignal() == TRUE)
//-----------------------------------------------------------------------------
// What 4: was there in the meantime any iRed signal received?
// PRE: FRONT or REAR must be selected correct,
//            own oszillator should not switched on for own sendings
// IN: nothing
// POST: nothing
// RETURN: TRUE if there was a iRedSignal be received
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// iRed.acknowledge();
//-----------------------------------------------------------------------------
// What 4: acknowledge the iRed flag
// IN: nothing
// POST: the iRed flag is set to FALSE - as long as there is a new signal detected
// RETURN: nothing
//-----------------------------------------------------------------------------

typedef struct struct_lineFollower LINE_FOLLOWER_TYPE;
struct struct_lineFollower
{
    void (*on)(void);
    void (*off)(void);
    unsigned char (*right)(void);
    unsigned char (*left)(void);
};
LINE_FOLLOWER_TYPE lineF;

//-----------------------------------------------------------------------------
// lineF.on();
//-----------------------------------------------------------------------------
// What 4: this function turns the line-follower on
// IN: nothing
// POST: the line-follower LED is switched on
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// lineF.ff();
//-----------------------------------------------------------------------------
// What 4: this function turns the line-follower off
// IN: nothing
// POST: the line-follower LED is switched off
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// lineF.right();
//-----------------------------------------------------------------------------
// What 4: measure the refelction value of the line-follower
// IN: nothing
// POST: nothing
// RETURN: returns the right line-follower value
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// lineF.left();
//-----------------------------------------------------------------------------
// What 4: measure the refelction value of the line-follower
// IN: nothing
// POST: nothing
// RETURN: returns the left line-follower value
//-----------------------------------------------------------------------------


typedef struct struct_timeCounter TIME_COUNTER_TYPE;
struct struct_timeCounter
{
    char (*expired)(void);
    void (*start)(int mSec);
    int  (*remaining)(void);
    volatile int tenMsec;
};
TIME_COUNTER_TYPE timeCounter;
TIME_COUNTER_TYPE timeCounter2;
TIME_COUNTER_TYPE timeCounter3;

//-----------------------------------------------------------------------------
// timeCounter.start(1000);
//-----------------------------------------------------------------------------
// What 4: this function starts e timer-down-counter
// IN: 1000 = an integer value, duration time in milliseconds
// POST: from the start on - the timer counts down. (each 10 msec - a tick)
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// if (timeCounter.expired())
//-----------------------------------------------------------------------------
// What 4: this function observe the timer
// IN: nothing
// POST: nothing
// RETURN: TRUE if the timer is expired, FALSE else
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// t = timeCounter.remaining();
//-----------------------------------------------------------------------------
// What 4: this function observe the remaining time from this timeCounter
// IN: nothing
// POST: nothing
// RETURN: the remaining time in millisekonds
//-----------------------------------------------------------------------------

typedef struct struct_i2c I2C_TYPE;
struct struct_i2c
{
    void (*write)(char addr, char data);
    void (*writeDis)(char addr, char data);

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
    volatile int tenMsec;
    volatile int shownCursorPosition;
    volatile int Linelength;
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
//                      0b00010001};  creates a symbol siilar to an 'A'
// POST: a later display.writeChar(n);  shows the stored special symbol
// RETURN:nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// display.clear();
//-----------------------------------------------------------------------------
// What 4: this functin clears the disply
// IN: nothing
// POST: the cursor position is set to 0 !!!
// RETURN: nothing
//-----------------------------------------------------------------------------

void initDriver(char target);



void delay(int msec); // use this function only if you have good reasons for!


#ifndef __OPTIMIZE__
# warning "SET [-Os]  in Project > Build options! schoene Gruesse von AV Kuran   "
# warning "                                                                      "​
# warning "             ___   ________  __________  ___   ________               "​
# warning "            /   | / ____/ / / /_  __/ / / / | / / ____/               "​
# warning "           / /| |/ /   / /_/ / / / / / / /  |/ / / __                 "​
# warning "          / ___ / /___/ __  / / / / /_/ / /|  / /_/ /                 "​
# warning "         /_/  |_\____/_/ /_/ /_/  \____/_/ |_/\____/                  "
# warning "                                                                      "​
# warning "======================================================================"​
#endif    // created by 3A and 3BHELS (2019/2020)- thanx!
