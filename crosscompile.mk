# CROSS-COMPILE SETTINGS ###################################

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

TARGET := $(TARGET)$($(PLATFORM)_EXT)

PREFIX := $($(PLATFORM)_PREFIX)
EPREFIX := $($(PLATFORM)_EPREFIX)
HOST := $($(PLATFORM)_HOST)

BINDIR := $(EPREFIX)/bin
LIBDIR := $(PREFIX)/lib
INCLUDEDIR := $(PREFIX)/include

ifneq ($(HOST), )
    CXX := $(HOST)-$(notdir $(CXX))
    STRIP := $(HOST)-$(notdir $(STRIP))
endif

CXX := $(BINDIR)/$(notdir $(CXX))
STRIP := $(BINDIR)/$(notdir $(STRIP))

SDL_CONFIG := $(PREFIX)/bin/$(SDL_CONFIG)

# FLAGS ####################################################

CXXFLAGS += -I$(INCLUDEDIR) $($(PLATFORM)_CXXFLAGS)
LDFLAGS += -L$(LIBDIR) $($(PLATFORM)_LDFLAGS)

ifeq ($(PLATFORM), GP2X)
    VORBIS_LDLIBS := -lvorbisidec
endif
