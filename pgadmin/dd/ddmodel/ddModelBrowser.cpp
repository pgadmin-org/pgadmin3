//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddModelBrowser.cpp - Tables Tree of Database Designer.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/imaglist.h>

// App headers
#include "dd/ddmodel/ddModelBrowser.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/ddmodel/ddBrowserDataContainer.h"

// Images
#include "images/table-sm.pngc"
#include "images/database-sm.pngc"


BEGIN_EVENT_TABLE(ddModelBrowser, wxTreeCtrl)
	EVT_TREE_ITEM_ACTIVATED(DD_BROWSER, ddModelBrowser::OnItemActivated)
	EVT_TREE_BEGIN_DRAG(DD_BROWSER, ddModelBrowser::OnBeginDrag)
END_EVENT_TABLE()

ddModelBrowser::ddModelBrowser(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, ddDatabaseDesign *design)
	: wxTreeCtrl(parent, id, pos, size, style)
{
	ownerDesign = design;
	rootNode = (wxTreeItemId *)NULL;
	createRoot(_("Database Design"));

	// Create normal images list of browser
	// Remember to update enum gqbImages in ddModelBrowser.h if changing the images!!
	imageList = new wxImageList(16, 16);
	imageList->Add(*database_sm_png_ico);
	imageList->Add(*table_sm_png_ico);
	this->AssignImageList(imageList);
}


// Destructor
ddModelBrowser::~ddModelBrowser()
{
	this->DeleteAllItems();        // This remove and delete data inside tree's node
}


// Create root node
wxTreeItemId &ddModelBrowser::createRoot(wxString Name)
{
	rootNode = this->AddRoot(Name, DD_IMG_FIG_DATABASE, DD_IMG_FIG_DATABASE);
	return rootNode;
}


void ddModelBrowser::refreshFromModel()
{

	this->DeleteAllItems();
	createRoot(_("Database Design"));

	hdIteratorBase *iterator = ownerDesign->getEditor()->modelFiguresEnumerator();
	hdIFigure *tmpFigure;
	ddTableFigure *table;

	while(iterator->HasNext())
	{
		tmpFigure = (hdIFigure *)iterator->Next();
		if(tmpFigure->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmpFigure;
			this->AppendItem(rootNode, table->getTableName(), DD_IMG_FIG_TABLE, DD_IMG_FIG_TABLE, new ddBrowserDataContainer(table));
		}
	}
	delete iterator;

	this->Expand(rootNode);
	this->SortChildren(rootNode);
}

void ddModelBrowser::OnItemActivated(wxTreeEvent &event)
{
}

void ddModelBrowser::OnBeginDrag(wxTreeEvent &event)
{
	wxTreeItemId itemId = event.GetItem();

	// Simplest solution, simulate DnD but actually don't do it
	ddBrowserDataContainer *object = (ddBrowserDataContainer *) GetItemData(itemId);

	if(object != NULL && (object->getFigureKindId() == DDTABLEFIGURE))
	{
		ddTableFigure *item = (ddTableFigure *) object->getFigure();
		wxString tableName = item->getTableName();
		wxTextDataObject textData(tableName);
		wxDropSource dragSource(this);
		dragSource.SetData(textData);
		wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
		if(result != wxDragCopy)
		{
			wxMessageBox(wxT("Invalid kind of data during drag and drop operation"), wxT("Drag and drop error"), wxICON_ERROR | wxOK);
		}
	}
}
