//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgFunction.h - Function property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_FUNCTIONPROP
#define __DLG_FUNCTIONPROP

#include "dlgProperty.h"

class pgSchema;
class pgFunction;

class dlgFunction : public dlgSecurityProperty
{
public:
    dlgFunction(frmMain *frame, pgFunction *func, pgSchema *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    void SetObject(pgObject *obj) { function = (pgFunction*)obj; }

private:
    pgSchema *schema;
    pgFunction *function;

    void OnChange(wxCommandEvent &ev);
    void OnChangeStc(wxStyledTextEvent& event);
    void OnSelChangeLanguage(wxCommandEvent &ev);
    void OnSelChangeArg(wxCommandEvent &ev);
    void OnSelChangeType(wxCommandEvent &ev);
    void OnAddArg(wxCommandEvent &ev);
    void OnRemoveArg(wxCommandEvent &ev);

    wxString GetArgs(bool quoted=false);
    void ReplaceSizer(wxWindow *w, bool isC, int border);

    wxArrayString typOids;
    wxArrayString types;
    wxArrayString argOids;

    wxSizer *libcSizer;

    DECLARE_EVENT_TABLE();
};


#endif
