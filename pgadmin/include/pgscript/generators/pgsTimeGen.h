//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsTimeGen.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSTIMEGEN_H_
#define PGSTIMEGEN_H_

#include "pgscript/pgScript.h"
#include <wx/datetime.h>
#include "pgscript/generators/pgsIntegerGen.h"

class pgsTimeGen : public pgsObjectGen
{
private:

	typedef pgsCopiedPtr<pgsIntegerGen> pgsRandomizer; // Needs a clone() method

	wxDateTime m_min;
	wxDateTime m_max;
	wxLongLong m_range;

	bool m_sequence;

	pgsRandomizer m_randomizer;

public:

	pgsTimeGen(wxDateTime min, wxDateTime max, const bool & sequence,
			const long & seed = wxDateTime::GetTimeNow());
	
	bool is_sequence() const;

	virtual wxString random();

	virtual ~pgsTimeGen();

	virtual pgsTimeGen * clone();

	/* pgsTimeGen & operator =(const pgsTimeGen & that); */

	/* pgsTimeGen(const pgsTimeGen & that); */

};

#endif /*PGSTIMEGEN_H_*/
