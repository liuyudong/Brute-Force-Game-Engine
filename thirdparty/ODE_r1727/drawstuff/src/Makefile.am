# Drawstuff is meant as an aid for testing and not as a full
# rendering library.

noinst_LTLIBRARIES = libdrawstuff.la
libdrawstuff_la_SOURCES = drawstuff.cpp internal.h
AM_CPPFLAGS = -I$(top_srcdir)/include \
        -DDEFAULT_PATH_TO_TEXTURES='"$(top_srcdir)/drawstuff/textures/"' \
        $(X_CFLAGS)

if WIN32
libdrawstuff_la_SOURCES+= windows.cpp resource.h resources.rc
libdrawstuff_la_LIBADD = -lwinmm -lgdi32
libdrawstuff_la_LDFLAGS = -no-undefined
endif

if X11
libdrawstuff_la_SOURCES+= x11.cpp
libdrawstuff_la_LIBADD = $(X_LIBS)
endif

if OSX
libdrawstuff_la_SOURCES+= osx.cpp
endif

