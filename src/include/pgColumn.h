//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgColumn.h PostgreSQL Column
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGColumn_H
#define PGColumn_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgColumn : public pgSchemaObject
{
public:
    pgColumn(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgColumn();

    int GetIcon() { return PGICON_COLUMN; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
    void ShowDependsOn(frmMain *form, ctlListView *dependsOn, const wxString &where=wxEmptyString);
    void ShowReferencedBy(frmMain *form, ctlListView *referencedBy, const wxString &where=wxEmptyString);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

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

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    bool GetSystemObject() const { return colNumber < 0; }
    wxString GetSql(wxTreeCtrl *browser);
	wxString GetCommentSql();
    wxString GetHelpPage(bool forCreate) const { return wxT("sql-createtable"); }

    virtual bool CanDrop() { return inheritedCount == 0 && pgSchemaObject::CanDrop(); }
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

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
