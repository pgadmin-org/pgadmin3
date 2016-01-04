//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsIdent.h"

#include <wx/datetime.h>
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"

const wxString pgsIdent::m_now = wxT("@NOW");

pgsIdent::pgsIdent(const wxString &name) :
	pgsExpression(), m_name(name)
{

}

pgsIdent::~pgsIdent()
{

}

pgsExpression *pgsIdent::clone() const
{
	return pnew pgsIdent(*this);
}

wxString pgsIdent::value() const
{
	return m_name;
}

pgsOperand pgsIdent::eval(pgsVarMap &vars) const
{
	if (vars.find(m_name) != vars.end())
	{
		return vars[m_name];
	}
	else if (m_name == m_now)
	{
		time_t now = wxDateTime::GetTimeNow();
		return pnew pgsNumber(wxString() << now);
	}
	else
	{
		return pnew pgsString(wxT(""));
	}
}
