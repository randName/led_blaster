from .led import LEDs
from .physical import get_layouts, get_slices
from .arduino import Arduino, arduino_devices


def setup(arduinos):
    layouts = get_layouts()
    slices = get_slices(layouts)
    leds = LEDs(sum(sum(l) for l in layouts.values()))

    for port in arduino_devices():
        a = Arduino(port)
        if a.id is None:
            continue
        a.leds = leds
        a.layout = layouts.get(a.id, (10,))
        a.slice = slices.get(a.id, (0, 0))
        arduinos.append(a)

    arduinos.sort(key=lambda a: a.id)


def loop(arduinos):
    try:
        while True:
            pass
    except KeyboardInterrupt:
        print('\nKeyboard interrupt received, exiting.')
    finally:
        for a in arduinos:
            a.close()
