//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepSequence.h - Slony-I sequence property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPSEQUENCEPROP
#define __DLG_REPSEQUENCEPROP

#include "slony/dlgRepProperty.h"

class slSet;
class slSequence;

class dlgRepSequence : public dlgRepProperty
{
public:
	dlgRepSequence(pgaFactory *factory, frmMain *frame, slSequence *tab, slSet *s);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-set#sequence");
	}

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:

	void OnChangeSel(wxCommandEvent &ev);

	slSequence *sequence;
	slSet *set;

	DECLARE_EVENT_TABLE()
};


#endif
