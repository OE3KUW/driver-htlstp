//*****************************************************************************
//
//
//                               d r i v e r . c
//
//
// Elektronik und Technische Informatik      dezember 2020 Wolfgang Uriel KURAN
//*****************************************************************************
// C o n f i g u r a t i o n   L i n e s :                   © htl st.pölten EL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "driver.h"

//-----------------------------------------------------------------------------
//  intern Defines:
//-----------------------------------------------------------------------------

#define KEY3_PRESSED            0x80
#define KEY2_PRESSED            0x40
#define KEY1_PRESSED            0x20
#define KEY0_PRESSED            0x10

#define KEY3_RELEASED           0x08
#define KEY2_RELEASED           0x04
#define KEY1_RELEASED           0x02
#define KEY0_RELEASED           0x01


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


#define TIMER_START_VALUE       0xb100
#define TIMER_START_VALUE_0     0xd3    // 0xd3  für 38.5 kHz

// MOTOR:

#define PWM_HOVER               93

// iREd:
#define I_COUNTER_MAX           12




// ==========================  GLOBAL VARIABLES:   =============================

char _useI2Cdisplay;
char system_target;
unsigned char iCounter;

// ==========================  PROTOTYPS:   ====================================

//-----------------------------------------------------------------------------
void led_on(unsigned char x);
void led_off(unsigned char x);
void led_barMeterLin(unsigned char x);
void led_number(unsigned char i);
//-----------------------------------------------------------------------------
void beeper_click(void);
//-----------------------------------------------------------------------------
void adc_use (unsigned char x);
unsigned char adc_get (void);
//-----------------------------------------------------------------------------
void eeprom_storeInt(int value, unsigned int address);
int eeprom_getInt(unsigned int address);
//-----------------------------------------------------------------------------
char key_pressed(char k);
char key_released(char k);
void key_quit(void);
char key_stillPressed(char k);
//-----------------------------------------------------------------------------
void timeCounter_start(int mSev);
char timeCounter_expired(void);
void timeCounter2_start(int mSev);
char timeCounter2_expired(void);
void timeCounter3_start(int mSev);
char timeCounter3_expired(void);
//-----------------------------------------------------------------------------
void serial_send(char c);
//char serial_receive(void); not needed ???
void serial_storeMyCallBackFunction(void (*cb)(char));
void _internalCallBackDoNothing(char c);
//-----------------------------------------------------------------------------
void motor_setSpeed(char s);
void motor_setDiff(char d);
void motor_stop(void);
//-----------------------------------------------------------------------------
void iRed_selectDirection(char d);
void iRed_selectSide(char s);
void iRed_selectQuarter(char n);
void iRed_switchTransmitter(char onOff);
char iRed_receivedSignal(void);
void iRed_quit(void);
//-----------------------------------------------------------------------------
void i2cInit(void);
void i2c_Write(char addr, char data);
void i2c_Start(void);
int  i2c_Wait(void);
void i2c_Stop(void);
//-----------------------------------------------------------------------------
void display_writeChar(char a);
void display_setCursor(char x);
void display_writeString(char * s);
void display_writeString2ndLine(char *s);
void display_writeInt(int i);
void display_writeFloat(float x);
void display_hideCursor(void);
void display_showCursor(void);
void display_storeSymbol(char s[], char space);
void display_clear(void);
//-----------------------------------------------------------------------------
void _doNothing_Void(void);
void _doNothing_Char(char);
void _doNothing_Int(int);
void _doNothing_Float(float);
void _doNothing_String(char *);
void _doNothing_Array_Char(char s[], char space);
void _internalCallBackDoNothing(char c);
//-----------------------------------------------------------------------------
void _writeCommand8(char rs, char command);
void _writeCommand4(char rs, char command);
//-----------------------------------------------------------------------------
void _wait_64_usec(void);
void delay(int msec);
//-----------------------------------------------------------------------------

//*****************************************************************************
//
//
//                           i n i t  D r i v e r
//
//
//*****************************************************************************

