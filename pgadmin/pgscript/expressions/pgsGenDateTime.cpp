//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGenDateTime.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/generators/pgsDateTimeGen.h"
#include "pgscript/objects/pgsGenerator.h"

pgsGenDateTime::pgsGenDateTime(const pgsExpression *min, const pgsExpression *max,
                               const pgsExpression *sequence, const pgsExpression *seed) :
	pgsExpression(), m_min(min), m_max(max), m_sequence(sequence), m_seed(seed)
{

}

pgsGenDateTime::~pgsGenDateTime()
{
	pdelete(m_min);
	pdelete(m_max);
	pdelete(m_sequence);
	pdelete(m_seed);
}

pgsExpression *pgsGenDateTime::clone() const
{
	return pnew pgsGenDateTime(*this);
}

pgsGenDateTime::pgsGenDateTime(const pgsGenDateTime &that) :
	pgsExpression(that)
{
	m_min = that.m_min->clone();
	m_max = that.m_max->clone();
	m_sequence = that.m_sequence->clone();
	m_seed = that.m_seed->clone();
}

pgsGenDateTime &pgsGenDateTime::operator =(const pgsGenDateTime &that)
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

wxString pgsGenDateTime::value() const
{
	return wxString() << wxT("date_time[ min = ") << m_min->value() << wxT(" max = ")
	       << m_max->value() << wxT(" sequence = ") << m_sequence->value()
	       << wxT(" seed = ") << m_seed->value() << wxT(" ]");
}

pgsOperand pgsGenDateTime::eval(pgsVarMap &vars) const
{
	// Evaluate parameters
	pgsOperand min(m_min->eval(vars));
	pgsOperand max(m_max->eval(vars));
	pgsOperand sequence(m_sequence->eval(vars));
	pgsOperand seed(m_seed->eval(vars));

	// Check parameters and create the generator
	if (min->is_string() && max->is_string() && sequence->is_integer()
	        && seed->is_integer())
	{
		wxDateTime aux_min, aux_max;
		if (aux_min.ParseDateTime(min->value()) != 0 && aux_max.ParseDateTime(max->value()) != 0
		        && aux_min.IsValid() && aux_max.IsValid())
		{
			long aux_sequence, aux_seed;
			sequence->value().ToLong(&aux_sequence);
			seed->value().ToLong(&aux_seed);
			return pnew pgsGenerator(pgsVariable::pgsTString,
			                         pnew pgsDateTimeGen(aux_min, aux_max, aux_sequence != 0, aux_seed));
		}
		else
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nmin and/or max datetimes are not valid"));
		}
	}
	else
	{
		// Deal with errors
		if (!min->is_string())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nmin should be a string"));
		}
		else if (!max->is_string())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nmax should be a string"));
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
