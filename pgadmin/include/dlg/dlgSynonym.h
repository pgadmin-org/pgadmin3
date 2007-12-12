//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSynonym.h - EnterpriseDB Synonym property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SYNONYMPROP
#define __DLG_SYNONYMPROP

#include "dlg/dlgProperty.h"

class edbSynonym;

class dlgSynonym : public dlgProperty
{
public:
    dlgSynonym(pgaFactory *factory, frmMain *frame, edbSynonym *sy);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-createpubsynonym"); }

private:
    void OnChangeTargetType(wxCommandEvent &ev) { ProcessTypeChange(); };
    void OnChangeTargetSchema(wxCommandEvent &ev) { ProcessSchemaChange(); };
    void ProcessTypeChange();
    void ProcessSchemaChange();

    edbSynonym *synonym;

    DECLARE_EVENT_TABLE()
};


#endif
