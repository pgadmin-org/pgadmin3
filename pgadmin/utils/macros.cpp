//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// macros.cpp - Query macros
//
//////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>

#include "pgAdmin3.h"
#include "utils/favourites.h"
#include "utils/macros.h"
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

queryMacroItem::queryMacroItem(const wxString newKey, const wxString newName, const wxString newQuery, const int newId)
{
	key = newKey;
	name = newName;
	query = newQuery;
	id = newId;
}

void queryMacroItem::AppendToMenu(wxMenu *menu, int newId)
{
	id = newId;
	menu->Append(id, name + wxT("\t") + key, query);
}

void queryMacroItem::Update(const wxString &newName, const wxString &newQuery)
{
	name = newName;
	query = newQuery;
}

queryMacroList::queryMacroList(xmlTextReaderPtr reader)
{
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

		xmlChar *ckey = xmlTextReaderGetAttribute(reader, XML_STR("key"));
		if (!ckey)
			continue;
		wxString key = WXSTRING_FROM_XML(ckey);
		xmlFree(ckey);

		xmlChar *cname = xmlTextReaderGetAttribute(reader, XML_STR("name"));
		if (!cname)
			continue;
		wxString name = WXSTRING_FROM_XML(cname);
		xmlFree(cname);

		if (nodename == wxT("macro"))
		{
			xmlChar *cquery = xmlTextReaderReadString(reader);
			if (!cquery)
				continue;
			wxString query = WXSTRING_FROM_XML(cquery);
			xmlFree(cquery);
			macros.Add(new queryMacroItem(key, name, query));
			SkipToEndElement(reader);
		}
	}
}

queryMacroList::~queryMacroList()
{
	WX_CLEAR_ARRAY(macros);
}

int queryMacroList::AppendAllToMenu(wxMenu *menu, int startId)
{
	int id = startId;
	size_t i;
	
	for (i = 0; i < macros.GetCount(); i++)
	{
		macros.Item(i)->AppendToMenu(menu, id);
		id++;
	}
	return id;
}

void queryMacroList::AddNewMacro(const wxString &key, const wxString &name, const wxString& query)
{
	macros.Add(new queryMacroItem(key, name, query));
}

void queryMacroList::AddOrUpdateMacro(const wxString &key, const wxString &name, const wxString &query)
{
	queryMacroItem *item = FindMacro(key);
	if (item != NULL)
	{
		item->Update(name, query);
	}
	else
	{
		AddNewMacro(key, name, query);
	}
}

bool queryMacroList::DelMacro(int id)
{
	size_t i;
	
	for (i = 0; i < macros.GetCount(); i++)
	{
		if (macros.Item(i)->GetId() == id)
		{
			queryMacroItem *itm = macros.Item(i);
			delete itm;
			macros.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool queryMacroList::DelMacro(const wxString &key)
{
	size_t i;

	for (i = 0; i < macros.GetCount(); i++)
	{
		if (macros.Item(i)->GetKey() == key)
		{
			queryMacroItem *itm = macros.Item(i);
			delete itm;
			macros.RemoveAt(i);
			return true;
		}
	}
	return false;
}

queryMacroItem *queryMacroList::FindMacro(int id)
{
	size_t i;
	
	for (i = 0; i < macros.GetCount(); i++)
	{
		if (macros.Item(i)->GetId() == id)
			return macros.Item(i);
	}
	
	return NULL;
}

queryMacroItem *queryMacroList::FindMacro(const wxString &key)
{
	size_t i;

	for (i = 0; i < macros.GetCount(); i++)
	{
		if (macros.Item(i)->GetKey() == key)
			return macros.Item(i);
	}

	return NULL;
}

void queryMacroList::saveList(xmlTextWriterPtr writer)
{
	size_t i;
	
	for (i = 0; i < macros.GetCount(); i++)
	{
		xmlTextWriterStartElement(writer, XML_STR("macro"));
		xmlTextWriterWriteAttribute(writer, XML_STR("key"), 
			XML_FROM_WXSTRING(macros.Item(i)->GetKey()));
		xmlTextWriterWriteAttribute(writer, XML_STR("name"), 
			XML_FROM_WXSTRING(macros.Item(i)->GetName()));
		xmlTextWriterWriteString(writer, 
			XML_FROM_WXSTRING(macros.Item(i)->GetQuery()));
		xmlTextWriterEndElement(writer);
	}
}

//
// queryMacroFileProvider - load and save macros from a XML file in the users
//                              home directory
//
queryMacroList *queryMacroFileProvider::LoadMacros(bool emptyOnFailure)
{
	xmlTextReaderPtr reader;
	int ret;

	if (!wxFile::Access(sysSettings::GetConfigFile(sysSettings::PGAMACROS), wxFile::read))
		return emptyOnFailure ? (new queryMacroList()) : NULL;

	reader = xmlReaderForFile((const char *)sysSettings::GetConfigFile(sysSettings::PGAMACROS).mb_str(wxConvUTF8),NULL,0);
	if (!reader)
	{
		wxMessageBox(_("Failed to load macros file!"));
		return emptyOnFailure ? (new queryMacroList()) : NULL;
	}

	ret = xmlTextReaderRead(reader);
	if (ret != 1)
	{
		wxMessageBox(_("Failed to read macros file!"));
		return emptyOnFailure ? (new queryMacroList()) : NULL;
	}

	queryMacroList *f = (queryMacroList *)(new queryMacroList(reader));

	xmlTextReaderClose(reader);
	xmlFreeTextReader(reader);
	xmlCleanupParser();

	return f;
}

void queryMacroFileProvider::SaveMacros(queryMacroList *macros)
{
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename((const char *)sysSettings::GetConfigFile(sysSettings::PGAMACROS).mb_str(wxConvUTF8),0);
	if (!writer)
	{
		wxMessageBox(_("Failed to write to macros file!"));
		return;
	}
	xmlTextWriterSetIndent(writer, 1);

	if ((xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0) ||
		(xmlTextWriterStartElement(writer, XML_STR("macros")) < 0))
	{
		wxMessageBox(_("Failed to write to macros file!"));
		xmlFreeTextWriter(writer);
		return;
	}

	((queryMacroList *)macros)->saveList(writer);

	if (xmlTextWriterEndDocument(writer))
	{
		wxMessageBox(_("Failed to write to macros file!"));
	}

	xmlFreeTextWriter(writer);
}
