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
	$(srcdir)/branding.ini

if !APPBUNDLE
brandingdir = $(pkgdatadir)/branding
else
brandingdir = $(datadir)/branding
endif

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(brandingdir)
	$(install_sh) -c -m 644 branding.ini $(DESTDIR)$(brandingdir)/$$DIR
