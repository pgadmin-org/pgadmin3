//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsIdentRecord.h"

#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"

pgsIdentRecord::pgsIdentRecord(const wxString &name, const pgsExpression *line,
                               const pgsExpression *column) :
	pgsIdent(name), m_line(line), m_column(column)
{

}

pgsIdentRecord::~pgsIdentRecord()
{
	pdelete(m_line);
	pdelete(m_column);
}

pgsExpression *pgsIdentRecord::clone() const
{
	return pnew pgsIdentRecord(*this);
}

pgsIdentRecord::pgsIdentRecord(const pgsIdentRecord &that) :
	pgsIdent(that)
{
	m_line = that.m_line->clone();
	m_column = that.m_column != 0 ? that.m_column->clone() : 0;
}

pgsIdentRecord &pgsIdentRecord::operator=(const pgsIdentRecord &that)
{
	if (this != &that)
	{
		pgsIdent::operator=(that);
		pdelete(m_line);
		pdelete(m_column);
		m_line = that.m_line->clone();
		m_column = that.m_column != 0 ? that.m_column->clone() : 0;
	}
	return (*this);
}

wxString pgsIdentRecord::value() const
{
	wxString result;
	result << m_name << wxT("[") << m_line->value() << wxT("]");
	if (m_column != 0)
	{
		result << wxT("[") << m_column->value() << wxT("]");
	}
	return result;
}

pgsOperand pgsIdentRecord::eval(pgsVarMap &vars) const
{
	// Check whether the variable is a record
	if (vars.find(m_name) != vars.end() && vars[m_name]->is_record())
	{
		// Get the operand as a record
		const pgsRecord &rec = dynamic_cast<const pgsRecord &>(*vars[m_name]);

		// Evaluate parameters
		pgsOperand line(m_line->eval(vars));
		if (line->is_integer())
		{
			long aux_line;
			line->value().ToLong(&aux_line);

			if (m_column != 0)
			{
				pgsOperand column(m_column->eval(vars));
				if (column->is_integer())
				{
					long aux_column;
					column->value().ToLong(&aux_column);
					return rec.get(aux_line, aux_column);
				}
				else if (column->is_string())
				{
					return rec.get(aux_line, rec.get_column(column->value()));
				}
			}
			else
			{
				return rec.get_line(aux_line);
			}
		}
	}

	return pnew pgsString(wxT(""));
}
