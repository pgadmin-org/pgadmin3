#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/exceptions/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/exceptions/pgsArithmeticException.h \
	$(srcdir)/include/pgscript/exceptions/pgsAssertException.h \
	$(srcdir)/include/pgscript/exceptions/pgsBreakException.h \
	$(srcdir)/include/pgscript/exceptions/pgsCastException.h \
	$(srcdir)/include/pgscript/exceptions/pgsContinueException.h \
	$(srcdir)/include/pgscript/exceptions/pgsException.h \
	$(srcdir)/include/pgscript/exceptions/pgsInterruptException.h \
	$(srcdir)/include/pgscript/exceptions/pgsParameterException.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/exceptions/module.mk

