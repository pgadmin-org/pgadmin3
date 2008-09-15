#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: pgadmin.sh,v 1.6 2008/07/27 16:25:05 pgunittest Exp $
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - pgadmin/include/pgscript/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/location.hh \
	$(srcdir)/include/pgscript/parser.tab.hh \
	$(srcdir)/include/pgscript/position.hh \
	$(srcdir)/include/pgscript/stack.hh \
	$(srcdir)/include/pgscript/FlexLexer.h \
	$(srcdir)/include/pgscript/pgsApplication.h \
	$(srcdir)/include/pgscript/pgScript.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/module.mk

include $(srcdir)/include/pgscript/exceptions/module.mk
include $(srcdir)/include/pgscript/expressions/module.mk
include $(srcdir)/include/pgscript/generators/module.mk
include $(srcdir)/include/pgscript/objects/module.mk
include $(srcdir)/include/pgscript/statements/module.mk
include $(srcdir)/include/pgscript/utilities/module.mk
