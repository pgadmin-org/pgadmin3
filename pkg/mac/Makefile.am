##############################################################################
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 4875 2006-01-06 21:06:46Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

EXTRA_DIST = \
		$(srcdir)/complete-bundle.sh \
		$(srcdir)/debug-bundle.sh \
		$(srcdir)/Info.plist \
		$(srcdir)/Info.plist.in \
		$(srcdir)/Info-Debug.plist \
		$(srcdir)/Info-Debug.plist.in \
		$(srcdir)/pgadmin3.icns \
		$(srcdir)/sql.icns \
		$(srcdir)/PkgInfo \
		$(srcdir)/installer/installer.pmproj \
		$(srcdir)/installer/intro.rtf \
		$(srcdir)/installer/licence.rtf \
		$(srcdir)/installer/logo.jpg

install-exec-hook:
if APPBUNDLE
	$(install_sh_DATA) PkgInfo $(bundledir)/Contents/PkgInfo
	$(install_sh_DATA) Info.plist $(bundledir)/Contents/Info.plist
	$(install_sh_DATA) "pgadmin3.icns" "$(bundledir)/Contents/Resources/pgAdmin3.icns"
	$(install_sh_DATA) "sql.icns" "$(bundledir)/Contents/Resources/sql.icns"
	$$($(WX_CONFIG) --rescomp | sed 's/-t[[:space:]]*APPL//') \
		 "$(bundledir)/Contents/Resources/pgAdmin3.rsrc" -useDF
	$(install_sh_PROGRAM) "$$($(PG_CONFIG) --bindir)/pg_dump" "$(bundledir)/Contents/SharedSupport/helper/pg_dump"
	$(install_sh_PROGRAM) "$$($(PG_CONFIG) --bindir)/pg_dump" "$(bundledir)/Contents/SharedSupport/helper/pg_dumpall"
	$(install_sh_PROGRAM) "$$($(PG_CONFIG) --bindir)/pg_restore" "$(bundledir)/Contents/SharedSupport/helper/pg_restore"
	./complete-bundle.sh "$(bundledir)"
endif
