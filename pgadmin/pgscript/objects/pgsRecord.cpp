//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/objects/pgsRecord.h"

#include "pgscript/expressions/pgsEqual.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/exceptions/pgsArithmeticException.h"
#include "pgscript/exceptions/pgsCastException.h"

#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(pgsVectorRecordLine);
WX_DEFINE_OBJARRAY(pgsVectorRecord);

pgsRecord::pgsRecord(const USHORT &nb_columns) :
	pgsVariable(pgsVariable::pgsTRecord)
{
	m_columns.SetCount(nb_columns);
}

pgsRecord::~pgsRecord()
{

}

pgsVariable *pgsRecord::clone() const
{
	return pnew pgsRecord(*this);
}

wxString pgsRecord::value() const
{
	wxString data;
	pgsVarMap vars;

	// Go through each line and enclose it into braces
	for (USHORT i = 0; i < count_lines(); i++)
	{
		data += wxT("(");

		// Go through each column and separate them with commas
		for (USHORT j = 0; j < count_columns(); j++)
		{
			wxString elm(m_record[i][j]->eval(vars)->value());
			if (!m_record[i][j]->is_number())
			{
				elm.Replace(wxT("\\"), wxT("\\\\"));
				elm.Replace(wxT("\""), wxT("\\\""));
				elm = wxT("\"") + elm + wxT("\"");
			}
			data += elm + (j != count_columns() - 1 ? wxT(",") : wxT(""));
		}

		data += (i == count_lines() - 1) ? wxT(")") : wxT(")\n");
	}

	// Return the string representation of the record
	return data;
}

pgsOperand pgsRecord::eval(pgsVarMap &vars) const
{
	return this->clone();
}

USHORT pgsRecord::count_lines() const
{
	return m_record.GetCount();
}

USHORT pgsRecord::count_columns() const
{
	return m_columns.GetCount();
}

bool pgsRecord::insert(const USHORT &line, const USHORT &column,
                       pgsOperand value)
{
	// Add lines to match the line number provided
	for (USHORT i = count_lines(); i <= line; i++)
	{
		newline();
	}

	// Cannot insert if column is invalid
	if (column >= count_columns())
	{
		return false;
	}
	// Insert the value at line.column
	else
	{
		m_record[line][column] = value;
		return true;
	}
}

pgsOperand pgsRecord::get(const USHORT &line,
                          const USHORT &column) const
{
	if (line < count_lines() && column < count_columns())
	{
		return m_record[line][column];
	}
	else
	{
		return pnew pgsString(wxT(""));
	}
}

pgsOperand pgsRecord::get_line(const USHORT &line) const
{
	if (line < count_lines())
	{
		pgsRecord *rec = pnew pgsRecord(count_columns());
		rec->m_columns = this->m_columns;
		rec->newline();
		rec->m_record[0] = this->m_record[line];
		return rec;
	}
	else
	{
		return pnew pgsRecord(0);
	}
}

bool pgsRecord::set_column_name(const USHORT &column, wxString name)
{
	// Column number must be valid
	if (column >= count_columns())
	{
		return false;
	}

	// Column name must not exist
	// Column name must not be empty
	name = name.Strip(wxString::both).Lower();
	if (m_columns.Index(name) != wxNOT_FOUND || name.IsEmpty())
	{
		return false;
	}

	// Set the column name
	m_columns[column] = name;

	return true;
}

USHORT pgsRecord::get_column(wxString name) const
{
	name = name.Strip(wxString::both).Lower();
	if (name.IsEmpty())
	{
		return count_columns();
	}
	for (USHORT i = 0; i < count_columns(); i++)
	{
		if (m_columns[i] == name)
			return i;
	}
	return count_columns();
}

bool pgsRecord::remove_line(const USHORT &line)
{
	if (line < count_lines())
	{
		m_record.RemoveAt(line);
		return true;
	}
	return false;
}

bool pgsRecord::newline()
{
	// Insert a line
	m_record.Add(pgsVectorRecordLine());
	// Initialize each column of the line with an empty string
	for (USHORT i = 0; i < count_columns(); i++)
	{
		m_record.Last().Add(pnew pgsString(wxT("")));
	}
	return true;
}

bool pgsRecord::valid() const
{
	return true;
}

bool pgsRecord::operator==(const pgsRecord &rhs) const
{
	// Test the number of lines
	if (this->count_lines() != rhs.count_lines())
	{
		return false;
	}

	return records_equal(*this, rhs, true);
}

bool pgsRecord::operator!=(const pgsRecord &rhs) const
{
	return !(*this == rhs);
}

bool pgsRecord::operator<(const pgsRecord &rhs) const
{
	// Test the number of lines
	if (this->count_lines() >= rhs.count_lines())
	{
		return false;
	}

	return records_equal(*this, rhs, true);
}

bool pgsRecord::operator>(const pgsRecord &rhs) const
{
	return (rhs < *this);
}

bool pgsRecord::operator<=(const pgsRecord &rhs) const
{
	// Test the number of lines
	if (this->count_lines() > rhs.count_lines())
	{
		return false;
	}

	return records_equal(*this, rhs, true);
}

bool pgsRecord::operator>=(const pgsRecord &rhs) const
{
	return (rhs <= *this);
}

bool pgsRecord::almost_equal(const pgsRecord &rhs) const
{
	// Test the number of lines
	if (this->count_lines() != rhs.count_lines())
	{
		return false;
	}

	return records_equal(*this, rhs, false);
}

