#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

EXTRA_DIST = \
$(srcdir)/af_ZA/pgadmin3.po \
$(srcdir)/ar_SA/pgadmin3.po \
$(srcdir)/ca_ES/pgadmin3.po \
$(srcdir)/da_DK/pgadmin3.po \
$(srcdir)/de_DE/pgadmin3.po \
$(srcdir)/es_ES/pgadmin3.po \
$(srcdir)/fi_FI/pgadmin3.po \
$(srcdir)/fr_FR/pgadmin3.po \
$(srcdir)/hu_HU/pgadmin3.po \
$(srcdir)/id_ID/pgadmin3.po \
$(srcdir)/it_IT/pgadmin3.po \
$(srcdir)/ja_JP/pgadmin3.po \
$(srcdir)/lv_LV/pgadmin3.po \
$(srcdir)/nl_NL/pgadmin3.po \
$(srcdir)/pl_PL/pgadmin3.po \
$(srcdir)/pt_PT/pgadmin3.po \
$(srcdir)/ru_RU/pgadmin3.po \
$(srcdir)/sk_SK/pgadmin3.po \
$(srcdir)/sl_SI/pgadmin3.po \
$(srcdir)/sr_YU/pgadmin3.po \
$(srcdir)/tr_TR/pgadmin3.po \
$(srcdir)/zh_CN/pgadmin3.po \
$(srcdir)/zh_TW/pgadmin3.po

##############################################################################
#
# Note, Only include translations above and below that are officially 
# published. These are listed at: 
#                              http://www.pgadmin.org/pgadmin3/translation.php
#
##############################################################################

TMP_ui = \
$(srcdir)/wxstd.mo \
$(srcdir)/pgadmin3.lng \
$(srcdir)/pg_settings.csv 
$(srcdir)/af_ZA/pgadmin3.mo \
$(srcdir)/ar_SA/pgadmin3.mo \
$(srcdir)/ca_ES/pgadmin3.mo \
$(srcdir)/da_DK/pgadmin3.mo \
$(srcdir)/de_DE/pgadmin3.mo \
$(srcdir)/es_ES/pgadmin3.mo \
$(srcdir)/fi_FI/pgadmin3.mo \
$(srcdir)/fr_FR/pgadmin3.mo \
$(srcdir)/hu_HU/pgadmin3.mo \
$(srcdir)/id_ID/pgadmin3.mo \
$(srcdir)/it_IT/pgadmin3.mo \
$(srcdir)/ja_JP/pgadmin3.mo \
$(srcdir)/lv_LV/pgadmin3.mo \
$(srcdir)/nl_NL/pgadmin3.mo \
$(srcdir)/pl_PL/pgadmin3.mo \
$(srcdir)/pt_PT/pgadmin3.mo \
$(srcdir)/ru_RU/pgadmin3.mo \
$(srcdir)/sk_SK/pgadmin3.mo \
$(srcdir)/sl_SI/pgadmin3.mo \
$(srcdir)/sr_YU/pgadmin3.mo \
$(srcdir)/tr_TR/pgadmin3.mo \
$(srcdir)/zh_CN/pgadmin3.mo \
$(srcdir)/zh_TW/pgadmin3.mo \
$(srcdir)/af_ZA/wxstd.mo \
$(srcdir)/ca_ES/wxstd.mo \
$(srcdir)/da_DK/wxstd.mo \
$(srcdir)/de_DE/wxstd.mo \
$(srcdir)/es_ES/wxstd.mo \
$(srcdir)/fi_FI/wxstd.mo \
$(srcdir)/fr_FR/wxstd.mo \
$(srcdir)/hu_HU/wxstd.mo \
$(srcdir)/id_ID/wxstd.mo \
$(srcdir)/it_IT/wxstd.mo \
$(srcdir)/ja_JP/wxstd.mo \
$(srcdir)/nl_NL/wxstd.mo \
$(srcdir)/pl_PL/wxstd.mo \
$(srcdir)/ru_RU/wxstd.mo \
$(srcdir)/sl_SI/wxstd.mo \
$(srcdir)/tr_TR/wxstd.mo \
$(srcdir)/zh_CN/wxstd.mo \
$(srcdir)/zh_TW/wxstd.mo

install-data-local:
	for DIR in `ls -d ??_??`; do \
		$(mkinstalldirs) $(datadir)/ui/$$DIR; \
		for FILE in `ls $$DIR/*.mo`; do \
			$(install_sh) -c -m 644 $$FILE $(datadir)/ui/$$DIR; \
		done \
	done
	$(install_sh) -c -m 644 pgadmin3.lng $(datadir)/ui/$$DIR
	$(install_sh) -c -m 644 wxstd.mo $(datadir)/ui/$$DIR

