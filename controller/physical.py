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


def get_slices(layouts, map_file='mapping.txt'):
    slices = {}
    offset = 0

    with open(map_file) as f:
        for l in f:
            try:
                layout = int(l, 16)
            except ValueError:
                continue

            length = sum(layouts[layout])
            slices[layout] = (offset, offset + length)
            offset += length

    return slices
