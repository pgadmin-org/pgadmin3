//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgForeignKey.h PostgreSQL ForeignKey
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGFOREIGNKEY_H
#define PGFOREIGNKEY_H

#include "pgIndex.h"

class pgForeignKeyFactory : public pgTableObjFactory
{
public:
    pgForeignKeyFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgForeignKeyFactory foreignKeyFactory;

class pgForeignKey : public pgTableObject
{
public:
    pgForeignKey(pgTable *newTable, const wxString& newName = wxT(""));
    ~pgForeignKey();
    
    wxString GetDefinition();
    wxString GetFullName() const;

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

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
    wxString GetMatch() const { return match; }
    void iSetMatch(const wxString &s) { match=s; }
    wxString GetRelTableOidStr() const { return NumToStr(relTableOid) + wxT("::oid"); }
    OID GetRelTableOid() const { return relTableOid; }
    void iSetRelTableOid(const OID d) { relTableOid = d; }

    wxString GetFkColumns() const { return fkColumns; }
    wxString GetRefColumns() const { return refColumns; }
    wxString GetQuotedFkColumns() const { return quotedFkColumns; }
    wxString GetQuotedRefColumns() const { return quotedRefColumns; }
    wxString GetCoveringIndex() const { return coveringIndex; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetConstraint();
    wxString GetSql(ctlTree *browser);
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-altertable"); }
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    wxString onUpdate, onDelete, conkey, confkey,
             fkTable, fkSchema, references, refSchema;
    wxString fkColumns, refColumns, quotedFkColumns, quotedRefColumns, coveringIndex, match;
    bool deferrable, deferred;
    OID relTableOid;
};

#endif
