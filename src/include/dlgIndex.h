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

// pointer to controls
#define txtName         CTRL("txtName", wxTextCtrl)
#define txtComment      CTRL("txtComment", wxTextCtrl)
#define cbColumns       CTRL("cbColumns", wxComboBox)


class pgIndex;
class pgTable;

class dlgIndexBase : public dlgProperty
{
public:
    dlgIndexBase(frmMain *frame, const wxString &resName, pgIndex *index, pgTable *parentNode);
    pgObject *GetObject();
    wxString GetColumns();
    int Go(bool modal);

protected:
    pgIndex *index;
    pgTable *table;
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
};



#endif
