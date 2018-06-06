from .led import LEDs
from .physical import get_layouts, get_slices
from .arduino import Arduino, arduino_devices


def setup(arduinos):
    layouts = get_layouts()
    slices = get_slices(layouts)
    leds = LEDs(sum(sum(l for l in layouts[s]) for s in slices))

    for port in arduino_devices():
        a = Arduino(port)
        if a.id is None:
            continue
        a.leds = leds
        a.layout = layouts.get(a.id, (10,))
        a.slice = slices.get(a.id, (0, 0))
        arduinos.append(a)

    arduinos.sort(key=lambda a: a.id)


def loop(arduinos, fps=12):
    from time import time
    try:
        interval = 1/fps
        last = time()
        while True:
            now = time()
            if (now - last) >= interval:
                for a in arduinos:
                    a.show()
                last = now
    except KeyboardInterrupt:
        print('\nKeyboard interrupt received, exiting.')
    finally:
        for a in arduinos:
            a.close()
