# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002, The pgAdmin Development Team
# This software is released under the pgAdmin Public Licence
#
# Makefile - Makefile for *nix systems

CC = gcc

PROGRAM = pgAdmin3

OBJECTS = $(PROGRAM).o \
          ui/frmAbout.o \
          ui/frmMain.o \
          ui/frmSplash.o


.SUFFIXES:	.o .cpp

.cpp.o :
	$(CC) -c `wx-config --cflags` -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) `wx-config --libs` -lstc

clean:
	rm -f *.o $(PROGRAM)
	rm -f ui/*.o
