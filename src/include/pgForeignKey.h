//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgForeignKey.h PostgreSQL ForeignKey
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGForeignKey_H
#define PGForeignKey_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgForeignKey : public pgSchemaObject
{
public:
    pgForeignKey(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgForeignKey();
    
    wxString GetFullName() const;

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    wxString GetOnUpdate() const { return onUpdate; }
    void iSetOnUpdate(const wxString& s) { onUpdate=s; }
    wxString GetOnDelete() const { return onDelete; }
    void iSetOnDelete(const wxString& s) { onDelete=s; }

    wxString GetFkTable() const { return fkTable; }
    void iSetFkTable(const wxString& s) { fkTable=s; }
    wxString GetFkSchema() const { return fkSchema; }
    void iSetFkSchema(const wxString& s) { fkSchema=s; }
    wxString GetReferences() const { return references; }
    void iSetReferences(const wxString& s) { references=s; }
    wxString GetRefSchema() const { return refSchema; }
    void iSetRefSchema(const wxString& s) { refSchema=s; }
    wxString GetConkey() const { return conkey; }
    void iSetConkey(const wxString& s) { conkey=s; }
    wxString GetConfkey() const { return confkey; }
    void iSetConfkey(const wxString& s) { confkey=s; }
    bool GetDeferrable() const { return deferrable; }
    void iSetDeferrable(const bool b) { deferrable=b; }
    bool GetDeferred() const { return deferred; }
    void iSetDeferred(const bool b) { deferred=b; }
    wxString GetRelTableOidStr() const { return NumToStr(relTableOid) + wxT("::oid"); }
    double GetRelTableOid() const { return relTableOid; }
    void iSetRelTableOid(const double d) { relTableOid = d; }

    wxString GetFkColumns() const { return fkColumns; }
    wxString GetRefColumns() const { return refColumns; }
    wxString GetQuotedFkColumns() const { return quotedFkColumns; }
    wxString GetQuotedRefColumns() const { return quotedRefColumns; }

    wxString GetConstraint();
    wxString GetSql(wxTreeCtrl *browser);


private:
    wxString onUpdate, onDelete, conkey, confkey,
             fkTable, fkSchema, references, refSchema;
    wxString fkColumns, refColumns, quotedFkColumns, quotedRefColumns;
    bool deferrable, deferred;
    double relTableOid;
};

#endif
