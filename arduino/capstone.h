#define SERIAL_BAUD 400000
#define SETUP_READY 0x1F

#define NUM_LEDS 512
CRGB leds[NUM_LEDS];

void configure(struct CRGB *leds);
