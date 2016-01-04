//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgGroup.h - Group property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_GROUPPROP
#define __DLG_GROUPPROP

#include "dlg/dlgProperty.h"

class pgGroup;

class dlgGroup : public dlgProperty
{
public:
	dlgGroup(pgaFactory *factory, frmMain *frame, pgGroup *node = 0);
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	void CheckChange();
	int Go(bool modal);

private:
	pgGroup *group;

	void OnUserAdd(wxCommandEvent &ev);
	void OnUserRemove(wxCommandEvent &ev);

	wxArrayString usersIn;

	DECLARE_EVENT_TABLE()
};


#endif
