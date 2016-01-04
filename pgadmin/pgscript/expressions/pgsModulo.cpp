//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsModulo.h"

#include "pgscript/objects/pgsVariable.h"

pgsModulo::pgsModulo(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsModulo::~pgsModulo()
{

}

pgsExpression *pgsModulo::clone() const
{
	return pnew pgsModulo(*this);
}

pgsModulo::pgsModulo(const pgsModulo &that) :
	pgsOperation(that)
{

}

pgsModulo &pgsModulo::operator =(const pgsModulo &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsModulo::value() const
{
	return wxString() << m_left->value() << wxT(" % ") << m_right->value();
}

pgsOperand pgsModulo::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (*left % *right);
}
