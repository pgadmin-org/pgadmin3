//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGenInt.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/generators/pgsDictionaryGen.h"
#include "pgscript/objects/pgsGenerator.h"

pgsGenInt::pgsGenInt(const pgsExpression *min, const pgsExpression *max,
                     const pgsExpression *sequence, const pgsExpression *seed) :
	pgsExpression(), m_min(min), m_max(max), m_sequence(sequence), m_seed(seed)
{

}

pgsGenInt::~pgsGenInt()
{
	pdelete(m_min);
	pdelete(m_max);
	pdelete(m_sequence);
	pdelete(m_seed);
}

pgsExpression *pgsGenInt::clone() const
{
	return pnew pgsGenInt(*this);
}

pgsGenInt::pgsGenInt(const pgsGenInt &that) :
	pgsExpression(that)
{
	m_min = that.m_min->clone();
	m_max = that.m_max->clone();
	m_sequence = that.m_sequence->clone();
	m_seed = that.m_seed->clone();
}

pgsGenInt &pgsGenInt::operator =(const pgsGenInt &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_min);
		pdelete(m_max);
		pdelete(m_sequence);
		pdelete(m_seed);
		m_min = that.m_min->clone();
		m_max = that.m_max->clone();
		m_sequence = that.m_sequence->clone();
		m_seed = that.m_seed->clone();
	}
	return (*this);
}

wxString pgsGenInt::value() const
{
	return wxString() << wxT("integer[ min = ") << m_min->value() << wxT(" max = ")
	       << m_max->value() << wxT(" sequence = ") << m_sequence->value()
	       << wxT(" seed = ") << m_seed->value() << wxT(" ]");
}

pgsOperand pgsGenInt::eval(pgsVarMap &vars) const
{
	// Evaluate parameters
	pgsOperand min(m_min->eval(vars));
	pgsOperand max(m_max->eval(vars));
	pgsOperand sequence(m_sequence->eval(vars));
	pgsOperand seed(m_seed->eval(vars));

	// Check parameters and create the generator
	if (min->is_integer() && max->is_integer() && sequence->is_integer()
	        && seed->is_integer())
	{
		long aux_sequence, aux_seed;
		sequence->value().ToLong(&aux_sequence);
		seed->value().ToLong(&aux_seed);
		return pnew pgsGenerator(pgsVariable::pgsTInt,
		                         pnew pgsIntegerGen(pgsVariable::num(min), pgsVariable::num(max),
		                                 aux_sequence != 0, aux_seed));
	}
	else
	{
		// Deal with errors
		if (!min->is_integer())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nmin should be an integer"));
		}
		else if (!max->is_integer())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nmax should be an integer"));
		}
		else if (!sequence->is_integer())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nsequence should be an integer"));
		}
		else
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nseed should be an integer"));
		}
	}
}