void initDriver(char target)
{
    CLKPR  = 0b10000000;               // Prescaler change enable!
    CLKPR  = 0;                        // use 16 MHz - cycle time now 62.5 nsec
    MCUCR  = MCUCR|(1 << JTD);		   // JTAG disable
    MCUCR  = MCUCR|(1 << JTD);

    system_target = target;

//PORT B:

    DDRB   = 0xff;                      // Leds - Display
    PORTB  = 0;

//  PORT C:

	DDRC   = (1<<DDC6);				    // PC6 ist #RST from the Portexpander
    PORTC  = (1<<PC7);                  // PC7 left wheel
	PORTC |= (1<<PC6);

    if (system_target != EL_ROBOT)
    {
//  PORT D:
        DDRD   = 0xf0;                  // used for the keys
        PORTD  = 0x0f;                  // pull ups

//  PORT F:
        DDRF   = 0;                     // read adc
    }
    else
    {

//  PORT D:
       DDRD   = 0xef;                  // PD4  left wheel

//  PORT F:
    DDRF  = 0x60;    // 0110 0000 F5, F6 controlls iRed  - F7 receives iRed


    PORTF = 0x40; // BACK LEFT
    PORTF |= 0x80; // PortF7 = High   Pullup


//###
//# ??
    }


//-----------------------------------------------------------------------------
//  LEDS:
//-----------------------------------------------------------------------------

    led.on          = led_on;
    led.off         = led_off;
    led.barMeterLin = led_barMeterLin;
    led.number      = led_number;
//-----------------------------------------------------------------------------
//  BEEPER:
//-----------------------------------------------------------------------------

    beeper.click    = beeper_click;

//-----------------------------------------------------------------------------
//  ADC converter:
//-----------------------------------------------------------------------------

    ADCSRA  = (1 << ADPS2) | (1 << ADPS1);
    ADMUX  |= (1 << REFS0);             // reference Voltage
    ADMUX  |= (1 << ADLAR);
    ADCSRA |= (1 << ADATE);
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADSC);

    adc.use = adc_use;
    adc.get = adc_get;

//-----------------------------------------------------------------------------
//  E2PROM:
//-----------------------------------------------------------------------------

    eeprom.getInt   = eeprom_getInt;
    eeprom.storeInt = eeprom_storeInt;

//-----------------------------------------------------------------------------
//  KEYS:
//-----------------------------------------------------------------------------

    key.last_keys     = 0x0f;
    key.next_keys     = 0x0f;
    key.flags         = 0;

    key.pressed       = key_pressed;
    key.released      = key_released;
    key.quit          = key_quit;
    key.stillPressed  = key_stillPressed;


//-----------------------------------------------------------------------------
//  TIMER:
//-----------------------------------------------------------------------------

    timeCounter.tenMsec  = timeCounter2.tenMsec = timeCounter3.tenMsec = 0;

    timeCounter.start    = timeCounter_start;
    timeCounter.expired  = timeCounter_expired;

    timeCounter2.start   = timeCounter2_start;
    timeCounter2.expired = timeCounter2_expired;

    timeCounter3.start   = timeCounter3_start;
    timeCounter3.expired = timeCounter3_expired;

//-----------------------------------------------------------------------------
//  UART SERIAL:
//-----------------------------------------------------------------------------

    UCSR1B = (1 << RXCIE1) | (1 << RXEN1)  | (1 << TXEN1);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); // Use 8-bit character sizes

    UBRR1H = 0;
    UBRR1L = BOUD_RATE_9600;


    serial.send = serial_send;
    serial.storeMyCallBackFunction = serial_storeMyCallBackFunction;

    serial.cb = _internalCallBackDoNothing;
    serial.received = 0;

    serial.flag = FALSE;

//-----------------------------------------------------------------------------
//  MOTOR:
//-----------------------------------------------------------------------------

    motor.setSpeed = motor_setSpeed;
    motor.setDiff =  motor_setDiff;
    motor.stop =     motor_stop;
    motor.left = motor.right = PWM_HOVER;
    motor.speed = motor.diff = 0;

