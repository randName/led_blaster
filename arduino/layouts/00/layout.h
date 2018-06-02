void configure(struct CRGB *leds)
{
    FastLED.addLeds<NEOPIXEL, 2>(leds, 0, 20);
}
