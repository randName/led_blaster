#include "FastLED.h"

#define SERIAL_BAUD 1000000
#define BOARD_READY 0xFF
#define ACTIVE_WAIT 65536
#define IDLE_WAIT 20
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

    FastLED.addLeds<UCS1903B, LED_PIN>(leds, NUM_LEDS);
    union { char b[2]; unsigned int i; } data;
    unsigned int i, j;
    CRGB col;

    i = 0;
    while ( Serial.available() < 2 && i < IDLE_WAIT ) {
        ++i;
        delay(1);
    }

    if ( i < IDLE_WAIT ) {
        Serial.readBytes(data.b, 2);
        ll = data.i * 3;
    } else {
        ll = 0;
    }

    for (i = 0; i < 5; ++i) {
        switch (i) {
            case 1: col = CRGB(100, 0, 0); break;
            case 2: col = CRGB(0, 100, 0); break;
            case 3: col = CRGB(0, 0, 100); break;
            default: col = CRGB::Black;
        }
        for (j = 0; j < NUM_LEDS; ++j) {
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
    static int i;
    static int rb;
    static uint8_t gHue = 0;

    Serial.write(BOARD_READY);

    rb = 0;
    i = 0;
    while ( rb < ll && i < ACTIVE_WAIT ) {
        if ( Serial.available() ) {
            rb += Serial.readBytes((char*)leds + rb, ll - rb);
        }
        ++i;
    }

    if ( ll == 0 || i >= ACTIVE_WAIT ) {
        fadeToBlackBy(leds, NUM_LEDS, 10);
        int pos = random16(NUM_LEDS);
        leds[pos] += CHSV(gHue + random8(64), 200, 255);
    }

    FastLED.show();

    EVERY_N_MILLISECONDS( 20 ) { ++gHue; }
}
