# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002, The pgAdmin Development Team
# This software is released under the pgAdmin Public Licence
#
# Makefile - Makefile for *nix systems



CC = gcc
PGDIR = /usr/local/pgsql
WXDIR = /usr/local/wx2

# You shouldn't need to edit below here.

PROGRAM = pgAdmin3
CFLAGS  = `wx-config --cflags` -I$(PGDIR)/include 
LDFLAGS = `wx-config --libs` -lstc -lpq -lwxxrc -L$(PGDIR)/lib -L$(WXDIR)/lib 

OBJECTS = $(PROGRAM).o \
          db/pg/pgConn.o \
          db/pg/pgSet.o \
          schema/pg/pgObject.o \
          schema/pg/pgServer.o \
          ui/forms/frmAbout.o \
          ui/forms/frmConnect.o \
          ui/forms/frmMain.o \
          ui/forms/frmOptions.o \
          ui/forms/frmPassword.o \
          ui/forms/frmSplash.o \
          ui/forms/frmUpgradeWizard.o \
          ui/controls/ctlSQLBox.o \
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
	rm -f db/pg/*.o
	rm -f schema/pg/*.o
	rm -f ui/forms/*.o
	rm -f ui/controls/*.o
	rm -f utils/*.o
