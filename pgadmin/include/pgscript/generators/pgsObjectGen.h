//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSOBJECTGEN_H_
#define PGSOBJECTGEN_H_

#include "pgscript/pgScript.h"
#include <wx/datetime.h>

class pgsObjectGen
{

protected:

	long m_seed;

	pgsObjectGen(const long &seed = wxDateTime::GetTimeNow());

	/* pgsObjectGen & operator =(const pgsObjectGen & that); */

	/* pgsObjectGen(const pgsObjectGen & that); */

public:

	virtual ~pgsObjectGen();

	virtual wxString random() = 0;

	virtual pgsObjectGen *clone() = 0;

};

#endif /*PGSOBJECTGEN_H_*/
