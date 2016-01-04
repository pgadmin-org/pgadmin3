//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepListen.h - Slony-I listen property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPLISTENPROP
#define __DLG_REPLISTENPROP

#include "slony/dlgRepProperty.h"

class slNode;
class slListen;

class dlgRepListen : public dlgRepProperty
{
public:
	dlgRepListen(pgaFactory *factory, frmMain *frame, slListen *l, slNode *n);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-path#listen");
	}

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:

	slListen *listen;
	slNode *node;

	DECLARE_EVENT_TABLE()
};


#endif
