#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 5466 2006-10-12 09:31:39Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/dlg/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dlg/dlgAddFavourite.h \
	$(srcdir)/include/dlg/dlgAggregate.h \
	$(srcdir)/include/dlg/dlgCast.h \
	$(srcdir)/include/dlg/dlgCheck.h \
	$(srcdir)/include/dlg/dlgClasses.h \
	$(srcdir)/include/dlg/dlgColumn.h \
	$(srcdir)/include/dlg/dlgConnect.h \
	$(srcdir)/include/dlg/dlgConversion.h \
	$(srcdir)/include/dlg/dlgDatabase.h \
	$(srcdir)/include/dlg/dlgDomain.h \
	$(srcdir)/include/dlg/dlgEditGridOptions.h \
	$(srcdir)/include/dlg/dlgFindReplace.h \
	$(srcdir)/include/dlg/dlgForeignKey.h \
	$(srcdir)/include/dlg/dlgFunction.h \
	$(srcdir)/include/dlg/dlgGroup.h \
	$(srcdir)/include/dlg/dlgHbaConfig.h \
	$(srcdir)/include/dlg/dlgIndex.h \
	$(srcdir)/include/dlg/dlgIndexConstraint.h \
	$(srcdir)/include/dlg/dlgLanguage.h \
	$(srcdir)/include/dlg/dlgMainConfig.h \
	$(srcdir)/include/dlg/dlgManageFavourites.h \
	$(srcdir)/include/dlg/dlgOperator.h \
	$(srcdir)/include/dlg/dlgPgpassConfig.h \
	$(srcdir)/include/dlg/dlgProperty.h \
	$(srcdir)/include/dlg/dlgRole.h \
	$(srcdir)/include/dlg/dlgRule.h \
	$(srcdir)/include/dlg/dlgSchema.h \
	$(srcdir)/include/dlg/dlgSelectConnection.h \
	$(srcdir)/include/dlg/dlgSequence.h \
	$(srcdir)/include/dlg/dlgServer.h \
	$(srcdir)/include/dlg/dlgTable.h \
	$(srcdir)/include/dlg/dlgTablespace.h \
	$(srcdir)/include/dlg/dlgTrigger.h \
	$(srcdir)/include/dlg/dlgType.h \
	$(srcdir)/include/dlg/dlgUser.h \
	$(srcdir)/include/dlg/dlgView.h

EXTRA_DIST += \
        $(srcdir)/include/dlg/module.mk

