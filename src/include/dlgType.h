//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgType.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    void OnVarAdd(wxCommandEvent &ev);
    void OnVarRemove(wxCommandEvent &ev);
    void OnVarSelChange(wxListEvent &ev);
    void OnSelChangeTyp(wxCommandEvent &ev);
    void OnSelChangeTypOrLen(wxCommandEvent &ev);
    void OnChangeMember(wxCommandEvent &ev);

    void showDefinition(bool isComposite);

    wxArrayString memberTypes, memberSizes;

    DECLARE_EVENT_TABLE();
};


#endif
