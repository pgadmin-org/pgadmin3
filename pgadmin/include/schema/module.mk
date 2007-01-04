#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 5466 2006-10-12 09:31:39Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/schema/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	  $(srcdir)/include/schema/pgAggregate.h \
	  $(srcdir)/include/schema/pgCast.h \
	  $(srcdir)/include/schema/pgCheck.h \
	  $(srcdir)/include/schema/pgCollection.h \
	  $(srcdir)/include/schema/pgColumn.h \
	  $(srcdir)/include/schema/pgConstraints.h \
	  $(srcdir)/include/schema/pgConversion.h \
	  $(srcdir)/include/schema/pgDatabase.h \
	  $(srcdir)/include/schema/pgDatatype.h \
	  $(srcdir)/include/schema/pgDefs.h \
	  $(srcdir)/include/schema/pgDomain.h \
	  $(srcdir)/include/schema/pgForeignKey.h \
	  $(srcdir)/include/schema/pgFunction.h \
	  $(srcdir)/include/schema/pgGroup.h \
	  $(srcdir)/include/schema/pgIndex.h \
	  $(srcdir)/include/schema/pgIndexConstraint.h \
	  $(srcdir)/include/schema/pgLanguage.h \
	  $(srcdir)/include/schema/pgObject.h \
	  $(srcdir)/include/schema/pgOperator.h \
	  $(srcdir)/include/schema/pgOperatorClass.h \
	  $(srcdir)/include/schema/pgQueryThread.h \
	  $(srcdir)/include/schema/pgRole.h \
	  $(srcdir)/include/schema/pgRule.h \
	  $(srcdir)/include/schema/pgSchema.h \
	  $(srcdir)/include/schema/pgSequence.h \
	  $(srcdir)/include/schema/pgServer.h \
	  $(srcdir)/include/schema/pgTable.h \
	  $(srcdir)/include/schema/pgTablespace.h \
	  $(srcdir)/include/schema/pgTrigger.h \
	  $(srcdir)/include/schema/pgType.h \
	  $(srcdir)/include/schema/pgUser.h \
	  $(srcdir)/include/schema/pgView.h

EXTRA_DIST += \
    $(srcdir)/include/schema/module.mk

