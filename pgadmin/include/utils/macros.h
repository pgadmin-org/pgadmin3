//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// macros.h - Query SQL macros
//
//////////////////////////////////////////////////////////////////////////

#ifndef MACROS_H
#define MACROS_H

#include <wx/wx.h>
#include <wx/treectrl.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

class queryMacroItem
{
public:
	queryMacroItem(const wxString newKey, const wxString newTitle, const wxString newQuery, const int newId = -1);
	
	wxString GetKey() { return key; };
	wxString GetName() { return name; };
	wxString GetQuery() { return query; };
	int GetId() { return id; };

	void AppendToMenu(wxMenu *menu, int newId);
	void Update(const wxString &newName, const wxString &newQuery);

protected:
	int id;
	wxString key, name, query;
};

WX_DEFINE_ARRAY_PTR(queryMacroItem *, queryMacroArray);
class queryMacroList
{
public:
	queryMacroList(){};
	queryMacroList(xmlTextReaderPtr reader);

	int AppendAllToMenu(wxMenu *menu, int startId);

	queryMacroItem *FindMacro(int id);
	queryMacroItem *FindMacro(const wxString &key);

	void AddNewMacro(const wxString &key, const wxString &name, const wxString& query);
	void AddOrUpdateMacro(const wxString &key, const wxString &name, const wxString& query);
	bool DelMacro(int id);
	bool DelMacro(const wxString &key);

	void saveList(xmlTextWriterPtr writer);

	~queryMacroList();
protected:
	queryMacroArray macros;
};

class queryMacroFileProvider
{
public:
	static queryMacroList *LoadMacros(bool emptyOnFailure = false);
	static void SaveMacros(queryMacroList *macros);
};

#endif /* MACROS_H */

