//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgDomain.h PostgreSQL Domain
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGDomain_H
#define PGDomain_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgDomain : public pgSchemaObject
{
public:
    pgDomain(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgDomain();

    int GetIcon() { return PGICON_DOMAIN; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

    wxString GetBasetype() const { return basetype; }
    void iSetBasetype(const wxString& s) { basetype = s; }
    wxString GetQuotedBasetype() const { return quotedBasetype; }
    void iSetQuotedBasetype(const wxString& s) { quotedBasetype = s; }
    long GetLength() const { return length; }
    void iSetLength(long l) { length=l; }
    long GetPrecision() const { return precision; }
    void iSetPrecision(long l) { precision = l; }
    wxString GetCheck() const { return check; }
    void iSetCheck(const wxString &s) { check=s; }
    wxString GetDefault() const { return defaultVal; }
    void iSetDefault(const wxString& s) { defaultVal = s; }
    bool GetNotNull() const { return notNull; }
    void iSetNotNull(bool b) { notNull = b; }
    long GetDimensions() const { return dimensions; }
    void iSetDimensions(long l) { dimensions=l; }
    wxString GetDelimiter() const { return delimiter; }
    void iSetDelimiter(const wxString& s) { delimiter = s; }
    OID GetBasetypeOid() const { return basetypeOid; }
    void iSetBasetypeOid(OID d) { basetypeOid = d; }
    long GetTyplen() const { return typlen; }
    void iSetTyplen(const long l) { typlen=l; }
    long GetTypmod() const { return typmod; }
    void iSetTypmod(const long l) { typmod=l; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString basetype, quotedBasetype, defaultVal, delimiter, check;
    long length, precision, dimensions;
    long typlen, typmod;
    bool notNull;
    OID basetypeOid;
};

#endif
