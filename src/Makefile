# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002, The pgAdmin Development Team
# This software is released under the pgAdmin Public Licence
#
# Makefile - Makefile for *nix systems



CC = g++
PGDIR = /usr/local/pgsql
WXDIR = /usr/local/wx2

# You shouldn't need to edit below here.

PROGRAM = pgAdmin3
CFLAGS  = `wx-config --cflags` -I$(WXDIR)/include -I$(WXDIR)/contrib/include -Iinclude -I$(PGDIR)/include 
LDFLAGS = $(WXDIR)/lib/libstc.a $(PGDIR)/lib/libpq.a $(WXDIR)/lib/libwxxrc.a /usr/lib/libcrypt.a `wx-config --static --libs`

OBJECTS = $(PROGRAM).o \
          db/pgConn.o \
          db/pgSet.o \
          schema/pgCollection.o \
          schema/pgDatabase.o \
          schema/pgObject.o \
          schema/pgServer.o \
          schema/pgUser.o \
          ui/browser.o \
          ui/ctlSQLBox.o \
          ui/events.o \
          ui/frmAbout.o \
          ui/frmConnect.o \
          ui/frmMain.o \
          ui/frmOptions.o \
          ui/frmPassword.o \
          ui/frmSplash.o \
          ui/frmUpgradeWizard.o \
          ui/statistics.o \
          utils/misc.o \
          utils/sysLogger.o \
          utils/sysSettings.o

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CC) -c $(CFLAGS) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o $(PROGRAM)
	rm -f db/*.o
	rm -f schema/*.o
	rm -f ui/*.o
	rm -f utils/*.o
