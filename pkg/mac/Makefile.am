##############################################################################
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

EXTRA_DIST = \
		$(srcdir)/Info.plist \
		$(srcdir)/pgadmin3.icns \
		$(srcdir)/sql.icns \
		$(srcdir)/PkgInfo

install-exec-hook:
if APPBUNDLE
	$(install_sh_DATA) PkgInfo $(bundledir)/PkgInfo
	$(install_sh_DATA) Info.plist $(bundledir)/Contents/Info.plist
	$(install_sh_DATA) "pgadmin3.icns" "$(bundledir)/Contents/Resources/pgAdmin3.icns"
	$(install_sh_DATA) "sql.icns" "$(bundledir)/Contents/Resources/sql.icns"
	$$($(WX_CONFIG) --rezflags | sed 's/-t[[:space:]]*APPL//') \
		 "$(bundledir)/Contents/Resources/pgAdmin3.rsrc" -useDF
	./complete-bundle.sh "$(bundledir)"
endif
