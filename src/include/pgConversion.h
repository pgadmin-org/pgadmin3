//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    pgConversion(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgConversion();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    wxString GetProc() const { return proc; }
    void iSetProc(const wxString &s) { proc=s; }
    wxString GetProcNamespace() const { return procNamespace; }
    void iSetProcNamespace(const wxString &s) { procNamespace=s; }
    long GetForEncoding() const { return forEncoding; }
    void iSetForEncoding(const long l) { forEncoding=l; }
    long GetToEncoding() const { return toEncoding; }
    void iSetToEncoding(const long l) { toEncoding=l; }
    bool GetDefaultConversion() const { return defaultConversion; }
    void iSetDefaultConversion(const bool b) { defaultConversion=b; }
    wxString GetSql(wxTreeCtrl *browser);

private:
    wxString conversionName, proc, procNamespace;
    long forEncoding, toEncoding;
    bool defaultConversion;
};

#endif