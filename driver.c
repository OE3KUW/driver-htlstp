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

#define DISPLAY_8_BIT_MODE      0x30
#define DISPLAY_4_BIT_MODE      0x20
#define DISPLAY_CLEAR           0x08
#define DISPLAY_SET_FUNCTION    0x28    // 4-bit-Mode, zweizeilig
#define DISPLAY_OFF             0x01
#define DISPLAY_SHIFT_OFF       0x04
#define DISPLAY_ENTRY_MODE      0x06    // Shift on
#define DISPLAY_ON              0x0e    // display on, cursor on
#define DISPLAY_ON_CURSOR_OFF   0x0c
#define DISPLAY_SET_CURSOR      0x80
#define DATA                    0x10

#define I2C_ADDR                0b01000000 // MCP23008 Portexpander
#define E	                    5
#define RS	                    4
#define DB4	                    0
#define DB5	                    1
#define DB6	                    2
#define DB7	                    3


// ==========================  GLOBAL VARIABLES:   =============================

char _useI2Cdisplay;

// ==========================  PROTOTYPS:   ====================================

void adc_use (unsigned char x);
unsigned char adc_get (void);
void led_on(unsigned char x);
void led_off(unsigned char x);
void led_barMeterLin(unsigned char x);



//  i2C:
void i2cInit(void);
void i2c_Write(char addr, char data);
void i2c_Start(void);
int  i2c_Wait(void);
void i2c_Stop(void);

// display functions:
//  display:

void _writeChar(char a);
void _setCursor(char x);
void _writeString(char * s);
void _writeString2ndLine(char *s);
void _writeInt(int i);
void _writeFloat(float x);
void _hideCursor(void);
void _showCursor(void);
void _storeSymbol(char s[], char space);
void _clear(void);

//  no display:
void _doNothing_Void(void);
void _doNothing_Char(char);
void _doNothing_Int(int);
void _doNothing_Float(float);
void _doNothing_String(char *);
void _doNothing_Array_Char(char s[], char space);
void _internalCallBackDoNothing(char c);

void _writeCommand8(char rs, char command);
void _writeCommand4(char rs, char command);

// das Letzte:
void _wait_64_usec(void);
void delay(int msec);


