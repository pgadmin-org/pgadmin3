# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems

EXTRA_DIST = \
	$(top_srcdir)/pkg/mac/Info.plist \
	"$(top_srcdir)/pkg/mac/pgadmin3.icns" \
	$(top_srcdir)/pkg/mac/PkgInfo

if APPBUNDLE
install-exec-hook:
	$(install_sh_DATA) PkgInfo $(bundledir)/PkgInfo
	$(install_sh_DATA) Info.plist $(bundledir)/Contents/Info.plist
	$(install_sh_DATA) "pgadmin3.icns" "$(bundledir)/Contents/Resources/pgAdmin3.icns"
	$$($(WX_CONFIG) --rezflags | sed 's/-t[[:space:]]*APPL//') \
		 "$(bundledir)/Contents/Resources/pgAdmin3.rsrc" -useDF
	./complete-bundle.sh "$(bundledir)"
endif
