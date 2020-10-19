//*****************************************************************************
//
//                               d r i v e r
//
//                       oct 2020  EL htl.st.pölten
//
//
//*****************************************************************************
// initDriver mit Parameter EL_TEST_BOARD EL_AUTO

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "driver.h"

void adc_use (unsigned char x);
unsigned char adc_get (void);
void led_on(unsigned char x);
void led_off(unsigned char x);


void initDriver(char target)
{
    CLKPR = 0b10000000;                // Prescaler change enable!
    CLKPR = 0;                         // use 16 MHz - cycle time now 62.5 nsec
    MCUCR = MCUCR|(1 << JTD);		   // JTAG disable
    MCUCR = MCUCR|(1 << JTD);

    DDRD = 0xff;   // FLIP-LD | x | x | x |   x | x | x | x |
    DDRF = 0;      // read adc

//  ADC converter:

    ADCSRA  = (1 << ADPS2) | (1 << ADPS1);
    ADMUX  |= (1 << REFS0);             // reference Voltage
    ADMUX  |= (1 << ADLAR);
    ADCSRA |= (1 << ADATE);
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADSC);


    adc.use = adc_use;
    adc.get = adc_get;

//  leds:

    led.on  = led_on;
    led.off = led_off;
}

// ADC:

void adc_use (unsigned char x)
{
     if ((x & 04) == 0x4) ADMUX |= (1 << MUX2); else ADMUX &= ~(1 << MUX2);
     if ((x & 02) == 0x2) ADMUX |= (1 << MUX1); else ADMUX &= ~(1 << MUX1);
     if ((x & 01) == 0x1) ADMUX |= (1 << MUX0); else ADMUX &= ~(1 << MUX0);

    /* ADMUX =  REFS1 | REFS0 | ADLAR | MUX4 | MUX3 | MUX2 | MUX1 | MUX0 */
    /** ADC0 : 000
      * ADC1 : 001
      * ADC4 : 100
      * ADC5 : 101
      * ADC6 : 110
      * ADC7 : 111  = MUX2 + MUX1 + MUX0
      **/
}

unsigned char adc_get (void)
{
    return ADCH;
}


// LED:

void led_on(unsigned char x)
{
    switch (x)
    {
        case FLIP:  PORTD &= ~FLIP;  break;
    }

}
void led_off(unsigned char x)
{
    switch (x)
    {
        case FLIP:  PORTD |= FLIP;  break;
    }
}

