//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
class pgaLanguageFactory : public pgaFactory
{
public:
    pgaLanguageFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, wxTreeCtrl *browser, const wxString &restr=wxEmptyString);
};
extern pgaLanguageFactory languageFactory;

class pgLanguage : public pgDatabaseObject
{
public:
    pgLanguage(const wxString& newName = wxT(""));
    ~pgLanguage();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return true; }

    wxString GetHandlerProc() const { return handlerProc; }
    void iSetHandlerProc(const wxString& s) { handlerProc = s; }
    wxString GetValidatorProc() const { return validatorProc; }
    void iSetValidatorProc(const wxString& s) { validatorProc = s; }
    bool GetTrusted() const { return trusted; }
    void iSetTrusted(const bool b) { trusted=b; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString handlerProc, validatorProc;
    bool trusted;
};

#endif
