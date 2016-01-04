//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSMODULO_H_
#define PGSMODULO_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsModulo : public pgsOperation
{

public:

	pgsModulo(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsModulo();

	virtual pgsExpression *clone() const;

	pgsModulo(const pgsModulo &that);

	pgsModulo &operator =(const pgsModulo &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSMODULO_H_*/
