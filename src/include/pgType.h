//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgType.h PostgreSQL Type
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGType_H
#define PGType_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgType : public pgSchemaObject
{
public:
    pgType(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgType();

    int GetIcon() { return PGICON_TYPE; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

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
    bool GetIsComposite() const {return isComposite; }
    void iSetIsComposite(const bool b) { isComposite=b; }
    bool GetIsRecordType() const { return isRecordType; }
    void iSetIsRecordType(const bool b) { isRecordType=b; }
    void iSetRelOid(const OID d) { relOid=d; }
    wxString GetTypesList() const { return typesList; }
    wxString GetQuotedTypesList() const {return quotedTypesList; }
    bool GetSystemObject() const { return pgSchemaObject::GetSystemObject() || isRecordType; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString inputFunction, outputFunction, defaultVal, element, delimiter, alignment, storage,
        typesList, quotedTypesList, sendFunction, receiveFunction;
    long internalLength;
    bool passedByValue, isComposite, isRecordType;
    OID relOid;
};

#endif
