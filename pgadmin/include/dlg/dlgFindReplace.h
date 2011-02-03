//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgFindReplace.h - Search and replace
//
//////////////////////////////////////////////////////////////////////////

#ifndef dlgFindReplace_H
#define dlgFindReplace_H

#include "dlg/dlgClasses.h"

class ctlSQLBox;

// Class declarations
class dlgFindReplace : public pgDialog
{
public:
	dlgFindReplace(ctlSQLBox *parent);
	~dlgFindReplace();
	void FocusSearch();
	void FindNext();

private:

	void OnClose(wxCloseEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);
	void OnFind(wxCommandEvent &ev);
	void OnReplace(wxCommandEvent &ev);
	void OnReplaceAll(wxCommandEvent &ev);
	void ResetTabOrder();

	ctlSQLBox *sqlbox;

	DECLARE_EVENT_TABLE()
};

#endif
