//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgType.h - Typeproperty 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TYPEPROP
#define __DLG_TYPEPROP

#include "dlgProperty.h"

class pgType;

class dlgType : public dlgTypeProperty
{
public:
    dlgType(frmMain *frame, pgType *node, pgSchema *schema);
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgSchema *schema;
    pgType *type;
    void OnChange(wxNotifyEvent &ev);
    void OnTypeChange(wxNotifyEvent &ev);
    void OnVarAdd(wxNotifyEvent &ev);
    void OnVarRemove(wxNotifyEvent &ev);
    void OnVarSelChange(wxListEvent &ev);
    void OnSelChangeTyp(wxNotifyEvent &ev);
    void OnChangeMember(wxNotifyEvent &ev);

    void showDefinition(bool isComposite);

    wxArrayString memberTypes;
    wxWindowList compositeWindows, externalWindows;

    DECLARE_EVENT_TABLE();
};


#endif
