//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    pgDomain(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgDomain();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);
    wxString GetBasetype() const { return basetype; }
    void iSetBasetype(const wxString& s) { basetype = s; }
    long GetLength() const { return length; }
    void iSetLength(long l) { length=l; }
    long GetPrecision() const { return precision; }
    void iSetPrecision(long l) { precision = l; }
    wxString GetDefault() const { return defaultVal; }
    void iSetDefault(const wxString& s) { defaultVal = s; }
    bool GetNotNull() const { return notNull; }
    void iSetNotNull(bool b) { notNull = b; }
    long GetDimensions() const { return dimensions; }
    void iSetDimensions(long l) { dimensions=l; }
    wxString GetDelimiter() const { return delimiter; }
    void iSetDelimiter(const wxString& s) { delimiter = s; }
    double GetBasetypeOid() const { return basetypeOid; }
    void iSetBasetypeOid(double d) { basetypeOid = d; }
    long GetTyplen() const { return typlen; }
    void iSetTyplen(const long l) { typlen=l; }
    long GetTypmod() const { return typmod; }
    void iSetTypmod(const long l) { typmod=l; }

    wxString GetSql(wxTreeCtrl *browser);

private:
    wxString basetype, defaultVal, delimiter;
    long length, precision, dimensions;
    long typlen, typmod;
    bool notNull;
    double basetypeOid;
};

#endif
