# BUILD SETTINGS ###########################################

PLATFORM := UNIX

# CROSS-COMPILE SETTINGS ###################################

UNIX_PREFIX := /usr
UNIX_EPREFIX := /usr
UNIX_HOST := 

WIN32_PREFIX := /usr/i486-mingw32
WIN32_EPREFIX := /usr
WIN32_HOST := i486-mingw32
WIN32_EXT := .exe

GP2X_PREFIX := /opt/open2x/gcc-4.1.1-glibc-2.3.6
GP2X_EPREFIX := $(GP2X_PREFIX)
GP2X_HOST := arm-open2x-linux
GP2X_EXT := .gpe
GP2X_CXXFLAGS := -mcpu=arm920t -mtune=arm920t -msoft-float -ffast-math

# END SETTINGS #############################################

TARGET := nanobounce$($(PLATFORM)_EXT)

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=obj/%.o)

# TOOLCHAIN ################################################

STRIP := strip

PREFIX := $($(PLATFORM)_PREFIX)
EPREFIX := $($(PLATFORM)_EPREFIX)
HOST := $($(PLATFORM)_HOST)

BINDIR := $(EPREFIX)/bin
LIBDIR := $(PREFIX)/lib
INCLUDEDIR := $(PREFIX)/include

ifneq ($(HOST), )
	CXX := $(HOST)-$(CXX)
	STRIP := $(HOST)-$(STRIP)
endif

CXX := $(BINDIR)/$(CXX)
STRIP := $(BINDIR)/$(STRIP)

SDL_CONFIG := sdl-config

ifneq ($(PREFIX), )
	SDL_CONFIG := $(PREFIX)/bin/$(SDL_CONFIG)
endif

# FLAGS ####################################################

NDEBUG_FLAGS := -g0 -O2 -DNDEBUG
DEBUG_FLAGS := -g3 -O0 -Werror

CXXFLAGS += --std=c++98 -pedantic -Wall -Wextra -Wno-long-long -Wno-missing-field-initializers
CXXFLAGS += -I./src -I$(INCLUDEDIR)
LDFLAGS += -L$(LIBDIR)

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags)
SDL_LDLIBS := $(shell $(SDL_CONFIG) --libs)

VORBIS_LDLIBS := -lvorbisfile

ifeq ($(PLATFORM), GP2X)
	VORBIS_LDLIBS := -lvorbisidec
endif

CXXFLAGS += $($(PLATFORM)_CXXFLAGS) -DTARGET_$(PLATFORM) $(SDL_CFLAGS)
LDFLAGS += $($(PLATFORM)_LDFLAGS) $(SDL_LDLIBS) $(VORBIS_LDLIBS)

# RULES ####################################################

.PHONY : all
all : $(TARGET)

.PHONY : release
release : DEBUG_FLAGS := $(NDEBUG_FLAGS)
release : all
	$(STRIP) $(TARGET)

.PHONY : clean
clean :
	rm -rf obj/* src/precompiled.hpp.gch
	rm -f $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

src/precompiled.hpp.gch : src/precompiled.hpp
	-$(CXX) -o $@ -c $(DEBUG_FLAGS) $(CXXFLAGS) $<

obj/%.d : obj/%.o
obj/%.o : src/%.cpp src/precompiled.hpp.gch
	@mkdir -p "$(dir $@)"
	$(CXX) -o $@ -MMD -c $(DEBUG_FLAGS) $(CXXFLAGS) -include "precompiled.hpp" $< 
