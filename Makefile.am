##############################################################################
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5015 2006-02-21 15:03:14Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

AUTOMAKE_OPTIONS = foreign

SUBDIRS = branding i18n pgadmin xtra pkg

EXTRA_DIST = \
	$(top_srcdir)/BUGS \
	$(top_srcdir)/CHANGELOG \
	$(top_srcdir)/DEVELOPERS \
	$(top_srcdir)/LICENSE \
	$(top_srcdir)/README \
	$(top_srcdir)/TODO \
	$(top_srcdir)/bootstrap \
	$(top_srcdir)/configure.ac.in \
	$(top_srcdir)/languages.hwx \
	$(top_srcdir)/languages.po \
	$(top_srcdir)/pgadmin3.pot \
	$(top_srcdir)/pgadmin3-release.pot \
	$(top_srcdir)/pgAdmin3.sln \
	$(top_srcdir)/stringextract \
	$(top_srcdir)/stringmerge

TMP_docs_local = \
	$(top_srcdir)/docs/en_US/hints/*.html \
	$(top_srcdir)/docs/en_US/images/*.png \
	$(top_srcdir)/docs/en_US/*.html \
	$(top_srcdir)/docs/en_US/*.css \
	$(top_srcdir)/docs/en_US/pgadmin3.hh* \
	$(top_srcdir)/docs/en_US/tips.txt \
	$(top_srcdir)/docs/de_DE/tips.txt \
	$(top_srcdir)/docs/de_DE/hints/*.html \
	$(top_srcdir)/docs/es_ES/tips.txt \
	$(top_srcdir)/docs/es_ES/hints/*.html \
	$(top_srcdir)/docs/fr_FR/*.html \
	$(top_srcdir)/docs/fr_FR/tips.txt \
	$(top_srcdir)/docs/fr_FR/hints/*.html \
	$(top_srcdir)/docs/sl_SI/tips.txt \
	$(top_srcdir)/docs/sl_SI/hints/*.html \
	$(top_srcdir)/docs/zh_CN/tips.txt \
	$(top_srcdir)/docs/zh_CN/hints/*.html \
	$(top_srcdir)/docs/zh_TW/tips.txt \
	$(top_srcdir)/docs/zh_TW/hints/*.html

TMP_docs_external = \
	$(top_srcdir)/docs/en_US/pg/*.css \
	$(top_srcdir)/docs/en_US/pg/*.html \
	$(top_srcdir)/docs/en_US/slony/*.css \
	$(top_srcdir)/docs/en_US/slony/*.html

if INSTALL_DOCS
TMP_docs = \
	$(TMP_docs_local) \
	$(TMP_docs_external)
else
TMP_docs =
endif

if !APPBUNDLE
nobase_dist_pkgdata_DATA = $(TMP_docs)
else
nobase_dist_data_DATA = $(TMP_docs)
endif
