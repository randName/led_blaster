#include "FastLED.h"
#include "capstone.h"
#include "layout.h"

const int ll = LED_LENGTH;

byte get_board_no() {
    return 0x00; // for now
    const unsigned int raw_id = analogRead(ID_PIN);
    return raw_id >> 4;
}

void setup() {
    const byte board_no = get_board_no();

    Serial.begin(SERIAL_BAUD);
    Serial.write(board_no);
    Serial.write(BOARD_LAYOUT);

    configure(leds);
}

void loop()
{
    static bool h;
    static int rb;

    while (true) {
        h = false;
        while (!Serial.available());
        if (Serial.read() == 0xFF) {
            while (!Serial.available());
            if (Serial.read() == 0xFF){ h = true; }
        }

        if (h) {
            while (!Serial.available());
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
