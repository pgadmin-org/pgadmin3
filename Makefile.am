# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
SUBDIRS = src
EXTRA_DIST = \
		$(top_srcdir)/README.txt \
		$(top_srcdir)/BUGS.txt \
		$(top_srcdir)/LICENCE.txt \
		$(top_srcdir)/pkg/pgadmin3.desktop \
		$(top_srcdir)/pkg/debian/rules \
		$(top_srcdir)/pkg/debian/README.Debian \
		$(top_srcdir)/pkg/debian/docs \
		$(top_srcdir)/pkg/debian/dirs \
		$(top_srcdir)/pkg/debian/copyright 
		$(top_srcdir)/pkg/debian/control \
		$(top_srcdir)/pkg/debian/changelog 
		$(top_srcdir)/pkg/debian/make-deb \
		$(top_srcdir)/pkg/debian/pgadmin3.menu \
		$(top_srcdir)/pkg/fc1/pgadmin3.spec \
		$(top_srcdir)/pkg/mac/Info.plist \
		$(top_srcdir)/pkg/mac/PgAdminIII.icns \
		$(top_srcdir)/pkg/mac/PkgInfo \
		$(top_srcdir)/pkg/mandrake/pgadmin3.spec \
		$(top_srcdir)/pkg/redhat/pgadmin3.spec \
		$(top_srcdir)/pkg/slackware/build-release \
		$(top_srcdir)/pkg/slackware/build-snapshot \
		$(top_srcdir)/pkg/src/build-tarball \
		$(top_srcdir)/pkg/win32/screen.bmp \
		$(top_srcdir)/pkg/win32/banner.bmp \
		$(top_srcdir)/pkg/win32/licence.rtf \
		$(top_srcdir)/pkg/win32/pgadmin3.wsi

nobase_dist_pkgdata_DATA = \
		$(top_srcdir)/docs/en_US/pg/*.html \
		$(top_srcdir)/docs/en_US/pg/*.css \
		$(top_srcdir)/docs/en_US/images/*.png $(top_srcdir)/docs/en_US/*.html \
		$(top_srcdir)/docs/en_US/*.css $(top_srcdir)/docs/en_US/pgadmin3.hh* \
		$(top_srcdir)/docs/en_US/tips.txt

PgAdminIII.app: 
		@echo "Building Bundle PgAdminIII.app"
		@test -d PgAdminIII.app || $(mkinstalldirs) PgAdminIII.app
		@test -d PgAdminIII.app/Contents/MacOS || $(mkinstalldirs) PgAdminIII.app/Contents/MacOS
		@test -d PgAdminIII.app/Contents/Resources || $(mkinstalldirs) PgAdminIII.app/Contents/Resources
		@test -d PgAdminIII.app/Contents/SharedSupport || $(mkinstalldirs) PgAdminIII.app/Contents/SharedSupport
		@test -d PgAdminIII.app/Contents/Frameworks || $(mkinstalldirs) PgAdminIII.app/Contents/Frameworks
		@cp pkg/mac/PkgInfo PgAdminIII.app
		@cp pkg/mac/Info.plist PgAdminIII.app/Contents
		@cp pkg/mac/PgAdminIII.icns PgAdminIII.app/Contents/Resources
		@$$(@WX_CONFIG@ --rezflags | sed 's/-t[[:space:]]*APPL//') \
			PgAdminIII.app/Contents/Resources/PgAdminIII.rsrc \
			-useDF
		@cp -r $(pkgdatadir)/* PgAdminIII.app/Contents/SharedSupport
		@cp -r $(bindir)/pgadmin3 PgAdminIII.app/Contents/MacOS/PgAdminIII
		@echo "Adding all non-standard shared libraries to PgAdminIII.app" ; \
		cd PgAdminIII.app/Contents ; \
		todo=MacOS/PgAdminIII ; \
		while test "$$todo" != ""; do \
			todo_old=$$todo ; \
			todo="" ; \
			for todo_obj in $$todo_old; do \
				for lib in $$( \
					otool -L $$todo_obj | \
					sed -n 's|^.*[[:space:]]\([^[:space:]]*\.dylib\).*$$|\1|p' | \
					egrep -v '^(/usr/lib)|(/System)' \
				); do \
					lib_bn="$$(basename "$$lib")" ;\
					if ! test -f "Frameworks/$$lib_bn"; then \
						cp "$$lib" "Frameworks/$$lib_bn" ; \
						install_name_tool \
							-id "@executable_path/../Frameworks/$$lib_bn" \
							"Frameworks/$$lib_bn" ; \
						todo="$$todo Frameworks/$$lib_bn" ; \
					fi ; \
					install_name_tool -change \
						"$$lib" \
						"@executable_path/../Frameworks/$$lib_bn" \
						"$$todo_obj" ; \
				done ; \
			done ; \
		done ;
