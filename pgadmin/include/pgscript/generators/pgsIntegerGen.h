//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSINTEGERGEN_H_
#define PGSINTEGERGEN_H_

#include "pgscript/pgScript.h"
#include "pgscript/generators/pgsNumberGen.h"
#include "pgscript/generators/pgsObjectGen.h"
#include "pgscript/utilities/pgsCopiedPtr.h"

class pgsIntegerGen : public pgsObjectGen
{

private:

	class pgsSequentialIntGen : public pgsNumberGen
	{

	private:

		MAPM m_state;
		MAPM m_m;

		static const MAPM arg_a;
		static const MAPM arg_c;

		MAPM m_remainder;

		pgsVectorMapm m_buffer;

	public:

		pgsSequentialIntGen(const MAPM &range, const long &seed);

		virtual MAPM random();

		virtual ~pgsSequentialIntGen();

		virtual pgsNumberGen *clone();

		/* pgsSequentialIntGen & operator =(const pgsSequentialIntGen & that); */

		/* pgsSequentialIntGen(const pgsSequentialIntGen & that); */
	};

	class pgsNormalIntGen : public pgsNumberGen
	{

	private:

		MAPM m_state;
		MAPM m_top;

		static const MAPM arg_a;
		static const MAPM arg_c;
		static const MAPM arg_m;

	public:

		pgsNormalIntGen(const MAPM &range, const long &seed);

		virtual MAPM random();

		virtual ~pgsNormalIntGen();

		virtual pgsNumberGen *clone();

		/* pgsNormalIntGen & operator =(const pgsNormalIntGen & that); */

		/* pgsNormalIntGen(const pgsNormalIntGen & that); */
	};

	friend class pgsRealGen;

private:

	typedef pgsCopiedPtr<pgsNumberGen> pgsRandomizer; // Needs a clone() method

	MAPM m_min;
	MAPM m_max;
	MAPM m_range;

	bool m_sequence;

	pgsRandomizer m_randomizer;

public:

	pgsIntegerGen(const MAPM &min, const MAPM &max,
	              const bool &sequence = false, const long &seed = wxDateTime::GetTimeNow());

	bool is_sequence() const;

	virtual wxString random();

	long random_long();

	virtual ~pgsIntegerGen();

	virtual pgsIntegerGen *clone();

	/* pgsIntegerGen & operator =(const pgsIntegerGen & that); */

	/* pgsIntegerGen(const pgsIntegerGen & that); */
};

#endif /*PGSINTEGERGEN_H_*/
