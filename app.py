from controller.led import LED
from controller.arduino import Arduino, arduino_devices


def get_layouts(lay_dir='layouts', lay_ext='.txt'):
    from os import listdir

    layouts = {}

    for fn in listdir(lay_dir):
        if not fn.endswith(lay_ext):
            continue

        try:
            key = int(fn.replace(lay_ext, ''), 16)
        except ValueError:
            continue

        with open('%s/%s' % (lay_dir, fn)) as f:
            layouts[key] = tuple(int(l) for l in f)

    return layouts


def test_arduino(a, cycles=1000, update=None):
    from time import time

    a.leds = tuple(LED() for i in range(sum(a.layout)))
    num_leds = len(a.leds)

    print('%d LEDs: ' % num_leds, end='')

    if update is None:
        def update(i, j):
            return ((j/num_leds + i)/cycles, 1, 0.5)

    start = time()
    for i in range(cycles):
        for j in range(num_leds):
            a.leds[j].hsv = update(i, j)
        a.show()
    end = time()

    elapsed = (end - start)/cycles
    print("%.4f seconds (%.2f FPS)" % (elapsed, 1/elapsed))


if __name__ == "__main__":
    arduinos = []
    layouts = get_layouts()

    for port in arduino_devices():
        a = Arduino(port)
        if a.id is None:
            continue
        a.layout = layouts.get(a.id, (10,))
        arduinos.append(a)

    print(arduinos)

    for a in arduinos:
        test_arduino(a)
