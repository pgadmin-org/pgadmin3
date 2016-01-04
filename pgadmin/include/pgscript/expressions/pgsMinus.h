//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSMINUS_H_
#define PGSMINUS_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsMinus : public pgsOperation
{

public:

	pgsMinus(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsMinus();

	virtual pgsExpression *clone() const;

	pgsMinus(const pgsMinus &that);

	pgsMinus &operator =(const pgsMinus &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSMINUS_H_*/
