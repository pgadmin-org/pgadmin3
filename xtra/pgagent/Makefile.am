##############################################################################
# pgAdmin III - PostgreSQL Tools 
# $Id$
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

SUBDIRS = pgaevent

bin_PROGRAMS = pgagent

pgagent_SOURCES = \
	$(srcdir)/connection.cpp \
	$(srcdir)/job.cpp \
	$(srcdir)/misc.cpp \
	$(srcdir)/pgAgent.cpp \
	$(srcdir)/unix.cpp 

EXTRA_DIST = \
        $(srcdir)/win32.cpp \
        $(srcdir)/pgAgent.dsp \
        $(srcdir)/pgAgent.dsw \
        $(srcdir)/pgAgent.rc \
        $(srcdir)/pgagent.sql

noinst_HEADERS = \
        $(srcdir)/include/connection.h \
        $(srcdir)/include/job.h \
        $(srcdir)/include/misc.h \
        $(srcdir)/include/pgAgent.h

if !APPBUNDLE

AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(srcdir)/include
AM_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(srcdir)/include
AM_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(srcdir)/include

# Automake trys to execute install-exec-hook if it appears anywhere in the file, so we need a dummy
# for non-APPBUNDLE cases.
install-exec-hook:

else

AM_CXXFLAGS = -Wall -g -I$(srcdir)/include 
AM_CPPFLAGS = -Wall -g -I$(srcdir)/include
AM_CFLAGS = -Wall -g -I$(srcdir)/include 
install-exec-hook:
        cd $(bindir) ;\
        mv pgagent tmp ;\
        mv tmp pgagent ;\
        if test "$(pg_debug_build)" = "no"; then \
                strip pgagent ;\
        fi

endif
