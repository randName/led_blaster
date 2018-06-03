ARDUINO_QUIET = 1

# base configuration
BOARD_TAG    = nano328
ARDUINO_LIBS = FastLED

# defaults
PORT   = 0
LAYOUT = 00

MONITOR_PORT = /dev/ttyUSB$(PORT)
TARGET       = capstone-$(LAYOUT)
OBJDIR       = build-$(BOARD_TAG)-$(LAYOUT)
CXXFLAGS    += -Ilayouts/$(LAYOUT)

include /usr/share/arduino/Arduino.mk
