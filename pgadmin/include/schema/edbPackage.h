//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: edbPackage.h 5828 2007-01-04 16:41:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// edbPackage.h - EnterpriseDB Package class
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBPACKAGE_H
#define EDBPACKAGE_H


// App headers
#include "pgDatabase.h"
#include "pgSchema.h"

class pgCollection;

class edbPackageFactory : public pgSchemaObjFactory
{
public:
    edbPackageFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern edbPackageFactory packageFactory;


class edbPackage : public pgSchemaObject
{
public:
    edbPackage(pgSchema *newSchema, const wxString& newName = wxT(""));

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    bool GetSystemObject() const { return GetOid() <= GetConnection()->GetLastSystemOID(); }
    void iSetNumProcedures(const long num) { numProcedures = num; };
    long GetNumProcedures() { return numProcedures; };
    void iSetNumFunctions(const long num) { numFunctions = num; };
    long GetNumFunctions() { return numFunctions; };
    void iSetNumVariables(const long num) { numVariables = num; };
    long GetNumVariables() { return numVariables; };

    void iSetHeader(const wxString &data) { header = data; };
    wxString GetHeader() { return header; };
    void iSetBody(const wxString &data) { body = data; };
    wxString GetBody() { return body; };

    wxString GetHeaderInner();
    wxString GetBodyInner();

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

    bool IsUpToDate();

private:
    wxString GetInner(const wxString &def);

    long numProcedures, numFunctions, numVariables;
    wxString body, header;
};

#endif
