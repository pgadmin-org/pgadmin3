//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgCast.h PostgreSQL Cast
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCast_H
#define PGCast_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgCast : public pgObject
{
public:
    pgCast(const wxString& newName = wxString(""));
    ~pgCast();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    wxString GetSourceType() const { return sourceType; }
    void iSetSourceType(const wxString& s) { sourceType=s; }
    wxString GetTargetType() const { return targetType; }
    void iSetTargetType(const wxString& s) { targetType=s; }
    wxString GetCastFunction() const { return castFunction; }
    void iSetCastFunction(const wxString& s) { castFunction=s; }
    wxString GetCastNamespace() const { return castNamespace; }
    void iSetCastNamespace(const wxString& s) { castNamespace=s; }
    
    wxString GetCastContext() const { return castContext; }
    void iSetCastContext(const wxString& s) { castContext=s; }

    wxString GetSql(wxTreeCtrl *browser);

private:
    wxString sourceType, targetType, castFunction, castContext, castNamespace;
};

#endif
