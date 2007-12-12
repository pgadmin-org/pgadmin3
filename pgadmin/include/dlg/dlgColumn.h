//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgColumn.h - Column property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_COLUMNPROP
#define __DLG_COLUMNPROP

#include "dlg/dlgProperty.h"

class pgColumn;
class pgTable;

class dlgColumn : public dlgTypeProperty
{
public:
    dlgColumn(pgaFactory *factory, frmMain *frame, pgColumn *column, pgTable *parentNode);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetDefinition();
    wxString GetPreviousDefinition() { return previousDefinition; }
	wxString GetComment() {	return txtComment->GetValue(); }
	wxString GetStatistics() {	return CTRL_TEXT("txtAttstattarget")->GetValue(); }

    int Go(bool modal);

    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-createtable"); }

private:
    pgColumn *column;
    pgTable *table;
    void OnSelChangeTyp(wxCommandEvent &ev);
 
    wxString previousDefinition;
    wxArrayString sequences;

    DECLARE_EVENT_TABLE()
};


#endif
