#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/generators/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/pgscript/generators/pgsDateGen.h \
	include/pgscript/generators/pgsDateTimeGen.h \
	include/pgscript/generators/pgsDictionaryGen.h \
	include/pgscript/generators/pgsIntegerGen.h \
	include/pgscript/generators/pgsNumberGen.h \
	include/pgscript/generators/pgsObjectGen.h \
	include/pgscript/generators/pgsRealGen.h \
	include/pgscript/generators/pgsReferenceGen.h \
	include/pgscript/generators/pgsRegexGen.h \
	include/pgscript/generators/pgsStringGen.h \
	include/pgscript/generators/pgsTimeGen.h

EXTRA_DIST += \
	include/pgscript/generators/module.mk

