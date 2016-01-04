//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbBrowser.h - Tables Tree of GQB.
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBBROWSER_H
#define GQBBROWSER_H

enum gqbImages
{
	GQB_IMG_DATABASE = 0,
	GQB_IMG_NAMESPACE = 1,
	GQB_IMG_TABLE = 2,
	GQB_IMG_NAMESPACES = 3,
	GQB_IMG_CATALOGS = 4,
	GQB_IMG_CATALOG = 5,
	GQB_IMG_CATALOG_OBJ = 6,
	GQB_IMG_VIEW = 7,
	GQB_IMG_EXTTABLE = 8
};

class gqbController;

class gqbBrowser : public wxTreeCtrl
{
public:
	gqbBrowser(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, gqbController *_controller);
	~gqbBrowser();
	wxTreeItemId &createRoot(wxString &Name);
	wxTreeItemId &getCatalogRootNode()
	{
		return catalogsNode;
	}
	wxTreeItemId &getTablesRootNode()
	{
		return schemasNode;
	}
	void refreshTables(pgConn *connection);
	void setDnDPoint(int x, int y)
	{
		xx = x;
		yy = y;
	};

private:
	enum typeSchema						// GQB-TODO: DELETE from here should be locate at gqbDatabase
	{
		GQB_CATALOG,
		GQB_OTHER
	};

	wxTreeItemId rootNode, catalogsNode, schemasNode;
	void OnItemActivated(wxTreeEvent &event);
	void OnBeginDrag(wxTreeEvent &event);
	wxString NumToStr(OID value);
	gqbController *controller;			//Allow access to controller functions like add table to model
	wxImageList *imageList;
	int xx, yy;

	DECLARE_EVENT_TABLE()
};
#endif