//-----------------------------------------------------------------------------
//  I-RED:
//-----------------------------------------------------------------------------

    iRed.selectDirection      =  iRed_selectDirection;
    iRed.selectSide           =  iRed_selectSide;
    iRed.selectQuarter        =  iRed_selectQuarter;
    iRed.switchTransmitter    =  iRed_switchTransmitter;
    iRed.receivedSignal       =  iRed_receivedSignal;
    iRed.quit                 =  iRed_quit;

    iCounter = 0;

//    iRed.switchTransmitter(OFF);
//    iRed.quit();
//    iRed.selectQuarter(1);


//-----------------------------------------------------------------------------
//  i2c-Container:
//-----------------------------------------------------------------------------

	TWBR = 12;     // TWBR=12, TWPS=0 im Reg. TWSR per default;  set f_SCL = 400 kHz
	i2cInit();                         // needs PORT D Pin 0 = CLK and 1 = SDA

//  i2c:

//  i2C:  -- Container

    i2c.write = i2c_Write;

//-----------------------------------------------------------------------------
//  DISPLAY:
//-----------------------------------------------------------------------------

    display.Linelength = ((target == DIS2_TEST) || (target == EL_ROBOT )) ? 16 : 8;
    display.shownCursorPosition = 0;

    if (target)   // all systems with an display have numbers greater than 0
    {
            display.writeChar          = display_writeChar;
            display.setCursor          = display_setCursor;
            display.writeString        = display_writeString;
            display.writeString2ndLine = display_writeString2ndLine;
            display.writeInt           = display_writeInt;
            display.writeFloat         = display_writeFloat;
            display.hideCursor         = display_hideCursor;
            display.showCursor         = display_showCursor;
            display.storeSymbol        = display_storeSymbol;
            display.clear              = display_clear;

        // initialize LC-Display:

            delay(50);

        if (target < EL_ROBOT)  // the roboter uses i2c port expander
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

            _writeCommand8(0, 0x03);/* 8 Bit       */ delay(20);
            _writeCommand8(0, 0x03);/* 8 Bit       */ delay(20);
            _writeCommand8(0, 0x03);/* 8 Bit       */ delay(20);
	        _writeCommand8(0, 0x02);/* 4 Bit       */ delay(400);

	        _writeCommand4(0, 0x01);/*Clear Display*/ delay(20);
            _writeCommand4(0, 0x28);/*2-lines, 5x8 */ delay(20);
            _writeCommand4(0, 0x08);/*Display Off  */ delay(20);
            _writeCommand4(0, 0x03);/*No Shift     */ delay(20);
            _writeCommand4(0, 0x06);/*Entry Mode   */ delay(20);
            _writeCommand4(0, 0x0e);/*Display ON   */ delay(20);

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

//-----------------------------------------------------------------------------
//  TIMER INTERRUPT:
//-----------------------------------------------------------------------------

//  Timer 1:

    TCCR1A = 0x0;
    TCCR1B = 1<< CS11;
    TIMSK1 = 1<<TOIE1;
    TCNT1 = TIMER_START_VALUE;  // 10 msec periodic timer

    if (target == EL_ROBOT)
    {

//  Timer 0:  I-RED Signals:

    TIMSK0 = (1 << TOIE0);
	TCNT0  =  TIMER_START_VALUE_0;
	TCCR0B = (1 << CS00);
	TCCR0B = (1 << CS01);


//  Timer 4 im Fast PWM Mode konfigurieren

	TCCR4A = TCCR4A | (1<<PWM4B);
	TCCR4C = TCCR4C | (1<<PWM4D);
	TCCR4D = TCCR4D &~(1<<WGM41);
	TCCR4D = TCCR4D &~(1<<WGM40);      // Fast PWM am OC4B und OC4D

	TCCR4A = TCCR4A &~(1<<COM4B0);
	TCCR4A = TCCR4A | (1<<COM4B1);
	TCCR4C = TCCR4C &~(1<<COM4D0);
	TCCR4C = TCCR4C | (1<<COM4D1);




	TC4H   = 0x03;
	OCR4C  = 0xE8;                     // f_PWM = f_CLK_T4/(1+OCR4C) = 62,5kHz/1000 = 62,5 Hz
	TC4H   = 0x00;
	OCR4B  = PWM_HOVER;                // Tastverhältnis am OC4B-Pin (PB6), PWM_rechts (retour_max = 62, vor_max = 125, stopp = 94)
	OCR4D  = PWM_HOVER;                // Tastverhältnis am OC4D-Pin (PD7), PWM_links  (retour_max = 62, vor_max = 125, stopp = 94)


	TCCR4B = TCCR4B | (1<<CS43);
	TCCR4B = TCCR4B &~(1<<CS42);
	TCCR4B = TCCR4B &~(1<<CS41);		// f_CLK_T4 = CLK_IO/Prescaler = 16MHz/256 = 62,5kHz
	TCCR4B = TCCR4B | (1<<CS40);		// Timer4 Prescaler = 1, Start PWM

    } // i2R and PWM only in use for EL_ROBOT !


//-----------------------------------------------------------------------------
//  ENABLE ALL INTERRUPTS:
//-----------------------------------------------------------------------------

    sei();

}  // end of initDriver

