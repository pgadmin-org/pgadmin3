//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "hotdraw/tools/hdSelectionTool.h"
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

hdDrawingView *ddDatabaseDesign::getView(int diagramIndex)
{
	return editor->getExistingView(diagramIndex);
}

void ddDatabaseDesign::registerBrowser(ddModelBrowser *browser)
{
	attachedBrowser = browser;
}

void ddDatabaseDesign::addTableToModel(hdIFigure *figure)
{
	editor->addModelFigure(figure);
	if(attachedBrowser)
	{
		attachedBrowser->refreshFromModel();
	}
}

void ddDatabaseDesign::addTableToView(int diagramIndex, hdIFigure *figure)
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

void ddDatabaseDesign::removeTable(int diagramIndex, hdIFigure *figure)
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

	hdIteratorBase *iterator = editor->modelFiguresEnumerator();
	hdIFigure *tmpFigure;
	ddTableFigure *table;

	while(iterator->HasNext())
	{
		tmpFigure = (hdIFigure *)iterator->Next();
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

wxString ddDatabaseDesign::generateList(wxArrayString tables, wxArrayInt options, pgConn *connection, wxString schemaName)
{
	int i;

	// Validate
	if(tables.Count() != options.Count())
	{
		// shouldn't it be a WXASSERT?
		wxMessageBox(_("Invalid number of arguments in call of function generate tables of list"), _("Error at generation process"),  wxICON_ERROR | wxOK);
		return wxEmptyString;
	}

	int tablesCount = tables.Count();
	for(i = 0; i < tablesCount; i++)
	{
		ddTableFigure *table = getTable(tables[i]);
		if(table == NULL)
		{
			// shouldn't it be a WXASSERT?
			wxMessageBox(_("Metadata of table to be generated not found at database designer model"), _("Error at generation process"),  wxICON_ERROR | wxOK);
			return wxEmptyString;
		}
	}

	// Start building of CREATE + ALTER PK(s) + ALTER UK(s) + ALTER FK(s)
	wxString out;
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Create sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	for(i = 0; i < tablesCount; i++)
	{
		if(options[i] == DDGENCREATE || options[i] == DDGENDROPCRE)
		{
			ddTableFigure *table = getTable(tables[i]);
			if(options[i] == DDGENDROPCRE)
			{
				out += wxT(" \n");
				out += wxT("DROP TABLE \"") + table->getTableName() + wxT("\";");
				out += wxT(" \n");
			}
			out += wxT(" \n");
			out += table->generateSQLCreate(schemaName);
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
	for(i = 0; i < tablesCount; i++)
	{
		if(options[i] == DDGENCREATE || options[i] == DDGENDROPCRE)
		{
			ddTableFigure *table = getTable(tables[i]);
			out += table->generateSQLAlterPks(schemaName);
		}
	}
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Uk sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	out += wxT(" \n");
	for(i = 0; i < tablesCount; i++)
	{
		if(options[i] == DDGENCREATE || options[i] == DDGENDROPCRE)
		{
			ddTableFigure *table = getTable(tables[i]);
			out += table->generateSQLAlterUks(schemaName);
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
	for(i = 0; i < tablesCount; i++)
	{
		if(options[i] == DDGENCREATE || options[i] == DDGENDROPCRE)
		{
			ddTableFigure *table = getTable(tables[i]);
			out += table->generateSQLAlterFks(schemaName);
		}
	}

	//Start generation of alter table instead of create
	//Check there is some
	int countAlter = 0;
	for(i = 0; i < tablesCount; i++)
	{
		if(options[i] == DDGENALTER)
		{
			countAlter++;
		}
	}

	if(countAlter > 0 && connection == NULL)
	{
		wxMessageBox(_("No connection found when building ALTER objects DDL."), _("Error at generation process"),  wxICON_ERROR | wxOK);
		return out;
	}
	else if(countAlter > 0 && connection != NULL)
	{
		if(schemaName.IsEmpty())
		{
			wxMessageBox(_("Schema defined when building ALTER TABLE DDL"), _("Error at generation process"),  wxICON_ERROR | wxOK);
			return out;
		}
		out += wxT(" \n");
		out += wxT(" \n");
		out += wxT(" \n");
		out += wxT("--\n-- ");
		out += _("Generating Alter table sentence(s) for table(s) ");
		out += wxT(" \n--\n");
		out += wxT(" \n");
		out += wxT(" \n");
		for(i = 0; i < tablesCount; i++)
		{
			if(options[i] == DDGENALTER)
			{
				ddTableFigure *table = getTable(tables[i]);
				out += table->generateAltersTable(connection, schemaName, this);
				out += wxT(" \n");
			}
		}
	}

	return out;
}

wxArrayString ddDatabaseDesign::getModelTables()
{
	wxArrayString out;
	hdIteratorBase *iterator = editor->modelFiguresEnumerator();
	hdIFigure *tmp;
	ddTableFigure *table;
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out.Add(table->getTableName());
		}
	}
	return out;
}

wxString ddDatabaseDesign::generateModel(wxString schemaName)
{
	wxString out;
	hdIteratorBase *iterator = editor->modelFiguresEnumerator();
	hdIFigure *tmp;
	ddTableFigure *table;
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Create sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			out += wxT(" \n");
			table = (ddTableFigure *)tmp;
			out += table->generateSQLCreate(schemaName);
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
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterPks(schemaName);
		}
	}

	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Uk sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	out += wxT(" \n");
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterUks(schemaName);
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
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterFks(schemaName);
		}
	}

	delete iterator;
	return out;
}

