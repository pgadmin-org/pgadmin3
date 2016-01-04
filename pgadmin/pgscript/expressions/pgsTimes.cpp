//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsTimes.h"

#include "pgscript/objects/pgsVariable.h"

pgsTimes::pgsTimes(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsTimes::~pgsTimes()
{

}

pgsExpression *pgsTimes::clone() const
{
	return pnew pgsTimes(*this);
}

pgsTimes::pgsTimes(const pgsTimes &that) :
	pgsOperation(that)
{

}

pgsTimes &pgsTimes::operator =(const pgsTimes &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsTimes::value() const
{
	return wxString() << m_left->value() << wxT(" * ") << m_right->value();
}

pgsOperand pgsTimes::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left **right);
}
