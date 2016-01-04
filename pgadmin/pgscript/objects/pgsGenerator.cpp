//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/objects/pgsGenerator.h"

#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/exceptions/pgsCastException.h"

pgsGenerator::pgsGenerator(const pgsTypes &generator_type,
                           pgsObjectGen *randomizer) :
	pgsVariable(generator_type), m_randomizer(randomizer)
{

}

pgsGenerator::~pgsGenerator()
{

}

pgsVariable *pgsGenerator::clone() const
{
	return pnew pgsGenerator(*this);
}

wxString pgsGenerator::value() const
{
	return m_randomizer->random();
}

pgsOperand pgsGenerator::operand() const
{
	switch (type())
	{
		case pgsTInt:
			return pnew pgsNumber(value(), pgsInt);
		case pgsTReal:
			return pnew pgsNumber(value(), pgsReal);
		default:
			return pnew pgsString(value());
	}
}

pgsOperand pgsGenerator::eval(pgsVarMap &vars) const
{
	return this->clone();
}

pgsOperand pgsGenerator::pgs_plus(const pgsVariable &rhs) const
{
	return *operand() + rhs;
}

pgsOperand pgsGenerator::pgs_minus(const pgsVariable &rhs) const
{
	return *operand() - rhs;
}

pgsOperand pgsGenerator::pgs_times(const pgsVariable &rhs) const
{
	return *operand() * rhs;
}

pgsOperand pgsGenerator::pgs_over(const pgsVariable &rhs) const
{
	return *operand() / rhs;
}

pgsOperand pgsGenerator::pgs_modulo(const pgsVariable &rhs) const
{
	return *operand() % rhs;
}

pgsOperand pgsGenerator::pgs_equal(const pgsVariable &rhs) const
{
	return *operand() == rhs;
}

pgsOperand pgsGenerator::pgs_different(const pgsVariable &rhs) const
{
	return *operand() != rhs;
}

pgsOperand pgsGenerator::pgs_greater(const pgsVariable &rhs) const
{
	return *operand() > rhs;
}

pgsOperand pgsGenerator::pgs_lower(const pgsVariable &rhs) const
{
	return *operand() < rhs;
}

pgsOperand pgsGenerator::pgs_lower_equal(const pgsVariable &rhs) const
{
	return *operand() <= rhs;
}

pgsOperand pgsGenerator::pgs_greater_equal(const pgsVariable &rhs) const
{
	return *operand() >= rhs;
}

pgsOperand pgsGenerator::pgs_not() const
{
	return !(*operand());
}

bool pgsGenerator::pgs_is_true() const
{
	return operand()->pgs_is_true();
}

pgsOperand pgsGenerator::pgs_almost_equal(const pgsVariable &rhs) const
{
	return *operand() &= rhs;
}

pgsNumber pgsGenerator::number() const
{
	wxString data = value().Strip(wxString::both);
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

pgsRecord pgsGenerator::record() const
{
	pgsRecord rec(1);
	rec.insert(0, 0, operand());
	return rec;
}

pgsString pgsGenerator::string() const
{
	return pgsString(value());
}
