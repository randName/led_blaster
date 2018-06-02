#include "FastLED.h"
#include "capstone.h"
#include "layout.h"

void setup() {
    configure(leds);

    const byte board_no = 0x01;

    Serial.begin(SERIAL_BAUD);
    Serial.write(board_no);

    while (Serial.available()){ Serial.read(); }
    Serial.write(SETUP_READY);
}

union len_conv {
    byte raw[2];
    int value;
};

void loop()
{
    static bool h;
    static int ll, rb;
    static len_conv len;

    while (true) {
        h = false;
        while (!Serial.available());
        if (Serial.read() == 0xFF) {
            while (!Serial.available());
            if (Serial.read() == 0xFF){ h = true; }
        }

        if (h) {
            while (Serial.available() < 2);
            len.raw[0] = Serial.read();
            len.raw[1] = Serial.read();
            ll = len.value * 3;

            rb = 0;
            while (rb < ll) {
                rb += Serial.readBytes(((char*)leds) + rb, ll - rb);
            }
            break;
        }
    }

    FastLED.show();
    while (Serial.available()){ Serial.read(); }
}
