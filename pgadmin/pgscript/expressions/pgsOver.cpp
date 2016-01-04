//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsOver.h"

#include "pgscript/objects/pgsVariable.h"

pgsOver::pgsOver(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsOver::~pgsOver()
{

}

pgsExpression *pgsOver::clone() const
{
	return pnew pgsOver(*this);
}

pgsOver::pgsOver(const pgsOver &that) :
	pgsOperation(that)
{

}

pgsOver &pgsOver::operator =(const pgsOver &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsOver::value() const
{
	return wxString() << m_left->value() << wxT(" / ") << m_right->value();
}

pgsOperand pgsOver::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left / *right);
}
