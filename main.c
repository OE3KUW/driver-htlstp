/*
 *    driver testprogramm
 */

#include <avr/io.h>
#include "driver.h"



int flatten(int x);


char a[] ={0b11100100,
           0b00001010,
           0b00001010,
           0b00010001,
           0b00010001,
           0b00010001,
           0b00011111,
           0b00011011,
           0b00011011 };

int main(void)
{
    unsigned char x;
    unsigned char i;
    int val;

    initDriver(EL_ROBOT);

    // for the display use instead of EL_TEST_BOARD   DIS_TEST (Display - Testboard)

    adc.use(ADC0);
    led.off(FLIP);
    led.number(1);
    led.on(DUAL_YELLOW);

    display.writeString("          V     ");
    display.hideCursor();
    i = 0;

    display.writeString2ndLine("     drive!     ");
    display.storeSymbol(a,7);

    eeprom.storeInt(40, 6);

    val = eeprom.getInt(5);
    display.setCursor(0);
    display.writeInt(val);

    display.setCursor(14);
    display.writeChar(7);

//    display.setCursor(0);
//    display.writeFloat(-123.467);

    for(;;)
    {

           x = adc.get();
           x = flatten(x);

           display.setCursor(5);
           display.writeFloat(x / ADC_VOLTAGE);

           delay(50);
           led.number(i);
           beeper.click();
           i++;
           if (i > 3) i = 0;



/*
           led.barMeterLin(x);
*/
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

