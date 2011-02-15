#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/pgsApplication.cpp \
	$(srcdir)/pgscript/lex.pgs.cc \
	$(srcdir)/pgscript/parser.tab.cc

EXTRA_DIST += \
	$(srcdir)/pgscript/module.mk \
	$(srcdir)/pgscript/parser.sh \
	$(srcdir)/pgscript/pgsParser.yy \
	$(srcdir)/pgscript/pgsScanner.ll \
	$(srcdir)/pgscript/README

include $(srcdir)/pgscript/exceptions/module.mk
include $(srcdir)/pgscript/expressions/module.mk
include $(srcdir)/pgscript/generators/module.mk
include $(srcdir)/pgscript/objects/module.mk
include $(srcdir)/pgscript/statements/module.mk
include $(srcdir)/pgscript/utilities/module.mk
