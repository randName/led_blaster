EXE = ../build/blaster

CXX = g++
SOURCES := $(wildcard src/*.cpp) $(wildcard include/*.cpp)
HEADERS := $(wildcard src/*.h) $(wildcard include/*.h)
OBJECTS := $(SOURCES:.cpp=.o)

INCLUDES += -Isrc/ -Iinclude/ -I/opt/vc/include
CFLAGS += -Wall -O3 -std=c++11 -fpermissive -DGLM_FORCE_CXX98
LDFLAGS += -L$(SDKSTAGE)/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lpthread

all: $(EXE)

%.o: %.cpp
	@echo $@
	$(CXX) $(CFLAGS) $(INCLUDES) -g -c $< -o $@

$(EXE): $(OBJECTS) $(HEADERS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -rdynamic -o $@

clean:
	@rm -rvf $(EXE) src/*.o
