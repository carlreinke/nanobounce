# BUILD SETTINGS ###########################################

PLATFORM := UNIX

TARGET := nanobounce

############################################################

STRIP := strip
SDL_CONFIG := sdl-config

ifneq ($(PLATFORM), UNIX)
    include crosscompile.mk
endif

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=obj/%.o)

# FLAGS ####################################################

ifeq ($(MAKECMDGOALS), release)
    EXTRA_CXXFLAGS += -O2 -DNDEBUG
else
    EXTRA_CXXFLAGS += -g3 -O0 -Werror
endif
EXTRA_CXXFLAGS += -MMD -pedantic -Wall -Wextra -Wno-long-long -Wno-missing-field-initializers

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags)
SDL_LDLIBS := $(shell $(SDL_CONFIG) --libs)

VORBIS_LDLIBS ?= -lvorbisfile

ALL_CXXFLAGS += --std=c++98 -I./src -DTARGET_$(PLATFORM) $(EXTRA_CXXFLAGS) $(SDL_CFLAGS) $(CXXFLAGS)
ALL_LDFLAGS += $(LDFLAGS)
LDLIBS += $(SDL_LDLIBS) $(VORBIS_LDLIBS)

# RULES ####################################################

.PHONY : all release clean

all : $(TARGET)

release : all
	$(STRIP) $(TARGET)

clean :
	rm -rf obj/* src/precompiled.hpp.gch
	rm -f $(TARGET)

ifneq ($(MAKECMDGOALS), clean)
    -include $(OBJS:.o=.d)
endif

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $(ALL_LDFLAGS) $^ $(LDLIBS)

src/precompiled.hpp.gch : src/precompiled.hpp
	-$(CXX) -c -o $@ $(ALL_CXXFLAGS) $<

obj/%.o : src/%.cpp src/precompiled.hpp.gch
	@mkdir -p "$(dir $@)"
	$(CXX) -c -o $@ $(ALL_CXXFLAGS) -include "precompiled.hpp" $< 
