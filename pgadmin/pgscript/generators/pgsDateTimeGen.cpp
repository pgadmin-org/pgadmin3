//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsDateTimeGen.h"

#include <string>

pgsDateTimeGen::pgsDateTimeGen(wxDateTime min, wxDateTime max,
                               const bool &sequence, const long &seed) :
	pgsObjectGen(seed), m_min(min.IsEarlierThan(max) || min.IsEqualTo(max) ? min : max),
	m_max(max.IsLaterThan(min) || max.IsEqualTo(min) ? max : min),
	m_range(m_max.Subtract(m_min).GetSeconds()), m_sequence(sequence)
{
	m_randomizer = pgsRandomizer(pnew pgsIntegerGen(0, std::string(m_range
	                             .ToString().mb_str()).c_str(), is_sequence(), m_seed));
}

bool pgsDateTimeGen::is_sequence() const
{
	return m_sequence;
}

wxString pgsDateTimeGen::random()
{
	// Get a random number representing seconds
	MAPM result = pgsMapm::pgs_str_mapm(m_randomizer->random()), quot, rem;

	// Use hours and seconds for avoiding overflows of seconds
	result.integer_div_rem(3600, quot, rem);
	long hours, seconds;
	pgsMapm::pgs_mapm_str(quot, true).ToLong(&hours);
	pgsMapm::pgs_mapm_str(rem, true).ToLong(&seconds);
	wxTimeSpan time_span(hours, 0, seconds, 0);

	// Add the TimeSpan to the MinDate
	wxDateTime aux_min(m_min);
	aux_min.Add(time_span);

	// Post conditions
	wxASSERT(aux_min.IsLaterThan(m_min) || aux_min.IsEqualTo(m_min));
	wxASSERT(aux_min.IsEarlierThan(m_max) || aux_min.IsEqualTo(m_max));

	return aux_min.Format(wxT("%Y-%m-%d %H:%M:%S"));
}

pgsDateTimeGen::~pgsDateTimeGen()
{

}

pgsDateTimeGen *pgsDateTimeGen::clone()
{
	return pnew pgsDateTimeGen(*this);
}
