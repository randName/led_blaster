LED_HEADER = bytes((0xFF, 0xFF))


class ArduinoException(Exception):
    pass


class Arduino:

    def __init__(self, device, baud=400000, tout=1):
        from serial import Serial

        self.port = device
        self.serial = Serial(port=device, baudrate=baud, timeout=tout)
        self.reset()

        try:
            self.board_id = ord(self.read(1))
        except TypeError:
            raise ArduinoException('No ID from %s' % device)

        while self.read(1) != b'\x1f':
            pass

        self.leds = None

    @property
    def id(self):
        return self.board_id

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

    def show(self):
        self.write(LED_HEADER)
        self.write(len(self.leds).to_bytes(2, 'little'))
        self.write(bytes(min(0xFE, b) for b in self.leds.raw))

    def __str__(self):
        return 'Arduino #%03d' % self.id

    def __repr__(self):
        return '<Arduino #%03d on %s>' % (self.id, self.port)


def arduino_devices():
    from serial.tools.list_ports import comports
    for p in comports():
        if p.description.startswith('USB'):
            yield p.device


if __name__ == "__main__":
    for d in arduino_devices():
        print(d)