bool pgsRecord::records_equal(const pgsRecord &lhs, const pgsRecord &rhs,
                              bool case_sensitive) const
{
	// Test the number of columns
	if (lhs.count_columns() != rhs.count_columns())
	{
		return false;
	}

	// Test each line
	wxArrayInt seen;
	for (USHORT i = 0; i < lhs.count_lines(); i++)
	{
		bool result = false;

		// Test if the line of lhs matches with an unseen line of rhs
		for (USHORT j = 0; result == false && j < rhs.count_lines(); j++)
		{
			int k = wx_static_cast(int, j);
			if (seen.Index(k) == wxNOT_FOUND
			        && lines_equal(lhs.m_record[i], rhs.m_record[j],
			                       case_sensitive))
			{
				result = true;
				seen.push_back(k);
			}
			else
			{
				// This is not OK... Continue with the next element
				continue;
			}
		}

		if (result == false)
		{
			return false;
		}
		else
		{
			continue;
		}
	}

	return true; // End of the test
}

bool pgsRecord::lines_equal(const pgsVectorRecordLine &lhs,
                            const pgsVectorRecordLine &rhs, bool case_sensitive) const
{
	pgsVarMap vars;

	// Both lines must have the same number of columns
	if (lhs.GetCount() != rhs.GetCount())
	{
		return false;
	}

	// Test each element (column) of the line
	for (USHORT j = 0; j < wx_static_cast(USHORT, lhs.GetCount()); j++)
	{
		// Test if the two elements are equal
		pgsEqual test(lhs[j]->string().clone(), rhs[j]->string().clone(),
		              case_sensitive);
		if (test.eval(vars)->value() == wxT("1"))
		{
			// lhs == rhs: continue
			continue;
		}
		else
		{
			// lhs != rhs: lines are not equal
			return false;
		}
	}

	return true;
}

pgsOperand pgsRecord::pgs_plus(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(value(), rhs.value());
}

pgsOperand pgsRecord::pgs_minus(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(value(), rhs.value());
}

pgsOperand pgsRecord::pgs_times(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(value(), rhs.value());
}

pgsOperand pgsRecord::pgs_over(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(value(), rhs.value());
}

pgsOperand pgsRecord::pgs_modulo(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(value(), rhs.value());
}

pgsOperand pgsRecord::pgs_equal(const pgsVariable &rhs) const
{
	if (rhs.is_record())
	{
		const pgsRecord &rhs_op = dynamic_cast<const pgsRecord &>(rhs);
		return pnew pgsNumber(*this == rhs_op ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(value(), rhs.value());
	}
}

pgsOperand pgsRecord::pgs_different(const pgsVariable &rhs) const
{
	if (rhs.is_record())
	{
		const pgsRecord &rhs_op = dynamic_cast<const pgsRecord &>(rhs);
		return pnew pgsNumber(*this != rhs_op ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(value(), rhs.value());
	}
}

pgsOperand pgsRecord::pgs_greater(const pgsVariable &rhs) const
{
	if (rhs.is_record())
	{
		const pgsRecord &rhs_op = dynamic_cast<const pgsRecord &>(rhs);
		return pnew pgsNumber(*this > rhs_op ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(value(), rhs.value());
	}
}

pgsOperand pgsRecord::pgs_lower(const pgsVariable &rhs) const
{
	if (rhs.is_record())
	{
		const pgsRecord &rhs_op = dynamic_cast<const pgsRecord &>(rhs);
		return pnew pgsNumber(*this < rhs_op ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(value(), rhs.value());
	}
}

pgsOperand pgsRecord::pgs_lower_equal(const pgsVariable &rhs) const
{
	if (rhs.is_record())
	{
		const pgsRecord &rhs_op = dynamic_cast<const pgsRecord &>(rhs);
		return pnew pgsNumber(*this <= rhs_op ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(value(), rhs.value());
	}
}

pgsOperand pgsRecord::pgs_greater_equal(const pgsVariable &rhs) const
{
	if (rhs.is_record())
	{
		const pgsRecord &rhs_op = dynamic_cast<const pgsRecord &>(rhs);
		return pnew pgsNumber(*this >= rhs_op ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(value(), rhs.value());
	}
}

pgsOperand pgsRecord::pgs_not() const
{
	if (pgs_is_true())
	{
		// A record with no line is false
		return pnew pgsRecord(count_columns());
	}
	else
	{
		// A record with at least one line is true
		pgsRecord *copy = pnew pgsRecord(*this);
		copy->newline();
		return copy; // Insert one line and return the record
	}
}

bool pgsRecord::pgs_is_true() const
{
	return (count_lines() > 0 ? true : false);
}

pgsOperand pgsRecord::pgs_almost_equal(const pgsVariable &rhs) const
{
	if (rhs.is_record())
	{
		const pgsRecord &rhs_op = dynamic_cast<const pgsRecord &>(rhs);
		return pnew pgsNumber(this->almost_equal(rhs_op) ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(value(), rhs.value());
	}
}

pgsNumber pgsRecord::number() const
{
	wxString data = value().Strip(wxString::both);

	if (data.StartsWith(wxT("(")))
	{
		data = data.Mid(1);
	}

	if (data.EndsWith(wxT(")")))
	{
		data = data.Mid(0, data.Len() - 1);
	}

	pgsTypes type = pgsNumber::num_type(data);
	switch (type)
	{
		case pgsTInt:
			return pgsNumber(data, pgsInt);
		case pgsTReal:
			return pgsNumber(data, pgsReal);
		default:
			throw pgsCastException(data, wxT("number"));
	}
}

pgsRecord pgsRecord::record() const
{
	return pgsRecord(*this);
}

pgsString pgsRecord::string() const
{
	return pgsString(value());
}
