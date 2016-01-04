//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSLOWER_H_
#define PGSLOWER_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsLower : public pgsOperation
{

public:

	pgsLower(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsLower();

	virtual pgsExpression *clone() const;

	pgsLower(const pgsLower &that);

	pgsLower &operator =(const pgsLower &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSLOWER_H_*/
