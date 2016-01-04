//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsReferenceGen.h"

#include "pgscript/expressions/pgsExecute.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/utilities/pgsThread.h"

pgsReferenceGen::pgsReferenceGen(pgsThread *app, const wxString &table,
                                 const wxString &column, const bool &sequence, const long &seed) :
	pgsObjectGen(seed), m_app(app), m_table(table), m_column(column),
	m_sequence(sequence)
{
	// We need an empty symbol table for calling pgsExecute.eval(...)
	pgsVarMap vars;

	// Count the number of lines in the table
	pgsOperand result = pgsExecute(wxString() << wxT("SELECT count(*) FROM ")
	                               << m_table, 0, m_app).eval(vars);
	wxASSERT(result->is_record());
	wxString value = result->value();
	if (!value.IsEmpty())
	{
		m_nb_rows = pgsMapm::pgs_str_mapm(result->number().value());
	}
	else
	{
		m_nb_rows = 0;
	}
	wxLogScriptVerbose(wxT("REFGEN: Number of rows in %s: %s"), m_table.c_str(),
	                   pgsMapm::pgs_mapm_str(m_nb_rows).c_str());

	// Create an integer generator with that number of lines
	m_randomizer = pgsRandomizer(pnew pgsIntegerGen(0, m_nb_rows - 1,
	                             is_sequence(), m_seed));
}

bool pgsReferenceGen::is_sequence() const
{
	return m_sequence;
}

wxString pgsReferenceGen::random()
{
	// We need an empty symbol table for calling pgsExecute.eval(...)
	pgsVarMap vars;

	// Choose one line
	pgsOperand result = pgsExecute(wxString() << wxT("SELECT ") << m_column
	                               << wxT(" FROM ") << m_table << wxT(" LIMIT 1 OFFSET ")
	                               << m_randomizer->random(), 0, m_app).eval(vars);
	wxASSERT(result->is_record());

	// Return the result as a single value
	return dynamic_cast<const pgsRecord &>(*result).get(0, 0)->value();
}

pgsReferenceGen::~pgsReferenceGen()
{

}

pgsReferenceGen *pgsReferenceGen::clone()
{
	return pnew pgsReferenceGen(*this);
}
