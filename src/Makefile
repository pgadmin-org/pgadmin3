# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002, The pgAdmin Development Team
# This software is released under the pgAdmin Public Licence
#
# Makefile - Makefile for *nix systems



CC = g++
PGDIR = /usr/local/pgsql-7.3
WXDIR = /usr/local/wx2

# You shouldn't need to edit below here.

PROGRAM = pgAdmin3
CFLAGS  = `wx-config --cflags` -I$(WXDIR)/include -I$(WXDIR)/contrib/include -Iinclude -I$(PGDIR)/include -g
LDFLAGS = $(WXDIR)/lib/libstc.a $(PGDIR)/lib/libpq.a $(WXDIR)/lib/libwxxrc.a /usr/lib/libcrypt.a `wx-config --static --libs`

OBJECTS = $(PROGRAM).o \
          db/pgConn.o \
          db/pgSet.o \
          schema/pgAggregate.o \
          schema/pgCast.o \
          schema/pgCheck.o \
          schema/pgCollection.o \
          schema/pgColumn.o \
          schema/pgConversion.o \
          schema/pgDatabase.o \
          schema/pgDomain.o \
          schema/pgForeignKey.o \
          schema/pgFunction.o \
          schema/pgGroup.o \
          schema/pgIndex.o \
          schema/pgLanguage.o \
          schema/pgObject.o \
          schema/pgOperator.o \
          schema/pgOperatorClass.o \
          schema/pgRule.o \
          schema/pgSchema.o \
          schema/pgSequence.o \
          schema/pgServer.o \
          schema/pgTable.o \
          schema/pgTrigger.o \
          schema/pgType.o \
          schema/pgUser.o \
          schema/pgView.o \
          ui/ctlSQLBox.o \
	  ui/ctlSQLResult.o \
          ui/dlgAddTableView.o \
          ui/events.o \
          ui/frmAbout.o \
          ui/frmChildTableViewFrame.o \
          ui/frmConnect.o \
          ui/frmMain.o \
          ui/frmOptions.o \
          ui/frmPassword.o \
          ui/frmQuery.o \
          ui/frmQueryBuilder.o \
          ui/frmQBJoin.o \
          ui/frmSplash.o \
          ui/frmVacuum.o \
          ui/frmEditGrid.o \
          ui/dlgProperty.o \
          ui/dlgUser.o \
          ui/frmStatus.o \
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
