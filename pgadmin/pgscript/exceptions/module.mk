#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/exceptions/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/exceptions/pgsArithmeticException.cpp \
	$(srcdir)/pgscript/exceptions/pgsAssertException.cpp \
	$(srcdir)/pgscript/exceptions/pgsBreakException.cpp \
	$(srcdir)/pgscript/exceptions/pgsCastException.cpp \
	$(srcdir)/pgscript/exceptions/pgsContinueException.cpp \
	$(srcdir)/pgscript/exceptions/pgsException.cpp \
	$(srcdir)/pgscript/exceptions/pgsInterruptException.cpp \
	$(srcdir)/pgscript/exceptions/pgsParameterException.cpp

EXTRA_DIST += \
	$(srcdir)/pgscript/exceptions/module.mk

