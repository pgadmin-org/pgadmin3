#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/objects/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	pgscript/objects/pgsGenerator.cpp \
	pgscript/objects/pgsNumber.cpp \
	pgscript/objects/pgsRecord.cpp \
	pgscript/objects/pgsString.cpp \
	pgscript/objects/pgsVariable.cpp

EXTRA_DIST += \
	pgscript/objects/module.mk

