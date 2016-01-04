//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGreater.h"

#include "pgscript/objects/pgsVariable.h"

pgsGreater::pgsGreater(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsGreater::~pgsGreater()
{

}

pgsExpression *pgsGreater::clone() const
{
	return pnew pgsGreater(*this);
}

pgsGreater::pgsGreater(const pgsGreater &that) :
	pgsOperation(that)
{

}

pgsGreater &pgsGreater::operator =(const pgsGreater &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsGreater::value() const
{
	return wxString() << m_left->value() << wxT(" > ") << m_right->value();
}

pgsOperand pgsGreater::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left > *right);
}
