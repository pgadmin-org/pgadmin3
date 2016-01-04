//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// factory.h - Object classes factory
//
//////////////////////////////////////////////////////////////////////////

#ifndef FACTORY_H
#define FACTORY_H

// wxWindows headers
#include <wx/wx.h>

#include "frm/menu.h"

#ifdef WIN32
#ifndef __GNUC__
#pragma warning(disable:4183)
#endif
#endif

class pgObject;
class frmMain;
class dlgProperty;
class ctlTree;
class pgCollection;
class pgSchema;
class pgaCollectionFactory;


class pgaFactory
{
public:
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent) = 0;
	virtual pgObject *CreateObjects(pgCollection  *obj, ctlTree *browser, const wxString &restr = wxEmptyString)
	{
		return 0;
	}
	virtual pgCollection *CreateCollection(pgObject *obj) = 0;
	virtual bool IsCollection()
	{
		return false;
	}
	virtual void AppendMenu(wxMenu *menu);
	bool IsCollectionFor(pgaFactory &f)
	{
		return f.GetCollectionFactory() == (pgaCollectionFactory *)this;
	}
	bool WantSmallIcon();

	static pgaFactory *GetFactory(int id);
	static pgaFactory *GetFactory(const wxString &name);
	static pgaFactory *GetFactoryByMetaType(const int type);
	int GetId()
	{
		return id;
	}
	int GetMetaType();
	wxChar *GetTypeName()
	{
		return typeName;
	}
	wxChar *GetNewString()
	{
		return newString;
	}
	wxChar *GetNewLongString()
	{
		return newLongString;
	}
	pgaCollectionFactory *GetCollectionFactory()
	{
		return collectionFactory;
	}

	int GetIconId();
	static void RegisterMenu(wxWindow *w, wxObjectEventFunction func);
	const wxImage &GetImage() const
	{
		return image;
	}

	static void RealizeImages();

protected:
	pgaFactory(const wxChar *tn = 0, const wxChar *ns = 0, const wxChar *nls = 0, wxImage *img = 0, wxImage *imgSm = 0);

	int addIcon(wxImage *img);

	int id, metaType;
	wxChar *typeName;
	wxChar *newString, *newLongString;
	int iconId, smallIconId;
	wxImage image;

	pgaCollectionFactory *collectionFactory;
	friend class pgaCollectionFactory;
};


class pgaCollectionFactory : public pgaFactory
{
public:
	pgaCollectionFactory(pgaFactory *f, const wxChar *tn = 0, wxImage *img = 0, wxImage *imgSm = 0);
	wxChar *GetItemTypeName()
	{
		return itemFactory->GetTypeName();
	}
	pgaFactory *GetItemFactory()
	{
		return itemFactory;
	}
	pgObject *CreateObjects(pgCollection  *obj, ctlTree *browser, const wxString &restr = wxEmptyString);

protected:
	virtual bool IsCollection()
	{
		return true;
	}
	dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgCollection *CreateCollection(pgObject *obj)
	{
		return 0;
	};

	pgaFactory *itemFactory;
};


class actionFactory;
class menuFactory;
class menuFactoryList : public wxArrayPtrVoid
{
public:
	~menuFactoryList();

	void CheckMenu(pgObject *obj, wxMenuBar *menubar, ctlMenuToolbar *toolbar);
	void AppendEnabledMenus(wxMenuBar *menuBar, wxMenu *treeContextMenu);
	actionFactory *GetFactory(int id, bool actionOnly = true);
	void RegisterMenu(wxWindow *w, wxObjectEventFunction func);
	void EnableSubmenu(wxMenuBar *menubar, int id);

private:
	void Add(menuFactory *f)
	{
		wxArrayPtrVoid::Add(f);
	}
	friend class menuFactory;
};


class menuFactory
{
public:
	virtual ~menuFactory();
	virtual bool IsAction()
	{
		return false;
	}
	virtual bool IsSubmenu()
	{
		return false;
	}

protected:
	menuFactory(menuFactoryList *list);
};


class actionFactory : public menuFactory
{
public:
	virtual bool IsAction()
	{
		return true;
	}
	virtual wxWindow *StartDialog(frmMain *form, pgObject *obj) = 0;
	virtual bool CheckEnable(pgObject *obj)
	{
		return true;
	}
	virtual bool CheckChecked(pgObject *obj)
	{
		return true;
	}
	bool GetContext()
	{
		return context;
	}
	int GetId()
	{
		return id;
	}

protected:
	actionFactory(menuFactoryList *list);

	int id;
	bool context;

	friend class menuFactoryList;
};


class contextActionFactory : public actionFactory
{
protected:
	contextActionFactory(menuFactoryList *list) : actionFactory(list)
	{
		context = true;
	}
};

class submenuFactory : public contextActionFactory
{
public:
	submenuFactory(menuFactoryList *list) : contextActionFactory(list) {};
	wxWindow *StartDialog(frmMain *form, pgObject *obj)
	{
		return 0;
	};
	virtual bool IsSubmenu()
	{
		return true;
	}
};

class separatorFactory : public menuFactory
{
public:
	separatorFactory(menuFactoryList *list) : menuFactory(list) {}
};
#endif
