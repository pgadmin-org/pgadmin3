//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsDifferent.h"

#include "pgscript/objects/pgsVariable.h"

pgsDifferent::pgsDifferent(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsDifferent::~pgsDifferent()
{

}

pgsExpression *pgsDifferent::clone() const
{
	return pnew pgsDifferent(*this);
}

pgsDifferent::pgsDifferent(const pgsDifferent &that) :
	pgsOperation(that)
{

}

pgsDifferent &pgsDifferent::operator =(const pgsDifferent &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsDifferent::value() const
{
	return wxString() << m_left->value() << wxT(" <> ") << m_right->value();
}

pgsOperand pgsDifferent::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left != *right);
}
