//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/objects/pgsNumber.h"

#include <wx/regex.h>
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/exceptions/pgsArithmeticException.h"
#include "pgscript/exceptions/pgsCastException.h"

#define PGS_INTEGER_FORM_1 wxT("^[+-]?[0-9]+$")
#define PGS_REAL_FORM_1 wxT("^[+-]?[0-9]+[Ee][+-]?[0-9]+$")
#define PGS_REAL_FORM_2 wxT("^[+-]?[0-9]*[.][0-9]+([Ee][+-]?[0-9]+)?$")
#define PGS_REAL_FORM_3 wxT("^[+-]?[0-9]+[.][0-9]*([Ee][+-]?[0-9]+)?$")

pgsNumber::pgsNumber(const wxString &data, const bool &is_real) :
	pgsVariable(!is_real ? pgsVariable::pgsTInt : pgsVariable::pgsTReal),
	m_data(data.Strip(wxString::both))
{
	wxASSERT(is_valid());
}

bool pgsNumber::is_valid() const
{
	pgsTypes type = num_type(m_data);
	return (type == pgsTInt) || (type == pgsTReal && is_real());
}

pgsNumber::~pgsNumber()
{

}

pgsNumber::pgsNumber(const pgsNumber &that) :
	pgsVariable(that), m_data(that.m_data)
{
	wxASSERT(is_valid());
}

pgsNumber &pgsNumber::operator =(const pgsNumber &that)
{
	if (this != &that)
	{
		pgsVariable::operator=(that);
		m_data = that.m_data;
	}

	wxASSERT(is_valid());

	return (*this);
}

pgsVariable *pgsNumber::clone() const
{
	return pnew pgsNumber(*this);
}

wxString pgsNumber::value() const
{
	return m_data;
}

pgsOperand pgsNumber::eval(pgsVarMap &vars) const
{
	return this->clone();
}

pgsVariable::pgsTypes pgsNumber::num_type(const wxString &num)
{
	if (wxRegEx(PGS_INTEGER_FORM_1).Matches(num))
	{
		return pgsTInt;
	}
	else if (( wxRegEx(PGS_REAL_FORM_1).Matches(num)
	           || wxRegEx(PGS_REAL_FORM_2).Matches(num)
	           || wxRegEx(PGS_REAL_FORM_3).Matches(num)))
	{
		return pgsTReal;
	}
	else
	{
		return pgsTString;
	}
}

pgsOperand pgsNumber::pgs_plus(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(pgsMapm::pgs_mapm_str(num(m_data)
		                      + num(rhs.value())), is_real() || rhs.is_real());
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_minus(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(pgsMapm::pgs_mapm_str(num(m_data)
		                      - num(rhs.value())), is_real() || rhs.is_real());
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_times(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(pgsMapm::pgs_mapm_str(num(m_data)
		                      * num(rhs.value())), is_real() || rhs.is_real());
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_over(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		if (num(rhs.value()) != 0)
		{
			if (is_real() || rhs.is_real())
				return pnew pgsNumber(pgsMapm::pgs_mapm_str(num(m_data)
				                      / num(rhs.value())), is_real() || rhs.is_real());
			else
				return pnew pgsNumber(pgsMapm::pgs_mapm_str(num(m_data)
				                      .div(num(rhs.value()))), is_real() || rhs.is_real());
		}
		else
		{
			throw pgsArithmeticException(m_data, rhs.value());
		}
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_modulo(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		if (num(rhs.value()) != 0)
		{
			return pnew pgsNumber(pgsMapm::pgs_mapm_str(num(m_data)
			                      % num(rhs.value())), is_real() || rhs.is_real());
		}
		else
		{
			throw pgsArithmeticException(m_data, rhs.value());
		}
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_equal(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(num(m_data) == num(rhs.value())
		                      ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_different(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(num(m_data) != num(rhs.value())
		                      ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_greater(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(num(m_data) > num(rhs.value())
		                      ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_lower(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(num(m_data) < num(rhs.value())
		                      ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_lower_equal(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(num(m_data) <= num(rhs.value())
		                      ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_greater_equal(const pgsVariable &rhs) const
{
	if (rhs.is_number())
	{
		return pnew pgsNumber(num(m_data) >= num(rhs.value())
		                      ? wxT("1") : wxT("0"));
	}
	else
	{
		throw pgsArithmeticException(m_data, rhs.value());
	}
}

pgsOperand pgsNumber::pgs_not() const
{
	return pnew pgsNumber(num(m_data) == 0 ? wxT("1") : wxT("0"));
}

bool pgsNumber::pgs_is_true() const
{
	return (num(m_data) != 0 ? true : false);
}

pgsOperand pgsNumber::pgs_almost_equal(const pgsVariable &rhs) const
{
	return pgs_equal(rhs);
}

pgsNumber pgsNumber::number() const
{
	return pgsNumber(*this);
}

pgsRecord pgsNumber::record() const
{
	pgsRecord rec(1);;
	rec.insert(0, 0, this->clone());
	return rec;
}

pgsString pgsNumber::string() const
{
	return pgsString(m_data);
}
