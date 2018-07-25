#include "FastLED.h"
#include "layout.h"

#define SERIAL_BAUD 500000
#define BOARD_READY 0xFF
#define NUM_LEDS 512

#define ID_PIN A0

CRGB leds[NUM_LEDS];
const int ll = LED_LENGTH * 3;

byte get_board_no() {
    return 0x00; // for now
    const unsigned int raw_id = analogRead(ID_PIN);
    return raw_id >> 4;
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.write(get_board_no());
    Serial.write(BOARD_LAYOUT);

    configure(leds);

    for (unsigned int i = 0; i < LED_LENGTH; ++i) leds[i] = CRGB::Black;
    FastLED.show();
    delay(1000);
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