//-----------------------------------------------------------------------------
//
//  c o n t a i n e r   -   f u n c t i o n s :
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  LEDs:
//-----------------------------------------------------------------------------

void led_on(unsigned char x)
{
    switch (x)
    {
        case FLIP        : PORTD &= ~x; break;
        case RIGHT_FRONT : PORTB |=  x; break;
        case RIGHT_REAR  : PORTB |=  x; break;
        case LEFT_FRONT  : PORTB |=  x; break;
        case LEFT_REAR   : PORTB |=  x; break;

        case DUAL_GREEN  : PORTB |=  x; break;
        case DUAL_RED    : PORTB |=  x; break;
        case DUAL_YELLOW : PORTB |=  x; break;
    }
}
void led_off(unsigned char x)
{
    switch (x)
    {
        case FLIP        : PORTD |=  x; break;
        case RIGHT_FRONT : PORTB &= ~x; break;
        case RIGHT_REAR  : PORTB &= ~x; break;
        case LEFT_FRONT  : PORTB &= ~x; break;
        case LEFT_REAR   : PORTB &= ~x; break;

        case DUAL_GREEN  : PORTB &= ~x; break;
        case DUAL_RED    : PORTB &= ~x; break;
        case DUAL_YELLOW : PORTB &= ~x; break;
    }
}

void led_barMeterLin(unsigned char x)
{
    PORTB = (x > 254) ? 0xff : (0xff >> (8 - (x >> 5))); // Lukas Stadler 3BHELS 2020
}

void led_number(unsigned char i)
{
char x = 0;

    x = (i == 0) ? 0x01 :
        (i == 1) ? 0x02 :
        (i == 2) ? 0x08 :
                   0x04 ;

    PORTB &= 0xf0;
    PORTB |= x;
}

//-----------------------------------------------------------------------------
//  BEEPER:
//-----------------------------------------------------------------------------

void beeper_click(void)
{
    PORTB |=  BEEPER_CLICK;
    _wait_64_usec();
    PORTB &= ~BEEPER_CLICK;
}

//-----------------------------------------------------------------------------
//  ADC:
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//  E2PROM:
//-----------------------------------------------------------------------------
void eeprom_storeInt(int value, unsigned int address)
{
uint8_t low, high;

    address <<= 2;
    address %= 0x200;  // maybee there is more space available....

    low = value & 0xff;
    eeprom_write_byte((uint8_t*)address, low);
    high = (value >> 8) & 0xff;
    eeprom_write_byte((uint8_t*)address + 1, high);
}
int eeprom_getInt(unsigned int address)
{
uint8_t low, high;

    address <<= 2;
    address %= 0x200;  // maybee there is more space available....


    low  = eeprom_read_byte((uint8_t*)address);
    high = eeprom_read_byte((uint8_t*)address + 1);

    return (int)((high << 8) + low);
}

