//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgFunction.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    dlgFunction(pgaFactory *factory, frmMain *frame, pgFunction *func, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    void SetObject(pgObject *obj) { function = (pgFunction*)obj; }

private:
    pgSchema *schema;
    pgFunction *function;

    void OnChangeArgName(wxCommandEvent &ev);
    void OnChangeReturn(wxCommandEvent &ev);
    void OnSelChangeLanguage(wxCommandEvent &ev);
    void OnSelChangeArg(wxListEvent &ev);
    void OnSelChangeType(wxCommandEvent &ev);
    void OnAddArg(wxCommandEvent &ev);
    void OnChangeArg(wxCommandEvent &ev);
    void OnRemoveArg(wxCommandEvent &ev);

    int GetDirection(const wxString &colName);
    wxString GetArgs(bool withNames=true, bool quoted=false);
    void ReplaceSizer(wxWindow *w, bool isC, int border);

	virtual bool IsUpToDate();

    wxArrayString typOids;
    wxArrayString types;
    wxArrayString argOids;

    wxSizer *libcSizer;
    int typeColNo;

protected:
    bool isProcedure;

    DECLARE_EVENT_TABLE()
};


class dlgProcedure : public dlgFunction
{
public:
    dlgProcedure(pgaFactory *factory, frmMain *frame, pgFunction *node, pgSchema *sch);
};


#endif
