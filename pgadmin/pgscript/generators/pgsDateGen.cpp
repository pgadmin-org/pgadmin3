//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsDateGen.h"

pgsDateGen::pgsDateGen(wxDateTime min, wxDateTime max, const bool &sequence,
                       const long &seed) :
	pgsObjectGen(seed), m_min(min.IsEarlierThan(max) || min.IsEqualTo(max) ? min : max),
	m_max(max.IsLaterThan(min) || max.IsEqualTo(min) ? max : min),
	m_range(m_max.Subtract(m_min).GetDays()), m_sequence(sequence)
{
	m_randomizer = pgsRandomizer(pnew pgsIntegerGen(0, m_range, is_sequence(),
	                             m_seed));
}

bool pgsDateGen::is_sequence() const
{
	return m_sequence;
}

wxString pgsDateGen::random()
{
	wxDateSpan date_span(0, 0, 0, m_randomizer->random_long());
	wxDateTime aux_min(m_min);
	aux_min.Add(date_span);
	wxASSERT(aux_min.IsLaterThan(m_min) || aux_min.IsEqualTo(m_min));
	wxASSERT(aux_min.IsEarlierThan(m_max) || aux_min.IsEqualTo(m_max));
	return aux_min.FormatISODate();
}

pgsDateGen::~pgsDateGen()
{

}

pgsDateGen *pgsDateGen::clone()
{
	return pnew pgsDateGen(*this);
}