//-----------------------------------------------------------------------------
//  KEYS:
//-----------------------------------------------------------------------------

char key_pressed(char k)
{
    char ret = 0;

    if (k == KEY0) ret = (key.flags & KEY0_PRESSED) ? TRUE : FALSE;
    if (k == KEY1) ret = (key.flags & KEY1_PRESSED) ? TRUE : FALSE;
    if (k == KEY2) ret = (key.flags & KEY2_PRESSED) ? TRUE : FALSE;
    if (k == KEY3) ret = (key.flags & KEY3_PRESSED) ? TRUE : FALSE;

    return ret;
}

char key_released(char k)
{
    char ret = 0;


    if (k == KEY0) ret = (key.flags & KEY0_RELEASED) ? TRUE : FALSE;
    if (k == KEY1) ret = (key.flags & KEY1_RELEASED) ? TRUE : FALSE;
    if (k == KEY2) ret = (key.flags & KEY2_RELEASED) ? TRUE : FALSE;
    if (k == KEY3) ret = (key.flags & KEY3_RELEASED) ? TRUE : FALSE;

    return ret;
}

void key_quit(void)
{
    key.flags = 0;
}

char key_stillPressed(char k)     ////// pressed
{
    char key;
    char ret;

    ret = FALSE;

    key = PIND & 0x0f;

    if ((key & k) == 0) ret = TRUE;   // key = KEY0 for e.q.

    return ret;
}



//-----------------------------------------------------------------------------
//  TIMER:
//-----------------------------------------------------------------------------

void timeCounter_start (int mSec) { timeCounter.tenMsec  = mSec / 10; }
void timeCounter2_start(int mSec) { timeCounter2.tenMsec = mSec / 10; }
void timeCounter3_start(int mSec) { timeCounter3.tenMsec = mSec / 10; }

char timeCounter_expired (void)   { return (timeCounter.tenMsec  == 0) ? TRUE : FALSE; }
char timeCounter2_expired(void)   { return (timeCounter2.tenMsec == 0) ? TRUE : FALSE; }
char timeCounter3_expired(void)   { return (timeCounter3.tenMsec == 0) ? TRUE : FALSE; }

//-----------------------------------------------------------------------------
//  UART SERIAL:
//-----------------------------------------------------------------------------


//  SERIAL:

void serial_send(char c)
{
     while ((UCSR1A & (1<<UDRE1))==0); UDR1 = c;
}
/*
char serial_receive(void)
{
char r = 0;

    return r;
}
*/
void serial_storeMyCallBackFunction(void (*cb)(char))
{
     serial.cb = cb;
}

//-----------------------------------------------------------------------------
//  MOTOR:
//-----------------------------------------------------------------------------

void motor_setSpeed(char s)
{
    motor.speed = s;
}
void motor_setDiff(char d)
{
    motor.diff = d;
}
void motor_stop(void)
{
    motor.speed = motor.diff = 0;
}

//-----------------------------------------------------------------------------
//  I-RED:
//-----------------------------------------------------------------------------

// diese nächsten drei Funktionen machen nicht was sie sollen..

void iRed_selectDirection(char d)
{
    if (d == IRED_BACK) PORTF |= IRED_BACK; else    PORTF &= ~IRED_BACK;
    PORTF |= 0x80;
}

void iRed_selectSide(char s)
{
    if (s == IRED_RIGHT) PORTF |= IRED_RIGHT; else  PORTF &= ~IRED_RIGHT;
    PORTF |= 0x80;

}

