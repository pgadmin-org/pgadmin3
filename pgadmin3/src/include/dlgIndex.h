//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgIndex.h - Index property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_INDEXPROP
#define __DLG_INDEXPROP

#include "dlgProperty.h"


class pgIndex;

class dlgIndexBase : public dlgCollistProperty
{
public:
    dlgIndexBase(frmMain *frame, const wxString &resName, pgIndex *index, pgTable *parentNode);
    dlgIndexBase(frmMain *frame, const wxString &resName, ctlListView *colList);

    void CheckChange();
    pgObject *GetObject();
    wxString GetColumns();
    int Go(bool modal);

protected:
    pgIndex *index;

private:
    void OnAddCol(wxCommandEvent &ev);
    void OnRemoveCol(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE();
};


class dlgIndex : public dlgIndexBase
{
public:
    dlgIndex(frmMain *frame, pgIndex *index, pgTable *parentNode);

    int Go(bool modal);
    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);

private:
    DECLARE_EVENT_TABLE();
};



#endif
