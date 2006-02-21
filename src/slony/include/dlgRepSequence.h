//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgRepSequence.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepSequence.h - Slony-I sequence property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPSEQUENCEPROP
#define __DLG_REPSEQUENCEPROP

#include "dlgRepProperty.h"

class slSet;
class slSequence;

class dlgRepSequence : public dlgRepProperty
{
public:
    dlgRepSequence(pgaFactory *factory, frmMain *frame, slSequence *tab, slSet *s);
    int Go(bool modal);
    wxString GetHelpPage() const { return wxT("slony-set#sequence"); }

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:

    void OnChangeSel(wxCommandEvent &ev);

    slSequence *sequence;
    slSet *set;

    DECLARE_EVENT_TABLE();
};


#endif
