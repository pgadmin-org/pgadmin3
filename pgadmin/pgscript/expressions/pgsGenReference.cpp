//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGenReference.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/generators/pgsReferenceGen.h"
#include "pgscript/expressions/pgsExecute.h"
#include "pgscript/objects/pgsGenerator.h"

pgsGenReference::pgsGenReference(const pgsExpression *table, const pgsExpression *column,
                                 const pgsExpression *sequence, const pgsExpression *seed, pgsThread *app) :
	pgsExpression(), m_table(table), m_column(column), m_sequence(sequence),
	m_seed(seed), m_app(app)
{

}

pgsGenReference::~pgsGenReference()
{
	pdelete(m_table);
	pdelete(m_column);
	pdelete(m_sequence);
	pdelete(m_seed);
}

pgsExpression *pgsGenReference::clone() const
{
	return pnew pgsGenReference(*this);
}

pgsGenReference::pgsGenReference(const pgsGenReference &that) :
	pgsExpression(that)
{
	m_table = that.m_table->clone();
	m_column = that.m_column->clone();
	m_sequence = that.m_sequence->clone();
	m_seed = that.m_seed->clone();
	m_app = that.m_app;
}

pgsGenReference &pgsGenReference::operator =(const pgsGenReference &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_table);
		pdelete(m_column);
		pdelete(m_sequence);
		pdelete(m_seed);
		m_table = that.m_table->clone();
		m_column = that.m_column->clone();
		m_sequence = that.m_sequence->clone();
		m_seed = that.m_seed->clone();
		m_app = that.m_app;
	}
	return (*this);
}

wxString pgsGenReference::value() const
{
	return wxString() << wxT("reference[ table = ") << m_table->value()
	       << wxT(" column = ") << m_column->value() << wxT(" sequence = ")
	       << m_sequence->value() << wxT(" seed = ") << m_seed->value() << wxT(" ]");
}

pgsOperand pgsGenReference::eval(pgsVarMap &vars) const
{
	// Evaluate parameters
	pgsOperand table(m_table->eval(vars));
	pgsOperand column(m_column->eval(vars));
	pgsOperand sequence(m_sequence->eval(vars));
	pgsOperand seed(m_seed->eval(vars));

	// Check parameters and create the generator
	if (table->is_string() && !table->value().IsEmpty() && column->is_string()
	        && !column->value().IsEmpty() && sequence->is_integer()
	        && seed->is_integer())
	{
		// Check wheter the table and the column do exist
		pgsOperand result = pgsExecute(wxString() << wxT("SELECT 1 FROM ")
		                               << table->value() << wxT(" WHERE ") << column->value()
		                               << wxT(" = ") << column->value(), 0, m_app).eval(vars);
		if (result->pgs_is_true())
		{
			long aux_sequence, aux_seed;
			sequence->value().ToLong(&aux_sequence);
			seed->value().ToLong(&aux_seed);
			return pnew pgsGenerator(pgsVariable::pgsTString,
			                         pnew pgsReferenceGen(m_app, table->value(),
			                                 column->value(), aux_sequence != 0, aux_seed));
		}
		else
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\ntable/column does not exist"));
		}
	}
	else
	{
		// Deal with errors
		if (!table->is_string() || table->value().IsEmpty())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\ntable should be a non-empty string"));
		}
		else if (!column->is_string() || column->value().IsEmpty())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\ncolumn should be a non-empty string"));
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
