//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgaStep.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaStep.h - PostgreSQL Agent Job Step
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGASTEP_H
#define PGASTEP_H

#include "pgaJob.h"



class pgaStepFactory : public pgaJobObjFactory
{
public:
    pgaStepFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgaStepFactory stepFactory;


class pgaStep : public pgaJobObject
{
public:
    pgaStep(pgCollection *collection, const wxString& newName = wxT(""));
    ~pgaStep();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);

    bool GetEnabled() const { return enabled; }
    void iSetEnabled(const bool b) { enabled=b; }
    wxChar GetKindChar() const { return kindChar; }
    void iSetKindChar(const wxChar c) { kindChar=c; }
    wxString GetKind() const { return kind; }
    void iSetKind(const wxString &s) { kind=s; }
    wxString GetCode() const { return code; }
    void iSetCode(const wxString &s) { code=s; }
    wxString GetDbname() const { return dbname; }
    void iSetDbname(const wxString &s) { dbname=s; }
    wxString GetOnError() const { return onError; }
    void iSetOnError(const wxString &s) { onError=s; }
    wxChar GetOnErrorChar() const { return onErrorChar; }
    void iSetOnErrorChar(const wxChar c) { onErrorChar = c; }
	long GetRecId() const { return recId; }
    void iSetRecId(const long l) { recId=l; }

	bool IsUpToDate();

    wxString GetHelpPage(bool forCreate) const { return wxT("pgagent-steps"); }

private:
    bool enabled;
    wxString kind, code, dbname, onError;
    wxChar kindChar, onErrorChar;
	long recId;
};

#endif
