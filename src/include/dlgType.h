//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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

    wxArrayString memberTypes;

    DECLARE_EVENT_TABLE();
};


#endif
