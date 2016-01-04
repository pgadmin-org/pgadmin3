//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSNEGATE_H_
#define PGSNEGATE_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsNegate : public pgsOperation
{

public:

	pgsNegate(const pgsExpression *left);

	virtual ~pgsNegate();

	virtual pgsExpression *clone() const;

	pgsNegate(const pgsNegate &that);

	pgsNegate &operator =(const pgsNegate &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSNEGATE_H_*/
