#include "FastLED.h"

#define SERIAL_BAUD 1000000
#define BOARD_READY 0xFF
#define NUM_LEDS 512
#define LED_PIN 4

const byte id_pins[] = { 12, 11, 10, 9, 8 };

CRGB leds[NUM_LEDS];
int ll;

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

    union { char b[2]; unsigned int i; } data;

    while ( Serial.available() < 2 );
    Serial.readBytes(data.b, 2);
    ll = data.i * 3;

    FastLED.addLeds<UCS1903B, LED_PIN>(leds, data.i);
    unsigned int i, j;
    CRGB col;

    for (i = 0; i < 5; ++i) {
        switch (i) {
            case 1: col = CRGB(255, 0, 0); break;
            case 2: col = CRGB(0, 255, 0); break;
            case 3: col = CRGB(0, 0, 255); break;
            default: col = CRGB::Black;
        }
        for (j = 0; j < data.i; ++j) {
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
