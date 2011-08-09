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
#include "dd/ddmodel/ddModelBrowser.h"


ddDatabaseDesign::ddDatabaseDesign(wxWindow *parent, wxWindow *frmOwner)
{
	editor = new ddDrawingEditor(parent, frmOwner, this);
	attachedBrowser = NULL;
}

ddDatabaseDesign::~ddDatabaseDesign()
{
	if(editor)
		delete editor;
}

ddDrawingEditor *ddDatabaseDesign::getEditor()
{
	return editor;
}

wxhdDrawingView *ddDatabaseDesign::getView(int diagramIndex)
{
	return editor->getExistingView(diagramIndex);
}

void ddDatabaseDesign::registerBrowser(ddModelBrowser *browser)
{
	attachedBrowser = browser;
}

void ddDatabaseDesign::addTableToModel(wxhdIFigure *figure)
{
	editor->addModelFigure(figure);
	if(attachedBrowser)
	{
		attachedBrowser->refreshFromModel();
	}
}

void ddDatabaseDesign::addTableToView(int diagramIndex, wxhdIFigure *figure)
{
	editor->addDiagramFigure(diagramIndex, figure);
	if(attachedBrowser)
	{
		attachedBrowser->refreshFromModel();
	}
}

void ddDatabaseDesign::refreshBrowser()
{
	if(attachedBrowser)
	{
		attachedBrowser->refreshFromModel();
	}
}

void ddDatabaseDesign::removeTable(int diagramIndex, wxhdIFigure *figure)
{
	editor->removeDiagramFigure(diagramIndex, figure);
}

void ddDatabaseDesign::refreshDraw(int diagramIndex)
{
	editor->getExistingView(diagramIndex)->Refresh();
}

void ddDatabaseDesign::eraseDiagram(int diagramIndex)
{
	editor->getExistingDiagram(diagramIndex)->removeAllFigures();
}

void ddDatabaseDesign::emptyModel()
{
	editor->deleteAllModelFigures();
	if(attachedBrowser)
	{
		attachedBrowser->refreshFromModel();
	}
}

bool ddDatabaseDesign::validateModel(wxString &errors)
{
	bool out = true;

	wxhdIteratorBase *iterator = editor->modelFiguresEnumerator();
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
	wxhdIteratorBase *iterator = editor->modelFiguresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table;
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Create sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			out += wxT(" \n");
			table = (ddTableFigure *)tmp;
			out += table->generateSQLCreate();
			out += wxT(" \n");
		}
	}

	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Pk sentence for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	out += wxT(" \n");
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterPks();
		}
	}

	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Fk sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	out += wxT(" \n");
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterFks();
		}
	}

	delete iterator;
	return out;
}

wxString ddDatabaseDesign::generateDiagram(int diagramIndex)
{
	wxString out;
	wxhdIteratorBase *iterator = editor->getExistingDiagram(diagramIndex)->figuresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table;
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Create sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			out += wxT(" \n");
			table = (ddTableFigure *)tmp;
			out += table->generateSQLCreate();
			out += wxT(" \n");
		}
	}

	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Pk sentence for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	out += wxT(" \n");
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterPks();
		}
	}

	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Fk sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	out += wxT(" \n");
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterFks();
		}
	}

	delete iterator;
	return out;
}

wxString ddDatabaseDesign::getNewTableName()
{
	wxString out, tmpStr;
	wxhdIteratorBase *iterator = editor->modelFiguresEnumerator();
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

ddTableFigure *ddDatabaseDesign::getSelectedTable(int diagramIndex)
{
	wxhdIteratorBase *iterator = editor->getExistingDiagram(diagramIndex)->figuresEnumerator();
	wxhdIFigure *tmp;
	ddTableFigure *table = NULL;
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if (tmp->isSelected(diagramIndex) && tmp->getKindId() == DDTABLEFIGURE)
			table = (ddTableFigure *)tmp;
	}
	delete iterator;
	return table;
}

ddTableFigure *ddDatabaseDesign::getTable(wxString tableName)
{
	ddTableFigure *out = NULL;
	wxhdIteratorBase *iterator = editor->modelFiguresEnumerator();
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
	else
	{
		ddXmlStorage::StartModel(xmlWriter, this);
		//initialize IDs of tables
		mappingNameToId.clear();
		wxhdIteratorBase *iterator = editor->modelFiguresEnumerator();
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
		iterator = editor->modelFiguresEnumerator();
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
		iterator = editor->modelFiguresEnumerator();
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

		//Create Diagrams xml info

		ddXmlStorage::StarDiagrams(xmlWriter);

		iterator = editor->diagramsEnumerator();
		wxhdDrawing *tmpDiagram;

		while(iterator->HasNext())
		{
			tmpDiagram = (wxhdDrawing *)iterator->Next();
			ddXmlStorage::WriteLocal(xmlWriter, tmpDiagram);
		}
		delete iterator;

		ddXmlStorage::EndDiagrams(xmlWriter);

		//End model xml info
		ddXmlStorage::EndModel(xmlWriter);
		xmlTextWriterEndDocument(xmlWriter);
		xmlFreeTextWriter(xmlWriter);
		return true;
	}
	return false;
}

bool ddDatabaseDesign::readXmlModel(wxString file, ctlAuiNotebook *notebook)
{
	emptyModel();

	mappingIdToName.clear();
	//Initial Parse Model
	xmlTextReaderPtr reader = xmlReaderForFile(file.mb_str(wxConvUTF8), NULL, 0);
	ddXmlStorage::setModel(this);
	ddXmlStorage::setModel(this);
	ddXmlStorage::initialModelParse(reader);

	//Parse Model
	xmlReaderNewFile(reader, file.mb_str(wxConvUTF8), NULL, 0);
	ddXmlStorage::setModel(this);
	ddXmlStorage::setNotebook(notebook);

	if(!ddXmlStorage::Read(reader))
	{
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

wxhdDrawing *ddDatabaseDesign::createDiagram(wxWindow *owner, wxString name, bool fromXml)
{
	wxhdDrawing *drawing = editor->createDiagram(owner, fromXml);
	drawing->setName(name);
	return drawing;
}

void ddDatabaseDesign::deleteDiagram(int diagramIndex, bool deleteView)
{
	editor->deleteDiagram(diagramIndex, deleteView);
}

wxString ddDatabaseDesign::getVersionXML()
{
	return wxString(_("1.0"));
}
