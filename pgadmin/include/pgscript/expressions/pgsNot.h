//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSNOT_H_
#define PGSNOT_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsNot : public pgsOperation
{

public:

	pgsNot(const pgsExpression *left);

	virtual ~pgsNot();

	virtual pgsExpression *clone() const;

	pgsNot(const pgsNot &that);

	pgsNot &operator =(const pgsNot &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSNOT_H_*/
