//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    dlgIndexBase(frmMain *frame, const wxString &resName, wxListCtrl *colList);
    pgObject *GetObject();
    wxString GetColumns();
    int Go(bool modal);

protected:
    pgIndex *index;
    void OnChange(wxNotifyEvent &ev);

private:
    void OnAddCol(wxNotifyEvent &ev);
    void OnRemoveCol(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


class dlgIndex : public dlgIndexBase
{
public:
    dlgIndex(frmMain *frame, pgIndex *index, pgTable *parentNode);

    int Go(bool modal);
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);

private:
    void OnChange(wxNotifyEvent &ev);
    DECLARE_EVENT_TABLE();
};



#endif
