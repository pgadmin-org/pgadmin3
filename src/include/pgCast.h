//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

class pgCast : public pgDatabaseObject
{
public:
    pgCast(const wxString& newName = wxT(""));
    ~pgCast();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));
    int GetIcon() { return PGICON_CAST; }

    bool GetSystemObject() const { return GetOid() <= GetConnection()->GetLastSystemOID(); }
    wxString GetSourceType() const { return sourceType; }
    void iSetSourceType(const wxString& s) { sourceType=s; }
    wxString GetTargetType() const { return targetType; }
    void iSetTargetType(const wxString& s) { targetType=s; }
    OID GetSourceTypeOid() const { return sourceTypeOid; }
    void iSetSourceTypeOid(const OID o) { sourceTypeOid=o; }
    OID GetTargetTypeOid() const { return targetTypeOid; }
    void iSetTargetTypeOid(const OID o) { targetTypeOid=o; }
    wxString GetCastFunction() const { return castFunction; }
    void iSetCastFunction(const wxString& s) { castFunction=s; }
    wxString GetCastNamespace() const { return castNamespace; }
    void iSetCastNamespace(const wxString& s) { castNamespace=s; }
    
    wxString GetCastContext() const { return castContext; }
    void iSetCastContext(const wxString& s) { castContext=s; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString sourceType, targetType, castFunction, castContext, castNamespace;
    OID sourceTypeOid, targetTypeOid;
};

#endif
