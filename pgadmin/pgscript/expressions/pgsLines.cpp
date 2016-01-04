//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsLines.h"

#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"

pgsLines::pgsLines(const wxString &name) :
	pgsExpression(), m_name(name)
{

}

pgsLines::~pgsLines()
{

}

pgsExpression *pgsLines::clone() const
{
	return pnew pgsLines(*this);
}

wxString pgsLines::value() const
{
	return wxString() << wxT("LINES(") << m_name << wxT(")");
}

pgsOperand pgsLines::eval(pgsVarMap &vars) const
{
	if (vars.find(m_name) != vars.end())
	{
		if (vars[m_name]->is_record())
		{
			const pgsRecord &rec = dynamic_cast<const pgsRecord &>(*vars[m_name]);
			return pnew pgsNumber(wxString() << rec.count_lines(), pgsInt);
		}
		else
		{
			// Not a record: 1 line and 1 column
			return pnew pgsNumber(wxT("1"), pgsInt);
		}
	}
	else
	{
		// Does not exist: 0 line and 0 column
		return pnew pgsNumber(wxT("0"), pgsInt);
	}
}
