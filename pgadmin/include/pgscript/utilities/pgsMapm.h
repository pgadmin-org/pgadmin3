//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSMAPMLIB_H_
#define PGSMAPMLIB_H_

#include "pgscript/pgScript.h"
#include "mapm-lib/m_apm.h"

WX_DECLARE_OBJARRAY(MAPM, pgsVectorMapm);

class pgsMapm
{

public:

	static wxString pgs_mapm_str(const MAPM &m, const bool &as_int = false);

	static MAPM pgs_mapm_round(const MAPM &m);

	static wxString pgs_mapm_str_fixed(const MAPM &m);

	static wxString pgs_mapm_str_float(const MAPM &m);

	static MAPM pgs_str_mapm(const wxString &s);

};

#endif /*PGSMAPMLIB_H_*/
