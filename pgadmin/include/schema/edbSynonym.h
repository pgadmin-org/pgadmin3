//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: edbSynonym.h 5828 2007-01-04 16:41:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// edbSynonym.h PostgreSQL Cast
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBSYNONYM_H
#define EDBSYNONYM_H


// App headers
#include "pgDatabase.h"

class pgCollection;
class edbSynonymFactory : public pgDatabaseObjFactory
{
public:
    edbSynonymFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern edbSynonymFactory synonymFactory;


class edbSynonym : public pgDatabaseObject
{
public:
    edbSynonym(const wxString& newName = wxT(""));
    ~edbSynonym();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    bool GetSystemObject() const { return GetOid() <= GetConnection()->GetLastSystemOID(); }
    wxString GetTargetType() const { return targetType; }
    void iSetTargetType(const wxString& s) { targetType=s; }
    wxString GetTargetSchema() const { return targetSchema; }
    void iSetTargetSchema(const wxString& s) { targetSchema=s; }
    wxString GetTargetObject() const { return targetObject; }
    void iSetTargetObject(const wxString& s) { targetObject=s; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString targetType, targetSchema, targetObject;
};

#endif
