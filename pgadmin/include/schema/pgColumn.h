//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgColumn.h PostgreSQL Column
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCOLUMN_H
#define PGCOLUMN_H

// App headers
#include "pgTable.h"

class pgCollection;

class pgColumnFactory : public pgTableObjFactory
{
public:
    pgColumnFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgColumnFactory columnFactory;

class pgColumn : public pgTableObject
{
public:
    pgColumn(pgTable *newTable, const wxString& newName = wxT(""));
    ~pgColumn();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
    void ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &where=wxEmptyString);
    void ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where=wxEmptyString);

    wxString GetDefinition();

    bool IsReferenced();

    wxString GetRawTypename() const { return rawTypename; }
    void iSetRawTypename(const wxString& s) { rawTypename=s; }
    wxString GetVarTypename() const { return varTypename; }
    void iSetVarTypename(const wxString& s) { varTypename=s; }
    wxString GetQuotedTypename() const { return quotedTypename; }
    void iSetQuotedTypename(const wxString& s) { quotedTypename=s; }
    wxString GetDefault() const { return defaultVal; }
    void iSetDefault(const wxString& s) { defaultVal=s; }
    long GetColNumber() const { return colNumber; }
    void iSetColNumber(const long l) { colNumber=l; }
    long GetLength() const { return length; }
    void iSetLength(const long l) { length=l; }
    long GetPrecision() const { return precision; }
    void iSetPrecision(const long l) { precision=l; }
    long GetStatistics() const { return statistics; }
    void iSetStatistics(const long l) { statistics=l; }
    bool GetIsPK() const {return isPK; }
    bool GetIsFK() const {return isFK; }
    bool GetNotNull() const {return notNull; }
    void iSetNotNull(const bool b) {notNull=b; }
    bool GetIsArray() const {return isArray; }
    void iSetIsArray(const bool b) {isArray=b; }
    long GetTyplen() const { return typlen; }
    void iSetTyplen(const long l) { typlen=l; }
    long GetTypmod() const { return typmod; }
    void iSetTypmod(const long l) { typmod=l; }
    wxString GetTableName() const { return tableName; }
    void iSetTableName(const wxString &s) { tableName=s; }
    wxString GetQuotedFullTable() const { return quotedFullTable; }
    void iSetQuotedFullTable(const wxString &s) { quotedFullTable=s; }
    wxString GetStorage() const {return storage; }
    void iSetStorage(const wxString& s) { storage=s; }
    long GetInheritedCount() const { return inheritedCount; }
    void iSetInheritedCount(const long l) { inheritedCount=l; }
    OID  GetAttTypId() const { return attTypId; }
    void iSetAttTypId(const OID o) { attTypId =o; }
    long GetAttstattarget() const { return attstattarget; }
    void iSetAttstattarget(const long l) { attstattarget=l; }
    wxString GetSerialSequence() const { return serialSequence; }
    void iSetSerialSequence(const wxString &s) { serialSequence=s; }
    wxString GetSerialSchema() const { return serialSchema; }
    void iSetSerialSchema(const wxString &s) { serialSchema=s; }
    void iSetPkCols(const wxString &s) { pkCols = s; }
    void iSetIsFK(const bool b) { isFK = b; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    bool GetSystemObject() const { return colNumber < 0; }
    wxString GetSql(ctlTree *browser);
	wxString GetCommentSql();
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-createtable"); }

    virtual bool CanDrop() { return inheritedCount == 0 && pgSchemaObject::CanDrop() && GetSchema()->GetMetaType() != PGM_CATALOG; }
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return true; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString varTypename, quotedTypename, defaultVal, tableName, quotedFullTable, storage, rawTypename;
    wxString serialSequence, serialSchema, pkCols;
    long colNumber, length, precision, statistics, attstattarget;
    long typlen, typmod, inheritedCount;
    bool isPK, isFK, notNull, isArray;
    OID attTypId;
    int isReferenced;
};

#endif
