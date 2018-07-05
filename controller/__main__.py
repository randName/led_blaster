from os import getenv

try:
    from . import Blaster
    b = Blaster(getenv('BLASTER_SETTINGS'))
    b.start()
except KeyboardInterrupt:
    b.stop()
    print('\nKeyboard interrupt received, exiting')
