//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgUserMapping.h PostgreSQL User Mapping
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGUSERMAPPING_H
#define PGUSERMAPPING_H

#include "pgForeignServer.h"

class pgCollection;
class pgUserMappingFactory : public pgForeignServerObjFactory
{
public:
	pgUserMappingFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgUserMappingFactory userMappingFactory;


class pgUserMapping : public pgForeignServerObject
{
public:
	pgUserMapping(pgForeignServer *newForeignServer, const wxString &newName = wxT(""));
	wxString GetTranslatedMessage(int kindOfMessage) const;

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return true;
	}
	bool CanCreate()
	{
		return true;
	}

	wxString GetUsr() const
	{
		return user;
	}
	void iSetUsr(const wxString &s)
	{
		user = s;
	}
	wxString GetOptions() const
	{
		return options;
	}
	wxString GetCreateOptions();
	void iSetOptions(const wxString &s)
	{
		options = s;
	}

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

private:
	wxString user, options;
};

class pgUserMappingObjFactory : public pgForeignServerObjFactory
{
public:
	pgUserMappingObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
		: pgForeignServerObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};


class pgUserMappingCollection : public pgCollection
{
public:
	pgUserMappingCollection(pgaFactory *factory, pgUserMapping *um);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
