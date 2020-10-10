//*****************************************************************************
//
//                               d r i v e r
//
//                       oct 2020  EL htl.st.pölten
//
//
//*****************************************************************************

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
    void (*on)  (unsigned char);
    void (*off) (unsigned char);

};
LED_TYPE led;

typedef struct struct_adc ADC_TYPE;
struct struct_adc
{
    void (*use)(unsigned char);
    unsigned char (*get)(void);
};
ADC_TYPE adc;


// Prototyps:

void initDriver(void);









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
