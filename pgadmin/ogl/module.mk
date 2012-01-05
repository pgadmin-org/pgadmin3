#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
<<<<<<< HEAD
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
=======
# Copyright (C) 2002 - 2012, The pgAdmin Development Team
>>>>>>> 573b7fd... Update copyright notices for 2012.
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/ogl/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/ogl/basic.cpp \
	$(srcdir)/ogl/bmpshape.cpp \
	$(srcdir)/ogl/composit.cpp \
	$(srcdir)/ogl/divided.cpp \
	$(srcdir)/ogl/lines.cpp \
	$(srcdir)/ogl/oglmisc.cpp \
	$(srcdir)/ogl/basic2.cpp \
	$(srcdir)/ogl/canvas.cpp \
	$(srcdir)/ogl/constrnt.cpp \
	$(srcdir)/ogl/drawn.cpp \
	$(srcdir)/ogl/mfutils.cpp \
	$(srcdir)/ogl/ogldiag.cpp

EXTRA_DIST += \
	$(srcdir)/ogl/module.mk \
	$(srcdir)/ogl/README


