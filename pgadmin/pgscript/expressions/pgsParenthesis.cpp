//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsParenthesis.h"

#include "pgscript/objects/pgsVariable.h"

pgsParenthesis::pgsParenthesis(const pgsExpression *left) :
	pgsOperation(left, 0)
{

}

pgsParenthesis::~pgsParenthesis()
{

}

pgsExpression *pgsParenthesis::clone() const
{
	return pnew pgsParenthesis(*this);
}

pgsParenthesis::pgsParenthesis(const pgsParenthesis &that) :
	pgsOperation(that)
{

}

pgsParenthesis &pgsParenthesis::operator =(const pgsParenthesis &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsParenthesis::value() const
{
	return wxString() << wxT("(") << m_left->value() << wxT(")");
}

pgsOperand pgsParenthesis::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));

	// Return the result
	return left;
}
