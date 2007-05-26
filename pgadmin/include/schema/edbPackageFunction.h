//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// edbPackageFunction.h - EnterpriseDB Package member function
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBPACKAGEFUNCTION_H
#define EDBPACKAGEFUNCTION_H

#include "edbPackage.h"

class pgCollection;
class edbPackageFunction;

class edbPackageFunctionFactory : public edbPackageObjFactory
{
public:
    edbPackageFunctionFactory(const wxChar *tn=0, const wxChar *ns=0, const wxChar *nls=0, char **img=0);
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent) { return 0; };
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);

    edbPackageFunction *AppendFunctions(pgObject *obj, edbPackage *package, ctlTree *browser, const wxString &restriction);
};
extern edbPackageFunctionFactory packageFunctionFactory;


class edbPackageFunction : public edbPackageObject
{
public:
    edbPackageFunction(edbPackage *newPackage, const wxString& newName = wxT(""));
    edbPackageFunction(edbPackage *newPackage, pgaFactory &factory, const wxString& newName = wxT(""));

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

	wxString GetSql(ctlTree *browser);

    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    virtual bool GetIsProcedure() const { return false; }

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
    wxString GetSource() const { return source; }
    void iSetSource(const wxString& s) { source = s; }
    wxString GetVisibility() const { return visibility; }
    void iSetVisibility(const wxString& s) { visibility = s; }
    long GetArgCount() const { return argCount; }
    void iSetArgCount(long ac) { argCount = ac; }

private:
    long argCount;
    wxArrayString argNamesArray, argTypesArray, argModesArray;
    wxString returnType, source, visibility;
};

class edbPackageProcedureFactory : public edbPackageFunctionFactory
{
public:
    edbPackageProcedureFactory();
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern edbPackageProcedureFactory packageProcedureFactory;


class edbPackageProcedure : public edbPackageFunction
{
public:
    edbPackageProcedure(edbPackage *newPackage, const wxString& newName=wxT(""));
    wxString GetFullName();

    bool GetIsProcedure() const { return true; }

    wxString GetSql(ctlTree *browser);
};


#endif
