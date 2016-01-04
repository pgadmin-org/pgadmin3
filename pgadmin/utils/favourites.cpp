//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// favourites.cpp - Query favourites
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

#include "pgAdmin3.h"
#include "utils/favourites.h"
#include "utils/sysSettings.h"

#include <wx/file.h>

//
// libxml convenience macros
//
#define XML_FROM_WXSTRING(s) ((const xmlChar *)(const char *)s.mb_str(wxConvUTF8))
#define WXSTRING_FROM_XML(s) wxString((char *)s, wxConvUTF8)
#define XML_STR(s) ((const xmlChar *)s)

//
// libxml convenience function
//
static void SkipToEndElement(xmlTextReaderPtr reader)
{
	while (xmlTextReaderRead(reader) == 1)
	{
		if (xmlTextReaderNodeType(reader) == 15)
			return;
	}
}


//
// queryFavouriteItem - base class representing a single favourite item, or
//                      acting as the base for folders.
//
queryFavouriteItem::queryFavouriteItem(const wxString newtitle, const wxString newcontents)
{
	title = newtitle;
	contents = newcontents;
	id = -1;
}

void queryFavouriteItem::AppendToMenu(wxMenu *menu, int newid)
{
	id = newid;
	menu->Append(id, title);
}


//
// queryFavouriteFolder - represents a folder containing zero or more favourites
//
queryFavouriteFolder::queryFavouriteFolder(wxString title) : queryFavouriteItem(title, wxT(""))
{
	id = -2;
}

queryFavouriteFolder::queryFavouriteFolder(xmlTextReaderPtr reader, wxString title) : queryFavouriteItem(title, wxT(""))
{
	id = -2;

	// Element of type <foo />, meaning empty folder
	if (xmlTextReaderIsEmptyElement(reader))
		return;

	while (xmlTextReaderRead(reader))
	{
		int type = xmlTextReaderNodeType(reader);

		if (type == 15)
			return; // Close on parent element
		if (xmlTextReaderNodeType(reader) != 1)
			continue; // Any unknown element type

		wxString nodename = WXSTRING_FROM_XML(xmlTextReaderConstName(reader));

		xmlChar *ctitle = xmlTextReaderGetAttribute(reader, XML_STR("title"));
		if (!ctitle)
			continue; // We ignore nodes without title
		wxString title = WXSTRING_FROM_XML(ctitle);
		xmlFree(ctitle);

		if (nodename == wxT("favourite"))
		{
			xmlChar *cont = xmlTextReaderReadString(reader);
			if (!cont)
				continue; // No contents, so ignore node

			favourites.Add(new queryFavouriteItem(title, WXSTRING_FROM_XML(cont)));
			xmlFree(cont);
			SkipToEndElement(reader);
		}
		else if (nodename == wxT("folder"))
		{
			favourites.Add(new queryFavouriteFolder(reader, title));
		}
	}
}

queryFavouriteFolder::~queryFavouriteFolder()
{
	WX_CLEAR_ARRAY(favourites);
}

void queryFavouriteFolder::AddNewFavourite(const wxString &title, const wxString &contents)
{
	favourites.Add(new queryFavouriteItem(title, contents));
}

queryFavouriteFolder *queryFavouriteFolder::AddNewFolder(const wxString &title)
{
	queryFavouriteFolder *fld = new queryFavouriteFolder(title);
	favourites.Add(fld);
	return fld;
}

int queryFavouriteFolder::AppendAllToMenu(wxMenu *menu, int startid)
{
	int id = startid;
	size_t i;

	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetId() == -2) // id=-2 reserved for submenus
		{
			queryFavouriteFolder *fld = (queryFavouriteFolder *)favourites.Item(i);

			wxMenu *newmenu = new wxMenu();
			id = fld->AppendAllToMenu(newmenu, id);
			menu->Append(-1, fld->title, newmenu);
		}
		else
		{
			favourites.Item(i)->AppendToMenu(menu, id);
			id++;
		}
	}
	return id;
}

void queryFavouriteFolder::AppendAllToTree(wxTreeCtrl *tree, const wxTreeItemId &parent, bool onlyfolders)
{
	size_t i;

	treeid = parent;

	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetId() == -2) // id=-1 reserved for submenus
		{
			queryFavouriteFolder *fld = (queryFavouriteFolder *)favourites.Item(i);

			wxTreeItemId newtreeitem = tree->AppendItem(parent, fld->title, onlyfolders ? 0 : 1);
			fld->AppendAllToTree(tree, newtreeitem, onlyfolders);
			tree->Expand(newtreeitem);
		}
		else
		{
			if (!onlyfolders)
				favourites.Item(i)->SetTreeId(tree->AppendItem(parent, favourites.Item(i)->GetTitle(), 0));
		}
	}
}

bool queryFavouriteFolder::DeleteTreeItem(const wxTreeItemId &treeitem)
{
	if (treeid == treeitem)
		/* Can't delete ourselves, parent should handle that. */
		return false;

	size_t i;
	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetTreeId() == treeitem)
		{
			queryFavouriteItem *itm = favourites.Item(i);
			delete itm;
			favourites.RemoveAt(i);
			return true;
		}
		if (favourites.Item(i)->GetId() == -2)
		{
			queryFavouriteFolder *subfolder = (queryFavouriteFolder *)favourites.Item(i);
			if (subfolder->DeleteTreeItem(treeitem))
				return true;
		}
	}
	return false;
}

