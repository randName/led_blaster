#include "FastLED.h"
#include "layout.h"

#define SERIAL_BAUD 1000000
#define BOARD_READY 0xFF
#define NUM_LEDS 512

const byte id_pins[] = { 12, 11, 10, 9, 8 };

CRGB leds[NUM_LEDS];
const int ll = LED_LENGTH * 3;

byte get_board_no() {
    byte i, s = 0;
    for (i = 0; i < sizeof(id_pins); ++i) {
        pinMode(id_pins[i], INPUT_PULLUP);
        s += ( ! digitalRead(id_pins[i]) ) << ( i + 1 );
    }
    return s;
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.write(get_board_no());
    Serial.write(BOARD_LAYOUT);

    configure(leds);
    unsigned int i, j;
    CRGB col;

    for (i = 0; i < 5; ++i) {
        switch (i) {
            case 1: col = CRGB(255, 0, 0); break;
            case 2: col = CRGB(0, 255, 0); break;
            case 3: col = CRGB(0, 0, 255); break;
            default: col = CRGB::Black;
        }
        for (j = 0; j < LED_LENGTH; ++j) {
            leds[j] = col;
            if ( j % 6 == 0 ) {
                FastLED.show();
            }
        }
    }

    FastLED.show();
    delay(10);
}

void loop()
{
    static int rb;

    Serial.write(BOARD_READY);

    rb = 0;
    while (!Serial.available());
    while (rb < ll) {
        rb += Serial.readBytes(((char*)leds) + rb, ll - rb);
    }

    FastLED.show();
}
