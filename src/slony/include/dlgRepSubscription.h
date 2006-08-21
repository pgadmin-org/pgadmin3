//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgRepSubscription.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepSubscription.h - Slony-I Subscription property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPSUBSCPROP
#define __DLG_REPSUBSCPROP

#include "dlgRepProperty.h"

class slSubscription;
class slSet;

class dlgRepSubscription : public dlgRepProperty
{
public:
    dlgRepSubscription(pgaFactory *factory, frmMain *frame, slSubscription *sub, slSet *s);
    int Go(bool modal);
    wxString GetHelpPage() const { return wxT("slony-set#subscription"); }

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    slSubscription *subscription;
    slSet *set;

    DECLARE_EVENT_TABLE()
};


#endif
