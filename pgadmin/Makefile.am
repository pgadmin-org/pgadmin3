#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5019 2006-02-21 15:29:07Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

bin_PROGRAMS = pgadmin3

pgadmin3_SOURCES = \
	$(srcdir)/include/svnversion.cpp \
	$(srcdir)/pgAdmin3.cpp 

EXTRA_DIST = \
	$(srcdir)/pgAdmin3.vcproj \
	$(srcdir)/precomp.cpp \
	$(srcdir)/pgAdmin3.rc

# Generate svnversion.h every time, but only if repo version has changed.
.PHONY: $(srcdir)/include/svnversion.cpp
$(srcdir)/include/svnversion.cpp:
	test -f $(srcdir)/include/svnversion.cpp || touch $(srcdir)/include/svnversion.cpp
	test -d $(srcdir)/.svn && ( V=`svnversion $(srcdir)/..` && OV=`cat $(srcdir)/include/svnversion.h | cut -d' ' -f3|tr -d '"'` ; if [ "$$V" != "$$OV" ]; then echo "#define VERSION_SVN \"$$V\"" > $(srcdir)/include/svnversion.h ; fi ) || true

TMP_ui = 

# Include all the sub-Makefiles
include $(srcdir)/agent/module.mk
include $(srcdir)/ctl/module.mk
include $(srcdir)/db/module.mk
include $(srcdir)/debugger/module.mk
include $(srcdir)/dlg/module.mk
include $(srcdir)/frm/module.mk
include $(srcdir)/include/module.mk
include $(srcdir)/schema/module.mk
include $(srcdir)/slony/module.mk
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
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor
endif

if EDB_LIBPQ
  __EDB_LIBPQ=-DEDB_LIBPQ
else
  __EDB_LIBPQ=""
endif

if !APPBUNDLE

if BUILD_DEBUG
nobase_dist_pkgdata_DATA = $(TMP_ui)
endif
AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" $(__CXXFLAGS) -I$(top_srcdir)/pgadmin/include $(__EDB_LIBPQ)
AM_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" $(__CFLAGS) -I$(top_srcdir)/pgadmin/include $(__EDB_LIBPQ)

# Automake trys to execute install-exec-hook if it appears anywhere in the file, so we need a dummy
# for non-APPBUNDLE cases.
install-exec-hook:

else

if BUILD_DEBUG
nobase_dist_data_DATA = $(TMP_ui)
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
