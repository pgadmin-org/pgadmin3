#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/generators/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	pgscript/generators/pgsDateGen.cpp \
	pgscript/generators/pgsDateTimeGen.cpp \
	pgscript/generators/pgsDictionaryGen.cpp \
	pgscript/generators/pgsIntegerGen.cpp \
	pgscript/generators/pgsNumberGen.cpp \
	pgscript/generators/pgsObjectGen.cpp \
	pgscript/generators/pgsRealGen.cpp \
	pgscript/generators/pgsReferenceGen.cpp \
	pgscript/generators/pgsRegexGen.cpp \
	pgscript/generators/pgsStringGen.cpp \
	pgscript/generators/pgsTimeGen.cpp

EXTRA_DIST += \
	pgscript/generators/module.mk

