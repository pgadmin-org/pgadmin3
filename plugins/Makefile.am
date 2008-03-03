#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 4920 2006-01-17 22:25:13Z dpage $
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

EXTRA_DIST = \
	$(srcdir)/utilities.ini

if !APPBUNDLE
utilitiesdir = $(pkgdatadir)/plugins
else
utilitiesdir = $(datadir)/plugins
endif

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(utilitiesdir)
	$(install_sh) -c -m 644 utilities.ini $(DESTDIR)$(utilitiesdir)

