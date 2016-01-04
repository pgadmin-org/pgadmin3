//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGreaterEqual.h"

#include "pgscript/objects/pgsVariable.h"

pgsGreaterEqual::pgsGreaterEqual(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsGreaterEqual::~pgsGreaterEqual()
{

}

pgsExpression *pgsGreaterEqual::clone() const
{
	return pnew pgsGreaterEqual(*this);
}

pgsGreaterEqual::pgsGreaterEqual(const pgsGreaterEqual &that) :
	pgsOperation(that)
{

}

pgsGreaterEqual &pgsGreaterEqual::operator =(const pgsGreaterEqual &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsGreaterEqual::value() const
{
	return wxString() << m_left->value() << wxT(" >= ") << m_right->value();
}

pgsOperand pgsGreaterEqual::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left >= *right);
}