void iRed_selectQuarter(char n)
{
    switch (n)
    {
        case 0: PORTF |= (IRED_FRONT + IRED_RIGHT); PORTF &= ~(IRED_FRONT + IRED_RIGHT); break;
        case 1: PORTF |= (IRED_BACK  + IRED_RIGHT); PORTF &= ~(IRED_BACK  + IRED_RIGHT); break;
        case 2: PORTF |= (IRED_FRONT + IRED_LEFT);  PORTF &= ~(IRED_FRONT + IRED_LEFT);  break;
        case 3: PORTF |= (IRED_BACK  + IRED_LEFT);  PORTF &= ~(IRED_BACK  + IRED_LEFT);  break;
    }
    PORTF |= 0x80;
}

void iRed_switchTransmitter(char onOff)
{
    iRed.transmit = onOff;
}

char iRed_receivedSignal(void)
{
    return iRed.flag;
}

void iRed_quit(void)
{
    iRed.flag = FALSE;
}


//-----------------------------------------------------------------------------
//  I2C:
//-----------------------------------------------------------------------------

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
// serial callback default function
void _internalCallBackDoNothing(char c) {}

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

//-----------------------------------------------------------------------------
// DISPLAY:
//-----------------------------------------------------------------------------

void display_writeChar(char a)
{
    _writeCommand4(DATA, a);
    display.shownCursorPosition++;

    if (display.shownCursorPosition >= display.Linelength)
        display_setCursor(display.shownCursorPosition);

}

void display_setCursor(char x)
{
    if (x < 0) x = 0;

    x = x % (display.Linelength <<1);

    display.shownCursorPosition = x;

    if (display.Linelength == 8)
    {
        if (x >= 8) x += 0x38; // 0x40 - 8  = 0x38
    }
    if (display.Linelength == 16)
    {
        if (x >= 16) x += 0x30; // 0x40 - 16 = 0x30
    }
    _writeCommand4(0, DISPLAY_SET_CURSOR | x);
}

void display_writeString(char * s)
{
int i = 0;
int j = 0;


    display_setCursor(0);

    while ((i - j < display.Linelength) && (*(s+i)))
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

        display_writeChar(*(s+i));
        i++;
    }

    display_setCursor(display.Linelength); // position 8 jumps intern to position 0x40 !
    while ((i - j < (display.Linelength << 1)) && (*(s+i)))
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

        display_writeChar(*(s+i));
        i++;
    }
}

void display_writeString2ndLine(char *s)
{
int i = 0;

    if (display.Linelength > 8)
    {
        _writeCommand4(0, DISPLAY_SET_CURSOR | 0x40);
        while ((i < display.Linelength) && (*(s+i)))
        {
            display_writeChar(*(s+i));
            i++;
        }
    }
}

void display_writeInt(int i)
{
char neg = FALSE;

        if (i > 999)  i =  999;
        if (i < -999) i = -999;

        if (i < 0)
        {
            neg = TRUE; i = -i;
        }

        ((neg == TRUE) && (i >=100)) ? display_writeChar('-') : display_writeChar(' ');

        if (i < 100)
            ((neg == TRUE) && (i >= 10)) ? display_writeChar('-') : display_writeChar(' ');
        else
        {
            display_writeChar((i/100)%10 + '0');
        }


        if (i < 10)
            (neg == TRUE) ? display_writeChar('-') : display_writeChar(' ');
        else
        {
            display_writeChar((i/10)%10 + '0');
        }

        display_writeChar((i)%10 + '0');

}

void display_writeFloat(float x)
{
int i;

    if (x < 0)
    {
        display_writeChar('-');
        x = -x;
    }

    if (x >= 1000.) {i = (int)x; i %= 10000; i /= 1000; display_writeChar(i + '0');}
    if (x >=  100.) {i = (int)x; i %=  1000; i /=  100; display_writeChar(i + '0');}
    if (x >=   10.) {i = (int)x; i %=   100; i /=   10; display_writeChar(i + '0');}
    if (x >=    0.) {i = (int)x; i %=    10;            display_writeChar(i + '0');}
    else display_writeChar('0');
         display_writeChar('.');

    while (x > 100) x-= 100; // to avoid overflows!

    i = (int)(x * 10.);   i %= 10;                      display_writeChar(i + '0');
    i = (int)(x * 100.);  i %= 10;                      display_writeChar(i + '0');
    i = (int)(x * 1000.); i %= 10;                      display_writeChar(i + '0');
}

