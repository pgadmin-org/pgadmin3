//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGenString.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/generators/pgsStringGen.h"
#include "pgscript/objects/pgsGenerator.h"

pgsGenString::pgsGenString(const pgsExpression *min, const pgsExpression *max,
                           const pgsExpression *nb_words, const pgsExpression *seed) :
	pgsExpression(), m_min(min), m_max(max), m_nb_words(nb_words), m_seed(seed)
{

}

pgsGenString::~pgsGenString()
{
	pdelete(m_min);
	pdelete(m_max);
	pdelete(m_nb_words);
	pdelete(m_seed);
}

pgsExpression *pgsGenString::clone() const
{
	return pnew pgsGenString(*this);
}

pgsGenString::pgsGenString(const pgsGenString &that) :
	pgsExpression(that)
{
	m_min = that.m_min->clone();
	m_max = that.m_max->clone();
	m_nb_words = that.m_nb_words->clone();
	m_seed = that.m_seed->clone();
}

pgsGenString &pgsGenString::operator =(const pgsGenString &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_min);
		pdelete(m_max);
		pdelete(m_nb_words);
		pdelete(m_seed);
		m_min = that.m_min->clone();
		m_max = that.m_max->clone();
		m_nb_words = that.m_nb_words->clone();
		m_seed = that.m_seed->clone();
	}
	return (*this);
}

wxString pgsGenString::value() const
{
	return wxString() << wxT("string[ min = ") << m_min->value() << wxT(" max = ")
	       << m_max->value() << wxT(" nb_words = ") << m_nb_words->value()
	       << wxT(" seed = ") << m_seed->value() << wxT(" ]");
}

pgsOperand pgsGenString::eval(pgsVarMap &vars) const
{
	// Evaluate parameters
	pgsOperand min(m_min->eval(vars));
	pgsOperand max(m_max->eval(vars));
	pgsOperand nb_words(m_nb_words->eval(vars));
	pgsOperand seed(m_seed->eval(vars));

	// Check parameters and create the generator
	if (min->is_integer() && max->is_integer() && nb_words->is_integer()
	        && seed->is_integer())
	{
		long aux_min, aux_max, aux_nb_words, aux_seed;
		min->value().ToLong(&aux_min);
		max->value().ToLong(&aux_max);
		nb_words->value().ToLong(&aux_nb_words);
		seed->value().ToLong(&aux_seed);
		return pnew pgsGenerator(pgsVariable::pgsTString,
		                         pnew pgsStringGen(aux_min, aux_max, aux_nb_words, aux_seed));
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
		else if (!nb_words->is_integer())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nnb_words should be an integer"));
		}
		else
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nseed should be an integer"));
		}
	}
}