void initDriver(char target)
{
    CLKPR = 0b10000000;                // Prescaler change enable!
    CLKPR = 0;                         // use 16 MHz - cycle time now 62.5 nsec
    MCUCR = MCUCR|(1 << JTD);		   // JTAG disable
    MCUCR = MCUCR|(1 << JTD);


//PORT B:
    DDRB = 0xff;   // Leds
    PORTB = 0;

//  PORT C:
	DDRC  = (1<<DDC6);				   // PC6 ist #RST from the Portexpander
    PORTC = (1<<PC7);                  // capture Interrupt	PORTC |= (1<<PC6);				   // Portexpander switched off
	PORTC |= (1<<PC6);

//  PORT D:
    DDRD = 0xff;   // FLIP-LD | x | x | x |   x | x | x | x |

//  PORT F:
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
    led.barMeterLin = led_barMeterLin;

//  i2c-Container:

	TWBR = 12;     // TWBR=12, TWPS=0 im Reg. TWSR per default;  set f_SCL = 400 kHz
	i2cInit();                         // needs PORT D Pin 0 = CLK and 1 = SDA

//  i2c:

//  i2C:  -- Container

    i2c.write = i2c_Write;

//  display:

    display.displayLinelength = ((target == DIS2_TEST) || (target == EL_ROBOTER_DIS2 )) ? 16 : 8;

    if (target)   // all systems with an display have numbers greater than 0
    {
            display.writeChar          = _writeChar;
            display.setCursor          = _setCursor;
            display.writeString        = _writeString;
            display.writeString2ndLine = _writeString2ndLine;
            display.writeInt           = _writeInt;
            display.writeFloat         = _writeFloat;
            display.hideCursor         = _hideCursor;
            display.showCursor         = _showCursor;
            display.storeSymbol        = _storeSymbol;
            display.clear              = _clear;

        // initialize LC-Display:

            delay(50);

        if (target < EL_ROBOTER)  // the roboter uses i2c port expander
        {
            _useI2Cdisplay = FALSE;

            _writeCommand8(0, DISPLAY_8_BIT_MODE);    delay(20);
            _writeCommand8(0, DISPLAY_8_BIT_MODE);    delay(20);
            _writeCommand8(0, DISPLAY_8_BIT_MODE);    delay(20);
            _writeCommand8(0, DISPLAY_4_BIT_MODE);    delay(400);
            _writeCommand4(0, DISPLAY_CLEAR);         delay(20);
            _writeCommand4(0, DISPLAY_SET_FUNCTION);  delay(20);
            _writeCommand4(0, DISPLAY_OFF);           delay(20);
            _writeCommand4(0, DISPLAY_SHIFT_OFF);     delay(20);
            _writeCommand4(0, DISPLAY_ENTRY_MODE);    delay(20);
            _writeCommand4(0, DISPLAY_ON);            delay(20);
        }
        else  // EL ROBOTER uses i2c with an portexpander for the display:
        {
            _useI2Cdisplay = TRUE;

            _writeCommand8(0, 0x03);                  delay(20);  // 8 BIT MODE
            _writeCommand8(0, 0x03);                  delay(20);
            _writeCommand8(0, 0x03);                  delay(20);
	        _writeCommand8(0, 0x02);                  delay(400); // 4 BIT MODE


	        _writeCommand4(0, 0x01);   delay(20);// Clear Display
            _writeCommand4(0, 0x28);   delay(20);// Z-zeilig, 5x8 Matrix
            _writeCommand4(0, 0x08);   delay(20);// Display Off
            _writeCommand4(0, 0x03);   delay(20);// No Display Shift
            _writeCommand4(0, 0x06);   delay(20);// Entry Mode
            _writeCommand4(0, 0x0e);   delay(20);// Display ON

        }
    }
    else
    {
            display.setCursor   = _doNothing_Char;
            display.writeChar   = _doNothing_Char;
            display.writeString = _doNothing_String;
            display.writeInt    = _doNothing_Int;
            display.writeFloat  = _doNothing_Float;
            display.hideCursor  = _doNothing_Void;
            display.showCursor  = _doNothing_Void;
            display.storeSymbol = _doNothing_Array_Char;
            display.clear       = _doNothing_Void;
    }


}  // end of initDriver

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

void led_barMeterLin(unsigned char x)
{
    PORTB = (x > 254) ? 0xff : (0xff >> (8 - (x >> 5))); // Lukas Stadler 3BHELS 2020
}



//  i2C:

void i2cInit(void)
{

   i2c_Start();    TWDR = 0x40;        // Adr. 0100 000W + Write (W=0)
   i2c_Wait();     TWDR = 0x05;        // write Registeradr. IOCON
   i2c_Wait();     TWDR = 0x2A;        // configure IOCON: Byte Mode:
                                       // Slew Rate enable, no Open Drain by INTn, INTn actice-high
   i2c_Stop();


//  configure GP with IODIR as OUTPUT:

   i2c_Start();	   TWDR = 0x40;        // Adr. 0100 000W + Write (W=0)
   i2c_Wait();     TWDR = 0x00;        // Registeradr. IODIR
   i2c_Wait(); 	   TWDR = 0x00;        // configure IODIR: all Pins OUTPUT

   i2c_Stop();
}

void i2c_Write(char addr, char data)
{
	delay(2);                                    i2c_Start();
	TWDR = addr & 0xfe;/* Adr & Write (= 0)  */  i2c_Wait();
	TWDR = 0x0A;                                 i2c_Wait();
	TWDR = data | (1 << E); /* Enable = HIGH */  i2c_Wait();
	TWDR = data &~(1 << E); /* Enable = LOW  */  i2c_Stop();
}

