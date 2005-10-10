##############################################################################
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

SUBDIRS = i18n src pkg xtra

EXTRA_DIST = \
	$(top_srcdir)/README.txt \
	$(top_srcdir)/BUGS.txt \
	$(top_srcdir)/LICENCE.txt

TMP_docs = \
	$(top_srcdir)/docs/en_US/pg/*.html \
	$(top_srcdir)/docs/en_US/hints/*.html \
	$(top_srcdir)/docs/de_DE/hints/*.html \
	$(top_srcdir)/docs/en_US/pg/*.css \
	$(top_srcdir)/docs/en_US/images/*.png \
	$(top_srcdir)/docs/en_US/*.html \
	$(top_srcdir)/docs/en_US/*.css \
	$(top_srcdir)/docs/en_US/pgadmin3.hh* \
	$(top_srcdir)/docs/en_US/tips.txt

if !APPBUNDLE
nobase_dist_pkgdata_DATA = $(TMP_docs)
else
nobase_dist_data_DATA = $(TMP_docs)
endif
