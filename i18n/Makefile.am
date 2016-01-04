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

# Define the list of 'published' languages. This should match those
# at  http://www.pgadmin.org/translation/status.php. These are the only ones
# that will be installed, but all will be shipped in the source tarball.

PUB_TX = ca_ES cs_CZ de_DE fr_FR ja_JP lv_LV pl_PL ru_RU sr_RS zh_CN


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
		for FILE in `ls $(srcdir)/$$DIR/*.mo`; do \
			$(install_sh) -c -m 644 $$FILE $(DESTDIR)$(i18ndir)/$$DIR/; \
		done \
	done
	$(install_sh) -c -m 644 $(srcdir)/pgadmin3.lng $(DESTDIR)$(i18ndir)/$$DIR
	$(install_sh) -c -m 644 $(srcdir)/pg_settings.csv $(DESTDIR)$(i18ndir)/$$DIR
	$(install_sh) -c -m 644 $(srcdir)/wxstd.mo $(DESTDIR)$(i18ndir)/$$DIR

# Remove the wildcard directory that automake will have added for us
dist-hook:
	rm -rf $(distdir)/\?\?_\?\?
