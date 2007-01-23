//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgOperatorFamily.h 5855 2007-01-11 16:59:43Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgOperatorFamily.h PostgreSQL OperatorFamily
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGOPERATORFAMILY_H
#define PGOPERATORFAMILY_H

#include "pgSchema.h"

class pgCollection;
class pgOperatorFamilyFactory : public pgSchemaObjFactory
{
public:
    pgOperatorFamilyFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgOperatorFamilyFactory operatorFamilyFactory;


class pgOperatorFamily : public pgSchemaObject
{
public:
    pgOperatorFamily(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgOperatorFamily();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    wxString GetFullName() const { return GetName() + wxT("(") + GetAccessMethod() + wxT(")"); }
    wxString GetAccessMethod() const { return accessMethod; }
    void iSetAccessMethod(const wxString&s) { accessMethod=s; }
	wxString GetSql(ctlTree *browser);

    bool CanCreate() { return false; }
    bool CanEdit() { return false; }
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-createopfamily"); }
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString accessMethod;
};

#endif
