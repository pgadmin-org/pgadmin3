//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

class pgLanguage : public pgDatabaseObject
{
public:
    pgLanguage(const wxString& newName = wxT(""));
    ~pgLanguage();

    int GetIcon() { return PGICON_LANGUAGE; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

    wxString GetHandlerProc() const { return handlerProc; }
    void iSetHandlerProc(const wxString& s) { handlerProc = s; }
    wxString GetValidatorProc() const { return validatorProc; }
    void iSetValidatorProc(const wxString& s) { validatorProc = s; }
    bool GetTrusted() const { return trusted; }
    void iSetTrusted(const bool b) { trusted=b; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString handlerProc, validatorProc;
    bool trusted;
};

#endif
