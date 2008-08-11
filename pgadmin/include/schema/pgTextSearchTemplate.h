//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgTextSearchTemplate.h PostgreSQL Text Search Template
//
//////////////////////////////////////////////////////////////////////////

#ifndef PG_TSTEMPLATE_H
#define PG_TSTEMPLATE_H


#include "pgSchema.h"


class pgTextSearchTemplateFactory : public pgSchemaObjFactory
{
public:
    pgTextSearchTemplateFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgTextSearchTemplateFactory textSearchTemplateFactory;

class pgTextSearchTemplate : public pgSchemaObject
{
public:
    pgTextSearchTemplate(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgTextSearchTemplate();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    virtual wxString GetQuotedIdentifier() const { return GetName(); }
    bool CanDropCascaded() { return GetSchema()->GetMetaType() != PGM_CATALOG; }

    wxString GetInit() const { return init; }
    void iSetInit(const wxString& s) { init=s; }
    wxString GetLexize() const { return lexize; }
    void iSetLexize(const wxString& s) { lexize=s; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString init, lexize;
};

#endif
