//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgOperatorClass.h PostgreSQL OperatorClass
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGOPERATORCLASS_H
#define PGOPERATORCLASS_H

#include "pgSchema.h"

class pgCollection;
class pgOperatorClassFactory : public pgSchemaObjFactory
{
public:
    pgOperatorClassFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgOperatorClassFactory operatorClassFactory;


class pgOperatorClass : public pgSchemaObject
{
public:
    pgOperatorClass(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgOperatorClass();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    wxString GetFullName() const { return GetName() + wxT("(") + GetAccessMethod() + wxT(")"); }
        wxString GetAccessMethod() const { return accessMethod; }
    void iSetAccessMethod(const wxString&s) { accessMethod=s; }

    wxArrayString GetOperators() { return operators; }
    wxArrayString GetFunctions() { return functions; }
    wxArrayString GetQuotedFunctions() { return quotedFunctions; }
    wxString GetInType() const {return inType; }
    void iSetInType(const wxString&s) { inType=s; }
    wxString GetKeyType() const {return keyType; }
    void iSetKeyType(const wxString&s) { keyType=s; }
    wxString GetSql(ctlTree *browser);
    bool GetOpcDefault() const { return opcDefault; }
    void iSetOpcDefault(const bool b) { opcDefault=b; }
    wxString GetFamily() const {return opFamily; }
    void iSetFamily(const wxString&s) { opFamily=s; }

    bool CanCreate() { return false; }
    bool CanEdit() { return false; }
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-createopclass"); }
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString inType, keyType, accessMethod,opFamily ;
    wxArrayString operators;
    wxArrayString functions, quotedFunctions;
    wxArrayString functionOids;
    bool opcDefault;
};

#endif
