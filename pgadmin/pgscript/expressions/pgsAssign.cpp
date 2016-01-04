//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsAssign.h"

#include "pgscript/objects/pgsVariable.h"

pgsAssign::pgsAssign(const wxString &name, const pgsExpression *var) :
	pgsExpression(), m_name(name), m_var(var)
{

}

pgsAssign::~pgsAssign()
{
	pdelete(m_var);
}

pgsExpression *pgsAssign::clone() const
{
	return pnew pgsAssign(*this);
}

pgsAssign::pgsAssign(const pgsAssign &that) :
	pgsExpression(that), m_name(that.m_name)
{
	m_var = that.m_var->clone();
}

pgsAssign &pgsAssign::operator =(const pgsAssign &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		m_name = that.m_name;
		pdelete(m_var);
		m_var = that.m_var->clone();
	}
	return (*this);
}

wxString pgsAssign::value() const
{
	return wxString() << wxT("SET ") << m_name << wxT(" = ") << m_var->value();
}

pgsOperand pgsAssign::eval(pgsVarMap &vars) const
{
	vars[m_name] = m_var->eval(vars);
	return vars[m_name];
}
