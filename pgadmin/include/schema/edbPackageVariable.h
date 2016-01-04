//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbPackageVariable.h - EnterpriseDB Package variable
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBPACKAGEVARIABLE_H
#define EDBPACKAGEVARIABLE_H

#include "edbPackage.h"
class pgCollection;
class edbPackageVariable;

class edbPackageVariableFactory : public edbPackageObjFactory
{
public:
	edbPackageVariableFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
	{
		return 0;
	};
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	pgCollection *CreateCollection(pgObject *obj);
};
extern edbPackageVariableFactory packageVariableFactory;


class edbPackageVariable : public edbPackageObject
{
public:
	edbPackageVariable(edbPackage *newPackage, const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetSql(ctlTree *browser);

	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	wxString GetDataType() const
	{
		return dataType;
	}
	void iSetDataType(const wxString &s)
	{
		dataType = s;
	}
	wxString GetVisibility() const
	{
		return visibility;
	}
	void iSetVisibility(const wxString &s)
	{
		visibility = s;
	}

	bool CanCreate()
	{
		return false;
	}

private:
	wxString dataType, visibility;
};

class edbPackageVariableCollection : public edbPackageObjCollection
{
public:
	edbPackageVariableCollection(pgaFactory *factory, edbPackage *pkg);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
