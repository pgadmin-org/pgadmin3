#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/objects/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/pgscript/objects/pgsGenerator.h \
	include/pgscript/objects/pgsNumber.h \
	include/pgscript/objects/pgsObjects.h \
	include/pgscript/objects/pgsRecord.h \
	include/pgscript/objects/pgsString.h \
	include/pgscript/objects/pgsVariable.h

EXTRA_DIST += \
	include/pgscript/objects/module.mk

