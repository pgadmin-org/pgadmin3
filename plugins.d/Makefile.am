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

EXTRA_DIST = \
	$(srcdir)/plugins.ini

if !APPBUNDLE
pluginsdir = $(pkgdatadir)/plugins.d
else
pluginsdir = $(datadir)/plugins.d
endif

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pluginsdir)
	$(install_sh) -c -m 644 $(EXTRA_DIST) $(DESTDIR)$(pluginsdir)
