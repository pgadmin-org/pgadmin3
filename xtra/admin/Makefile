subdir = contrib/admin
top_builddir = ../..
include $(top_builddir)/src/Makefile.global

MODULE_big = admin
PG_CPPFLAGS = -I$(libpq_srcdir)
DATA_built = admin.sql
DOCS = README.admin
OBJS = genfile.o misc.o

include $(top_srcdir)/contrib/contrib-global.mk
