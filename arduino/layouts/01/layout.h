void configure(struct CRGB *leds)
{
    FastLED.addLeds<NEOPIXEL, 2>(leds, 0, 20);
    FastLED.addLeds<NEOPIXEL, 3>(leds, 20, 400);
    FastLED.addLeds<NEOPIXEL, 4>(leds, 420, 20);
    FastLED.addLeds<NEOPIXEL, 5>(leds, 440, 20);
}
