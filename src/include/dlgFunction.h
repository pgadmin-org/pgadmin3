//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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
class ctlSQLBox;

class dlgFunction : public dlgSecurityProperty
{
public:
    dlgFunction(frmMain *frame, pgFunction *func, pgSchema *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;
    pgFunction *function;

    void OnChange(wxNotifyEvent &ev);
    void OnSelChangeLanguage(wxNotifyEvent &ev);
    void OnSelChangeArg(wxNotifyEvent &ev);
    void OnSelChangeType(wxNotifyEvent &ev);
    void OnAddArg(wxNotifyEvent &ev);
    void OnRemoveArg(wxNotifyEvent &ev);

    wxString GetArgs(bool quoted=false);

    ctlSQLBox *sqlBox;
    wxArrayString typOids;
    wxArrayString types;
    wxArrayString argOids;

    DECLARE_EVENT_TABLE();
};


#endif
