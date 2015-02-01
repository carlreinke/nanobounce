CXX := g++
STRIP := strip
SDL_CONFIG := sdl-config

TARGET := nanobounce

## target device features
EXTRA_CXXFLAGS += -DHAS_KEYBOARD
EXTRA_CXXFLAGS += -DHAS_MOUSE
#EXTRA_CXXFLAGS += -DHAS_SHOULDER_BUTTONS
#EXTRA_CXXFLAGS += -DHAS_320x240_VIDEO
#EXTRA_CXXFLAGS += -DAUDIO_QUALITY=2

## if libc provides sync()
EXTRA_CXXFLAGS += -DHAVE_SYNC

## if want fixed-point Vorbis decoder
#VORBIS_CFLAGS := -DUSE_TREMOR
#VORBIS_LDLIBS := -lvorbisidec

###

SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS := $(SRCS:src/%.cpp=obj/%.o)

###

EXTRA_CXXFLAGS += -MMD
EXTRA_CXXFLAGS += -pedantic
EXTRA_CXXFLAGS += -Wall \
                  -Wextra \
                  -Wold-style-cast \
                  -Wno-long-long \
                  -Wno-missing-field-initializers

ifeq ($(MAKECMDGOALS), release)
    EXTRA_CXXFLAGS += -O2
    EXTRA_CXXFLAGS += -DNDEBUG
else
    EXTRA_CXXFLAGS += -g3
    EXTRA_CXXFLAGS += -O0
    EXTRA_CXXFLAGS += -Werror
endif

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags)
SDL_LDLIBS := $(shell $(SDL_CONFIG) --libs)

VORBIS_CFLAGS ?= 
VORBIS_LDLIBS ?= -lvorbisfile

ALL_CXXFLAGS := -std=c++98 \
                -I./src \
                $(EXTRA_CXXFLAGS) \
                $(SDL_CFLAGS) \
                $(VORBIS_CFLAGS) \
                $(CXXFLAGS)
ALL_LDFLAGS := $(LDFLAGS)
LDLIBS += $(SDL_LDLIBS) \
          $(VORBIS_LDLIBS)

###

.PHONY : all
all : $(TARGET)

.PHONY : release
release : all
	$(STRIP) $(TARGET)

.PHONY : clean
clean :
	rm -rf obj/* src/precompiled.hpp.*
	rm -f $(TARGET)

ifneq ($(MAKECMDGOALS), clean)
    -include $(OBJS:.o=.d)
endif

$(TARGET) : $(OBJS)
	$(CXX) $(ALL_LDFLAGS) -o $@ $^ $(LDLIBS)

src/precompiled.hpp.gch : src/precompiled.hpp
	-$(CXX) $(ALL_CXXFLAGS) -c -o $@ $<

obj/json/%.o : src/json/%.cpp
	@mkdir -p "$(dir $@)"
	$(CXX) $(ALL_CXXFLAGS) -Wno-error -c -o $@ $< 

obj/%.o : src/%.cpp src/precompiled.hpp.gch
	@mkdir -p "$(dir $@)"
	$(CXX) $(ALL_CXXFLAGS) -c -o $@ -include "precompiled.hpp" $< 
