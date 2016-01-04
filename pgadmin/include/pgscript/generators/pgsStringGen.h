//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSSTRINGGEN_H_
#define PGSSTRINGGEN_H_

#include "pgscript/pgScript.h"
#include "pgscript/generators/pgsIntegerGen.h"

WX_DECLARE_OBJARRAY(wxChar, pgsVectorChar);

class pgsStringGen : public pgsObjectGen
{
private:

	typedef pgsCopiedPtr<pgsIntegerGen> pgsRandomizer; // Needs a clone() method

	pgsRandomizer m_w_size_randomizer;
	pgsRandomizer m_letter_randomizer;

	UCHAR m_nb_words;

	pgsVectorChar m_characters;

public:

	pgsStringGen(USHORT w_size_min, USHORT w_size_max = 0, const UCHAR &nb_words = 1,
	             const long &seed = wxDateTime::GetTimeNow(),
	             pgsVectorChar characters = pgsVectorChar());

	virtual wxString random();

	virtual ~pgsStringGen();

	virtual pgsStringGen *clone();

	/* pgsStringGen & operator =(const pgsStringGen & that); */

	/* pgsStringGen(const pgsStringGen & that); */

private:

	void init_characters();

};

#endif /*PGSSTRINGGEN_H_*/
