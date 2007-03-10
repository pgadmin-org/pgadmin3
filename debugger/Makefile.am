##############################################################################
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 4875 2007-03-01 21:06:46Z h-saito $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

SUBDIRS = debugger

bin_PROGRAMS =debugger

debugger_SOURCES = \
	$(srcdir)/src/wsapp.cpp \
	$(srcdir)/src/wsbreakpoint.cpp \
	$(srcdir)/src/wscodewindow.cpp \
	$(srcdir)/src/wsconnprop.cpp \
	$(srcdir)/src/wsconsole.cpp \
	$(srcdir)/src/wsdbresult.cpp \
	$(srcdir)/src/wsdirectdbg.cpp \
	$(srcdir)/src/wsdocmgr.cpp \
	$(srcdir)/src/wsfuncdoc.cpp \
	$(srcdir)/src/wsfuncview.cpp \
	$(srcdir)/src/wsmainframe.cpp \
	$(srcdir)/src/wsmessagewindow.cpp \
	$(srcdir)/src/wspgconn.cpp \
	$(srcdir)/src/wspgthread.cpp \
	$(srcdir)/src/wsquerywindow.cpp \
	$(srcdir)/src/wsresultgrid.cpp \
	$(srcdir)/src/wsresultset.cpp \
	$(srcdir)/src/wsrichwindow.cpp \
	$(srcdir)/src/wsstackwindow.cpp \
	$(srcdir)/src/wstabwindow.cpp \
	$(srcdir)/src/wstargetinfo.cpp \
	$(srcdir)/src/wsvarwindow.cpp \
	$(srcdir)/src/wswaitingdialog.cpp 

EXTRA_DIST = \
		$(srcdir)/debugger.rc \
		$(srcdir)/debugger.dsp

if !SUN_CC
  __CFLAGS=-Wall -I$(srcdir)/include
else
  __CFLAGS=-I$(srcdir)/include
endif

if !SUN_CXX
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor
else
  __CXXFLAGS=""
endif
