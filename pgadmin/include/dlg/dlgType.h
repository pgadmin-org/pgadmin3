//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgType.h - Typeproperty 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TYPEPROP
#define __DLG_TYPEPROP

#include "dlg/dlgProperty.h"

class pgType;

class dlgType : public dlgTypeProperty
{
public:
    dlgType(pgaFactory *factory, frmMain *frame, pgType *node, pgSchema *schema);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgSchema *schema;
    pgType *type;
    void OnTypeChange(wxCommandEvent &ev);
    void OnMemberAdd(wxCommandEvent &ev);
    void OnMemberRemove(wxCommandEvent &ev);
    void OnMemberSelChange(wxListEvent &ev);
    void OnLabelAdd(wxCommandEvent &ev);
    void OnLabelRemove(wxCommandEvent &ev);
    void OnLabelSelChange(wxListEvent &ev);
    void OnSelChangeTyp(wxCommandEvent &ev);
    void OnSelChangeTypOrLen(wxCommandEvent &ev);
    void OnChangeMember(wxCommandEvent &ev);

    void showDefinition(int panel);
    wxString GetFullTypeName(int type);

    wxArrayString memberTypes, memberLengths, memberPrecisions;

    DECLARE_EVENT_TABLE()
};


#endif
