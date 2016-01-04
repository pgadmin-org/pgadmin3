//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsRemoveLine.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"

pgsRemoveLine::pgsRemoveLine(const wxString &rec, const pgsExpression *line) :
	pgsExpression(), m_rec(rec), m_line(line)
{

}

pgsRemoveLine::~pgsRemoveLine()
{
	pdelete(m_line);
}

pgsExpression *pgsRemoveLine::clone() const
{
	return pnew pgsRemoveLine(*this);
}

pgsRemoveLine::pgsRemoveLine(const pgsRemoveLine &that) :
	pgsExpression(that), m_rec(that.m_rec)
{
	m_line = that.m_line->clone();
}

pgsRemoveLine &pgsRemoveLine::operator =(const pgsRemoveLine &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		m_rec = that.m_rec;
		pdelete(m_line);
		m_line = that.m_line->clone();
	}
	return (*this);
}

wxString pgsRemoveLine::value() const
{
	return wxString() << wxT("RMLINE(") << m_rec << wxT("[")
	       << m_line->value() << wxT("])");
}

pgsOperand pgsRemoveLine::eval(pgsVarMap &vars) const
{
	if (vars.find(m_rec) != vars.end() && vars[m_rec]->is_record())
	{
		pgsRecord &rec = dynamic_cast<pgsRecord &>(*vars[m_rec]);

		// Evaluate parameter
		pgsOperand line(m_line->eval(vars));
		if (line->is_integer())
		{
			long aux_line;
			line->value().ToLong(&aux_line);

			if (!rec.remove_line(aux_line))
			{
				throw pgsParameterException(wxString() << wxT("an error ")
				                            << wxT("occurred while executing ") << value());
			}
		}
		else
		{
			throw pgsParameterException(wxString() << line->value()
			                            << wxT(" is not a valid line number"));
		}

		return vars[m_rec];
	}
	else
	{
		throw pgsParameterException(wxString() << m_rec << wxT(" is not a record"));
	}
}
