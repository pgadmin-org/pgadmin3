//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsEqual.h"

#include "pgscript/objects/pgsVariable.h"

pgsEqual::pgsEqual(const pgsExpression *left, const pgsExpression *right,
                   bool case_sensitive) :
	pgsOperation(left, right), m_case_sensitive(case_sensitive)
{

}

pgsEqual::~pgsEqual()
{

}

pgsExpression *pgsEqual::clone() const
{
	return pnew pgsEqual(*this);
}

pgsEqual::pgsEqual(const pgsEqual &that) :
	pgsOperation(that), m_case_sensitive(that.m_case_sensitive)
{

}

pgsEqual &pgsEqual::operator =(const pgsEqual &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
		const pgsEqual *p = dynamic_cast<const pgsEqual *>(&that);
		m_case_sensitive = (p != 0) ?
		                   p->m_case_sensitive : true;
	}
	return (*this);
}

wxString pgsEqual::value() const
{
	return wxString() << m_left->value() << (m_case_sensitive ? wxT(" = ")
	        : wxT(" ~= ")) << m_right->value();
}

pgsOperand pgsEqual::eval(pgsVarMap &vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));

	// Return the result
	return (m_case_sensitive ? (*left == *right) : (*left &= *right));
}
