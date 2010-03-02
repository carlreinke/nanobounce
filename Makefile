# BUILD SETTINGS ###########################################

PLATFORM := UNIX

TARGET := nanobounce

############################################################

STRIP := strip
SDL_CONFIG := sdl-config

ifneq ($(PLATFORM), UNIX)
    include crosscompile.mk
endif

SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS := $(SRCS:src/%.cpp=obj/%.o)

# FLAGS ####################################################

ifeq ($(MAKECMDGOALS), release)
    EXTRA_CXXFLAGS += -O2 -DNDEBUG
else
    EXTRA_CXXFLAGS += -g3 -O0 -Werror
endif
EXTRA_CXXFLAGS += -MMD -pedantic -Wall -Wextra -Wold-style-cast -Wno-long-long -Wno-missing-field-initializers

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags)
SDL_LDLIBS := $(shell $(SDL_CONFIG) --libs)

VORBIS_LDLIBS ?= -lvorbisfile

ALL_CXXFLAGS += -std=c++98 -I./src -DTARGET_$(PLATFORM) $(EXTRA_CXXFLAGS) $(SDL_CFLAGS) $(CXXFLAGS)
ALL_LDFLAGS += $(LDFLAGS)
LDLIBS += $(SDL_LDLIBS) $(VORBIS_LDLIBS)

# RULES ####################################################

.PHONY : all release clean

all : $(TARGET)

release : all
	$(STRIP) $(TARGET)

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
