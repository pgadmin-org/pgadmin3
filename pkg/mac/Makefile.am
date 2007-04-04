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
		$(srcdir)/create-dmg.sh \
		$(srcdir)/debug-bundle.sh \
		$(srcdir)/debugger.Info.plist \
		$(srcdir)/debugger.Info.plist.in \
		$(srcdir)/pgadmin.Info.plist \
		$(srcdir)/pgadmin.Info.plist.in \
		$(srcdir)/debug.debugger.Info.plist \
		$(srcdir)/debug.debugger.Info.plist.in \
		$(srcdir)/debug.pgadmin.Info.plist \
		$(srcdir)/debug.pgadmin.Info.plist.in \
		$(srcdir)/licence.r \
		$(srcdir)/pgadmin3.icns \
		$(srcdir)/sql.icns \
		$(srcdir)/PkgInfo 

install-exec-hook:
if APPBUNDLE
	$(install_sh_DATA) PkgInfo $(bundledir)/Contents/PkgInfo
	$(install_sh_DATA) pgadmin.Info.plist $(bundledir)/Contents/Info.plist
	$(install_sh_DATA) "pgadmin3.icns" "$(bundledir)/Contents/Resources/pgAdmin3.icns"
	$(install_sh_DATA) "sql.icns" "$(bundledir)/Contents/Resources/sql.icns"
	$$($(WX_CONFIG) --rescomp | sed 's/-t[[:space:]]*APPL//') \
		 "$(bundledir)/Contents/Resources/pgAdmin3.rsrc" -useDF
	$(install_sh_DATA) PkgInfo $(bundledir)/Contents/Resources/Debugger.app/Contents/PkgInfo
	$(install_sh_DATA) debugger.Info.plist $(bundledir)/Contents/Resources/Debugger.app/Contents/Info.plist
	$(install_sh_DATA) "debugger.icns" "$(bundledir)/Contents/Resources/Debugger.app/Contents/Resources/Debugger.icns"
#	$$($(WX_CONFIG) --rescomp | sed 's/-t[[:space:]]*APPL//') \
#		 "$(bundledir)/Contents/Resources/Debugger.app/Contents/Resources/Debugger.rsrc" -useDF
	$(install_sh_PROGRAM) "$$($(PG_CONFIG) --bindir)/pg_dump" "$(bundledir)/Contents/SharedSupport/helper/pg_dump"
	$(install_sh_PROGRAM) "$$($(PG_CONFIG) --bindir)/pg_dumpall" "$(bundledir)/Contents/SharedSupport/helper/pg_dumpall"
	$(install_sh_PROGRAM) "$$($(PG_CONFIG) --bindir)/pg_restore" "$(bundledir)/Contents/SharedSupport/helper/pg_restore"
	./complete-bundle.sh "$(bundledir)"
endif
