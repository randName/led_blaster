from arduino import Arduino, arduino_devices
from led import LEDs

if __name__ == "__main__":
    from time import sleep, time

    dev = next(arduino_devices())
    ard = Arduino(dev)
    ard.leds = LEDs(100)

    print('%d LEDs' % len(ard.leds))

    start = time()
    for i in range(100):
        for j in range(100):
            ard.leds[j].hsv = ((j/100 + i)/100, 1, 0.01)
        ard.show()
    end = time()

    per = (end - start)/100
    print("%.4f seconds (%.2f FPS)" % (per, 1/per))

    ard.close()
