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
	$(srcdir)/branding.ini

if !APPBUNDLE
brandingdir = $(pkgdatadir)/branding
else
brandingdir = $(datadir)/branding
endif

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(brandingdir)
	$(install_sh) -c -m 644 $(EXTRA_DIST) $(DESTDIR)$(brandingdir)
