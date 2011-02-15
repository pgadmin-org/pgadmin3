#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/utilities/m_apm/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/utilities/m_apm/mapm5sin.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmasin.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmasn0.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmcbrt.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmcnst.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmfact.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmfmul.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmgues.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmhasn.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmhsin.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmipwr.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmistr.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmpwr2.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmrsin.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmsqrt.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmstck.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmutil.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmutl1.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapmutl2.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_add.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_cpi.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_div.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_exp.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_fam.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_fft.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_flr.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_fpf.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_gcd.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_lg2.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_lg3.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_lg4.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_log.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_mul.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_pow.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_rcp.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_rnd.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_set.cpp \
	$(srcdir)/pgscript/utilities/m_apm/mapm_sin.cpp

EXTRA_DIST += \
	$(srcdir)/pgscript/utilities/m_apm/module.mk