wxArrayString ddDatabaseDesign::getDiagramTables(int diagramIndex)
{
	wxArrayString out;
	hdIteratorBase *iterator = editor->getExistingDiagram(diagramIndex)->figuresEnumerator();
	hdIFigure *tmp;
	ddTableFigure *table;
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out.Add(table->getTableName());
		}
	}

	return out;
}

wxString ddDatabaseDesign::generateDiagram(int diagramIndex, wxString schemaName)
{
	wxString out;
	hdIteratorBase *iterator = editor->getExistingDiagram(diagramIndex)->figuresEnumerator();
	hdIFigure *tmp;
	ddTableFigure *table;
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Create sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			out += wxT(" \n");
			table = (ddTableFigure *)tmp;
			out += table->generateSQLCreate(schemaName);
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
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterPks(schemaName);
		}
	}

	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT(" \n");
	out += wxT("--\n-- ");
	out += _("Generating Uk sentence(s) for table(s) ");
	out += wxT(" \n--\n");
	out += wxT(" \n");
	out += wxT(" \n");
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterUks(schemaName);
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
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			out += table->generateSQLAlterFks(schemaName);
		}
	}

	delete iterator;
	return out;
}

ddTableFigure *ddDatabaseDesign::getSelectedTable(int diagramIndex)
{
	hdIteratorBase *iterator = editor->getExistingDiagram(diagramIndex)->figuresEnumerator();
	hdIFigure *tmp;
	ddTableFigure *table = NULL;
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if (tmp->isSelected(diagramIndex) && tmp->getKindId() == DDTABLEFIGURE)
			table = (ddTableFigure *)tmp;
	}
	delete iterator;
	return table;
}

ddTableFigure *ddDatabaseDesign::getTable(wxString tableName)
{
	ddTableFigure *out = NULL;
	hdIteratorBase *iterator = editor->modelFiguresEnumerator();
	hdIFigure *tmp;
	ddTableFigure *table;
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmp;
			if(table->getTableName().IsSameAs(tableName, false))
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
		wxMessageBox(_("Failed to write the model file!"), _("Error"), wxICON_ERROR | wxOK);
		return false;
	}
	rc = xmlTextWriterStartDocument(xmlWriter, NULL, "UTF-8" , NULL);
	if(rc < 0)
	{
		wxMessageBox(_("Failed to write the model file!"), _("Error"), wxICON_ERROR | wxOK);
		return false;
	}
	else
	{
		ddXmlStorage::StartModel(xmlWriter, this);
		//initialize IDs of tables
		mappingNameToId.clear();
		hdIteratorBase *iterator = editor->modelFiguresEnumerator();
		hdIFigure *tmp;
		ddTableFigure *table;
		int nextID = 10;

		while(iterator->HasNext())
		{
			tmp = (hdIFigure *)iterator->Next();
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
			tmp = (hdIFigure *)iterator->Next();
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
			tmp = (hdIFigure *)iterator->Next();
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
		hdDrawing *tmpDiagram;

		while(iterator->HasNext())
		{
			tmpDiagram = (hdDrawing *)iterator->Next();
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

hdDrawing *ddDatabaseDesign::createDiagram(wxWindow *owner, wxString name, bool fromXml)
{
	hdDrawing *drawing = editor->createDiagram(owner, fromXml);
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

void ddDatabaseDesign::markSchemaOn(wxArrayString tables)
{

	int i, tablesCount = tables.Count();
	for(i = 0; i < tablesCount; i++)
	{
		ddTableFigure *table = getTable(tables[i]);
		if(table != NULL) //mark on if table exists in other case do nothing
		{
			table->setBelongsToSchema(true);
		}
	}
}

void ddDatabaseDesign::unMarkSchemaOnAll()
{
	hdIteratorBase *iterator = editor->modelFiguresEnumerator();
	hdIFigure *tmpFigure;
	ddTableFigure *table;

	while(iterator->HasNext())
	{
		tmpFigure = (hdIFigure *)iterator->Next();
		if(tmpFigure->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmpFigure;
			table->setBelongsToSchema(false);
		}
	}
	delete iterator;
}
