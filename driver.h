//*****************************************************************************
//
//                               d r i v e r
//
//                       oct 2020  EL htl.st.pölten
//
//
//*****************************************************************************

// target:
//-----------------------------------------------------------------------------
#define EL_TEST_BOARD                  0
#define DIS_TEST                       1    // Display conncted on PortB of the EL tst board
#define DIS2_TEST                      2    // as DIS_TEST but with two lines
#define EL_ROBOTER                     3    // this system has a DISPLAY_2LINES_I2C_CONNECTED
                                            // connected via i2c bus on a portexpander-IC
#define EL_ROBOTER_DIS2                4    // Diplay with two lines on a roboter





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

// usage:
//-----------------------------------------------------------------------------
// adc.use(pinNr);
//-----------------------------------------------------------------------------
// What4:  this function selects an ADC-Pin, for e.g. ADC0 --- PORTF0
// IN:     pinNr switchs the multiplex modul,
//         choose on of the defines: ADC0. ADC1, ADC4, ADC5, ADC6 or ADC7
//         attention: PORTF6, and PORTF7 can also be used for ir - component
// POST:   the multiplexer is switched
// RETURN: nothing
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// unsigned char x = adc.get();
//-----------------------------------------------------------------------------
// What 4:




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
    void (*writeFloat)(float x);  // wird erst entwicklet...
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