void display_hideCursor(void)
{
     _writeCommand4(0, DISPLAY_ON_CURSOR_OFF);
}

void display_showCursor(void)
{
     _writeCommand4(0, DISPLAY_ON);
}

void display_storeSymbol(char s[], char space)
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

void display_clear(void)
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


//-----------------------------------------------------------------------------
//
//  ALL INTERRUPTS:
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  UART SERIAL:
//-----------------------------------------------------------------------------

ISR(USART1_RX_vect) // Uart Empfang abgeschlossen
{
   serial.received = UDR1; // Fetch the received byte value into the variable "ByteReceived"
   serial.flag = TRUE;
   serial.cb(serial.received);
}



ISR(USART1_TX_vect) // Uart Empfang abgeschlossen
{
    // do nothing
}

//-----------------------------------------------------------------------------
//  TIMER 0 - infra red controll
//-----------------------------------------------------------------------------


ISR(TIMER0_OVF_vect)
{
    TCNT0 = TIMER_START_VALUE_0;

    PORTD ^= (iRed.transmit == ON)? 0x20 : 0; // if transmit == ON toggle PD5

//    if ((PINF & 0x80) == 0) iRed.flag = TRUE;

    if ((PINF & 0x80) == 0)
    {
        iCounter++;

        if (iCounter > I_COUNTER_MAX)
        {
            iCounter = I_COUNTER_MAX;
            iRed.flag = TRUE;

        }

    }
    else
    {
        if (iCounter) iCounter--;
    }
}

//-----------------------------------------------------------------------------
//  TIMER 1 - 10 msec timer - interrupt
//-----------------------------------------------------------------------------

ISR(TIMER1_OVF_vect)
{
    TCNT1 = TIMER_START_VALUE;

    if (timeCounter. tenMsec) timeCounter. tenMsec--;
    if (timeCounter2.tenMsec) timeCounter2.tenMsec--;
    if (timeCounter3.tenMsec) timeCounter3.tenMsec--;

    OCR4D = motor.left  = PWM_HOVER + motor.speed + motor.diff;
    OCR4B = motor.right = PWM_HOVER - motor.speed + motor.diff;

if (system_target != EL_ROBOT)
{
    key.next_keys = PIND & 0x0f;

    if (((key.next_keys & KEY0) == 0) && ((key.last_keys & KEY0) == KEY0)) key.flags |= KEY0_PRESSED;
    if (((key.next_keys & KEY1) == 0) && ((key.last_keys & KEY1) == KEY1)) key.flags |= KEY1_PRESSED;
    if (((key.next_keys & KEY2) == 0) && ((key.last_keys & KEY2) == KEY2)) key.flags |= KEY2_PRESSED;
    if (((key.next_keys & KEY3) == 0) && ((key.last_keys & KEY3) == KEY3)) key.flags |= KEY3_PRESSED;

    if (((key.last_keys & KEY0) == 0) && ((key.next_keys & KEY0) == KEY0)) key.flags |= KEY0_RELEASED;
    if (((key.last_keys & KEY1) == 0) && ((key.next_keys & KEY1) == KEY1)) key.flags |= KEY1_RELEASED;
    if (((key.last_keys & KEY2) == 0) && ((key.next_keys & KEY2) == KEY2)) key.flags |= KEY2_RELEASED;
    if (((key.last_keys & KEY3) == 0) && ((key.next_keys & KEY3) == KEY3)) key.flags |= KEY3_RELEASED;

    key.last_keys = key.next_keys;
}
else
{

}

// hier wird weiter entwickelt...

/*    if ((PIND & 0x10) == 0x10)
    {
        PORTB |= 0x01;
    }
    else
    {
        PORTB &= 0xfe;
    }
    */


    //PORTD ^= 0xff;

}













