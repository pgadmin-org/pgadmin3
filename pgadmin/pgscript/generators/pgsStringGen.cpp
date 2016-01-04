//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsStringGen.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(pgsVectorChar);

pgsStringGen::pgsStringGen(USHORT w_size_min, USHORT w_size_max,
                           const UCHAR &nb_words, const long &seed, pgsVectorChar characters) :
	pgsObjectGen(seed), m_nb_words(nb_words), m_characters(characters)
{
	init_characters(); // Initialize vector if it is empty

	const USHORT w_size_aux = w_size_min;
	w_size_min = wxMin(w_size_aux, w_size_max);
	w_size_max = wxMax(w_size_aux, w_size_max);

	size_t char_count = m_characters.GetCount();
	m_w_size_randomizer = pgsRandomizer(pnew pgsIntegerGen(w_size_min,
	                                    w_size_max, false, seed));
	m_letter_randomizer = pgsRandomizer(pnew pgsIntegerGen(0,
	                                    wx_static_cast(long, char_count) - 1, false, seed));
}

wxString pgsStringGen::random()
{
	wxString str_result;
	long w_size;

	for (UCHAR i = 0; i < m_nb_words; i++)
	{
		w_size = m_w_size_randomizer->random_long();
		for (long j = 0; j < w_size; j++)
		{
			str_result.Append(m_characters
			                  .Item(m_letter_randomizer->random_long()), 1);
		}
		if (w_size > 0 && m_nb_words != (i + 1))
		{
			str_result.Append(wxT(' '), 1);
		}
	}

	return str_result;
}

pgsStringGen::~pgsStringGen()
{

}

void pgsStringGen::init_characters()
{
	if (m_characters.size() == 0)
	{
		for (char c = '0'; c <= '9'; c++)
		{
			m_characters.push_back(c);
		}
		for (char c = 'A'; c <= 'Z'; c++)
		{
			m_characters.push_back(c);
		}
		for (char c = 'a'; c <= 'z'; c++)
		{
			m_characters.push_back(c);
		}
	}
}

pgsStringGen *pgsStringGen::clone()
{
	return pnew pgsStringGen(*this);
}
