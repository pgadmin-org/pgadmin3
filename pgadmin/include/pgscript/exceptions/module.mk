#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: pgadmin.sh,v 1.6 2008/07/27 16:25:05 pgunittest Exp $
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
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

