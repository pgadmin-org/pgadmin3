#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################


AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = pgadmin3

pgadmin3_SOURCES = \
	pgAdmin3.cpp

EXTRA_DIST = \
	Makefile.deps \
	pgAdmin3.vcxproj \
	pgAdmin3.vcxproj.filters \
	precomp.cpp \
	pgAdmin3.rc \
	ver_svn.bat \
	ver_svn.sh

# Make sure we always have these dependencies, even if dependency tracking
# is turend off.  The leading dash keeps automake from trying to process this.
-include $(top_srcdir)/pgadmin/Makefile.deps

TMP_ui =

# Include all the sub-Makefiles
include agent/module.mk
include ctl/module.mk
include db/module.mk
include dd/module.mk
include debugger/module.mk
include dlg/module.mk
include frm/module.mk
include include/module.mk
include ogl/module.mk
include pgscript/module.mk
include schema/module.mk
include slony/module.mk
include gqb/module.mk
include hotdraw/module.mk
include ui/module.mk
include utils/module.mk
include libssh2/module.mk

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

if BUILD_SSH_TUNNEL
AM_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -I$(top_srcdir)/pgadmin/include -I$(top_srcdir)/pgadmin/include/libssh2 $(__CPPFLAGS) $(__EDB_LIBPQ)
else
AM_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -I$(top_srcdir)/pgadmin/include $(__CPPFLAGS) $(__EDB_LIBPQ)
endif

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

if BUILD_SSH_TUNNEL
AM_CPPFLAGS = -I$(top_srcdir)/pgadmin/include -I$(top_srcdir)/pgadmin/include/libssh2 $(__CPPFLAGS) $(__EDB_LIBPQ)
else
AM_CPPFLAGS = -I$(top_srcdir)/pgadmin/include $(__CPPFLAGS) $(__EDB_LIBPQ)
endif

install-exec-hook:
	cd $(bindir) ;\
	mv pgadmin3 tmp ;\
	mv tmp pgAdmin3 ;\
	if test "$(BUILD_DEBUG)" = "no"; then \
	    strip pgAdmin3 ;\
	fi

endif

# Convert images to an embeddable format
BUILT_SOURCES = $(patsubst %.png,%.pngc,$(wildcard $(top_srcdir)/pgadmin/include/images/*.png))

$(top_builddir)/xtra/png2c/png2c:
	(cd $(top_builddir)/xtra/png2c && $(MAKE) $(AM_MAKEFLAGS) png2c)

$(BUILT_SOURCES): $(top_builddir)/xtra/png2c/png2c

%.pngc: %.png
	$(top_builddir)/xtra/png2c/png2c $< $@

clean-local:
	rm -f include/images/*.pngc

