def bytify(v):
    try:
        return bytes((v,))
    except ValueError:
        return bytes((v % 256),)
    except TypeError:
        try:
            return bytes((int(v) % 256,))
        except TypeError:
            return bytes(1)


class LED:

    def __init__(self):
        self._r = bytes(1)
        self._g = bytes(1)
        self._b = bytes(1)
        self._raw_cache = self.raw

        self._h = 0
        self._s = 0
        self._v = 0

    @property
    def raw(self):
        return b''.join((self._g, self._r, self._b))

    @raw.setter
    def raw(self, v):
        try:
            v = v.to_bytes(3, 'big')
        except AttributeError:
            if len(v) == 3:
                v = b''.join(bytify(b) for b in v)
            else:
                v = bytes(3)

        self._g, self._r, self._b = v

    @property
    def r(self):
        return self._r / 256

    @r.setter
    def r(self, v):
        self._r = bytify(v * 255)

    @property
    def g(self):
        return self._g / 256

    @g.setter
    def g(self, v):
        self._g = bytify(v * 255)

    @property
    def b(self):
        return self._b / 256

    @b.setter
    def b(self, v):
        self._b = bytify(v * 255)

    @property
    def rgb(self):
        return (self.r, self.g, self.b)

    @rgb.setter
    def rgb(self, v):
        self.r, self.g, self.b = v

    @property
    def h(self):
        self._compute_hsv()
        return self._h

    @h.setter
    def h(self, v):
        self._h = v
        self._compute_rgb()

    @property
    def s(self):
        self._compute_hsv()
        return self._s

    @s.setter
    def s(self, v):
        self._s = v
        self._compute_rgb()

    @property
    def v(self):
        self._compute_hsv()
        return self._v

    @v.setter
    def v(self, v):
        self._v = v
        self._compute_rgb()

    @property
    def hsv(self):
        self._compute_hsv()
        return (self._h, self._s, self._v)

    @hsv.setter
    def hsv(self, v):
        self._h, self._s, self._v = v
        self._compute_rgb()

    def _compute_hsv(self):
        if self._raw_cache == self.raw:
            return
        self._raw_cache = self.raw

        m_i, M = max(enumerate(self.rgb), key=lambda x: x[1])
        m = min(self.rgb)
        c = M - m
        if c == 0:
            self._h = 0
            return

        if M[0] == 0:
            hp = (self.g - self.b)/c
        elif M[0] == 1:
            hp = (self.b - self.r)/c + 2
        elif M[0] == 2:
            hp = (self.r - self.g)/c + 4

        self._h = hp / 6
        self._s = 0 if M == 0 else c / M
        self._v = M

    def _compute_rgb(self):
        self._h = self._h % 1
        self._s = min(max(0, self._s), 1)
        self._v = min(max(0, self._v), 1)

        r, g, b = (0, 0, 0)

        c = self._v * self._s
        hp = self._h * 6
        x = c * (1 - abs(hp % 2 - 1))
        m = self._v - c

        if 0 <= hp < 1:
            r = c
            g = x
        elif 1 <= hp < 2:
            r = x
            g = c
        elif 2 <= hp < 3:
            g = c
            b = x
        elif 3 <= hp < 4:
            g = x
            b = c
        elif 4 <= hp < 5:
            r = x
            b = c
        elif 5 <= hp <= 6:
            r = c
            b = x

        self.r = m + r
        self.g = m + g
        self.b = m + b


class LEDs:

    def __init__(self, length):
        self.leds = tuple(LED() for i in range(length))

    @property
    def raw(self):
        return b''.join(l.raw for l in self.leds)

    def __getitem__(self, i):
        return self.leds[i]

    def __len__(self):
        return len(self.leds)
