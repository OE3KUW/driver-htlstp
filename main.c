/***************************************************************************************
 * Beispiel für das einstellen einer Zahl am Display und Verwendung von DIS_I2C
 * kurzer Tasten-Click: die Zahl (ausgegeben am Display) wird um eins erhöht.
 * langer Tasten-Druck: nach 1 Sekunde (=1000 msec) wird die Zahl  um 1 erhöht.
 * dann nach weiteren alle 500 msec wieder und dann alle 100 msec wieder
 * die neue Zahl soll dabei stets ins eeprom gespeichert werden.
 * wird das System abgeschalten und wieder eingeschalten, soll die zuletzt eingestellte
 * Zahl wieder am Display erscheinen und als neue Ausgangszahl (n) verwendet werden.
 *
 * Entwickelt für die3AHELS und 3BHELS Dezember 2020 - frohe Weihnachten! AV Kuran
 **************************************************************************************/

#include <avr/io.h>
#include "driver.h"


#define STATE_WAIT                     0
#define STATE_SLOW_INCREMENTSTATE      1
#define STATE_FAST_INCREMENTSTATE      2

#define N_POSITION                     10

int main(void)
{
unsigned int n;
int state;

    initDriver(DIS_I2C);    // Muss der erste Befehl sein!
    display.hideCursor();   // schaltet das Cursorblinken ab

    state = STATE_WAIT;

    n = eeprom.getInt(0);   // die Zahl n wird hier stets am Adressplatz 0 des
                            // EEPROMS abgespeichert - hier wieder abgeholt.
    /* ------------------01234567890123456----------------------------------*/

    display.writeString("press T2:      ");
    display.setCursor(N_POSITION);
    display.writeInt(n);

    for(;;)
    {
        switch (state)
        {
            case STATE_WAIT:    // hier wird darauf gewartet, dass ein Tater gedrückt wird.

                if (key.pressed(KEY2))
                {
                    key.acknowledge(); // internes Flag wird zurück gesetzt
                    timeCounter.start(1000);
                    // um später entscheiden zu können,
                    // ob der Taster schon länger als 1 Sekunde gedrückt wurde.
                    // Deshalb muss sofort! mit der Zeitmessung begonnen werden.
                    n++;   eeprom.storeInt(n, 0);
                    display.setCursor(N_POSITION);
                    display.writeInt(n);

                    state = STATE_SLOW_INCREMENTSTATE;
                }

            break;

            case STATE_SLOW_INCREMENTSTATE:

                if (key.released(KEY2))  // wird der Tater sofort losgelassen
                {                        // zurück zu WAIT
                    key.acknowledge();
                    state = STATE_WAIT;
                }

                if ((timeCounter.expired() && key.stillPressed(KEY2)))
                {
                    timeCounter.start(500);
                    n++;   eeprom.storeInt(n, 0);
                    display.setCursor(N_POSITION);
                    display.writeInt(n);
                    state = STATE_FAST_INCREMENTSTATE;
                }
            break;

            case STATE_FAST_INCREMENTSTATE:

                if (key.released(KEY2))  // wird der Tater sofort losgelassen
                {                        // zurück zu WAIT
                    key.acknowledge();
                    state = STATE_WAIT;
                }

                if (timeCounter.expired())
                {
                    timeCounter.start(100);
                    n++;   eeprom.storeInt(n, 0);
                    display.setCursor(N_POSITION);
                    display.writeInt(n);
                }

            break;

        }
    }

    return 0;

}

