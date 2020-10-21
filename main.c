/*
 *    driver testprogramm
 */

#include <avr/io.h>
#include "driver.h"

int main(void)
{
//    unsigned char x;

    initDriver(EL_ROBOTER);

    // for the display use instead of EL_TEST_BOARD   DIS_TEST (Display - Testboard)

    adc.use(ADC0);
    led.on(FLIP);

    //display.writeString("St.Pölten :-)");
    display.hideCursor();

    display.setCursor(0);
    display.writeFloat(-123.467);

    for(;;)
    {
/*
           x = adc.get();

           display.setCursor(0);
           display.writeInt(x);

           led.barMeterLin(x);
*/
    }

    return 0;
}

