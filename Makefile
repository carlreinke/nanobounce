CXX := g++
STRIP := strip
PKG_CONFIG := pkg-config

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

EXTRA_LDFLAGS += -Wl,-rpath=.

SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl)
SDL_LDLIBS := $(shell $(PKG_CONFIG) --libs sdl)

VORBIS_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags vorbisfile)
VORBIS_LDLIBS ?= $(shell $(PKG_CONFIG) --libs vorbisfile)

ALL_CXXFLAGS := -std=c++14 \
                -I./src \
                $(EXTRA_CXXFLAGS) \
                $(SDL_CFLAGS) \
                $(VORBIS_CFLAGS) \
                $(CXXFLAGS)
ALL_LDFLAGS := $(EXTRA_LDFLAGS) \
               $(LDFLAGS)
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

$(TARGET) : $(OBJS)
	$(CXX) $(ALL_LDFLAGS) -o $@ $^ $(LDLIBS)

ifneq ($(MAKECMDGOALS), clean)
    -include $(OBJS:.o=.d)
endif

src/precompiled.hpp.gch : src/precompiled.hpp
	-$(CXX) $(ALL_CXXFLAGS) -c -o $@ $<

obj/json/%.o : src/json/%.cpp
	@mkdir -p "$(dir $@)"
	$(CXX) $(ALL_CXXFLAGS) -Wno-error -c -o $@ $< 

obj/%.o : src/%.cpp src/precompiled.hpp.gch
	@mkdir -p "$(dir $@)"
	$(CXX) $(ALL_CXXFLAGS) -c -o $@ -include "precompiled.hpp" $< 
