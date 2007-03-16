##############################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 4875 2007-03-01 21:06:46Z h-saito $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
#
##############################################################################

SUBDIRS = debugger

bin_PROGRAMS =debugger

debugger_SOURCES = \
	$(srcdir)/src/debugger.cpp \
	$(srcdir)/src/wsBreakPoint.cpp \
	$(srcdir)/src/wsCodeWindow.cpp \
	$(srcdir)/src/wsConnProp.cpp \
	$(srcdir)/src/wsConsole.cpp \
	$(srcdir)/src/wsDbresult.cpp \
	$(srcdir)/src/wsDirectdbg.cpp \
	$(srcdir)/src/wsDocmgr.cpp \
	$(srcdir)/src/wsFuncdoc.cpp \
	$(srcdir)/src/wsFuncView.cpp \
	$(srcdir)/src/wsMainFrame.cpp \
	$(srcdir)/src/wsMessageWindow.cpp \
	$(srcdir)/src/wsPgconn.cpp \
	$(srcdir)/src/wsPgthread.cpp \
	$(srcdir)/src/wsQueryWindow.cpp \
	$(srcdir)/src/wsResultGrid.cpp \
	$(srcdir)/src/wsResultset.cpp \
	$(srcdir)/src/wsRichWindow.cpp \
	$(srcdir)/src/wsStackWindow.cpp \
	$(srcdir)/src/wsTabWindow.cpp \
	$(srcdir)/src/wsTargetInfo.cpp \
	$(srcdir)/src/wsVarWindow.cpp \
	$(srcdir)/src/wsWaitingDialog.cpp

EXTRA_DIST = \
	$(srcdir)/debugger.rc \
	$(srcdir)/debugger.vcproj

# Include all the sub-Makefiles
include $(srcdir)/include/module.mk


if SUN_CC
  __CFLAGS=""
else
  __CFLAGS=-Wall
endif

if SUN_CXX
  __CXXFLAGS=""
else
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor
endif

if !APPBUNDLE

nobase_dist_pkgdata_DATA = $(TMP_ui)
AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" $(__CXXFLAGS) -I$(top_srcdir)/debugger/include
AM_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" $(__CXXFLAGS) -I$(top_srcdir)/debugger/include
AM_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" $(__CFLAGS) -I$(top_srcdir)/debugger/include

# Automake trys to execute install-exec-hook if it appears anywhere in the file, so we need a dummy
# for non-APPBUNDLE cases.
install-exec-hook:

else

nobase_dist_data_DATA = $(TMP_ui)
AM_CXXFLAGS = $(__CXXFLAGS) -I$(top_srcdir)/debugger/include
AM_CPPFLAGS = $(__CXXFLAGS) -I$(top_srcdir)/debugger/include
AM_CFLAGS = $(__CFLAGS) -I$(top_srcdir)/debugger/include

install-exec-hook:
	cd $(bindir) ;\
	if test "$(BUILD_DEBUG)" = "no"; then \
	    strip debugger ;\
	fi

endif