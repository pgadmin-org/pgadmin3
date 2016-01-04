//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGenRegex.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/generators/pgsRegexGen.h"
#include "pgscript/objects/pgsGenerator.h"

pgsGenRegex::pgsGenRegex(const pgsExpression *regex, const pgsExpression *seed) :
	pgsExpression(), m_regex(regex), m_seed(seed)
{

}

pgsGenRegex::~pgsGenRegex()
{
	pdelete(m_regex);
	pdelete(m_seed);
}

pgsExpression *pgsGenRegex::clone() const
{
	return pnew pgsGenRegex(*this);
}

pgsGenRegex::pgsGenRegex(const pgsGenRegex &that) :
	pgsExpression(that)
{
	m_regex = that.m_regex->clone();
	m_seed = that.m_seed->clone();
}

pgsGenRegex &pgsGenRegex::operator =(const pgsGenRegex &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_regex);
		pdelete(m_seed);
		m_regex = that.m_regex->clone();
		m_seed = that.m_seed->clone();
	}
	return (*this);
}

wxString pgsGenRegex::value() const
{
	return wxString() << wxT("string[ regex = ") << m_regex->value()
	       << wxT(" seed = ") << m_seed->value() << wxT(" ]");
}

pgsOperand pgsGenRegex::eval(pgsVarMap &vars) const
{
	// Evaluate parameters
	pgsOperand regex(m_regex->eval(vars));
	pgsOperand seed(m_seed->eval(vars));

	// Check parameters and create the generator
	if (regex->is_string() && seed->is_integer())
	{
		long aux_seed;
		seed->value().ToLong(&aux_seed);
		pgsRegexGen *gen = pnew pgsRegexGen(regex->value(), aux_seed);
		if (!gen->is_valid())
		{
			pdelete(gen);
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nregex is not a valid regular expression"));
		}
		else
		{
			return pnew pgsGenerator(pgsVariable::pgsTString, gen);
		}
	}
	else
	{
		// Deal with errors
		if (!regex->is_string())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nregex should be a string"));
		}
		else
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nseed should be an integer"));
		}
	}
}
