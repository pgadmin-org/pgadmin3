#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	pgscript/pgsApplication.cpp \
	pgscript/lex.pgs.cc \
	pgscript/parser.tab.cc

EXTRA_DIST += \
	pgscript/module.mk \
	pgscript/parser.sh \
	pgscript/pgsParser.yy \
	pgscript/pgsScanner.ll \
	pgscript/README

include pgscript/exceptions/module.mk
include pgscript/expressions/module.mk
include pgscript/generators/module.mk
include pgscript/objects/module.mk
include pgscript/statements/module.mk
include pgscript/utilities/module.mk
