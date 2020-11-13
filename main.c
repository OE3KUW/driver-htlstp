/*
 *    driver testprogramm
 */

#include <avr/io.h>
#include "driver.h"


#define STATE_0                         0
#define STATE_1                         1
#define STATE_2                         2
#define STATE_3                         3


int flatten(int x);
void Receive(char c);
char Char;
char oldChar;

int main(void)
{
unsigned char x;
int state;

    initDriver(EL_ROBOT);

    led.off(FLIP);

    display.writeString("          V     ");
    display.hideCursor();

    Char = oldChar = 0;

    serial.storeMyCallBackFunction(Receive);

    serial.send(10);
    serial.send(13);
    serial.send('s');
    serial.send('t');
    serial.send(':');
    serial.send('A');
    serial.send('r');
    serial.send('t');
    serial.send('!');
    serial.send(10);
    serial.send(13);

    led.on(DUAL_GREEN);

    lineF.off();

    state = 0;

    for(;;)
    {
        x = adc.get(ADC0);
        x = flatten(x);

        display.setCursor(5);
        display.writeFloat(x / ADC_VOLTAGE);

        display.setCursor(0);
        display.writeInt(motor.left);

        display.setCursor(12);
        display.writeInt(motor.right);

        display.setCursor(21);
        display.writeInt(x);

        x = lineF.left();
        display.setCursor(16);
        display.writeInt(x);

        x = lineF.right();
        display.setCursor(28);
        display.writeInt(x);


        switch (state)
        {
            case STATE_0:
                if (iRed.receivedSignal())
                {
                    serial.send('i');
                    serial.send('R');
                    serial.send('e');
                    serial.send('d');
                    serial.send(10);
                    serial.send(13);
                    iRed.quit();
                    beeper.click();
                    timeCounter.start(3000);
                    motor.setSpeed(-7);   // gerade aus
                    state = STATE_1;
                }
            break;

            case STATE_1:
                if (timeCounter.expired())
                {
                    beeper.click();
                    timeCounter.start(2000);
                    motor.setSpeed(0);
                    motor.setDiff(5);      // 2 Sec drehen
                    state = STATE_2;
                }
            break;

            case STATE_2:
                if (timeCounter.expired())
                {
                    beeper.click();
                    timeCounter.start(3000);
                    motor.setDiff(0);
                    motor.setSpeed(-7);      // 5 sec zurück
                    state = STATE_3;
                }
            break;

            case STATE_3:
                if (timeCounter.expired())
                {
                    beeper.click();
                    motor.setDiff(0);
                    motor.setSpeed(0);      // und stehen
                    state = STATE_0;
                }
            break;

        }
    }

    return 0;

}

int flatten(int x)
{
static int array[16] = {0};
static unsigned char i = 0;
int j;
int val;

    array[i%16] = x;
    i++;
    val = 0;
    for(j = 0; j < 16; j++) val += array[j];

    return val >> 4;

}


void Receive(char c)
{

/*   if (c == 's') motor.setSpeed(motor.speed - 1);
   if (c == 'S') motor.setSpeed(motor.speed + 1);
   if (c == 'd') motor.setDiff (motor.diff - 1);
   if (c == 'D') motor.setDiff (motor.diff + 1);
   if (c == 'c') motor.stop    ();

*/
    if (c == 's') motor.setSpeed(motor.speed - 1);
    if (c == 'S') motor.setSpeed(motor.speed + 1);
    if (c == 'd') motor.setDiff(motor.diff - 1);
    if (c == 'D') motor.setDiff(motor.diff + 1);
    if (c == '.') motor.stop    ();

    if (c == 'i') iRed.switchTransmitter(ON);
    if (c == 'o') iRed.switchTransmitter(OFF);

    if (c == 'u') iRed.selectDirection(IRED_FRONT);
    if (c == 'U') iRed.selectDirection(IRED_BACK);

    if (c == 'p') iRed.selectSide(IRED_LEFT);
    if (c == 'P') iRed.selectSide(IRED_RIGHT);

    if (c == '1') {iRed.selectQuarter(1); led.number(1);}
    if (c == '2') {iRed.selectQuarter(2); led.number(2);}
    if (c == '3') {iRed.selectQuarter(3); led.number(3);}
    if (c == '4') {iRed.selectQuarter(0); led.number(0);}

    led.on(FLIP);
    Char = c;

}



