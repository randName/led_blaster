import socket as sk


class Blaster:

    def __init__(self, addresses=(('localhost', 8080),)):
        self.addr = addresses
        self._len = len(self.addr)
        self.a = range(self._len)
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
                self._sks[i].send(packet.encode())
                return self._sks[i].recv(4096).decode()
            except sk.error:
                self._sks[i] = None

        return False

    def blast(self, q):
        if not q.endswith('\n'):
            q += '\n'
        return tuple(self.send(i, q).strip() for i in self.a)

    @property
    def t(self):
        return tuple(float(i) for i in self.blast('t'))

    @t.setter
    def t(self, val):
        self.blast('t %.5f' % val)

    @property
    def fps(self):
        return tuple(float(i) for i in self.blast('fps'))

    def __setitem__(self, key, val):
        vals = ' '.join('%.4f' % float(v) for v in val)
        self.blast('u %s %s' % (key, vals))
        self._uniforms[key] = val

    def __getitem__(self, key):
        try:
            return self._uniforms[key]
        except KeyError:
            return None

    def __len__(self):
        return self._len
