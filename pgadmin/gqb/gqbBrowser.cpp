//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbBrowser.cpp - Tables Tree of GQB.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/imaglist.h>

// App headers
#include "gqb/gqbBrowser.h"
#include "gqb/gqbEvents.h"
#include "gqb/gqbSchema.h"
#include "gqb/gqbDatabase.h"
#include "gqb/gqbViewController.h"

// Images
#include "images/table-sm.pngc"
#include "images/view-sm.pngc"
#include "images/namespace-sm.pngc"
#include "images/namespaces.pngc"
#include "images/database-sm.pngc"
#include "images/catalog-sm.pngc"
#include "images/catalogs.pngc"
#include "images/catalogobject-sm.pngc"
#include "images/exttable-sm.pngc"	// Greenplum external tables

BEGIN_EVENT_TABLE(gqbBrowser, wxTreeCtrl)
	EVT_TREE_ITEM_ACTIVATED(GQB_BROWSER, gqbBrowser::OnItemActivated)
	EVT_TREE_BEGIN_DRAG(GQB_BROWSER, gqbBrowser::OnBeginDrag)
END_EVENT_TABLE()

gqbBrowser::gqbBrowser(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, gqbController *_controller)
	: wxTreeCtrl(parent, id, pos, size, style)
{
	controller = _controller;
	rootNode = (wxTreeItemId *)NULL;

	// Create normal images list of browser
	// Remember to update enum gqbImages in gqbBrowser.h if changing the images!!
	imageList = new wxImageList(16, 16);
	imageList->Add(*database_sm_png_ico);
	imageList->Add(*namespace_sm_png_ico);
	imageList->Add(*table_sm_png_ico);
	imageList->Add(*namespaces_png_ico);
	imageList->Add(*catalogs_png_ico);
	imageList->Add(*catalog_sm_png_ico);
	imageList->Add(*catalogobject_sm_png_ico);
	imageList->Add(*view_sm_png_ico);
	imageList->Add(*exttable_sm_png_ico);
	this->AssignImageList(imageList);
}


// Destructor
gqbBrowser::~gqbBrowser()
{
	this->DeleteAllItems();        // This remove and delete data inside tree's node
}


// Create root node
wxTreeItemId &gqbBrowser::createRoot(wxString &Name)
{
	rootNode = this->AddRoot(Name, 0, 0);
	catalogsNode = this->AppendItem(rootNode, _("Catalogs"), GQB_IMG_CATALOGS, GQB_IMG_CATALOGS, NULL);
	schemasNode = this->AppendItem(rootNode, _("Schemas"), GQB_IMG_NAMESPACES, GQB_IMG_NAMESPACES, NULL);
	return rootNode;
}


// Event activated when user double click on a item of tree
void gqbBrowser::OnItemActivated(wxTreeEvent &event)
{
	wxTreeItemId itemId = event.GetItem();
	gqbObject *object = (gqbObject *) GetItemData(itemId);
	if(object)
	{
		if (object->getType() == GQB_TABLE || object->getType() == GQB_VIEW)
		{
			gqbTable *item = (gqbTable *)  object;
			controller->addTableToModel(item, wxPoint(10, 10));
			controller->getView()->Refresh();
		}
		else if (GetChildrenCount(itemId) == 0 && object->getType() == GQB_SCHEMA)
		{
			gqbSchema *schema = (gqbSchema *)object;
			schema->createObjects(this, schema->getOid(), itemId, GQB_IMG_TABLE, GQB_IMG_VIEW, GQB_IMG_EXTTABLE);
		}
	}

}

void gqbBrowser::refreshTables(pgConn *connection)
{
	controller->emptyModel();
	this->DeleteAllItems();
	gqbDatabase *Data = new gqbDatabase(wxEmptyString, connection);
	Data->createObjects(this);
	this->Expand(rootNode);
}


void gqbBrowser::OnBeginDrag(wxTreeEvent &event)
{
	wxTreeItemId itemId = event.GetItem();

	// Simplest solution, simulate DnD but actually don't do it
	gqbObject *object = (gqbObject *) GetItemData(itemId);
	if(object != NULL && (object->getType() == GQB_TABLE || object->getType() == GQB_VIEW))
	{
		gqbTable *item = (gqbTable *) object;
		wxString tableName = item->getName();
		wxTextDataObject textData(tableName);
		wxDropSource dragSource(this);
		dragSource.SetData(textData);
		wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
		if(result == wxDragCopy)
		{
			controller->getView()->CalcUnscrolledPosition(xx, yy, &xx, &yy);
			gqbQueryObject *queryObj = controller->addTableToModel(item, wxPoint(xx, yy));
			controller->getView()->Refresh();
			if (queryObj)
			{
				controller->getView()->Update();
				controller->getView()->updateModelSize(queryObj, false);
			}
		}
	}
}
