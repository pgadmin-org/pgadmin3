//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConversion.h PostgreSQL Conversion
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGConversion_H
#define PGConversion_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgConversion : public pgSchemaObject
{
public:
    pgConversion(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgConversion();

    int GetIcon() { return PGICON_CONVERSION; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

    wxString GetProc() const { return proc; }
    void iSetProc(const wxString &s) { proc=s; }
    wxString GetProcNamespace() const { return procNamespace; }
    void iSetProcNamespace(const wxString &s) { procNamespace=s; }
    wxString GetForEncoding() const { return forEncoding; }
    void iSetForEncoding(const wxString &s) { forEncoding=s; }
    wxString GetToEncoding() const { return toEncoding; }
    void iSetToEncoding(const wxString &s) { toEncoding=s; }
    bool GetDefaultConversion() const { return defaultConversion; }
    void iSetDefaultConversion(const bool b) { defaultConversion=b; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString conversionName, proc, procNamespace, forEncoding, toEncoding;
    bool defaultConversion;
};

#endif
