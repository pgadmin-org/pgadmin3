//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgFunction.h PostgreSQL Function
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGFUNCTION_H
#define PGFUNCTION_H

#include "pgSchema.h"

class pgCollection;
class pgFunction;

class pgFunctionFactory : public pgSchemaObjFactory
{
public:
    pgFunctionFactory(const wxChar *tn=0, const wxChar *ns=0, const wxChar *nls=0, char **img=0);
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);

    pgFunction *AppendFunctions(pgObject *obj, pgSchema *schema, ctlTree *browser, const wxString &restriction);
};


extern pgFunctionFactory functionFactory;

class pgFunction : public pgSchemaObject
{
public:
    pgFunction(pgSchema *newSchema, const wxString& newName = wxT(""));
    pgFunction(pgSchema *newSchema, pgaFactory &factory, const wxString& newName = wxT(""));


    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return GetSchema()->GetMetaType() != PGM_CATALOG; }

    virtual bool GetIsProcedure() const {return false; }

    wxString GetFullName();
    wxString GetArgListWithNames();
    wxString GetArgSigList();

    wxArrayString &GetArgNamesArray() { return argNamesArray; }
    void iAddArgName(const wxString &s) { argNamesArray.Add(s); }
    wxArrayString &GetArgTypesArray() { return argTypesArray; }
    void iAddArgType(const wxString &s) { argTypesArray.Add(s); }
    wxArrayString &GetArgModesArray() { return argModesArray; }
    void iAddArgMode(const wxString &s) { argModesArray.Add(s); }

    wxString GetReturnType() const { return returnType; }
    void iSetReturnType(const wxString& s) { returnType = s; }
    wxString GetLanguage() const { return language; }
    void iSetLanguage(const wxString& s) { language = s; }
    wxString GetVolatility() const { return volatility; }
    void iSetVolatility(const wxString& s) { volatility = s; }
    wxString GetSource() const { return source; }
    void iSetSource(const wxString& s) { source=s; }
    wxString GetBin() const { return bin; }
    void iSetBin(const wxString& s) { bin=s; }
    long GetArgCount() const { return argCount; }
    void iSetArgCount(long ac) { argCount = ac; }
    long GetCost() const { return cost; }
    void iSetCost(long c) { cost = c; }
    long GetRows() const { return rows; }
    void iSetRows(long r) { rows = r; }
    bool GetReturnAsSet() const { return returnAsSet; }
    void iSetReturnAsSet(bool b) { returnAsSet = b; }
    bool GetSecureDefiner() const { return secureDefiner; }
    void iSetSecureDefiner(bool b) { secureDefiner = b; }
    bool GetIsStrict() const { return isStrict; }
    void iSetIsStrict(bool b) { isStrict = b; }
    wxArrayString& GetConfigList() { return configList; }

    bool CanRestore() { return true; }
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-createfunction"); }
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	void ShowHint(frmMain *form, bool force);
	bool GetCanHint() { return true; };

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

	bool IsUpToDate();

protected:
    pgFunction(pgSchema *newSchema, int newType, const wxString& newName = wxT(""));

private:
    wxString returnType, language, volatility, source, bin;
    wxArrayString argNamesArray, argTypesArray, argModesArray;
    bool returnAsSet, secureDefiner, isStrict;
    long argCount, cost, rows;
    wxArrayString configList;
};


class pgTriggerFunctionFactory : public pgFunctionFactory
{
public:
    pgTriggerFunctionFactory();
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgTriggerFunctionFactory triggerFunctionFactory;


class pgTriggerFunction : public pgFunction
{
public:
    pgTriggerFunction(pgSchema *newSchema, const wxString& newName = wxT(""));
    static pgObject *ReadObjects(pgCollection *collection, ctlTree *browser);
};


class pgProcedureFactory : public pgFunctionFactory
{
public:
    pgProcedureFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgProcedureFactory procedureFactory;


class pgProcedure : public pgFunction
{
public:
    pgProcedure(pgSchema *newSchema, const wxString& newName = wxT(""));
    wxString GetFullName();
    static pgObject *ReadObjects(pgCollection *collection, ctlTree *browser);

    bool GetIsProcedure() const { return true; }

    wxString GetSql(ctlTree *browser);
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
};

#endif
