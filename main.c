/***************************************************************************************
//                       d r i v e r   m a i n . c
//
//                                                                      қuran jun 2021
**************************************************************************************/

#include <avr/io.h>
#include "driver.h"

#define STATE_WAIT                     0
#define STATE_WORK                     1


void Receive(char c);
int i;

int main(void)
{
int state;
unsigned char x;

    initDriver(EL_ROBOT);
    serial.storeMyCallBackFunction(Receive);

    serial.send('s');
    serial.send('t');
    serial.send('a');
    serial.send('r');
    serial.send('t');
    serial.send('!');
    serial.send(10);
    serial.send(13);

    display.writeString("hi!");
    display.hideCursor();


    led.off(FLIP);
    led.on(LEFT_FRONT);
    led.on(LEFT_REAR);
    led.on(RIGHT_FRONT);
    led.on(RIGHT_REAR);

    state = STATE_WAIT;

    timeCounter.start(3000);
    i = 0;

    motor.setSpeed(0);

    for(;;)
    {
        switch (state)
        {
            case STATE_WAIT:

                if (timeCounter.expired())
                {
                    state = STATE_WORK;
                    led.on(DUAL_GREEN);
                    led.on(RIGHT_FRONT);


                    serial.send('g');
                    serial.send('o');
                    serial.send(' ');
                    serial.send('0' + i);
                    serial.send('!');
                    serial.send(10);
                    serial.send(13);

                    timeCounter.start(500);
                    i++;

                    led.off(FLIP);
                    led.on(LEFT_FRONT);
                    led.on(LEFT_REAR);
                    led.on(RIGHT_FRONT);
                    led.on(RIGHT_REAR);
                    led.on(DUAL_YELLOW);
                    x = adc.get(ADC0);
                    display.setCursor(7);
                    display.writeFloat(x / ADC_VOLTAGE);

//                  motor.setSpeed(-3);

                }

            break;

            case STATE_WORK:

                if (timeCounter.expired())
                {
//                    motor.setSpeed(5);
                    led.on(DUAL_RED);
                    led.off(RIGHT_FRONT);

                    led.on(FLIP);
                    led.off(LEFT_FRONT);
                    led.off(LEFT_REAR);
                    led.off(RIGHT_FRONT);
                    led.off(RIGHT_REAR);
                    led.off(DUAL_YELLOW);
                    timeCounter.start(1000);
                    state = STATE_WAIT;
                }
            break;


        }
    }

    return 0;

}

void Receive(char c)
{
    i = 0;

    if (c == 'o')
    {
                    serial.send(10);
                    serial.send(13);
                    serial.send('o');
                    serial.send('k');
                    serial.send('!');
                    serial.send(10);
                    serial.send(13);

    }
}

