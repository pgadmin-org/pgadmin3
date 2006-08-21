//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgIndex.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgIndex.h - Index property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_INDEXPROP
#define __DLG_INDEXPROP

#include "dlgProperty.h"


class pgIndex;
class pgIndexBase;

class dlgIndexBase : public dlgCollistProperty
{
public:
    dlgIndexBase(pgaFactory *factory, frmMain *frame, const wxString &resName, pgIndexBase *index, pgTable *parentNode);
    dlgIndexBase(pgaFactory *factory, frmMain *frame, const wxString &resName, ctlListView *colList);

    void CheckChange();
    pgObject *GetObject();
    wxString GetColumns();
    int Go(bool modal);

protected:
    pgIndexBase *index;

private:
    void OnAddCol(wxCommandEvent &ev);
    void OnRemoveCol(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE()
};


class dlgIndex : public dlgIndexBase
{
public:
    dlgIndex(pgaFactory *factory, frmMain *frame, pgIndex *index, pgTable *parentNode);

    int Go(bool modal);
    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);

private:
    DECLARE_EVENT_TABLE()
};


#endif
