# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002, The pgAdmin Development Team
# This software is released under the pgAdmin Public Licence
#
# Makefile - Makefile for *nix systems

CC = gcc
PROGRAM = pgAdmin3
CFLAGS  = `wx-config --cflags`
LDFLAGS = `wx-config --libs` -lstc

OBJECTS = $(PROGRAM).o \
          ui/forms/frmAbout.o \
          ui/forms/frmMain.o \
          ui/forms/frmSplash.o \
          ui/controls/ctlSQLBox.o \
          utils/sysLogger.o

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CC) -c $(CFLAGS) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o $(PROGRAM)
	rm -f ui/*.o
	rm -f ui/forms/*.o
  rm -f utils/*.o
