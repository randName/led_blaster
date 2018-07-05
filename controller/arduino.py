LED_HEADER = bytes((0xFF, 0xFF))


class Arduino:

    interval = 1/24

    def __init__(self, device, halt=None, baud=400000, tout=1):
        from serial import Serial
        from threading import Thread

        self._bytes = bytes()

        self.port = device
        self.serial = Serial(port=device, baudrate=baud, timeout=tout)
        self.reset()

        try:
            self.board_id = ord(self.read(1))
        except TypeError:
            print('no id from %s' % device)
            self.board_id = None
            return

        try:
            layout = ord(self.read(1))
        except TypeError:
            print('no layout from %s' % device)
            return

        if self.id != layout:
            print('mismatch on %s (%s, %s)' % (device, self.id, layout))

        if halt is not None:
            self.is_halted = halt.is_set
            self.stop = halt.set

        self._task = Thread(target=self._run)

    def start(self):
        self._task.start()

    def is_halted(self):
        return False

    def stop(self):
        pass

    def _run(self):
        from time import time, sleep
        last = time()
        while not self.is_halted():
            now = time()
            if (now - last) >= self.interval:
                self.write(LED_HEADER)
                self.write(self.bytes)
                last = now
            else:
                sleep(self.interval / 10)
        self.close()

    @property
    def id(self):
        return self.board_id

    @property
    def bytes(self):
        return self._bytes

    @bytes.setter
    def bytes(self, v):
        self._bytes = bytes(min(0xFE, b) for b in v)

    def reset(self):
        self.serial.dtr = False
        self.serial.reset_input_buffer()
        self.serial.dtr = True
        self.serial.read(1)

    def write(self, data):
        return self.serial.write(data)

    def read(self, size):
        return self.serial.read(size)

    def close(self):
        self.serial.close()

    def __str__(self):
        if self.id is None:
            return 'Arduino (invalid)'
        return 'Arduino #%03d' % self.id

    def __repr__(self):
        if self.id is None:
            return '<Arduino (invalid) on %s>' % self.port
        return '<Arduino #%03d on %s>' % (self.id, self.port)


def arduino_devices():
    from serial.tools.list_ports import comports
    for p in comports():
        if p.description.startswith('USB'):
            yield p.device


if __name__ == "__main__":
    for d in arduino_devices():
        print(d)
