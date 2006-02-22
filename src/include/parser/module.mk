#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5019 2006-02-21 15:29:07Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/parser/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/parser/keywords.h \
	$(srcdir)/include/parser/parse.h

EXTRA_DIST += \
        $(srcdir)/include/parser/module.mk
