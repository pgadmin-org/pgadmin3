# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
SUBDIRS = src pkg
EXTRA_DIST = \
		$(top_srcdir)/README.txt \
		$(top_srcdir)/BUGS.txt \
		$(top_srcdir)/LICENCE.txt \
		$(top_srcdir)/pkg/pgadmin3.desktop \
		$(top_srcdir)/pkg/debian/rules \
		$(top_srcdir)/pkg/debian/README.Debian \
		$(top_srcdir)/pkg/debian/docs \
		$(top_srcdir)/pkg/debian/dirs \
		$(top_srcdir)/pkg/debian/copyright \
		$(top_srcdir)/pkg/debian/control \
		$(top_srcdir)/pkg/debian/changelog \
		$(top_srcdir)/pkg/debian/make-deb \
		$(top_srcdir)/pkg/debian/pgadmin3.menu \
		$(top_srcdir)/pkg/fc1/pgadmin3.spec \
		$(top_srcdir)/pkg/mandrake/pgadmin3.spec \
		$(top_srcdir)/pkg/redhat/pgadmin3.spec \
		$(top_srcdir)/pkg/slackware/build-release \
		$(top_srcdir)/pkg/slackware/build-snapshot \
		$(top_srcdir)/pkg/src/build-tarball \
		$(top_srcdir)/pkg/win32/screen.bmp \
		$(top_srcdir)/pkg/win32/banner.bmp \
		$(top_srcdir)/pkg/win32/licence.rtf \
		$(top_srcdir)/pkg/win32/pgadmin3.wsi

TMP_docs = \
		$(top_srcdir)/docs/en_US/pg/*.html \
		$(top_srcdir)/docs/en_US/pg/*.css \
		$(top_srcdir)/docs/en_US/images/*.png $(top_srcdir)/docs/en_US/*.html \
		$(top_srcdir)/docs/en_US/*.css $(top_srcdir)/docs/en_US/pgadmin3.hh* \
		$(top_srcdir)/docs/en_US/tips.txt

if !APPBUNDLE
nobase_dist_pkgdata_DATA = $(TMP_docs)
else
nobase_dist_data_DATA = $(TMP_docs)
endif
