//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSREFERENCEGEN_H_
#define PGSREFERENCEGEN_H_

#include "pgscript/pgScript.h"
#include "pgscript/generators/pgsIntegerGen.h"

class pgsThread;

class pgsReferenceGen : public pgsObjectGen
{

private:

	typedef pgsCopiedPtr<pgsIntegerGen> pgsRandomizer; // Needs a clone() method

	pgsThread *m_app;
	wxString m_table;
	wxString m_column;
	bool m_sequence;

	MAPM m_nb_rows;

	pgsRandomizer m_randomizer;

public:

	pgsReferenceGen(pgsThread *app, const wxString &table, const wxString &column,
	                const bool &sequence = false, const long &seed = wxDateTime::GetTimeNow());

	bool is_sequence() const;

	virtual wxString random();

	virtual ~pgsReferenceGen();

	virtual pgsReferenceGen *clone();

	/* pgsReferenceGen & operator =(const pgsReferenceGen & that); */

	/* pgsReferenceGen(const pgsReferenceGen & that); */
};

#endif /*PGSREFERENCEGEN_H_*/
