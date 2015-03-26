CC:=gcc
CXX:=g++
RC:=windres
AR:=ar rc
RANLIB:=ranlib
LINK:=$(CXX)
RM:=rm -f

CFLAGS:=-O2 -Wall -pipe
LFLAGS:=-L../lib
DEFINES:=-DPCRE_STATIC
INCLUDES:=-I. -I../pcre -I../squirrel
LIBS:=-lsquirrel -lpcre

ifeq ($(DEBUG),y)
CFLAGS:=-g $(CFLAGS)
RCFLAGS:=-D_DEBUG
DEFINES:=$(DEFINES) -D_DEBUG
else
CFLAGS:=$(CFLAGS) -fomit-frame-pointer
RCFLAGS:=-DNDEBUG
LFLAGS:=-s $(LFLAGS)
DEFINES:=$(DEFINES) -DNDEBUG
endif

CXXFLAGS:=$(CFLAGS) -fno-rtti
mkoutdir=@if [ ! -d "$(target_dir)" ]; then mkdir "$(target_dir)"; fi

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $< $(INCLUDES)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(DEFINES) -c -o $@ $< $(INCLUDES)
