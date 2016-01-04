//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsLower.h"

#include "pgscript/objects/pgsVariable.h"

pgsLower::pgsLower(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsLower::~pgsLower()
{

}

pgsExpression *pgsLower::clone() const
{
	return pnew pgsLower(*this);
}

pgsLower::pgsLower(const pgsLower &that) :
	pgsOperation(that)
{

}

pgsLower &pgsLower::operator =(const pgsLower &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsLower::value() const
{
	return wxString() << m_left->value() << wxT(" < ") << m_right->value();
}

pgsOperand pgsLower::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left < *right);
}
