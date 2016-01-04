//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsMinus.h"

#include "pgscript/objects/pgsVariable.h"

pgsMinus::pgsMinus(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsMinus::~pgsMinus()
{

}

pgsExpression *pgsMinus::clone() const
{
	return pnew pgsMinus(*this);
}

pgsMinus::pgsMinus(const pgsMinus &that) :
	pgsOperation(that)
{

}

pgsMinus &pgsMinus::operator =(const pgsMinus &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsMinus::value() const
{
	return wxString() << m_left->value() << wxT(" - ") << m_right->value();
}

pgsOperand pgsMinus::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left - *right);
}
