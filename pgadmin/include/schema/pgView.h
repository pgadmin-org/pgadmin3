//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgView.h PostgreSQL View
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGVIEW_H
#define PGVIEW_H

#include "pgRule.h"

class pgCollection;

class pgViewFactory : public pgSchemaObjFactory
{
public:
    pgViewFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgViewFactory viewFactory;


class pgView : public pgRuleObject
{
public:
    pgView(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgView();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return true; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    bool CanView() { return true; }
    bool WantDummyChild() { return true; }

	bool HasInsertRule() { return hasInsertRule; }
	bool HasUpdateRule() { return hasUpdateRule; }
	bool HasDeleteRule() { return hasDeleteRule; }

    wxMenu *GetNewMenu();
    wxString GetSql(ctlTree *browser);
    wxString GetSelectSql(ctlTree *browser);
    wxString GetInsertSql(ctlTree *browser);
    wxString GetUpdateSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

	void ShowHint(frmMain *form, bool force);
	bool GetCanHint() { return true; };

	bool IsUpToDate();

private:
	wxString GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM);
	bool hasInsertRule, hasUpdateRule, hasDeleteRule;
};

#endif
