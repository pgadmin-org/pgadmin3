//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbPackage.h - EnterpriseDB Package class
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBPACKAGE_H
#define EDBPACKAGE_H


// App headers
#include "pgDatabase.h"
#include "pgSchema.h"

class pgCollection;

class edbPackageFactory : public pgSchemaObjFactory
{
public:
	edbPackageFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	pgCollection *CreateCollection(pgObject *obj);
};
extern edbPackageFactory packageFactory;


class edbPackage : public pgSchemaObject
{
public:
	edbPackage(pgSchema *newSchema, const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	bool GetSystemObject() const
	{
		return GetOid() <= GetConnection()->GetLastSystemOID();
	}

	void iSetHeader(const wxString &data)
	{
		header = data;
	};
	wxString GetHeader()
	{
		return header;
	};
	void iSetBody(const wxString &data)
	{
		body = data;
	};
	wxString GetBody()
	{
		return body;
	};

	wxString GetHeaderInner();
	wxString GetBodyInner();

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}

	bool IsUpToDate();

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/packages-create");
	}
private:
	wxString GetInner(const wxString &def);

	long numProcedures, numFunctions, numVariables;
	wxString body, header;
};

class edbPackageObject : public pgSchemaObject
{
public:
	edbPackageObject(edbPackage *newPackage, pgaFactory &factory, const wxString &newName = wxT(""))
		: pgSchemaObject(newPackage->GetSchema(), factory, newName)
	{
		package = newPackage;
	}
	virtual edbPackage *GetPackage() const
	{
		return package;
	}
	OID GetPackageOid() const
	{
		return package->GetOid();
	}
	wxString GetPackageOidStr() const
	{
		return NumToStr(package->GetOid()) + wxT("::oid");
	}

	bool CanCreate()
	{
		return false;
	}
	bool CanEdit()
	{
		return false;
	}
	bool CanDrop()
	{
		return false;
	}
	bool CanDropCascaded()
	{
		return false;
	}

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return false;
	}
	bool HasReferences()
	{
		return false;
	}

protected:
	edbPackage *package;
};

class edbPackageCollection : public pgSchemaObjCollection
{
public:
	edbPackageCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

class edbPackageObjCollection : public pgSchemaObjCollection
{
public:
	edbPackageObjCollection(pgaFactory *factory, edbPackage *_package)
		: pgSchemaObjCollection(factory, _package->GetSchema())
	{
		iSetOid(_package->GetOid());
		package = _package;
	}
	virtual edbPackage *GetPackage() const
	{
		return package;
	}

	virtual bool CanCreate()
	{
		return false;
	}

protected:
	edbPackage *package;
};

class edbPackageObjFactory : public pgSchemaObjFactory
{
public:
	edbPackageObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
		: pgSchemaObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};

#endif
