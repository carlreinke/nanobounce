# BUILD SETTINGS ###################################

PLATFORM := UNIX

# If building for the GP2X
GP2X_CHAINPREFIX := /opt/open2x/gcc-4.1.1-glibc-2.3.6
GP2X_CHAIN := $(GP2X_CHAINPREFIX)/bin/arm-open2x-linux-

# END SETTINGS #####################################

TARGET := bouncing
OBJS := audio.o audio_sample.o ball.o block.o bouncing.o controller.o font.o level.o misc.o sprite.o video.o

STRIP := strip

CXXFLAGS += --std=c++98 -pedantic -Wall -Wno-long-long -I$(CURDIR)/src/
LDFLAGS += -lm

SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

ifeq ($(PLATFORM), WIN32)
	TARGET := $(TARGET).exe
	
	CXX := i486-mingw32-g++
	STRIP := i486-mingw32-strip
	
	CXXFLAGS += -DTARGET_WIN32
	
	SDL_CFLAGS := $(shell ../SDL/bin/sdl-config --cflags)
	SDL_LDFLAGS := $(shell ../SDL/bin/sdl-config --libs)
endif
ifeq ($(PLATFORM), GP2X)
	TARGET := $(TARGET).gpe
	
	CXX := $(GP2X_CHAIN)g++
	STRIP := $(GP2X_CHAIN)strip
	
	CXXFLAGS += -DTARGET_GP2X -mcpu=arm920t -mtune=arm920t -msoft-float -ffast-math
	
	SDL_CFLAGS := `$(GP2X_CHAINPREFIX)/bin/sdl-config --cflags` -I$(GP2X_CHAINPREFIX)/include
	SDL_LDFLAGS := `$(GP2X_CHAINPREFIX)/bin/sdl-config --libs` -L$(GP2X_CHAINPREFIX)/lib
endif

CXXFLAGS += $(SDL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS)

DEBUG := 1
DEBUG_FLAGS_0 := -g0 -O2 -DNDEBUG
DEBUG_FLAGS_1 := -g3 -O0 -Werror

####################################################

.PHONY : all
all : $(TARGET)

.PHONY : release
release : DEBUG := 0
release : all
	$(STRIP) $(TARGET)

.PHONY : clean
clean :
	rm -rf obj/* src/precompiled.h.gch
	rm -f $(TARGET)

OBJS := $(foreach obj, $(OBJS), obj/$(obj))

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

src/precompiled.h.gch : src/precompiled.h
	-$(CXX) -o $@ -c $(DEBUG_FLAGS_$(DEBUG)) $(CXXFLAGS) $<

obj/%.d : obj/%.o
obj/%.o : src/%.cpp src/precompiled.h.gch
	@mkdir -p "$(dir $@)"
	$(CXX) -o $@ -MMD -c $(DEBUG_FLAGS_$(DEBUG)) $(CXXFLAGS) -include "precompiled.h" $< 
