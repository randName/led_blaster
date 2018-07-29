def strip(height, offset, size=6):
    base = tuple(range(offset * size, (offset + height) * size))
    return base[-1::-2] + base[::2]


def parse(raw_layout):
    layout = {}

    for col in raw_layout:
        x, h, ofs, lay, order = col

        if lay not in layout:
            layout[lay] = {'x': [], 'y': []}

        layout[lay]['x'].append(x)
        layout[lay]['y'].append(strip(h, ofs))

    for lay, v in layout.items():
        if v['x'][0] < v['x'][1]:
            for i, w in enumerate(v['y']):
                v['y'][i] = tuple(reversed(w))

        v['length'] = sum(len(i) for i in v['y'])

        v['index'] = []
        for i, x in enumerate(v['x']):
            for y in v['y'][i]:
                v['index'].append((x, y))

    return layout


header_template = """// generated layout

#define BOARD_LAYOUT 0x{lay:02X}

void configure(struct CRGB *leds) {{
    FastLED.addLeds<UCS1903B, 4>(leds, {n});
}}

#define LED_LENGTH {n}"""

if __name__ == "__main__":
    from os import makedirs

    with open('layout.txt') as f:
        layout = [tuple(int(i) for i in l.split()) for l in f]
        layout.sort(key=lambda x: x[3:])

    layout = parse(layout)

    for l, v in layout.items():
        dn = 'output/%02X' % l
        makedirs(dn, exist_ok=True)
        with open('%s/layout.txt' % dn, 'w') as f:
            for c in v['index']:
                print('%3d %3d' % c, file=f)

        with open('%s/layout.h' % dn, 'w') as f:
            print(header_template.format(lay=l, n=v['length']), file=f)
