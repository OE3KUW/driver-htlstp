//*****************************************************************************
//
//                               d r i v e r
//
//                       oct 2020  EL htl.st.pölten
//
//
//*****************************************************************************

// target:
#define EL_TEST_BOARD   0
#define EL_AUTO         1

#define TRUE                           1
#define FALSE                          0


#define NO_DISPLAY_AVAILABLE           0
#define DISPLAY_AVAILABLE              1
#define DISPLAY_WITH_2_LINES           2
#define DISPLAY_I2C_CONNECTED          3
#define DISPLAY_2LINES_I2C_CONNECTED   4

#define ADC0            0x0
#define ADC1            0x01
#define ADC4            0x04
#define ADC5            0x05
#define ADC6            0x06
#define ADC7            0x07

#define FLIP            0x80    // on board led

// compound components:

typedef struct struct_led LED_TYPE;
struct struct_led
{
    void (*on)         (unsigned char);
    void (*off)        (unsigned char);
    void (*barMeterLin)(unsigned char);

};
LED_TYPE led;

typedef struct struct_adc ADC_TYPE;
struct struct_adc
{
    void (*use)(unsigned char);
    unsigned char (*get)(void);
};
ADC_TYPE adc;

typedef struct struct_i2c I2C_TYPE;
struct struct_i2c
{
    void (*write)(char addr, char data);

};
I2C_TYPE i2c;


// baustelle...
typedef struct display_struct DISPLAY;
struct display_struct
{
    int displayLinelength;
    void (*writeChar)(char a);
    void (*setCursor)(char x);
    void (*writeString)(char * s);
    void (*writeString2ndLine)(char * s);
    void (*writeInt)(int i);
    void (*writeInt02)(int i);
    void (*hideCursor)(void);
    void (*showCursor)(void);
    void (*storeSymbol)(char s[], char space);
    void (*clear)(void);
};
DISPLAY display;




// Prototyps:

void initDriver(char target);









#ifndef __OPTIMIZE__
# warning "                                                                      "​
# warning "             ___   ________  __________  ___   ________               "​
# warning "            /   | / ____/ / / /_  __/ / / / | / / ____/               "​
# warning "           / /| |/ /   / /_/ / / / / / / /  |/ / / __                 "​
# warning "          / ___ / /___/ __  / / / / /_/ / /|  / /_/ /                 "​
# warning "_______  /_/  |_\____/_/ /_/ /_/  \____/_/ |_/\____/ ___________._____"
# warning "SET [-Os]  in Project > Build options! schoene Gruesse von AV Kuran   "
# warning "======================================================================"​
#endif    // created by 3A and 3BHELS - thanx!
