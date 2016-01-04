//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddModelBrowser.h - Tables Tree of Database Designer.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDMODELBROWSER_H
#define DDMODELBROWSER_H

#include "dd/ddmodel/ddDatabaseDesign.h"

enum ddBrowser
{
	DD_BROWSER = 13000
};

enum ddBrowserImages
{
	DD_IMG_FIG_DATABASE = 0,
	DD_IMG_FIG_TABLE = 1
};

class ddModelBrowser : public wxTreeCtrl
{
public:
	ddModelBrowser(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, ddDatabaseDesign *design);
	~ddModelBrowser();
	wxTreeItemId &createRoot(wxString Name);
	void refreshFromModel();

private:
	void OnItemActivated(wxTreeEvent &event);
	void OnBeginDrag(wxTreeEvent &event);

	wxTreeItemId rootNode;
	ddDatabaseDesign *ownerDesign;
	wxImageList *imageList;

	DECLARE_EVENT_TABLE()
};
#endif
