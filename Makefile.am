##############################################################################
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

AUTOMAKE_OPTIONS = foreign

SUBDIRS = branding i18n plugins.d xtra pgadmin pkg

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
	$(top_srcdir)/config.rpath.in \
	$(top_srcdir)/languages.hwx \
	$(top_srcdir)/languages.po \
	$(top_srcdir)/pgadmin3.pot \
	$(top_srcdir)/pgadmin3-release.pot \
	$(top_srcdir)/pgAdmin3.props \
	$(top_srcdir)/pgAdmin3.targets \
	$(top_srcdir)/pgAdmin3.xml \
	$(top_srcdir)/pgAdmin3.sln \
	$(top_srcdir)/stringextract \
	$(top_srcdir)/stringmerge \
	$(top_srcdir)/docs/Docs.vcxproj \
	$(top_srcdir)/docs/Docs.vcxproj.filters \
	$(top_srcdir)/docs/builddocs.bat \
	$(top_srcdir)/docs/createConf.vbs \
	$(top_srcdir)/docs/en_US/*.rst \
	$(top_srcdir)/docs/en_US/conf.py \
	$(top_srcdir)/docs/en_US/conf.py.in \
	$(top_srcdir)/docs/en_US/make.bat \
	$(top_srcdir)/docs/en_US/Makefile.sphinx \
	$(top_srcdir)/docs/en_US/images/*.png 

TMP_docs = \
	$(top_srcdir)/docs/en_US/*.css \
	$(top_srcdir)/docs/en_US/hints/*.html \
	$(top_srcdir)/docs/cs_CZ/*.css \
	$(top_srcdir)/docs/cs_CZ/hints/*.html \
	$(top_srcdir)/docs/de_DE/*.css \
	$(top_srcdir)/docs/de_DE/hints/*.html \
	$(top_srcdir)/docs/es_ES/*.css \
	$(top_srcdir)/docs/es_ES/hints/*.html \
	$(top_srcdir)/docs/fi_FI/*.css \
	$(top_srcdir)/docs/fi_FI/hints/*.html \
	$(top_srcdir)/docs/fr_FR/*.css \
	$(top_srcdir)/docs/fr_FR/hints/*.html \
	$(top_srcdir)/docs/sl_SI/*.css \
	$(top_srcdir)/docs/sl_SI/hints/*.html \
	$(top_srcdir)/docs/zh_CN/*.css \
	$(top_srcdir)/docs/zh_CN/hints/*.html \
	$(top_srcdir)/docs/zh_TW/*.css \
	$(top_srcdir)/docs/zh_TW/hints/*.html

if !APPBUNDLE
nobase_dist_pkgdata_DATA = $(TMP_docs)
help_dir = $(DESTDIR)/$(pkgdatadir)
else
nobase_dist_data_DATA = $(TMP_docs)
help_dir = $(DESTDIR)/$(datadir)
endif

install-data-hook:
if SPHINX_BUILD
	if [ -d $(top_srcdir)/docs/en_US/_build/htmlhelp ]; then cp -R $(top_srcdir)/docs/en_US/_build/htmlhelp/* $(help_dir)/docs/en_US/; fi

# Fix up the charset so wxWidgets doesn't bleat
	for f in `ls $(help_dir)/docs/en_US/*.html`; do LANG=C sed "s/iso8859_1/iso8859\-1/g" $$f > temp; mv temp $$f; done
endif

# Perform astyle cleanup per rules:
# * -p - insert space around parenthesis
# * --style=allman - bracket style
# * -S - indent switches
# * -t4 - intent with tabs, 4 of them
# * -k3 - align pointer or reference operators
# * -z2 - Unix style line endings
# Also add the -n to make it not save .orig files (we have git for that)
style:
	astyle -n -p --style=allman -S -t4 -k3 -z2 `find . -name "*.cpp" -o -name "*.h" | egrep -v "(wx-build|xrcDialogs|pgadmin/libssh2|pgadmin/include/libssh2)"`

# Create HTML docs
doc:
if SPHINX_BUILD
	cd $(top_srcdir)/docs/en_US && $(MAKE) -f Makefile.sphinx SPHINXBUILD=${SPHINX_BUILD} htmlhelp
endif

xrc:
	$(top_srcdir)/pgadmin/ui/embed-xrc

all: doc xrc

clean-local:
	rm -rf $(top_srcdir)/docs/en_US/_build
