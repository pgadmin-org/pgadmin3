//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsRealGen.h"

pgsRealGen::pgsRealGen(const MAPM &min, const MAPM &max,
                       const UCHAR &precision, const bool &sequence, const long &seed) :
	pgsObjectGen(seed), m_min(wxMin(min, max)), m_max(wxMax(min, max)),
	m_range(m_max - m_min), m_sequence(sequence)
{
	m_pow = MAPM(10).pow(MAPM(precision));
	m_int_max = pgsMapm::pgs_mapm_round(m_range * m_pow) + 1;

	m_randomizer = is_sequence()
	               ? pgsRandomizer(pnew pgsIntegerGen::pgsSequentialIntGen(m_int_max, m_seed))
	               : pgsRandomizer(pnew pgsIntegerGen::pgsNormalIntGen(m_int_max, m_seed));
}

bool pgsRealGen::is_sequence() const
{
	return m_sequence;
}

wxString pgsRealGen::random()
{
	MAPM data = m_randomizer->random() / m_pow;
	data = data + m_min;
	wxASSERT(data >= m_min && data <= m_max);
	return pgsMapm::pgs_mapm_str(data);
}

pgsRealGen::~pgsRealGen()
{

}

pgsRealGen *pgsRealGen::clone()
{
	return pnew pgsRealGen(*this);
}
