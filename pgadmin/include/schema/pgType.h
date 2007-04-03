//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgType.h PostgreSQL Type
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTYPE_H
#define PGTYPE_H

#include "pgSchema.h"

// Note: This must match the radio buttons on dlgType
enum TYPE_CLASS
{
    TYPE_COMPOSITE = 0,
    TYPE_ENUM,
    TYPE_EXTERNAL
};

class pgTypeFactory : public pgSchemaObjFactory
{
public:
    pgTypeFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgTypeFactory typeFactory;

class pgType : public pgSchemaObject
{
public:
    pgType(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgType();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return !GetSystemObject() && pgSchemaObject::CanDrop(); }

    wxString GetAlias() const { return alias; }
    void iSetAlias(const wxString& s) { alias=s; }
    wxString GetInputFunction() const { return inputFunction; }
    void iSetInputFunction(const wxString& s) { inputFunction=s; }
    wxString GetOutputFunction() const { return outputFunction; }
    void iSetOutputFunction(const wxString& s) { outputFunction=s; }
    wxString GetReceiveFunction() const { return receiveFunction; }
    void iSetReceiveFunction(const wxString& s) { receiveFunction=s; }
    wxString GetSendFunction() const { return sendFunction; }
    void iSetSendFunction(const wxString& s) { sendFunction=s; }
    wxString GetDefault() const { return defaultVal; }
    void iSetDefault(const wxString& s) { defaultVal=s; }
    wxString GetElement()  { return element; }
    void iSetElement(const wxString& s) { element=s; }
    wxString GetDelimiter() const { return delimiter; }
    void iSetDelimiter(const wxString& s)  { delimiter=s; }
    wxString GetAlignment() const { return alignment; }
    void iSetAlignment(const wxString& s)  { alignment=s; }
    wxString GetStorage() const { return storage; }
    void iSetStorage(const wxString& s)  { storage=s; }
    long GetInternalLength() const { return internalLength; }
    void iSetInternalLength(const long l)  { internalLength=l; }
    bool GetPassedByValue() const { return passedByValue; }
    void iSetPassedByValue(const bool b) { passedByValue=b; }
    int GetTypeClass() const {return typeClass; }
    void iSetTypeClass(const int c) { typeClass=c; }
    bool GetIsRecordType() const { return isRecordType; }
    void iSetIsRecordType(const bool b) { isRecordType=b; }
    void iSetRelOid(const OID d) { relOid=d; }
    const wxArrayString &GetTypesArray() { return typesArray; }
    const wxArrayString &GetLabelArray() { return labelArray; }
    wxString GetTypesList() const { return typesList; }
    wxString GetQuotedTypesList() const {return quotedTypesList; }
    wxString GetLabelList() const { return labelList; }
    wxString GetQuotedLabelList() const {return quotedLabelList; }
    bool GetSystemObject() const { return pgSchemaObject::GetSystemObject() || isRecordType; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString alias, inputFunction, outputFunction, defaultVal, element, delimiter, alignment, storage,
        typesList, quotedTypesList, labelList, quotedLabelList, sendFunction, receiveFunction;
	wxArrayString typesArray, labelArray;
    long internalLength;
    int typeClass;
    bool passedByValue, isRecordType;
    OID relOid;
};

#endif
