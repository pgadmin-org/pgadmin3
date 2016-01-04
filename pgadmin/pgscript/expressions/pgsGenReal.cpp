//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGenReal.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/generators/pgsRealGen.h"
#include "pgscript/objects/pgsGenerator.h"

pgsGenReal::pgsGenReal(const pgsExpression *min, const pgsExpression *max,
                       const pgsExpression *precision, const pgsExpression *sequence,
                       const pgsExpression *seed) :
	pgsExpression(), m_min(min), m_max(max), m_precision(precision),
	m_sequence(sequence), m_seed(seed)
{

}

pgsGenReal::~pgsGenReal()
{
	pdelete(m_min);
	pdelete(m_max);
	pdelete(m_precision);
	pdelete(m_sequence);
	pdelete(m_seed);
}

pgsExpression *pgsGenReal::clone() const
{
	return pnew pgsGenReal(*this);
}

pgsGenReal::pgsGenReal(const pgsGenReal &that) :
	pgsExpression(that)
{
	m_min = that.m_min->clone();
	m_max = that.m_max->clone();
	m_precision = that.m_precision->clone();
	m_sequence = that.m_sequence->clone();
	m_seed = that.m_seed->clone();
}

pgsGenReal &pgsGenReal::operator =(const pgsGenReal &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_min);
		pdelete(m_max);
		pdelete(m_precision);
		pdelete(m_sequence);
		pdelete(m_seed);
		m_min = that.m_min->clone();
		m_max = that.m_max->clone();
		m_precision = that.m_precision->clone();
		m_sequence = that.m_sequence->clone();
		m_seed = that.m_seed->clone();
	}
	return (*this);
}

wxString pgsGenReal::value() const
{
	return wxString() << wxT("real[ min = ") << m_min->value() << wxT(" max = ")
	       << m_max->value() << wxT(" precision = ") << m_precision->value()
	       << wxT(" sequence = ") << m_sequence->value() << wxT(" seed = ")
	       << m_seed->value() << wxT(" ]");
}

pgsOperand pgsGenReal::eval(pgsVarMap &vars) const
{
	// Evaluate parameters
	pgsOperand min(m_min->eval(vars));
	pgsOperand max(m_max->eval(vars));
	pgsOperand precision(m_precision->eval(vars));
	pgsOperand sequence(m_sequence->eval(vars));
	pgsOperand seed(m_seed->eval(vars));

	// Check parameters and create the generator
	if (min->is_number() && max->is_number() && sequence->is_integer()
	        && seed->is_integer() && precision->is_integer())
	{
		long aux_sequence, aux_seed, aux_precision;
		sequence->value().ToLong(&aux_sequence);
		seed->value().ToLong(&aux_seed);
		precision->value().ToLong(&aux_precision);
		return pnew pgsGenerator(pgsVariable::pgsTReal,
		                         pnew pgsRealGen(pgsVariable::num(min), pgsVariable::num(max), aux_precision,
		                                 aux_sequence != 0, aux_seed));
	}
	else
	{
		// Deal with errors
		if (!min->is_number())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nmin should be a number"));
		}
		else if (!max->is_number())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nmax should be a number"));
		}
		else if (!precision->is_integer())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nprecision should be an integer"));
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
