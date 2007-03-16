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

bin_PROGRAMS = debugger

debugger_SOURCES = \
	$(srcdir)/debugger.cpp \
	$(srcdir)/wsBreakPoint.cpp \
	$(srcdir)/wsCodeWindow.cpp \
	$(srcdir)/wsConnProp.cpp \
	$(srcdir)/wsConsole.cpp \
	$(srcdir)/wsDbresult.cpp \
	$(srcdir)/wsDirectdbg.cpp \
	$(srcdir)/wsDocmgr.cpp \
	$(srcdir)/wsFuncdoc.cpp \
	$(srcdir)/wsFuncView.cpp \
	$(srcdir)/wsMainFrame.cpp \
	$(srcdir)/wsMessageWindow.cpp \
	$(srcdir)/wsPgconn.cpp \
	$(srcdir)/wsPgthread.cpp \
	$(srcdir)/wsQueryWindow.cpp \
	$(srcdir)/wsResultGrid.cpp \
	$(srcdir)/wsResultset.cpp \
	$(srcdir)/wsRichWindow.cpp \
	$(srcdir)/wsStackWindow.cpp \
	$(srcdir)/wsTabWindow.cpp \
	$(srcdir)/wsTargetInfo.cpp \
	$(srcdir)/wsVarWindow.cpp \
	$(srcdir)/wsWaitingDialog.cpp

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
