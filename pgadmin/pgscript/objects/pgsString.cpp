//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/objects/pgsString.h"

#include <wx/regex.h>
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/exceptions/pgsArithmeticException.h"
#include "pgscript/exceptions/pgsCastException.h"

pgsString::pgsString(const wxString &data) :
	pgsVariable(pgsVariable::pgsTString), m_data(data)
{

}

pgsString::~pgsString()
{

}

pgsVariable *pgsString::clone() const
{
	return pnew pgsString(*this);
}

wxString pgsString::value() const
{
	return m_data;
}

pgsOperand pgsString::eval(pgsVarMap &vars) const
{
	return this->clone();
}

pgsOperand pgsString::pgs_plus(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsString(m_data + rhs.value());
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsString::pgs_minus(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(m_data, rhs.value());
}

pgsOperand pgsString::pgs_times(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(m_data, rhs.value());
}

pgsOperand pgsString::pgs_over(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(m_data, rhs.value());
}

pgsOperand pgsString::pgs_modulo(const pgsVariable &rhs) const
{
	throw pgsArithmeticException(m_data, rhs.value());
}

pgsOperand pgsString::pgs_equal(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsNumber(m_data == rhs.value() ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsString::pgs_different(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsNumber(m_data != rhs.value() ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsString::pgs_greater(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsNumber(m_data > rhs.value() ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsString::pgs_lower(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsNumber(m_data < rhs.value() ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsString::pgs_lower_equal(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsNumber(m_data <= rhs.value() ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsString::pgs_greater_equal(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsNumber(m_data >= rhs.value() ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsString::pgs_not() const
{
	wxString data = m_data.Strip(wxString::both);
	return pnew pgsString(data.IsEmpty() ? wxT("1") : wxT(""));
}

bool pgsString::pgs_is_true() const
{
	wxString data = m_data.Strip(wxString::both);
	return (!data.IsEmpty() ? true : false);
}

pgsOperand pgsString::pgs_almost_equal(const pgsVariable &rhs) const
{
	if (rhs.is_string())
	{
		return pnew pgsNumber(m_data.CmpNoCase(rhs.value()) == 0
		                      ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsNumber pgsString::number() const
{
	wxString data = m_data.Strip(wxString::both);
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

pgsRecord pgsString::record() const
{
	pgsRecord *rec = 0;

	// Try to find the representation of a record in the string
	{
		wxString element(wxT("(\"([^\"\\\\]|\\\\.)*\")|((-|[a-zA-Z0-9\\+\\.])+)"));
		wxString data(m_data);
		wxRegEx regex1(wxString() << wxT("^[[:space:]]*\\([[:space:]]*(")
		               << element << wxT(")[[:space:]]*([,][[:space:]]*(")
		               << element << wxT(")[[:space:]]*)*\\)"), wxRE_DEFAULT | wxRE_ICASE);

		// Find each line
		size_t line_nb = 0, nb_of_columns = 0;
		bool count_columns = true;
		while (regex1.Matches(data))
		{
			// Process that line: find each element
			wxString line(regex1.GetMatch(data));
			wxRegEx regex2(element);
			size_t column_nb = 0;
			while (regex2.Matches(line))
			{
				if (count_columns == true)
				{
					++nb_of_columns;
				}
				else
				{
					if (column_nb < nb_of_columns && rec != 0)
					{
						wxString value(regex2.GetMatch(line));
						if (value.StartsWith(wxT("\""))
						        && value.EndsWith(wxT("\"")))
						{
							// This is a string
							value = value.Mid(1, value.Len() - 2);
							value.Replace(wxT("\\\""), wxT("\""));
							value.Replace(wxT("\\\\"), wxT("\\"));
							rec->insert(line_nb, column_nb,
							            pnew pgsString(value));
						}
						else
						{
							// This is a number or a string
							pgsTypes type = pgsNumber::num_type(value);
							switch (type)
							{
								case pgsTInt:
									rec->insert(line_nb, column_nb,
									            pnew pgsNumber(value, pgsInt));
									break;
								case pgsTReal:
									rec->insert(line_nb, column_nb,
									            pnew pgsNumber(value, pgsReal));
									break;
								default:
									rec->insert(line_nb, column_nb,
									            pnew pgsString(value));
									break;
							}
						}
					}
					++column_nb;
				}

				regex2.ReplaceFirst(&line, wxT(""));
			}

			// If it is the first loop we want to process this line a
			// second time because the first one was meant to count
			// the number of columns
			if (count_columns == true)
			{
				count_columns = false;
				rec = pnew pgsRecord(nb_of_columns);
			}
			else
			{
				regex1.ReplaceFirst(&data, wxT(""));
				++line_nb;
			}
		}
	}

	// Process the case
	if (rec == 0)
	{
		rec = pnew pgsRecord(1);
		rec->insert(0, 0, this->clone());
	}

	pgsRecord ret_val(*rec);
	pdelete(rec);
	return ret_val;
}

pgsString pgsString::string() const
{
	return pgsString(*this);
}
