//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSAND_H_
#define PGSAND_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsAnd : public pgsOperation
{

public:

	pgsAnd(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsAnd();

	virtual pgsExpression *clone() const;

	pgsAnd(const pgsAnd &that);

	pgsAnd &operator =(const pgsAnd &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSAND_H_*/
