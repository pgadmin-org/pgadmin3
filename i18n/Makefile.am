#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 4920 2006-01-17 22:25:13Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

# Define the list of 'published' languages. This should match those
# at  http://www.pgadmin.org/translation/status.php. These are the only ones
# that will be installed, but all will be shipped in the source tarball.

PUB_TX = af_ZA bg_BG ca_ES de_DE es_ES fi_FI fr_FR it_IT ja_JP nl_NL pl_PL


EXTRA_DIST = \
	$(srcdir)/wxstd.pot \
	$(srcdir)/??_??/pgadmin3.po \
	$(srcdir)/wxstd.mo \
	$(srcdir)/pgadmin3.lng \
	$(srcdir)/pg_settings.csv \
	$(srcdir)/??_??/pgadmin3.mo \
	$(srcdir)/??_??/wxstd.mo

if !APPBUNDLE
i18ndir = $(pkgdatadir)/i18n
else
i18ndir = $(datadir)/i18n
endif

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(i18ndir)
	@pub_tx='$(PUB_TX)'; \
	for DIR in $$pub_tx; do \
		$(mkinstalldirs) $(DESTDIR)$(i18ndir)/$$DIR; \
		for FILE in `ls $$DIR/*.mo`; do \
			$(install_sh) -c -m 644 $$FILE $(DESTDIR)$(i18ndir)/$$DIR/; \
		done \
	done
	$(install_sh) -c -m 644 pgadmin3.lng $(DESTDIR)$(i18ndir)/$$DIR
	$(install_sh) -c -m 644 pg_settings.csv $(DESTDIR)$(i18ndir)/$$DIR
	$(install_sh) -c -m 644 wxstd.mo $(DESTDIR)$(i18ndir)/$$DIR
