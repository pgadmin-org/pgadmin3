//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsTrim.h"

#include "pgscript/objects/pgsString.h"

pgsTrim::pgsTrim(const pgsExpression *exp) :
	pgsExpression(), m_exp(exp)
{

}

pgsTrim::~pgsTrim()
{
	pdelete(m_exp);
}

pgsTrim::pgsTrim(const pgsTrim &that) :
	pgsExpression(that)
{
	m_exp = that.m_exp->clone();
}

pgsTrim &pgsTrim::operator=(const pgsTrim &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_exp);
		m_exp = that.m_exp->clone();
	}
	return (*this);
}

pgsExpression *pgsTrim::clone() const
{
	return pnew pgsTrim(*this);
}

wxString pgsTrim::value() const
{
	return wxString() << wxT("TRIM(") << m_exp->value() << wxT(")");
}

pgsOperand pgsTrim::eval(pgsVarMap &vars) const
{
	return pnew pgsString(m_exp->eval(vars)->value().Strip(wxString::both));
}