queryFavouriteItem *queryFavouriteFolder::FindFavourite(int id)
{
	size_t i;
	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetId() == id)
			return favourites.Item(i);
		if (favourites.Item(i)->GetId() == -2)
		{
			queryFavouriteFolder *subfolder = (queryFavouriteFolder *)favourites.Item(i);
			queryFavouriteItem *match = subfolder->FindFavourite(id);
			if (match != NULL)
				return match;
		}
	}
	return NULL;
}

queryFavouriteItem *queryFavouriteFolder::FindFavourite(const wxString &title)
{
	size_t i;
	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetId() == -2)
		{
			queryFavouriteFolder *subfolder = (queryFavouriteFolder *)favourites.Item(i);
			queryFavouriteItem *match = subfolder->FindFavourite(title);
			if (match != NULL)
				return match;
		}
		else if (favourites.Item(i)->GetTitle() == title)
			return favourites.Item(i);
	}
	return NULL;
}

queryFavouriteItem *queryFavouriteFolder::FindTreeItem(const wxTreeItemId &treeitem)
{
	if (treeid == treeitem)
		return this;

	size_t i;
	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetId() == -2)
		{
			queryFavouriteFolder *subfolder = (queryFavouriteFolder *)favourites.Item(i);
			queryFavouriteItem *match = subfolder->FindTreeItem(treeitem);
			if (match != NULL)
				return match;
		}
		else if (favourites.Item(i)->GetTreeId() == treeitem)
			return favourites.Item(i);
	}
	return NULL;
}

bool queryFavouriteFolder::ContainsFolder(const wxString &title)
{
	size_t i;
	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetId() == -2)
		{
			if (favourites.Item(i)->GetTitle() == title)
				return true;
		}
	}
	return false;
}

void queryFavouriteFolder::saveFolder(xmlTextWriterPtr writer)
{
	size_t i;

	for (i = 0; i < favourites.GetCount(); i++)
	{
		if (favourites.Item(i)->GetId() == -2)
		{
			queryFavouriteFolder *subfolder = (queryFavouriteFolder *)favourites.Item(i);

			xmlTextWriterStartElement(writer, XML_STR("folder"));
			xmlTextWriterWriteAttribute(writer, XML_STR("title"), XML_FROM_WXSTRING(subfolder->title));
			subfolder->saveFolder(writer);
			xmlTextWriterEndElement(writer);
		}
		else
		{
			xmlTextWriterStartElement(writer, XML_STR("favourite"));
			xmlTextWriterWriteAttribute(writer, XML_STR("title"), XML_FROM_WXSTRING(favourites.Item(i)->GetTitle()));
			xmlTextWriterWriteString(writer, XML_FROM_WXSTRING(favourites.Item(i)->GetContents()));
			xmlTextWriterEndElement(writer);
		}
	}
}

//
// queryFavouriteFileProvider - load and save favourites from a XML file in the users
//                              home directory
//
queryFavouriteFolder *queryFavouriteFileProvider::LoadFavourites(bool emptyonfailure)
{
	xmlTextReaderPtr reader;
	int ret;

	if (!wxFile::Access(settings->GetFavouritesFile(), wxFile::read))
		return emptyonfailure ? (new queryFavouriteFolder()) : NULL;

	reader = xmlReaderForFile((const char *)settings->GetFavouritesFile().mb_str(wxConvUTF8), NULL, 0);
	if (!reader)
	{
		wxMessageBox(_("Failed to load favourites file!"));
		return emptyonfailure ? (new queryFavouriteFolder()) : NULL;
	}

	ret = xmlTextReaderRead(reader);
	if (ret != 1)
	{
		wxMessageBox(_("Failed to read favourites file!"));
		return emptyonfailure ? (new queryFavouriteFolder()) : NULL;
	}

	queryFavouriteFolder *f = (queryFavouriteFolder *)(new queryFavouriteFolder(reader, wxT("")));

	xmlTextReaderClose(reader);
	xmlFreeTextReader(reader);
	xmlCleanupParser();

	return f;
}

void queryFavouriteFileProvider::SaveFavourites(queryFavouriteFolder *favourites)
{
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename((const char *)settings->GetFavouritesFile().mb_str(wxConvUTF8), 0);
	if (!writer)
	{
		wxMessageBox(_("Failed to write to favourites file!"));
		return;
	}
	xmlTextWriterSetIndent(writer, 1);

	if ((xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0) ||
	        (xmlTextWriterStartElement(writer, XML_STR("favourites")) < 0))
	{
		wxMessageBox(_("Failed to write to favourites file!"));
		xmlFreeTextWriter(writer);
		return;
	}

	((queryFavouriteFolder *)favourites)->saveFolder(writer);

	if (xmlTextWriterEndDocument(writer) < 0)
	{
		wxMessageBox(_("Failed to write to favourites file!"));
	}

	xmlFreeTextWriter(writer);
}
