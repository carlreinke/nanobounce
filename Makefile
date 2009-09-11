# BUILD SETTINGS ###################################

PLATFORM := UNIX

PREFIX := 
EPREFIX := /usr
HOST := 

ifeq ($(PLATFORM), WIN32)
	PREFIX := /usr/i486-mingw32
	HOST := i486-mingw32
endif
ifeq ($(PLATFORM), GP2X)
	PREFIX := /opt/open2x/gcc-4.1.1-glibc-2.3.6
	EPREFIX := $(PREFIX)
	HOST := arm-open2x-linux
endif

# END SETTINGS #####################################

TARGET := nanobounce
OBJS := audio.o audio_channel.o audio_sample.o audio_stream.o ball.o block.o controller.o font.o game.o highscore.o level.o main.o menu.o misc.o sprite.o video.o volume.o

STRIP := strip

BINDIR := $(EPREFIX)/bin
LIBDIR := $(PREFIX)/lib
INCLUDEDIR := $(PREFIX)/include

ifneq ($(HOST), )
	CXX := $(HOST)-$(CXX)
	STRIP := $(HOST)-$(CXX)
endif

CXX := $(BINDIR)/$(CXX)
STRIP := $(BINDIR)/$(STRIP)

CXXFLAGS += --std=c++98 -pedantic -Wall -Wextra -Wno-long-long -Wno-missing-field-initializers
CXXFLAGS += -I./src -I$(INCLUDEDIR)
LDFLAGS += -L$(LIBDIR) -lm

ifneq ($(PREFIX), )
	SLD_CONFIG_PREFIX := $(PREFIX)/bin
else
	SLD_CONFIG_PREFIX := $(BINDIR)
endif

SDL_CFLAGS := $(shell $(SLD_CONFIG_PREFIX)/sdl-config --cflags)
SDL_LDFLAGS := $(shell $(SLD_CONFIG_PREFIX)/sdl-config --libs)

VORBIS_LDFLAGS := -lvorbisfile

ifeq ($(PLATFORM), WIN32)
	TARGET := $(TARGET).exe
endif
ifeq ($(PLATFORM), GP2X)
	TARGET := $(TARGET).gpe
	
	CXXFLAGS += -mcpu=arm920t -mtune=arm920t -msoft-float -ffast-math
	
	VORBIS_LDFLAGS := -lvorbisidec
endif

CXXFLAGS += -DTARGET_$(PLATFORM) $(SDL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS) $(VORBIS_LDFLAGS)

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
	rm -rf obj/* src/precompiled.hpp.gch
	rm -f $(TARGET)

OBJS := $(foreach obj, $(OBJS), obj/$(obj))

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

src/precompiled.hpp.gch : src/precompiled.hpp
	-$(CXX) -o $@ -c $(DEBUG_FLAGS_$(DEBUG)) $(CXXFLAGS) $<

obj/%.d : obj/%.o
obj/%.o : src/%.cpp src/precompiled.hpp.gch
	@mkdir -p "$(dir $@)"
	$(CXX) -o $@ -MMD -c $(DEBUG_FLAGS_$(DEBUG)) $(CXXFLAGS) -include "precompiled.hpp" $< 
