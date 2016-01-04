//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef RANDREAL_H_
#define RANDREAL_H_

#include "pgscript/pgScript.h"
#include "pgscript/generators/pgsIntegerGen.h"

class pgsRealGen : public pgsObjectGen
{

private:

	typedef pgsCopiedPtr<pgsNumberGen> pgsRandomizer; // Needs a clone() method

	MAPM m_min;
	MAPM m_max;
	MAPM m_range;

	bool m_sequence;

	MAPM m_pow;
	MAPM m_int_max;

	pgsRandomizer m_randomizer;

public:

	/**
	 * Builds a new real number generator. Precision cannot be greater that 255.
	 */
	pgsRealGen(const MAPM &min, const MAPM &max, const UCHAR &precision,
	           const bool &sequence = false, const long &seed = wxDateTime::GetTimeNow());

	bool is_sequence() const;

	virtual wxString random();

	virtual ~pgsRealGen();

	virtual pgsRealGen *clone();

	/* pgsRealGen & operator =(const pgsRealGen & that); */

	/* pgsRealGen(const pgsRealGen & that); */
};

#endif /*RANDREAL_H_*/
