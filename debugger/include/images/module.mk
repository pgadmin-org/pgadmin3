#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 6067 2007-03-15 15:44:55Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - debugger/include/images/ Makefile fragment
#
#######################################################################

debugger_SOURCES += \
	$(srcdir)/include/images/clearAll.xpm \
      	$(srcdir)/include/images/continue.xpm \
	$(srcdir)/include/images/debugger.xpm \
	$(srcdir)/include/images/setBreak.xpm \
	$(srcdir)/include/images/stepInto.xpm \
	$(srcdir)/include/images/stepOver.xpm \
	$(srcdir)/include/images/stop.xpm

EXTRA_DIST += \
        $(srcdir)/include/images/module.mk
