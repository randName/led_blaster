import socket as sk


class Blaster:

    def __init__(self, addresses=(('localhost', 8080),)):
        self.addr = addresses
        self.a = range(len(self.addr))
        self._sks = [None for _ in self.addr]
        self._uniforms = {}

    def send(self, i, packet):
        if self._sks[i] is None:
            try:
                self._sks[i] = sk.socket(sk.AF_INET, sk.SOCK_STREAM)
                self._sks[i].connect(self.addr[i])
                self._sks[i].setsockopt(sk.IPPROTO_TCP, sk.TCP_NODELAY, True)
            except sk.error:
                self._sks[i] = None

        if self._sks[i] is not None:
            try:
                self._sks[i].send(packet)
                return self._sks[i].recv(4096)
            except sk.error:
                self._sks[i] = None

        return None

    def blast(self, q):
        if not q.endswith('\n'):
            q += '\n'
        q = q.encode()

        for i in self.a:
            reply = self.send(i, q)
            if not reply:
                continue
            reply = reply.decode().strip()
            if reply:
                yield reply

    @property
    def t(self):
        return tuple(float(i) for i in self.blast('t'))

    @t.setter
    def t(self, val):
        tuple(self.blast('t %.5f' % val))

    @property
    def fps(self):
        return tuple(float(i) for i in self.blast('fps'))

    @property
    def uniforms(self):
        return self._uniforms

    def __setitem__(self, key, val):
        try:
            vals = ' '.join('%.4f' % float(v) for v in val)
            length = len(val)
        except TypeError:
            vals = '%.4f' % float(val)
            length = 1
        tuple(self.blast('u %s %d %s' % (key, length, vals)))
        self._uniforms[key] = val

    def __getitem__(self, key):
        try:
            return self._uniforms[key]
        except KeyError:
            return 0

    def __len__(self):
        return len(self.addr)

    def stop(self, halt=False):
        tuple(self.blast('q halt' if halt else 'q'))
