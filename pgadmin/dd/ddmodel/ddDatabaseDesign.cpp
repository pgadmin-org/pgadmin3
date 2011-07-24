//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddDatabaseDesign.cpp - Manages all design related info and contains all model(s) and tables.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// libxml2 headers
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

// App headers
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/wxhotdraw/tools/wxhdSelectionTool.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "dd/ddmodel/ddDrawingEditor.h"
#include "dd/dditems/figures/xml/ddXmlStorage.h"


ddDatabaseDesign::ddDatabaseDesign(wxWindow *parent)
{
	draw = new ddDrawingEditor(parent, this);
	tool = new wxhdSelectionTool(draw);
	draw->setTool(tool);
}

ddDatabaseDesign::~ddDatabaseDesign()
{
	if(draw)
		delete draw;
}

wxhdDrawingEditor *ddDatabaseDesign::getEditor()
{
	return draw;
}

wxhdDrawingView *ddDatabaseDesign::getView()
{
	return draw->view();
}

void ddDatabaseDesign::addTable(wxhdIFigure *figure)
{
	draw->view()->add(figure);
}

void ddDatabaseDesign::removeTable(wxhdIFigure *figure)
{
	draw->view()->remove(figure);
}

void ddDatabaseDesign::setTool(wxhdITool *tool)
{
	draw->setTool(tool);
}

void ddDatabaseDesign::refreshDraw()
{
	draw->view()->Refresh();
}

void ddDatabaseDesign::eraseModel()
{
	draw->view()->removeAll();
}

bool ddDatabaseDesign::validateModel(wxString &errors)
{
	bool out = true;

	wxhdIteratorBase *iterator = draw->model()->figuresEnumerator();
	wxhdIFigure *tmpFigure;
	ddTableFigure *table;

	while(iterator->HasNext())
	{
		tmpFigure = (wxhdIFigure *)iterator->Next();
		if(tmpFigure->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmpFigure;
			if(!table->validateTable(errors))
			{
				out = false;
			}
		}
	}
	delete iterator;

	return out;
}

wxString ddDatabaseDesign::generateModel()
{
	wxString out;
	wxhdIteratorBase *iterator = draw->model()->figuresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table;
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			out += wxT(" \n");
			table = (ddTableFigure *)tmp;
			out += wxT("--\n-- ");
			out += _("Generating SQL for table: ");
			out += table->getTableName();
			out += wxT(" \n--\n");
			out += table->generateSQL();
			out += wxT(" \n");
			out += wxT(" \n");
		}
	}
	delete iterator;
	return out;
}

wxString ddDatabaseDesign::getNewTableName()
{
	wxString out, tmpStr;
	wxhdIteratorBase *iterator = draw->model()->figuresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table;
	int indx = 0;
	bool repeat;
	do
	{
		repeat = false;
		iterator->ResetIterator();
		while(iterator->HasNext())
		{
			tmp = (wxhdIFigure *)iterator->Next();
			if(tmp->getKindId() == DDTABLEFIGURE)
			{
				table = (ddTableFigure *)tmp;
				if(indx == 0)
					tmpStr = _("NewTable");
				else
					tmpStr = wxString::Format(_("NewTable%d"), indx);

				if(table->getTableName().Contains(tmpStr))
				{
					indx++;
					repeat = true;
					break;
				}
			}
		}
	}
	while(repeat);
	delete iterator;
	out = wxString::Format(_("NewTable%d"), indx);
	return out;
}

ddTableFigure *ddDatabaseDesign::getSelectedTable()
{
	wxhdIteratorBase *iterator = draw->model()->figuresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table = 0L;
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if (tmp->isSelected() && tmp->getKindId() == DDTABLEFIGURE)
			table = (ddTableFigure *)tmp;
	}
	delete iterator;
	return table;
}

ddTableFigure *ddDatabaseDesign::getTable(wxString tableName)
{
	ddTableFigure *out = NULL;
	wxhdIteratorBase *iterator = draw->model()->figuresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table;
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			if(table->getTableName().Contains(tableName))
			{
				out = table;
			}
		}
	}
	delete iterator;
	return out;
}

#define XML_FROM_WXSTRING(s) ((xmlChar *)(const char *)s.mb_str(wxConvUTF8))
#define WXSTRING_FROM_XML(s) wxString((char *)s, wxConvUTF8)

bool ddDatabaseDesign::writeXmlModel(wxString file)
{
	int rc;

	xmlWriter = xmlNewTextWriterFilename(file.mb_str(wxConvUTF8), 0);
	if (xmlWriter == NULL)
	{
		wxMessageBox(_("Failed to write the model file!"), _("Error"), wxICON_ERROR);
		return false;
	}

	rc = xmlTextWriterStartDocument(xmlWriter, NULL, "UTF-8" , NULL);
	if(rc < 0)
	{
		wxMessageBox(_("Failed to write the model file!"), _("Error"), wxICON_ERROR);
		return false;
	}

	ddXmlStorage::StartModel(xmlWriter, this);
	//initialize IDs of tables
	mappingNameToId.clear();
	wxhdIteratorBase *iterator = draw->model()->figuresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table;
	int nextID = 10;

	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			mappingNameToId[table->getTableName()] = wxString::Format(wxT("TID%d"), nextID);
			nextID += 10;
		}
	}
	delete iterator;


	//Create table xml info
	iterator = draw->model()->figuresEnumerator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			ddXmlStorage::Write(xmlWriter, table);
		}
	}
	delete iterator;


	//Create relationships xml info
	ddRelationshipFigure *relationship;
	iterator = draw->model()->figuresEnumerator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
		{
			relationship = (ddRelationshipFigure *)tmp;
			ddXmlStorage::Write(xmlWriter, relationship);
		}
	}
	delete iterator;

	ddXmlStorage::EndModel(xmlWriter);

	rc = xmlTextWriterEndDocument(xmlWriter);
	if(rc < 0)
	{
		wxMessageBox(_("Failed to write the model file!"), _("Error"), wxICON_ERROR);
		return false;
	}

	xmlFreeTextWriter(xmlWriter);
	return true;
}

bool ddDatabaseDesign::readXmlModel(wxString file)
{
	eraseModel();
	mappingIdToName.clear();

	//Initial Parse Model
	xmlTextReaderPtr reader = xmlReaderForFile(file.mb_str(wxConvUTF8), NULL, 0);
	if (!reader)
	{
		wxMessageBox(_("Failed to load model file!"), _("Error"), wxICON_ERROR);
		return false;
	}

	ddXmlStorage::setModel(this);
	ddXmlStorage::initialModelParse(reader);

	//Parse Model
	xmlReaderNewFile(reader, file.mb_str(wxConvUTF8), NULL, 0);
	ddXmlStorage::setModel(this);
	if(!ddXmlStorage::Read(reader))
	{
		wxMessageBox(_("Failed to load model file!"), _("Error"), wxICON_ERROR);
		return false;
	}
	xmlFreeTextReader(reader);
	return true;
}

wxString ddDatabaseDesign::getTableId(wxString tableName)
{
	return mappingNameToId[tableName];
}

void ddDatabaseDesign::addTableToMapping(wxString IdKey, wxString tableName)
{
	mappingIdToName[IdKey] = tableName;
}

wxString ddDatabaseDesign::getTableName(wxString Id)
{
	wxString tableName = wxEmptyString;

	tablesMappingHashMap::iterator it;
	for (it = mappingIdToName.begin(); it != mappingIdToName.end(); ++it)
	{
		wxString key = it->first;
		if(key.IsSameAs(Id, false))
		{
			tableName = it->second;
			break;
		}
	}
	return tableName;
}
