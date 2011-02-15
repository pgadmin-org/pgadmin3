#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/generators/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/generators/pgsDateGen.h \
	$(srcdir)/include/pgscript/generators/pgsDateTimeGen.h \
	$(srcdir)/include/pgscript/generators/pgsDictionaryGen.h \
	$(srcdir)/include/pgscript/generators/pgsIntegerGen.h \
	$(srcdir)/include/pgscript/generators/pgsNumberGen.h \
	$(srcdir)/include/pgscript/generators/pgsObjectGen.h \
	$(srcdir)/include/pgscript/generators/pgsRealGen.h \
	$(srcdir)/include/pgscript/generators/pgsReferenceGen.h \
	$(srcdir)/include/pgscript/generators/pgsRegexGen.h \
	$(srcdir)/include/pgscript/generators/pgsStringGen.h \
	$(srcdir)/include/pgscript/generators/pgsTimeGen.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/generators/module.mk

