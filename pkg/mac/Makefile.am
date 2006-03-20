##############################################################################
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

EXTRA_DIST = \
		$(srcdir)/complete-bundle.sh \
		$(srcdir)/debug-bundle.sh \
		$(srcdir)/Info.plist \
		$(srcdir)/Info-Debug.plist \
		$(srcdir)/pgadmin3.icns \
		$(srcdir)/sql.icns \
		$(srcdir)/PkgInfo

install-exec-hook:
if APPBUNDLE
	$(install_sh_DATA) PkgInfo $(bundledir)/Contents/PkgInfo
	$(install_sh_DATA) Info.plist $(bundledir)/Contents/Info.plist
	$(install_sh_DATA) "pgadmin3.icns" "$(bundledir)/Contents/Resources/pgAdmin3.icns"
	$(install_sh_DATA) "sql.icns" "$(bundledir)/Contents/Resources/sql.icns"
	$$($(WX_CONFIG) --rezflags | sed 's/-t[[:space:]]*APPL//') \
		 "$(bundledir)/Contents/Resources/pgAdmin3.rsrc" -useDF
	$(install_sh_PROGRAM) "$(LIBPQ_HOME)/bin/pg_dump" "$(bundledir)/Contents/SharedSupport/helper/pg_dump"
	$(install_sh_PROGRAM) "$(LIBPQ_HOME)/bin/pg_restore" "$(bundledir)/Contents/SharedSupport/helper/pg_restore"
	./complete-bundle.sh "$(bundledir)"
endif
