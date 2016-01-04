//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsOr.h"

#include "pgscript/objects/pgsNumber.h"

pgsOr::pgsOr(const pgsExpression *left, const pgsExpression *right) :
	pgsOperation(left, right)
{

}

pgsOr::~pgsOr()
{

}

pgsExpression *pgsOr::clone() const
{
	return pnew pgsOr(*this);
}

pgsOr::pgsOr(const pgsOr &that) :
	pgsOperation(that)
{

}

pgsOr &pgsOr::operator =(const pgsOr &that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsOr::value() const
{
	return wxString() << m_left->value() << wxT(" OR ") << m_right->value();
}

pgsOperand pgsOr::eval(pgsVarMap &vars) const
{
	return pnew pgsNumber(wxString() << (m_left->eval(vars)->pgs_is_true()
	                                     || m_right->eval(vars)->pgs_is_true()), pgsInt);
}
