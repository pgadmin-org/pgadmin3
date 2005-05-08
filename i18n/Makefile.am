#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

# Define the list of 'published' languages. This should match those
# at  http://www.pgadmin.org/translation.php. These are the only ones
# that will be installed, but all will be shipped in the source tarball.

PUB_TX = af_ZA ar_SA ca_ES da_DK de_DE el_GR es_ES fi_FI fr_FR hu_HU \
	id_ID it_IT ja_JP lv_LV nl_NL pl_PL pt_PT ru_RU sk_SK sl_SI \
	sr_YU tr_TR zh_CN zh_TW


EXTRA_DIST = \
	$(srcdir)/wxstd.pot \
	$(srcdir)/??_??/pgadmin3.po

TMP_ui = \
	$(srcdir)/wxstd.mo \
	$(srcdir)/pgadmin3.lng \
	$(srcdir)/pg_settings.csv \
	$(srcdir)/??_??/pgadmin3.mo \
	$(srcdir)/??_??/wxstd.mo


install-data-local:
	$(mkinstalldirs) $(datadir)/pgadmin3/i18n
	@pub_tx='$(PUB_TX)'; \
	for DIR in $$pub_tx; do \
		$(mkinstalldirs) $(datadir)/pgadmin3/i18n/$$DIR; \
		for FILE in `ls $$DIR/*.mo`; do \
			$(install_sh) -c -m 644 $$FILE $(datadir)/pgadmin3/i18n/$$DIR/; \
		done \
	done
	$(install_sh) -c -m 644 pgadmin3.lng $(datadir)/pgadmin3/i18n/$$DIR
	$(install_sh) -c -m 644 wxstd.mo $(datadir)/pgadmin3/i18n/$$DIR