void i2c_Start(void)
{
unsigned int t = 0;

    TWCR = TWCR | (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); /* wait */
    while ((!(TWCR & (1 << TWINT))) && (t < 60000)) t++;

    // je nachdem: falls nur deshalb while beendet wurde, weil t = 60000 - dann ist der Ic wohl nicht da....
}
int i2c_Wait(void)
{
unsigned int t = 0;

    TWCR = (1 << TWINT) | (1 << TWEN); /* send and  wait */
    while ((!(TWCR & (1 << TWINT))) && (t < 60000)) t++;
    return (t < 60000) ? 1 : 0;
}
void i2c_Stop(void)
{
unsigned int t = 0;
    TWCR = (1 << TWINT) | (1 << TWEN); /* send and  wait */
    while ((!(TWCR & (1 << TWINT))) && (t < 60000)) t++;
	TWCR = TWCR | (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // STOP
}

// display functions:
void _doNothing_Void(void)              {}
void _doNothing_Char(char a)            {}
void _doNothing_Int(int i)              {}
void _doNothing_Float(float x)          {}
void _doNothing_String(char * s)        {}
void _doNothing_Array_Char(char s[], char space) {}
void _internalCallBackDoNothing(char c) { }

// display functions:

void _writeCommand8(char rs, char command)
{
char x = 0xC0 | command;	// prepare for P7 and P6 has to be HIGH for PCF8574

    if (_useI2Cdisplay == FALSE)
    {

        command >>= 4;
        PORTB = command | rs;
        PORTB |= 0x20;                     // enable
        {asm volatile ("nop"); }
        PORTB &= ~0x20;
        _wait_64_usec();
        PORTB = 0x0;
    }
    else
    {
        if (rs == DATA) x = x |  (1 << RS) ;  // for characters
        else            x = x & ~(1 << RS) ;  // for commands
        x = x | (1<<E);
        i2c.write(I2C_ADDR, command);
    }
}

void _writeCommand4(char rs, char command)
{
char temp = command, x = 0xC0;	// prepare for P7 and P6 has to be HIGH for PCF8574

    if (_useI2Cdisplay == FALSE)
    {
        PORTB = ((command >> 4) & 0x0f) | rs;
    //  enable:
        PORTB |= 0x20; {asm volatile ("nop"); } PORTB &= ~0x20; _wait_64_usec();

        PORTB = ((command) & 0x0f) | rs;
    //  enable:
        PORTB |= 0x20; {asm volatile ("nop"); } PORTB &= ~0x20; _wait_64_usec();
        PORTB = 0x0;
    }
    else
    {

        if (rs == DATA) x = x |  (1 << RS);  // for characters
        else            x = x & ~(1 << RS);  // for commands

        x = x | (1<<E);

	//  Upper Nibble:

        if (temp & 0b10000000) {x = x | (1<<DB7);} else {x = x & ~(1<<DB7);}
        if (temp & 0b01000000) {x = x | (1<<DB6);} else {x = x & ~(1<<DB6);}
        if (temp & 0b00100000) {x = x | (1<<DB5);} else {x = x & ~(1<<DB5);}
        if (temp & 0b00010000) {x = x | (1<<DB4);} else {x = x & ~(1<<DB4);}

        i2c.write(I2C_ADDR, x);

	//  Lower Nibble:

        if (temp & 0b00001000) {x = x | (1<<DB7);} else {x = x & ~(1<<DB7);}
        if (temp & 0b00000100) {x = x | (1<<DB6);} else {x = x & ~(1<<DB6);}
        if (temp & 0b00000010) {x = x | (1<<DB5);} else {x = x & ~(1<<DB5);}
        if (temp & 0b00000001) {x = x | (1<<DB4);} else {x = x & ~(1<<DB4);}

        i2c.write(I2C_ADDR, x);

    }
}

void _writeChar(char a)
{
    _writeCommand4(DATA, a);
}

void _setCursor(char x)
{
    if (x < 0) x = 0;

    if (display.displayLinelength == 8)
    {
        if (x == 8) x += 0x38; // 0x40 - 8  = 0x38
    }
    if (display.displayLinelength == 16)
    {
        if (x >= 16) x += 0x30; // 0x40 - 16 = 0x30
    }
    _writeCommand4(0, DISPLAY_SET_CURSOR | x);
}

void _writeString(char * s)
{
int i = 0;
int j = 0;


    _setCursor(0);

    while ((i - j < display.displayLinelength) && (*(s+i)))
    {
        if (*(s+i) == -61)
        {
            i++; j++;

            switch (*(s+i))
            {
                case  -68: *(s+i) = 0xf5; /*ö*/ break;
                case  -74: *(s+i) = 0xef; /*ö*/ break;
                case  -92: *(s+i) = 0xe1; /*ä*/ break;
                case  -97: *(s+i) = 0xe2; /*ß*/ break;
                case -124: *(s+i) = 0xe1; /*ä*/ break;
                case -106: *(s+i) = 0xef; /*ö*/ break;
                case -100: *(s+i) = 0xf5; /*ü*/ break;

            }
        }

        _writeChar(*(s+i));
        i++;
    }

    _setCursor(display.displayLinelength); // position 8 jumps intern to position 0x40 !
    while ((i - j < (display.displayLinelength << 1)) && (*(s+i)))
    {

        if (*(s+i) == -61)
        {
            i++; j++;

            switch (*(s+i))
            {
                case  -68: *(s+i) = 0xf5; /*ö*/ break;
                case  -74: *(s+i) = 0xef; /*ö*/ break;
                case  -92: *(s+i) = 0xe1; /*ä*/ break;
                case  -97: *(s+i) = 0xe2; /*ß*/ break;
                case -124: *(s+i) = 0xe1; /*ä*/ break;
                case -106: *(s+i) = 0xef; /*ö*/ break;
                case -100: *(s+i) = 0xf5; /*ü*/ break;

            }
        }

        _writeChar(*(s+i));
        i++;
    }
}

void _writeString2ndLine(char *s)
{
int i = 0;

    if (display.displayLinelength > 8)
    {
        _writeCommand4(0, DISPLAY_SET_CURSOR | 0x40);
        while ((i < display.displayLinelength) && (*(s+i)))
        {
            _writeChar(*(s+i));
            i++;
        }
    }
}

void _writeInt(int i)
{
char neg = FALSE;

        if (i > 999)  i =  999;
        if (i < -999) i = -999;

        if (i < 0)
        {
            neg = TRUE; i = -i;
        }

        ((neg == TRUE) && (i >=100)) ? _writeChar('-') : _writeChar(' ');

        if (i < 100)
            ((neg == TRUE) && (i >= 10)) ? _writeChar('-') : _writeChar(' ');
        else
        {
            _writeChar((i/100)%10 + '0');
        }


        if (i < 10)
            (neg == TRUE) ? _writeChar('-') : _writeChar(' ');
        else
        {
            _writeChar((i/10)%10 + '0');
        }

        _writeChar((i)%10 + '0');

}

void _writeFloat(float x)
{
int i;

    if (x < 0)
    {
        _writeChar('-');
        x = -x;
    }

    if (x >= 1000.) {i = (int)x; i %= 10000; i /= 1000; _writeChar(i + '0');}
    if (x >=  100.) {i = (int)x; i %=  1000; i /=  100; _writeChar(i + '0');}
    if (x >=   10.) {i = (int)x; i %=   100; i /=   10; _writeChar(i + '0');}
    if (x >=    0.) {i = (int)x; i %=    10;            _writeChar(i + '0');}
    else _writeChar('0');
         _writeChar('.');

    while (x > 100) x-= 100; // to avoid overflows!

    i = (int)(x * 10.);   i %= 10;                      _writeChar(i + '0');
    i = (int)(x * 100.);  i %= 10;                      _writeChar(i + '0');
    i = (int)(x * 1000.); i %= 10;                      _writeChar(i + '0');
}

void _hideCursor(void)
{
     _writeCommand4(0, DISPLAY_ON_CURSOR_OFF);
}

void _showCursor(void)
{
     _writeCommand4(0, DISPLAY_ON);
}

void _storeSymbol(char s[], char space)
{

    char base = space << 3;
    int i;

    for (i = 0; i < 8; i++)
    {
        _writeCommand4(0, 0x40 + base + i);
        _writeCommand4(DATA, s[i]&0x1f);
    }

    _writeCommand4(0, 0);  // switch back from DATA!
}

void _clear(void)
{
    display.writeString("                ");
    display.writeString2ndLine("                ");
}








//  WAITING TIMES - SMALLES UNIT  50 micro seconds
void _wait_64_usec(void)
{
    int i;
    for(i = 0; i < 200; i++) {asm volatile ("nop"); }
}


void delay(int msec)
{
    msec <<= 4;  //
    for(; msec > 0 ; msec--) _wait_64_usec();
}

