//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// favourites.h - Query favourites
//
//////////////////////////////////////////////////////////////////////////

#ifndef FAVOURITES_H
#define FAVOURITES_H

#include <wx/wx.h>
#include <wx/treectrl.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

class queryFavouriteItem
{
public:
	queryFavouriteItem(const wxString newtitle, const wxString newcontents);

	wxString GetTitle()
	{
		return title;
	};
	void SetTitle(const wxString &newtitle)
	{
		if (!newtitle.IsEmpty()) title = newtitle;
	};

	int GetId()
	{
		return id;
	};
	wxString GetContents()
	{
		return contents;
	};

	wxTreeItemId &GetTreeId()
	{
		return treeid;
	};
	void SetTreeId(const wxTreeItemId &newtreeid)
	{
		treeid = newtreeid;
	};

	void AppendToMenu(wxMenu *menu, int newid);
protected:
	int id;
	wxString title, contents;
	wxTreeItemId treeid;
};


WX_DEFINE_ARRAY_PTR(queryFavouriteItem *, queryFavouriteArray);
class queryFavouriteFolder : public queryFavouriteItem
{
public:
	queryFavouriteFolder(wxString title = wxT(""));
	queryFavouriteFolder(xmlTextReaderPtr reader, wxString title);

	int AppendAllToMenu(wxMenu *menu, int startid);
	void AppendAllToTree(wxTreeCtrl *tree, const wxTreeItemId &parent, bool onlyfolders);
	bool DeleteTreeItem(const wxTreeItemId &treeitem);

	queryFavouriteItem *FindFavourite(int id);
	queryFavouriteItem *FindFavourite(const wxString &title);
	queryFavouriteItem *FindTreeItem(const wxTreeItemId &treeitem);

	void AddNewFavourite(const wxString &title, const wxString &contents);
	queryFavouriteFolder *AddNewFolder(const wxString &title);

	bool ContainsFolder(const wxString &title);

	void saveFolder(xmlTextWriterPtr writer);

	~queryFavouriteFolder();
protected:
	queryFavouriteArray favourites;
};

class queryFavouriteFileProvider
{
public:
	static queryFavouriteFolder *LoadFavourites(bool emptyonfailure = false);
	static void SaveFavourites(queryFavouriteFolder *favourites);
};

#endif
