//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSDICTIONARYGEN_H_
#define PGSDICTIONARYGEN_H_

#include "pgscript/pgScript.h"
#include "pgscript/generators/pgsIntegerGen.h"

class pgsDictionaryGen : public pgsObjectGen
{

private:

	typedef pgsCopiedPtr<pgsIntegerGen> pgsRandomizer; // Needs a clone() method

	wxString m_file_path;
	wxCSConv m_conv;

	long m_nb_lines;

	pgsRandomizer m_randomizer;

public:

	pgsDictionaryGen(const wxString &file_path, const bool &sequence = false,
	                 const long &seed = wxDateTime::GetTimeNow(), wxCSConv conv = wxConvLocal);

	virtual wxString random();

	virtual ~pgsDictionaryGen();

	virtual pgsDictionaryGen *clone();

	/* pgsDictionaryGen(const pgsDictionaryGen & that); */

	const long &nb_lines() const;

private:

	pgsDictionaryGen &operator =(const pgsDictionaryGen &that);

private:

	long count_lines();

	wxString get_line(long line_nb);

};

#endif /*PGSDICTIONARYGEN_H_*/
