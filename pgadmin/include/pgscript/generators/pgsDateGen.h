//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSDATEGEN_H_
#define PGSDATEGEN_H_

#include "pgscript/pgScript.h"
#include <wx/datetime.h>
#include "pgscript/generators/pgsIntegerGen.h"

class pgsDateGen : public pgsObjectGen
{
private:

	typedef pgsCopiedPtr<pgsIntegerGen> pgsRandomizer; // Needs a clone() method

	wxDateTime m_min;
	wxDateTime m_max;
	int m_range;

	bool m_sequence;

	pgsRandomizer m_randomizer;

public:

	pgsDateGen(wxDateTime min, wxDateTime max, const bool &sequence,
	           const long &seed = wxDateTime::GetTimeNow());

	bool is_sequence() const;

	virtual wxString random();

	virtual ~pgsDateGen();

	virtual pgsDateGen *clone();

	/* pgsDateGen & operator =(const pgsDateGen & that); */

	/* pgsDateGen(const pgsDateGen & that); */

};

#endif /*PGSDATEGEN_H_*/
