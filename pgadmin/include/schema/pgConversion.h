//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConversion.h PostgreSQL Conversion
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCONVERSION_H
#define PGCONVERSION_H

#include "pgSchema.h"

class pgCollection;

class pgConversionFactory : public pgSchemaObjFactory
{
public:
    pgConversionFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgConversionFactory conversionFactory;


class pgConversion : public pgSchemaObject
{
public:
    pgConversion(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgConversion();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return true; }

    wxString GetProc() const { return proc; }
    void iSetProc(const wxString &s) { proc=s; }
    wxString GetProcNamespace() const { return procNamespace; }
    void iSetProcNamespace(const wxString &s) { procNamespace=s; }
    wxString GetForEncoding() const { return forEncoding; }
    void iSetForEncoding(const wxString &s) { forEncoding=s; }
    wxString GetToEncoding() const { return toEncoding; }
    void iSetToEncoding(const wxString &s) { toEncoding=s; }
    bool GetDefaultConversion() const { return defaultConversion; }
    void iSetDefaultConversion(const bool b) { defaultConversion=b; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString proc, procNamespace, forEncoding, toEncoding;
    bool defaultConversion;
};

#endif
