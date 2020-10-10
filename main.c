/*
 *    driver testprogramm
 */

#include <avr/io.h>
#include "driver.h"

int main(void)
{
    initDriver();

    adc.use(ADC0);
    led.on(FLIP);

    for(;;)
    {

         if (adc.get() < 100)
         {
            led.on(FLIP);
         }
         else
         {
            led.off(FLIP);
         }

    }

    return 0;
}

