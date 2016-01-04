//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsNot.h"

#include "pgscript/objects/pgsVariable.h"

pgsNot::pgsNot(const pgsExpression *left) :
	pgsOperation(left, 0)
{

}

pgsNot::~pgsNot()
{

}

pgsExpression *pgsNot::clone() const
{
	return pnew pgsNot(*this);
}

pgsNot::pgsNot(const pgsNot &that) :
	pgsOperation(that)
{

}

pgsNot &pgsNot::operator =(const pgsNot &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsNot::value() const
{
	return wxString() << wxT("NOT ") << m_left->value();
}

pgsOperand pgsNot::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));

	// Return the result
	return (!(*left));
}
