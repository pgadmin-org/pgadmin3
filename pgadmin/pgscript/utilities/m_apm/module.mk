#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/utilities/m_apm/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	pgscript/utilities/m_apm/mapm5sin.cpp \
	pgscript/utilities/m_apm/mapmasin.cpp \
	pgscript/utilities/m_apm/mapmasn0.cpp \
	pgscript/utilities/m_apm/mapmcbrt.cpp \
	pgscript/utilities/m_apm/mapmcnst.cpp \
	pgscript/utilities/m_apm/mapmfact.cpp \
	pgscript/utilities/m_apm/mapmfmul.cpp \
	pgscript/utilities/m_apm/mapmgues.cpp \
	pgscript/utilities/m_apm/mapmhasn.cpp \
	pgscript/utilities/m_apm/mapmhsin.cpp \
	pgscript/utilities/m_apm/mapmipwr.cpp \
	pgscript/utilities/m_apm/mapmistr.cpp \
	pgscript/utilities/m_apm/mapmpwr2.cpp \
	pgscript/utilities/m_apm/mapmrsin.cpp \
	pgscript/utilities/m_apm/mapmsqrt.cpp \
	pgscript/utilities/m_apm/mapmstck.cpp \
	pgscript/utilities/m_apm/mapmutil.cpp \
	pgscript/utilities/m_apm/mapmutl1.cpp \
	pgscript/utilities/m_apm/mapmutl2.cpp \
	pgscript/utilities/m_apm/mapm_add.cpp \
	pgscript/utilities/m_apm/mapm_cpi.cpp \
	pgscript/utilities/m_apm/mapm_div.cpp \
	pgscript/utilities/m_apm/mapm_exp.cpp \
	pgscript/utilities/m_apm/mapm_fam.cpp \
	pgscript/utilities/m_apm/mapm_fft.cpp \
	pgscript/utilities/m_apm/mapm_flr.cpp \
	pgscript/utilities/m_apm/mapm_fpf.cpp \
	pgscript/utilities/m_apm/mapm_gcd.cpp \
	pgscript/utilities/m_apm/mapm_lg2.cpp \
	pgscript/utilities/m_apm/mapm_lg3.cpp \
	pgscript/utilities/m_apm/mapm_lg4.cpp \
	pgscript/utilities/m_apm/mapm_log.cpp \
	pgscript/utilities/m_apm/mapm_mul.cpp \
	pgscript/utilities/m_apm/mapm_pow.cpp \
	pgscript/utilities/m_apm/mapm_rcp.cpp \
	pgscript/utilities/m_apm/mapm_rnd.cpp \
	pgscript/utilities/m_apm/mapm_set.cpp \
	pgscript/utilities/m_apm/mapm_sin.cpp

EXTRA_DIST += \
	pgscript/utilities/m_apm/module.mk

