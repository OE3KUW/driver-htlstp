/***************************************************************************************
 * DRIVE ROBOT
 *
 **************************************************************************************/

#include <avr/io.h>
#include "driver.h"

#define STATE_0                         0
#define STATE_1                         1
#define STATE_2                         2
#define STATE_3                         3

int flatten(int x);

int main(void)
{
unsigned char x;
unsigned int n;
int state;

    initDriver(EL_ROBOT);


    display.writeString("          V     ");
    display.hideCursor();


    state = STATE_0;
    n = 0;
    timeCounter.start(1000);

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

        display.setCursor(28);
        display.writeInt(n);


        switch (state)
        {
            case STATE_0:
                if (timeCounter.expired())
                {

                    timeCounter.start(1500);  //1.5 Sekunden
                    motor.setSpeed(-15);   // gerade aus
                    motor.setDiff(0);      // nicht drehen
                    state = STATE_1;
                    display.writeString2ndLine("STATE 1");
                    n++;
                }
            break;

            case STATE_1:
                if (timeCounter.expired())
                {
                    timeCounter.start(2000);  // 2 Sekunden
                    motor.setSpeed(0);     // nur
                    motor.setDiff(5);      // drehen
                    state = STATE_2;
                    display.writeString2ndLine("STATE 2");
                    n++;
                }
            break;

            case STATE_2:
                if (timeCounter.expired())
                {
                    timeCounter.start(1500);  // 1.5 Sekunden
                    motor.setSpeed(7);   // zurück
                    motor.setDiff(0);    // nicht drehen
                    state = STATE_3;
                    display.writeString2ndLine("STATE 3");
                    n++;
                }
            break;

            case STATE_3:
                if (timeCounter.expired())
                {
                    timeCounter.start(5000); // 5 Sekunden
                    motor.setSpeed(12);      // drehen und fahren
                    motor.setDiff(-3);       // drehen
                    state = STATE_0;
                    display.writeString2ndLine("STATE 0");
                    n++;
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
