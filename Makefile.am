##############################################################################
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

AUTOMAKE_OPTIONS = foreign

SUBDIRS = xtra/png2c branding i18n plugins.d pgadmin xtra pkg

EXTRA_DIST = \
	$(top_srcdir)/BUGS \
	$(top_srcdir)/CHANGELOG \
	$(top_srcdir)/DEVELOPERS \
	$(top_srcdir)/INSTALL \
	$(top_srcdir)/LICENSE \
	$(top_srcdir)/README \
	$(top_srcdir)/TODO \
	$(top_srcdir)/bootstrap \
	$(top_srcdir)/configure.ac.in \
	$(top_srcdir)/languages.hwx \
	$(top_srcdir)/languages.po \
	$(top_srcdir)/pgadmin3.pot \
	$(top_srcdir)/pgadmin3-release.pot \
	$(top_srcdir)/pgAdmin3.rules \
	$(top_srcdir)/pgAdmin3.sln \
	$(top_srcdir)/stringextract \
	$(top_srcdir)/stringmerge \
        $(top_srcdir)/docs/Docs.vcproj \
        $(top_srcdir)/docs/builddocs.bat

TMP_docs = \
	$(top_srcdir)/docs/en_US/*.css \
	$(top_srcdir)/docs/en_US/*.html \
	$(top_srcdir)/docs/en_US/pgadmin3.hh* \
	$(top_srcdir)/docs/en_US/hints/*.html \
	$(top_srcdir)/docs/en_US/images/*.png \
	$(top_srcdir)/docs/cs_CZ/*.css \
	$(top_srcdir)/docs/cs_CZ/*.html \
	$(top_srcdir)/docs/cs_CZ/pgadmin3.hh* \
	$(top_srcdir)/docs/cs_CZ/hints/*.html \
	$(top_srcdir)/docs/cs_CZ/images/*.png \
	$(top_srcdir)/docs/de_DE/hints/*.html \
	$(top_srcdir)/docs/es_ES/hints/*.html \
	$(top_srcdir)/docs/fi_FI/hints/*.html \
	$(top_srcdir)/docs/fr_FR/*.css \
	$(top_srcdir)/docs/fr_FR/*.html \
	$(top_srcdir)/docs/fr_FR/pgadmin3.hh* \
	$(top_srcdir)/docs/fr_FR/hints/*.html \
	$(top_srcdir)/docs/fr_FR/images/*.png \
	$(top_srcdir)/docs/sl_SI/hints/*.html \
	$(top_srcdir)/docs/zh_CN/hints/*.html \
	$(top_srcdir)/docs/zh_TW/hints/*.html

if !APPBUNDLE
nobase_dist_pkgdata_DATA = $(TMP_docs)
help_cache = $(DESTDIR)/$(pkgdatadir)/docs/en_US/pgadmin3.hhp.cached
else
nobase_dist_data_DATA = $(TMP_docs)
help_cache = $(DESTDIR)/$(datadir)/docs/en_US/pgadmin3.hhp.cached
endif

# We need to ensure the help cache is world writeable
install-data-hook:
	chmod 0666 $(help_cache)

# Perform astyle cleanup per rules:
# * -p - insert space around parenthesis
# * -b - bracket style
# * -S - indent switches
# * -t4 - intent with tabs, 4 of them
# * -k3 - align pointer or reference operators
# * -z2 - Unix style line endings
# Also add the -n to make it not save .orig files (we have git for that)
style:
	astyle -n -p -b -S -t4 -k3 -z2 `find . -name "*.cpp" -o -name "*.h" | egrep -v "(wx-build|xrcDialogs|pgscript/test)"`
