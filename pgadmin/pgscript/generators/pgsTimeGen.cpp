//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsTimeGen.cpp,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsTimeGen.h"

#include <string>

pgsTimeGen::pgsTimeGen(wxDateTime min, wxDateTime max, const bool & sequence,
		const long & seed) :
	pgsObjectGen(seed), m_min(min.IsEarlierThan(max) || min.IsEqualTo(max) ? min : max),
			m_max(max.IsLaterThan(min) || max.IsEqualTo(min) ? max : min),
			m_range(m_max.Subtract(m_min).GetSeconds()), m_sequence(sequence)
{
	m_min.SetYear(1970); // We know this date is not a DST date
	m_min.SetMonth(wxDateTime::Jan);
	m_min.SetDay(1);
	m_randomizer = pgsRandomizer(pnew pgsIntegerGen(0, std::string(m_range
			.ToString().mb_str()).c_str(), is_sequence(), m_seed));
}

bool pgsTimeGen::is_sequence() const
{
	return m_sequence;
}

wxString pgsTimeGen::random()
{
	wxTimeSpan time_span(0, 0, m_randomizer->random_long(), 0);
	wxDateTime aux_min(m_min);
	aux_min.Add(time_span);
	wxASSERT(aux_min.IsLaterThan(m_min) || aux_min.IsEqualTo(m_min));
	wxASSERT(aux_min.IsEarlierThan(m_max) || aux_min.IsEqualTo(m_max));
	return aux_min.FormatISOTime();
}

pgsTimeGen::~pgsTimeGen()
{

}

pgsTimeGen * pgsTimeGen::clone()
{
	return pnew pgsTimeGen(*this);
}
