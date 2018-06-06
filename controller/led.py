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
        return ord(self._r) / 255

    @r.setter
    def r(self, v):
        self._r = bytify(v * 255)

    @property
    def g(self):
        return ord(self._g) / 255

    @g.setter
    def g(self, v):
        self._g = bytify(v * 255)

    @property
    def b(self):
        return ord(self._b) / 255

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

        if m_i == 0:
            hp = (self.g - self.b)/c
        elif m_i == 1:
            hp = (self.b - self.r)/c + 2
        elif m_i == 2:
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

    def __str__(self):
        return '0x%s' % b''.join((self._r, self._g, self._b)).hex()

    def __repr__(self):
        return '<LED at %s>' % hex(id(self))


class LEDs:

    def __init__(self, size=1, key='led_store', **kw):
        from redis import StrictRedis
        self.redis = StrictRedis(**kw)

        self.key = key
        self.size = size
        self.redis.delete(key)
        self.redis.setrange(key, 0, bytes(size * 3))

    def _handle_index(self, i):
        if not isinstance(i, int):
            raise TypeError
        if i < 0:
            i += self.size
        if i < 0 or i >= self.size:
            raise IndexError
        return i * 3, i * 3 + 2

    def _handle_item(self, i):
        try:
            start, _ = self._handle_index(i.start or 0)
            _, end = self._handle_index((i.stop or self.size) - 1)
        except AttributeError:
            start, end = self._handle_index(i)
        return start, end

    def __len__(self):
        return self.size

    def __getitem__(self, i):
        start, end = self._handle_item(i)
        return self.redis.getrange(self.key, start, end)

    def __setitem__(self, i, v):
        start, end = self._handle_item(i)
        self.redis.setrange(self.key, start, v)

    def __str__(self):
        return '%d LEDs' % self.size

    def __repr__(self):
        return '<LEDs at %s>' % hex(id(self))
