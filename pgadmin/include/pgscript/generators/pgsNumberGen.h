//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSNUMBERGEN_H_
#define PGSNUMBERGEN_H_

#include "pgscript/pgScript.h"
#include "pgscript/utilities/pgsMapm.h"

class pgsNumberGen
{

protected:

	MAPM m_range;

	static const int BUFFER_SIZE = 1000;

	pgsNumberGen(const MAPM &range);

public:

	virtual ~pgsNumberGen();

	virtual MAPM random() = 0;

	virtual pgsNumberGen *clone() = 0;

	/* pgsNumberGen & operator =(const pgsNumberGen & that); */

	/* pgsNumberGen(const pgsNumberGen & that); */
};

#endif /*PGSNUMBERGEN_H_*/
