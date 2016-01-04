//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/objects/pgsVariable.h"

pgsVariable::pgsVariable(const pgsTypes &type) :
	pgsExpression(), m_type(type)
{

}

pgsVariable::~pgsVariable()
{

}

MAPM pgsVariable::num(const pgsOperand &var)
{
	return pgsMapm::pgs_str_mapm(var->value());
}

MAPM pgsVariable::num(const wxString &var)
{
	return pgsMapm::pgs_str_mapm(var);
}

bool pgsVariable::is_number() const
{
	return is_integer() || is_real();
}

bool pgsVariable::is_integer() const
{
	return m_type == pgsTInt;
}

bool pgsVariable::is_real() const
{
	return m_type == pgsTReal;
}

bool pgsVariable::is_string() const
{
	return m_type == pgsTString;
}

bool pgsVariable::is_record() const
{
	return m_type == pgsTRecord;
}

const pgsVariable::pgsTypes &pgsVariable::type() const
{
	return m_type;
}

pgsOperand operator+(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_plus(rhs);
}

pgsOperand operator-(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_minus(rhs);
}

pgsOperand operator*(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_times(rhs);
}

pgsOperand operator/(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_over(rhs);
}

pgsOperand operator%(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_modulo(rhs);
}

pgsOperand operator==(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_equal(rhs);
}

pgsOperand operator!=(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_different(rhs);
}

pgsOperand operator<(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_lower(rhs);
}

pgsOperand operator>(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_greater(rhs);
}

pgsOperand operator<=(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_lower_equal(rhs);
}

pgsOperand operator>=(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_greater_equal(rhs);
}

pgsOperand operator!(const pgsVariable &lhs)
{
	return lhs.pgs_not();
}

pgsOperand operator&=(const pgsVariable &lhs, const pgsVariable &rhs)
{
	return lhs.pgs_almost_equal(rhs);
}
