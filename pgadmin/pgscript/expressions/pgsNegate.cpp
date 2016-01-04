//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsNegate.h"

#include "pgscript/objects/pgsNumber.h"

pgsNegate::pgsNegate(const pgsExpression *left) :
	pgsOperation(left, 0)
{

}

pgsNegate::~pgsNegate()
{

}

pgsExpression *pgsNegate::clone() const
{
	return pnew pgsNegate(*this);
}

pgsNegate::pgsNegate(const pgsNegate &that) :
	pgsOperation(that)
{

}

pgsNegate &pgsNegate::operator =(const pgsNegate &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsNegate::value() const
{
	return wxString() << wxT("-") << m_left->value();
}

pgsOperand pgsNegate::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(pnew pgsNumber(wxT("0"), pgsInt));
	pgsOperand right(m_left->eval(vars));

	// Return the result
	return (*left - *right);
}
