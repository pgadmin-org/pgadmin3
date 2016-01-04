//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsAnd.h"

#include "pgscript/objects/pgsNumber.h"

pgsAnd::pgsAnd(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsAnd::~pgsAnd()
{

}

pgsExpression *pgsAnd::clone() const
{
	return pnew pgsAnd(*this);
}

pgsAnd::pgsAnd(const pgsAnd &that) :
	pgsOperation(that)
{

}

pgsAnd &pgsAnd::operator =(const pgsAnd &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}

	return (*this);
}

wxString pgsAnd::value() const
{
	return wxString() << m_left->value() << wxT(" AND ") << m_right->value();
}

pgsOperand pgsAnd::eval(pgsVarMap &vars) const
{
	return pnew pgsNumber(wxString() << (m_left->eval(vars)->pgs_is_true()
	                                     && m_right->eval(vars)->pgs_is_true()), pgsInt);
}
