#!/bin/bash
cd $(dirname "$(readlink -f "$0")")

for layout in *.txt; do
    fol="output/${layout//.*}"
    fn="${fol}/layout.h"
    mkdir -p $fol

    echo "// generate_layouts.sh" > $fn
    echo "void configure(struct CRGB *leds) {" >> $fn

    ofs=0
    pin=2
    for len in $(cat $layout); do
        echo "    FastLED.addLeds<NEOPIXEL, ${pin}>(leds, ${ofs}, ${len});"
        (( offset += len ))
        (( pin++ ))
    done >> $fn

    echo "}" >> $fn
done
