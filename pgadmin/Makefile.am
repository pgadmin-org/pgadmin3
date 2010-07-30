#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2010, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

bin_PROGRAMS = pgadmin3

pgadmin3_SOURCES = \
	$(srcdir)/pgAdmin3.cpp 

EXTRA_DIST = \
	$(srcdir)/Makefile.deps \
	$(srcdir)/pgAdmin3.vcproj \
	$(srcdir)/precomp.cpp \
	$(srcdir)/pgAdmin3.rc \
	$(srcdir)/ver_svn.bat \
	$(srcdir)/ver_svn.sh

# Make sure we always have these dependencies, even if dependency tracking
# is turend off.  The leading dash keeps automake from trying to process this.
-include Makefile.deps

TMP_ui = 

# Include all the sub-Makefiles
include $(srcdir)/agent/module.mk
include $(srcdir)/ctl/module.mk
include $(srcdir)/db/module.mk
include $(srcdir)/debugger/module.mk
include $(srcdir)/dlg/module.mk
include $(srcdir)/frm/module.mk
include $(srcdir)/include/module.mk
include $(srcdir)/pgscript/module.mk
include $(srcdir)/schema/module.mk
include $(srcdir)/slony/module.mk
include $(srcdir)/gqb/module.mk
include $(srcdir)/ui/module.mk
include $(srcdir)/utils/module.mk

if SUN_CC
  __CFLAGS=""
else
  __CFLAGS=-Wall
endif

if SUN_CXX
  __CXXFLAGS=""
else
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor -fno-strict-aliasing
endif

if EDB_LIBPQ
  __EDB_LIBPQ=-DEDB_LIBPQ
else
  __EDB_LIBPQ=
endif

if !APPBUNDLE

nobase_dist_pkgdata_DATA = \
	$(srcdir)/settings.ini


if BUILD_DEBUG
nobase_dist_pkgdata_DATA += \
	$(TMP_ui)
endif

AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" $(__CXXFLAGS) -I$(top_srcdir)/pgadmin/include $(__EDB_LIBPQ)
AM_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" $(__CFLAGS) -I$(top_srcdir)/pgadmin/include $(__EDB_LIBPQ)

# Automake trys to execute install-exec-hook if it appears anywhere in the file, so we need a dummy
# for non-APPBUNDLE cases.
install-exec-hook:

else

nobase_dist_data_DATA = \
	$(srcdir)/settings.ini

if BUILD_DEBUG
nobase_dist_data_DATA += \
	$(TMP_ui)
endif

AM_CXXFLAGS = $(__CXXFLAGS) -I$(top_srcdir)/pgadmin/include $(__EDB_LIBPQ)
AM_CFLAGS = $(__CFLAGS) -I$(top_srcdir)/pgadmin/include $(__EDB_LIBPQ)

install-exec-hook:
	cd $(bindir) ;\
	mv pgadmin3 tmp ;\
	mv tmp pgAdmin3 ;\
	if test "$(BUILD_DEBUG)" = "no"; then \
	    strip pgAdmin3 ;\
	fi

endif
