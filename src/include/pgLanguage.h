//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgLanguage.h PostgreSQL Language
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGLanguage_H
#define PGLanguage_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgLanguage : public pgObject
{
public:
    pgLanguage(const wxString& newName = wxString(""));
    ~pgLanguage();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);
    pgDatabase *GetDatabase() const {return database; }
    void SetDatabase(pgDatabase *newDatabase) { database = newDatabase; }
    wxString GetHandlerProc() const { return handlerProc; }
    void iSetHandlerProc(const wxString& s) { handlerProc = s; }
    wxString GetValidatorProc() const { return validatorProc; }
    void iSetValidatorProc(const wxString& s) { validatorProc = s; }
    bool GetTrusted() const { return trusted; }
    void iSetTrusted(const bool b) { trusted=b; }

    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));
    wxString handlerProc, validatorProc;
    pgDatabase *database;
    bool trusted;
};

#endif
