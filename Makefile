# BUILD SETTINGS ###################################
DEBUG := 1
PLATFORM := UNIX
USE_PRECOMPILED := 1

# If building for the GP2X
GP2X_CHAINPREFIX := /opt/open2x/gcc-4.1.1-glibc-2.3.6
GP2X_CHAIN := $(GP2X_CHAINPREFIX)/bin/arm-open2x-linux-

# END SETTINGS #####################################

TARGET := bouncing
OBJS := audio.o audio_sample.o ball.o block.o bouncing.o controller.o font.o level.o misc.o sprite.o video.o

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS := -g3 -O0 -Werror
else
	DEBUG_FLAGS := -g0 -O2 -DNDEBUG
endif

ifeq ($(USE_PRECOMPILED), 1)
	PRECOMPILED_H_GCH := src/precompiled.h.gch
	INCLUDE_PRECOMPILED := -include "precompiled.h"
else
	PRECOMPILED_H_GCH :=
	INCLUDE_PRECOMPILED :=
endif

SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

ifeq ($(PLATFORM), WIN32)
	TARGET := $(TARGET).exe
	
	CXX := i486-mingw32-g++
	CXXFLAGS += -DTARGET_WIN32
	
	SDL_CFLAGS := $(shell ../SDL/bin/sdl-config --cflags)
	SDL_LDFLAGS := $(shell ../SDL/bin/sdl-config --libs)
endif
ifeq ($(PLATFORM), GP2X)
	CXX := $(GP2X_CHAIN)g++
	CXXFLAGS += -DTARGET_GP2X -mcpu=arm920t -mtune=arm920t -msoft-float -ffast-math
	
	SDL_CFLAGS := `$(GP2X_CHAINPREFIX)/bin/sdl-config --cflags` -I$(GP2X_CHAINPREFIX)/include
	SDL_LDFLAGS := `$(GP2X_CHAINPREFIX)/bin/sdl-config --libs` -L$(GP2X_CHAINPREFIX)/lib
endif

CXXFLAGS += --std=c++98 -pedantic -Wall -Wno-long-long -I$(CURDIR)/src/ $(DEBUG_FLAGS) $(SDL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS) -lm
#CXXFLAGS += -pg
#LDFLAGS += -pg

####################################################

all : $(TARGET)

OBJS := $(foreach obj, $(OBJS), obj/$(obj))

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

$(PRECOMPILED_H_GCH) : src/precompiled.h
	$(CXX) -o $@ -c $(CXXFLAGS) $<

obj/%.d : obj/%.o
obj/%.o : src/%.cpp $(PRECOMPILED_H_GCH)
	@mkdir -p "$(dir $@)"
	$(CXX) -o $@ -MMD -c $(CXXFLAGS) $(INCLUDE_PRECOMPILED) $< 

.PHONY : clean

clean :
	rm -rf obj/* $(PRECOMPILED_H_GCH)
	rm -f $(TARGET)
