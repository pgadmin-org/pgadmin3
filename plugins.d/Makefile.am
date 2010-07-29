#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 4920 2006-01-17 22:25:13Z dpage $
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
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
