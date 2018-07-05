from .config import Config
from .arduino import Arduino, arduino_devices


class Blaster:

    def __init__(self, config_file=None):
        from threading import Event
        self.halt = Event()

        if config_file:
            self.config = Config.from_json(config_file)
        else:
            self.config = {}

        self.arduinos = []

        for port in arduino_devices():
            a = Arduino(port, halt=self.halt)
            if a.id is None:
                continue
            self.arduinos.append(a)

        self.arduinos.sort(key=lambda a: a.id)

    def start(self):
        from time import sleep
        for a in self.arduinos:
            a.start()
        while True:
            sleep(1)

    def stop(self):
        self.halt.set()
