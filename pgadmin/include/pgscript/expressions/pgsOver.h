//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSOVER_H_
#define PGSOVER_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsOver : public pgsOperation
{

public:

	pgsOver(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsOver();

	virtual pgsExpression *clone() const;

	pgsOver(const pgsOver &that);

	pgsOver &operator =(const pgsOver &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSOVER_H_*/
