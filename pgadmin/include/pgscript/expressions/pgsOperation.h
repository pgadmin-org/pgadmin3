//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSOPERATION_H_
#define PGSOPERATION_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsOperation : public pgsExpression
{

protected:

	const pgsExpression *m_left;
	const pgsExpression *m_right;

public:

	pgsOperation(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsOperation();

	virtual pgsExpression *clone() const = 0;

	pgsOperation(const pgsOperation &that);

	pgsOperation &operator =(const pgsOperation &that);

	virtual wxString value() const = 0;

	virtual pgsOperand eval(pgsVarMap &vars) const = 0;

};

#endif /*PGSOPERATION_H_*/
