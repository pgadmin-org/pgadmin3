//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgFunction.h PostgreSQL Function
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGFunction_H
#define PGFunction_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgFunction : public pgSchemaObject
{
public:
    pgFunction(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgFunction();


    int GetIcon() { return PGICON_FUNCTION; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser);
    static pgFunction *pgFunction::AppendFunctions(pgObject *obj, pgSchema *schema, wxTreeCtrl *browser, const wxString &restriction);

    wxString GetFullName() const {return GetName()+wxT("(")+GetArgTypes()+wxT(")"); }
    wxString GetArgTypes() const { return argTypes; }
    void iSetArgTypes(const wxString& s) { argTypes=s; }
    wxString GetArgTypeOids() const { return argTypeOids; }
    void iSetArgTypeOids(const wxString& s) { argTypeOids = s; }
    wxString GetReturnType() const { return returnType; }
    void iSetReturnType(const wxString& s) { returnType = s; }
    wxString GetLanguage() const { return language; }
    void iSetLanguage(const wxString& s) { language = s; }
    wxString GetVolatility() const { return volatility; }
    void iSetVolatility(const wxString& s) { volatility = s; }
    wxString GetSource() const { return source; }
    void iSetSource(const wxString& s) { source=s; }
    long GetArgCount() const { return argCount; }
    void iSetArgCount(long ac) { argCount = ac; }
    bool GetReturnAsSet() const { return returnAsSet; }
    void iSetReturnAsSet(bool b) { returnAsSet = b; }
    bool GetSecureDefiner() const { return secureDefiner; }
    void iSetSecureDefiner(bool b) { secureDefiner = b; }
    bool GetIsStrict() const { return isStrict; }
    void iSetIsStrict(bool b) { isStrict = b; }

    bool CanDrop() { return true; }
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

protected:
    pgFunction(pgSchema *newSchema, int newType, const wxString& newName = wxString(""));

private:
    wxString argTypeOids, argTypes, returnType, language, volatility, source;
    bool returnAsSet, secureDefiner, isStrict;
    long argCount;
};

class pgTriggerFunction : public pgFunction
{
public:
    pgTriggerFunction(pgSchema *newSchema, const wxString& newName = wxString(""));
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser);
    int GetIcon() { return PGICON_TRIGGERFUNCTION; }
};

#endif
