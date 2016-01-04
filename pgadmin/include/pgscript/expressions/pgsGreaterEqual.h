//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGREATEREQUAL_H_
#define PGSGREATEREQUAL_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsGreaterEqual : public pgsOperation
{

public:

	pgsGreaterEqual(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsGreaterEqual();

	virtual pgsExpression *clone() const;

	pgsGreaterEqual(const pgsGreaterEqual &that);

	pgsGreaterEqual &operator =(const pgsGreaterEqual &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGREATEREQUAL_H_*/
