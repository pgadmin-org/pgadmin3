//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSEXPRESSION_H_
#define PGSEXPRESSION_H_

#include "pgscript/pgScript.h"
#include "pgscript/utilities/pgsCopiedPtr.h"

class pgsProgram;
class pgsVariable;

WX_DECLARE_STRING_HASH_MAP(pgsCopiedPtr<pgsVariable>, pgsVarMap);
typedef pgsCopiedPtr<pgsVariable> pgsOperand;

class pgsExpression
{

protected:

	pgsExpression();

public:

	virtual ~pgsExpression();

	virtual pgsExpression *clone() const = 0;

	/* pgsExpression(const pgsExpression & that); */

	/* pgsExpression & operator =(const pgsExpression & that); */

public:

	virtual wxString value() const = 0;

	virtual pgsOperand eval(pgsVarMap &vars) const = 0;

};

#endif /*PGSEXPRESSION_H_*/
