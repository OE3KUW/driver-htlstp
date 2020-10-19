/*
 *    driver testprogramm
 */

#include <avr/io.h>
#include "driver.h"

int main(void)
{
    unsigned char x;
    initDriver(DISPLAY_AVAILABLE /*EL_TEST_BOARD*/);

    adc.use(ADC0);
    led.on(FLIP);

    display.writeString("ok!");
    display.hideCursor();

    for(;;)
    {

           x = adc.get();
           display.setCursor(4);
           display.writeInt(x);
           //led.barMeterLin(x);

    }

    return 0;
}

