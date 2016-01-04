//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsAssignToRecord.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/expressions/pgsIdentRecord.h"
#include "pgscript/objects/pgsRecord.h"

pgsAssignToRecord::pgsAssignToRecord(const wxString &name, const pgsExpression *line,
                                     const pgsExpression *column, const pgsExpression *var) :
	pgsAssign(name, var), m_line(line), m_column(column)
{

}

pgsAssignToRecord::~pgsAssignToRecord()
{
	pdelete(m_line);
	pdelete(m_column);
}

pgsExpression *pgsAssignToRecord::clone() const
{
	return pnew pgsAssignToRecord(*this);
}

pgsAssignToRecord::pgsAssignToRecord(const pgsAssignToRecord &that) :
	pgsAssign(that)
{
	m_line = that.m_line->clone();
	m_column = that.m_column->clone();
}

pgsAssignToRecord &pgsAssignToRecord::operator =(const pgsAssignToRecord &that)
{
	if (this != &that)
	{
		pgsAssign::operator=(that);
		pdelete(m_line);
		pdelete(m_column);
		m_line = that.m_line->clone();
		m_column = that.m_column->clone();
	}
	return (*this);
}

wxString pgsAssignToRecord::value() const
{
	return wxString() << wxT("SET ")  << m_name << wxT("[") << m_line->value()
	       << wxT("]") << wxT("[") << m_column->value() << wxT("]")
	       << wxT(" = ") << m_var->value();
}

pgsOperand pgsAssignToRecord::eval(pgsVarMap &vars) const
{
	if (vars.find(m_name) != vars.end() && vars[m_name]->is_record())
	{
		// Get the operand as a record
		pgsRecord &rec = dynamic_cast<pgsRecord &>(*vars[m_name]);

		// Get the value to assign
		pgsOperand var(m_var->eval(vars));

		if (!var->is_record())
		{
			// Evaluate parameters
			pgsOperand line(m_line->eval(vars));
			pgsOperand column(m_column->eval(vars));
			if (line->is_integer())
			{
				long aux_line;
				line->value().ToLong(&aux_line);

				if (column->is_integer() || column->is_string())
				{
					bool success = false;

					if (column->is_integer())
					{
						long aux_column;
						column->value().ToLong(&aux_column);
						if (aux_column < rec.count_columns())
						{
							success = rec.insert(aux_line, aux_column, var);
						}
					}
					else if (column->is_string())
					{
						USHORT aux_column = rec.get_column(column->value());
						if (aux_column < rec.count_columns())
						{
							success = rec.insert(aux_line, aux_column, var);
						}
					}

					if (success == false)
					{
						throw pgsParameterException(wxString() << wxT("An error ")
						                            << wxT("occurred in record affectation: ") << value()
						                            << wxT("\n") << wxT("One possible reason is a ")
						                            << wxT("column index out of range"));
					}
				}

				else
				{
					throw pgsParameterException(wxString() << column->value()
					                            << wxT(" is not a valid column number/name"));
				}
			}
			else
			{
				throw pgsParameterException(wxString() << line->value()
				                            << wxT(" is not a valid line number"));
			}
		}
		else
		{
			throw pgsParameterException(wxString() << wxT("Cannot assign a record")
			                            << wxT(" into a record: right member is a record"));
		}
	}
	else
	{
		throw pgsParameterException(wxString() << m_name << wxT(" is not a record"));
	}

	return pgsIdentRecord(m_name, m_line->clone(), m_column->clone()).eval(vars);
}
