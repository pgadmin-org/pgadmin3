//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsPlus.h"

#include "pgscript/objects/pgsVariable.h"

pgsPlus::pgsPlus(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsPlus::~pgsPlus()
{

}

pgsExpression *pgsPlus::clone() const
{
	return pnew pgsPlus(*this);
}

pgsPlus::pgsPlus(const pgsPlus &that) :
	pgsOperation(that)
{

}

pgsPlus &pgsPlus::operator =(const pgsPlus &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsPlus::value() const
{
	return wxString() << m_left->value() << wxT(" + ") << m_right->value();
}

pgsOperand pgsPlus::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left + *right);
}
