//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepSet.h - Slony-I Set property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPSETPROP
#define __DLG_REPSETPROP

#include "slony/dlgRepProperty.h"

class slCluster;
class slSet;

class dlgRepSet : public dlgRepProperty
{
public:
	dlgRepSet(pgaFactory *factory, frmMain *frame, slSet *set, slCluster *c);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-set");
	}

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject()
	{
		return (pgObject *)set;
	}

private:
	slSet *set;

	DECLARE_EVENT_TABLE()
};


class dlgRepSetMerge : public dlgRepProperty
{
public:
	dlgRepSetMerge(pgaFactory *factory, frmMain *frame, slSet *set);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-functions");
	}

	void CheckChange();
	wxString GetSql();
	pgObject *GetObject()
	{
		return (pgObject *)set;
	}
	pgObject *CreateObject(pgCollection *collection)
	{
		return 0;
	}

private:
	slSet *set;

	DECLARE_EVENT_TABLE()
};

class dlgRepSetMove : public dlgRepProperty
{
public:
	dlgRepSetMove(pgaFactory *f, frmMain *frame, slSet *set);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-functions");
	}

	void CheckChange();
	wxString GetSql();
	pgObject *GetObject()
	{
		return (pgObject *)set;
	}
	pgObject *CreateObject(pgCollection *collection)
	{
		return 0;
	}

private:
	slSet *set;

	DECLARE_EVENT_TABLE()
};

#endif
