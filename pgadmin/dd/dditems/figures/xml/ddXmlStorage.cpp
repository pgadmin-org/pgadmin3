//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddXmlStorage.cpp - Database designer class for storing / loading objects
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"


#include <ctl/ctlAuiNotebook.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

// App headers
#include "dd/dditems/figures/xml/ddXmlStorage.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "hotdraw/figures/hdIFigure.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/figures/ddColumnFigure.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/figures/ddRelationshipItem.h"
#include "dd/dditems/figures/ddRelationshipTerminal.h"
#include "dd/dditems/figures/ddColumnKindIcon.h"
#include "dd/dditems/figures/ddColumnOptionIcon.h"
#include "dd/ddmodel/ddDatabaseDesign.h"

ddDatabaseDesign *ddXmlStorage::design = NULL;
ctlAuiNotebook *ddXmlStorage::tabs = NULL;

ddXmlStorage::ddXmlStorage():
	hdStorage()
{
	design = NULL;
	tabs = NULL;
}

#define XML_FROM_WXSTRING(s) ((xmlChar *)(const char *)s.mb_str(wxConvUTF8))
#define WXSTRING_FROM_XML(s) wxString((char *)s, wxConvUTF8)

void ddXmlStorage::setModel(ddDatabaseDesign *sourceDesign)
{
	design = sourceDesign;
}

void ddXmlStorage::setNotebook(ctlAuiNotebook *notebook)
{
	tabs = notebook;
}

void ddXmlStorage::StartModel(xmlTextWriterPtr writer, ddDatabaseDesign *sourceDesign)
{
	int tmp;
	design = sourceDesign;
	//<!DOCTYPE MODEL ...pgRelationModel.dtd> but add all dtd to doc instead url
	wxString name = _("MODEL");
	wxString pubid = _("pgrm");
	wxString uri = _("pgrm");

	xmlTextWriterWriteDTD(writer, XML_FROM_WXSTRING(name), NULL, NULL, XML_FROM_WXSTRING(getModelDTD()));
	//<!ELEMENT MODEL (TABLE+)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "MODEL");
	processResult(tmp);

	//<!ELEMENT VERSION EMPTY>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "VERSION");
	processResult(tmp);
	// <!ATTLIST VERSION VERIXLU CDATA #REQUIRED>
	wxString version = sourceDesign->getVersionXML();
	tmp = xmlTextWriterWriteAttribute(writer, BAD_CAST "VERIXLU", XML_FROM_WXSTRING(version) );
	processResult(tmp);
	//Close VERSION Element
	xmlTextWriterEndElement(writer);
}

void ddXmlStorage::EndModel( xmlTextWriterPtr writer)
{
	//Close MODEL Element
	xmlTextWriterEndElement(writer);
	design = NULL;
}

bool ddXmlStorage::Write(xmlTextWriterPtr writer, hdIFigure *figure)
{
	if(design == NULL)
	{
		wxMessageBox(_("Error saving model: not source model have been set"));
		return false;
	}

	switch(figure->getKindId())
	{
		case DDCOLUMNOPTIONICON:
			WriteLocal(writer, (ddColumnOptionIcon *)figure);
			break;
		case DDCOLUMNKINDICON:
			WriteLocal(writer, (ddColumnKindIcon *)figure);
			break;
		case DDTEXTTABLEITEMFIGURE:
			WriteLocal(writer, (ddTextTableItemFigure *)figure);
			break;
		case DDCOLUMNFIGURE:
			WriteLocal(writer, (ddColumnFigure *)figure);
			break;
		case DDTABLEFIGURE:
			WriteLocal(writer, (ddTableFigure *)figure);
			break;
		case DDRELATIONSHIPFIGURE:
			WriteLocal(writer, (ddRelationshipFigure *)figure);
			break;
	}
	return true;
}

bool ddXmlStorage::processResult(int value)
{
	if (value < 0)
	{
		wxMessageBox(_("Fatal Error at libxml for figure persistency"));
		return false;
	}
	return true;
}

void ddXmlStorage::WriteLocal(xmlTextWriterPtr writer, ddColumnOptionIcon *figure)
{
	int tmp;

	//At COLUMN Element
	//<!ELEMENT OPTION (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPTION", "%d", figure->getOption() );
	processResult(tmp);
}

void ddXmlStorage::WriteLocal(xmlTextWriterPtr writer, ddColumnKindIcon *figure)
{
	int tmp;

	//At COLUMN Element
	//<!ELEMENT UKINDEX (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "UKINDEX", "%d", figure->getUniqueConstraintIndex());
	processResult(tmp);

	//<!ELEMENT ISPK (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ISPK", "%c", figure->isPrimaryKey() ? 'T' : 'F' );
	processResult(tmp);
}

void ddXmlStorage::WriteLocal( xmlTextWriterPtr writer, ddTextTableItemFigure *figure)
{
	int tmp;

	//<!ELEMENT COLUMNTYPE (#PCDATA)>
	wxString ddType = figure->dataTypes()[figure->getDataType()];
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "COLUMNTYPE", "%s", XML_FROM_WXSTRING(ddType));
	processResult(tmp);

	if(figure->getPrecision() != -1)
	{
		//<!ELEMENT PRECISION (#PCDATA)>
		tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "PRECISION", "%d", figure->getPrecision());
		processResult(tmp);
	}

	if(figure->getScale() != -1)
	{
		//<!ELEMENT SCALE (#PCDATA)>
		tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "SCALE", "%d", figure->getScale());
		processResult(tmp);
	}

}

void ddXmlStorage::WriteLocal( xmlTextWriterPtr writer, ddColumnFigure *figure)
{
	int tmp;

	//At TABLE Element
	//<!ELEMENT COLUMN (Attribute+,OPTION,UKINDEX,ISPK,SHOWDATATYPE,SHOWALIAS,COLUMNTYPE,PRECISION?,SCALE?,GENERATEFKNAME)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "COLUMN");

	//<!ELEMENT NAME (#PCDATA)>
	wxString columnName = figure->getColumnName(false);
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "NAME", "%s", XML_FROM_WXSTRING(columnName));
	processResult(tmp);

	//At Column Element
	Write(writer, figure->getOptionImage());
	Write(writer, figure->getKindImage());
	Write(writer, figure->getColumnText());

	//At Column Element
	//<!ELEMENT GENERATEFKNAME (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "GENERATEFKNAME", "%c", figure->isFkNameGenerated() ?  'T' : 'F' );
	processResult(tmp);

	//Close COLUMN Element
	xmlTextWriterEndElement(writer);
	processResult(tmp);
}

void ddXmlStorage::WriteLocal( xmlTextWriterPtr writer, ddTableFigure *figure)
{
	int tmp;

	//At TABLES ELEMENT
	//<!ELEMENT TABLE (Attribute*,POINTS,TITLE,UKNAMES?, PKNAME, BEGINDRAWCOLS, BEGINDRAWIDXS, MAXCOLINDEX, MINIDXINDEX, MAXIDXINDEX, COLSROWSSIZE, COLSWINDOW,  IDXSROWSSIZE, IDXSWINDOW, COLUMNS )>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "TABLE");
	processResult(tmp);

	//<!ATTLIST TABLE	TableID ID #REQUIRED > --> ddTableFigure ID
	wxString TableId = design->getTableId(figure->getTableName());
	tmp = xmlTextWriterWriteAttribute(writer, BAD_CAST "TableID", XML_FROM_WXSTRING(TableId) );
	processResult(tmp);

	//Start POINTS <!ELEMENT POINTS (POINT*)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "POINTS");
	processResult(tmp);
	int posIdx, positionsCount = figure->getBasicDisplayBox().CountPositions();
	for(posIdx = 0; posIdx < positionsCount; posIdx++)
	{
		//Start POINT <!ELEMENT POINT (X,Y)>
		tmp = xmlTextWriterStartElement(writer, BAD_CAST "POINT");
		//<!ELEMENT X (#PCDATA)>
		tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "X", "%d", figure->getBasicDisplayBox().x[posIdx]);
		processResult(tmp);
		//<!ELEMENT Y (#PCDATA)>
		tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Y", "%d", figure->getBasicDisplayBox().y[posIdx]);
		processResult(tmp);
		//Close POINT Element
		xmlTextWriterEndElement(writer);
	}
	xmlTextWriterEndElement(writer);
	//Close POINTS Element

	//Add Table Title
	ddColumnFigure *f;
	hdIteratorBase *iterator = figure->figuresEnumerator();
	iterator->Next(); //First figure is main rect

	//<!ELEMENT TITLE (NAME,ALIAS?)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "TITLE");
	processResult(tmp);
	f = (ddColumnFigure *) iterator->Next();

	//<!ELEMENT NAME (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "NAME", "%s", XML_FROM_WXSTRING(figure->getTableName()));
	processResult(tmp);

	//Close TITLE Element
	xmlTextWriterEndElement(writer);

	if(figure->getUkConstraintsNames().Count() > 0)
	{
		//<!ELEMENT UKNAMES (UKNAME+)>
		tmp = xmlTextWriterStartElement(writer, BAD_CAST "UKNAMES");
		processResult(tmp);

		//<!ELEMENT UKNAME (#PCDATA)>  one for each name
		int i, last = figure->getUkConstraintsNames().Count();
		wxString ukName;
		for(i = 0; i < last; i++)
		{
			ukName = figure->getUkConstraintsNames()[i];
			tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "UKNAME", "%s", XML_FROM_WXSTRING(ukName));
			processResult(tmp);
		}

		//Close UKNAMES Element
		xmlTextWriterEndElement(writer);
	}

	//<!ELEMENT PKNAME (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "PKNAME", "%s", XML_FROM_WXSTRING(figure->getPkConstraintName()));
	processResult(tmp);

	//<!ELEMENT BEGINDRAWCOLS (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "BEGINDRAWCOLS", "%d", figure->getBeginDrawCols());
	processResult(tmp);

	//<!ELEMENT BEGINDRAWIDXS (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "BEGINDRAWIDXS", "%d", figure->getBeginDrawIdxs());
	processResult(tmp);

	//<!ELEMENT MAXCOLINDEX (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "MAXCOLINDEX", "%d", figure->getMaxColIndex());
	processResult(tmp);

	//<!ELEMENT MINIDXINDEX (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "MINIDXINDEX", "%d", figure->getMinIdxIndex());
	processResult(tmp);

	//<!ELEMENT MAXIDXINDEX (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "MAXIDXINDEX", "%d", figure->getMaxIdxIndex());
	processResult(tmp);

	//<!ELEMENT COLSROWSSIZE (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "COLSROWSSIZE", "%d", figure->getColsRowsSize());
	processResult(tmp);

	//<!ELEMENT COLSWINDOW (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "COLSWINDOW", "%d", figure->getColsWindow());
	processResult(tmp);

	//<!ELEMENT IDXSROWSSIZE (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "IDXSROWSSIZE", "%d", figure->getIdxsRowsSize());
	processResult(tmp);

	//<!ELEMENT IDXSWINDOW (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "IDXSWINDOW", "%d", figure->getIdxsWindow());
	processResult(tmp);

	//<!ELEMENT COLUMNS (COLUMN*)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "COLUMNS");
	processResult(tmp);

	//ALL COLUMN ELEMENT
	while(iterator->HasNext())
	{
		f = (ddColumnFigure *) iterator->Next();
		Write(writer, f);
	}
	delete iterator;

	//Close COLUMNS Element
	xmlTextWriterEndElement(writer);

	//Close TABLE Element
	xmlTextWriterEndElement(writer);

}


void ddXmlStorage::WriteLocal( xmlTextWriterPtr writer, ddRelationshipFigure *figure)
{
	int tmp;

	//At RELATIONSHIPS Element
	//<!ELEMENT RELATIONSHIP (Attributes*, POINTSRELATION, RELATIONITEMS, UKINDEX, NAME, ONUPDATE, ONDELETE, MATCHSIMPLE, IDENTIFYING, ONETOMANY, MANDATORY, FKFROMPK)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "RELATIONSHIP");

	//<!ATTLIST RELATIONITEM SourceTableID IDREF #REQUIRED > --> ddTableFigure ID
	wxString TableId = design->getTableId(figure->getStartTable()->getTableName());
	tmp = xmlTextWriterWriteAttribute(writer, BAD_CAST "SourceTableID", XML_FROM_WXSTRING(TableId) );
	processResult(tmp);

	//<!ATTLIST RELATIONSHIP DestTableID IDREF #REQUIRED >
	TableId = design->getTableId(figure->getEndTable()->getTableName());
	tmp = xmlTextWriterWriteAttribute(writer, BAD_CAST "DestTableID", XML_FROM_WXSTRING(TableId) );
	processResult(tmp);

	//At RELATIONSHIP Element

	//Start POINTSRELATION <!ELEMENT POINTSRELATION (POINTS+)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "POINTSRELATION");
	hdPoint point;

	for(int posIdx = 0 ; posIdx < figure->pointLinesCount(); posIdx++)
	{
		//At POINTSRELATION Element
		//Start POINTS <!ELEMENT POINTS (POINT*)>
		tmp = xmlTextWriterStartElement(writer, BAD_CAST "POINTS");
		for(int i = 0; i < figure->pointCount(posIdx); i++)
		{
			//At POINTS Element
			//Start POINT <!ELEMENT POINT (X,Y)>
			tmp = xmlTextWriterStartElement(writer, BAD_CAST "POINT");
			point = figure->pointAt(posIdx, i);
			//<!ELEMENT X (#PCDATA)>
			tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "X", "%d", point.x);
			processResult(tmp);
			//<!ELEMENT Y (#PCDATA)>
			tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "Y", "%d", point.y);
			processResult(tmp);
			//Close POINT Element
			xmlTextWriterEndElement(writer);
		}
		//Close POINTS Element
		xmlTextWriterEndElement(writer);
	}
	//Close POINTSRELATION Element
	xmlTextWriterEndElement(writer);

	//At RELATIONSHIP Element
	//<!ELEMENT UKINDEX (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "UKINDEX", "%d", figure->getUkIndex());
	processResult(tmp);

	if(figure->getConstraintName().Length() > 0)
	{
		//<!ELEMENT NAME (#PCDATA)>
		wxString name = figure->getConstraintName();
		tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "NAME", "%s", XML_FROM_WXSTRING(name));
		processResult(tmp);
	}

	//<!ELEMENT ONUPDATE (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ONUPDATE", "%d", figure->getOnUpdateAction());
	processResult(tmp);

	//<!ELEMENT ONDELETE (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ONDELETE", "%d", figure->getOnDeleteAction());
	processResult(tmp);

	//<!ELEMENT MATCHSIMPLE (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "MATCHSIMPLE", "%c", figure->getMatchSimple() ? 'T' : 'F' );
	processResult(tmp);

	//<!ELEMENT IDENTIFYING (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "IDENTIFYING", "%c", figure->getIdentifying() ? 'T' : 'F' );
	processResult(tmp);

	//<!ELEMENT ONETOMANY (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ONETOMANY", "%c", figure->getOneToMany() ? 'T' : 'F' );
	processResult(tmp);

	//<!ELEMENT MANDATORY (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "MANDATORY", "%c", figure->getMandatory() ? 'T' : 'F' );
	processResult(tmp);

	//<!ELEMENT FKFROMPK (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "FKFROMPK", "%c", figure->isForeignKeyFromPk() ? 'T' : 'F' );
	processResult(tmp);

	//At RELATIONSHIP Element
	//Start RELATIONITEMS <!ELEMENT RELATIONITEMS (RELATIONITEM*)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "RELATIONITEMS");

	ddRelationshipItem *item;
	wxString key;
	columnsHashMap::iterator it;
	for( it = figure->getItemsHashMap().begin(); it != figure->getItemsHashMap().end(); ++it )
	{
		key = it->first;
		item = it->second;
		ddXmlStorage::WriteLocal(writer, item);
	}

	//Close RELATIONITEMS
	xmlTextWriterEndElement(writer);

	//Close RELATIONSHIP Element
	xmlTextWriterEndElement(writer);
}

void ddXmlStorage::WriteLocal( xmlTextWriterPtr writer, ddRelationshipItem *item)
{
	int tmp;

	//At RELATIONITEMS Element
	//<!ELEMENT RELATIONITEM (AUTOGENFK, FKCOLNAME,SOURCECOLNAME, INITIALCOLNAME, INITIALALIASNAME )>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "RELATIONITEM");

	//<!ELEMENT AUTOGENFK (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "AUTOGENFK", "%c", item->isAutomaticallyGenerated() ? 'T' : 'F' );
	processResult(tmp);

	//<!ELEMENT FKCOLNAME (#PCDATA)>
	wxString fkColName = item->fkColumn->getColumnName();
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "FKCOLNAME", "%s", XML_FROM_WXSTRING(fkColName));
	processResult(tmp);

	//<!ELEMENT SOURCECOLNAME (#PCDATA)>
	wxString sourceColName = item->original->getColumnName();
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "SOURCECOLNAME", "%s", XML_FROM_WXSTRING(sourceColName));
	processResult(tmp);

	//<!ELEMENT INITIALCOLNAME (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "INITIALCOLNAME", "%s", XML_FROM_WXSTRING(item->originalStartColName));
	processResult(tmp);

	//Close RELATIONITEM Element
	xmlTextWriterEndElement(writer);
}

void ddXmlStorage::StarDiagrams( xmlTextWriterPtr writer)
{
	//At MODEL Element
	//<!ELEMENT DIAGRAMS (DIAGRAM)>
	xmlTextWriterStartElement(writer, BAD_CAST "DIAGRAMS");
}

void ddXmlStorage::EndDiagrams( xmlTextWriterPtr writer)
{

	//Close DIAGRAMS Element
	xmlTextWriterEndElement(writer);
}

void ddXmlStorage::WriteLocal(xmlTextWriterPtr writer, hdDrawing *diagram)
{
	int tmp;

	//At DIAGRAMS Element
	//<!ELEMENT DIAGRAM (NAME, TABLEREF*)>
	tmp = xmlTextWriterStartElement(writer, BAD_CAST "DIAGRAM");

	//At DIAGRAM Element
	//<!ELEMENT NAME (#PCDATA)>
	tmp = xmlTextWriterWriteFormatElement(writer, BAD_CAST "NAME", "%s", XML_FROM_WXSTRING(diagram->getName()) );
	processResult(tmp);

	hdIteratorBase *iterator = diagram->figuresEnumerator();
	hdIFigure *tmpFigure;
	ddTableFigure *table;

	while(iterator->HasNext())
	{
		tmpFigure = (hdIFigure *)iterator->Next();
		if(tmpFigure->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmpFigure;
			//<!ELEMENT TABLEREF EMPTY>
			tmp = xmlTextWriterStartElement(writer, BAD_CAST "TABLEREF");

			//<!ATTLIST TABLEREF TableID IDREF #REQUIRED >  --> ddTableFigure ID
			wxString TableId = design->getTableId(table->getTableName());
			tmp = xmlTextWriterWriteAttribute(writer, BAD_CAST "TableID", XML_FROM_WXSTRING(TableId) );
			processResult(tmp);

			//Close TABLEREF Element
			xmlTextWriterEndElement(writer);
		}
	}
	delete iterator;

	//Close DIAGRAM Element
	xmlTextWriterEndElement(writer);
}

//This is needed because table doesn't have any explicit order when stored then I created all first
void ddXmlStorage::initialModelParse(xmlTextReaderPtr reader)
{
	int ret, tmp;
	xmlChar *value;

	if(reader != NULL)
	{
		wxString tableName = wxEmptyString, tableAlias = wxEmptyString, TableID;
		xmlTextReaderSetParserProp( reader, XML_PARSER_VALIDATE, 1 );
		ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			if(getNodeType(reader) == 1 && getNodeName(reader).IsSameAs(_("TABLE"), false)) //libxml 1 for start element
			{
				//Look for table ID
				tmp = xmlTextReaderHasAttributes(reader);
				TableID = _("TableID");
				if(tmp)
				{
					value = xmlTextReaderGetAttribute(reader, XML_FROM_WXSTRING(TableID));
				}

				if(value)
				{
					TableID = WXSTRING_FROM_XML(value);
					xmlFree(value);
				}

				// <!ELEMENT POINTS (POINT*)>
				wxString tmpInt;
				tmp = xmlTextReaderRead(reader);	//go to POINTS
				wxArrayInt x, y;
				if(getNodeName(reader).IsSameAs(_("POINTS")) && getNodeType(reader) == 1 && !xmlTextReaderIsEmptyElement(reader) )
				{
					tmp = xmlTextReaderRead(reader);	//go POINT
					do
					{
						// --> POINT
						//<!ELEMENT POINT (X,Y)>
						tmp = xmlTextReaderRead(reader);	//go X
						tmp = xmlTextReaderRead(reader);	//go X Value
						tmp = xmlTextReaderRead(reader);	//go to /X

						tmp = xmlTextReaderRead(reader);	//go Y
						tmp = xmlTextReaderRead(reader);	//go Y Value
						tmp = xmlTextReaderRead(reader);	//go to /Y

						tmp = xmlTextReaderRead(reader);	//go /POINT
						tmp = xmlTextReaderRead(reader);	//go POINT or /POINTS ?
					}
					while(getNodeName(reader).IsSameAs(_("POINT"), false));
				}

				tmp = xmlTextReaderRead(reader);	//go to TITLE
				tmp = xmlTextReaderRead(reader);	//go to NAME
				tmp = xmlTextReaderRead(reader);	//go to NAME Value
				value = xmlTextReaderValue(reader);  //Value of NAME
				if(value)
				{
					tableName = WXSTRING_FROM_XML(value);
					design->addTableToMapping(TableID, tableName);
					xmlFree(value);
				}
				tmp = xmlTextReaderRead(reader);	//go to /NAME

				tmp = xmlTextReaderRead(reader);	//go to ALIAS or /TITLE
				if(getNodeName(reader).IsSameAs(_("ALIAS"), false))
				{
					tmp = xmlTextReaderRead(reader);	//go to ALIAS Value
					value = xmlTextReaderValue(reader);  //Value of ALIAS
					if(value)
					{
						tableAlias = WXSTRING_FROM_XML(value);
						xmlFree(value);
					}
					tmp = xmlTextReaderRead(reader);	//go to /ALIAS
					tmp = xmlTextReaderRead(reader);	//go to /TITLE
				}
				ddTableFigure *t = new ddTableFigure(tableName, -1, -1);

				design->addTableToModel(t);
			}
			ret = xmlTextReaderRead(reader);
		}

		//Once the document has been fully parsed check the validation results
		if (xmlTextReaderIsValid(reader) != 1)
		{
			wxMessageBox(_("Model is not following embedded DTD definition, check it.\n"));
			return;
		}
	}
	//reset reader to begining of stream
}



bool ddXmlStorage::Read(xmlTextReaderPtr reader)
{
	int ret;

	if(reader != NULL)
	{
		xmlTextReaderSetParserProp( reader, XML_PARSER_VALIDATE, 1 );
		ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			selectReader(reader);
			ret = xmlTextReaderRead(reader);
		}

		//Once the document has been fully parsed check the validation results
		if (xmlTextReaderIsValid(reader) != 1)
		{
			wxMessageBox(_("Model is not following embedded DTD definition, check it.\n"));
			return false;
		}

		if (ret != 0)
		{
			wxMessageBox(_("Failed to load document from disk"));
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

wxString ddXmlStorage::getNodeName(xmlTextReaderPtr reader)
{
	xmlChar *name;
	wxString out = wxEmptyString;
	name = xmlTextReaderName(reader);
	if (name == NULL)
		name = xmlStrdup(BAD_CAST "--");
	out = WXSTRING_FROM_XML(name);
	xmlFree(name);
	return out;
}
int ddXmlStorage::getNodeType(xmlTextReaderPtr reader)
{
	return xmlTextReaderNodeType(reader);
}

wxString ddXmlStorage::getNodeValue(xmlTextReaderPtr reader)
{
	wxString out = wxEmptyString;
	xmlChar *value;
	value = xmlTextReaderValue(reader);
	if (value != NULL)
	{
		out = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}
	return out;
}


void ddXmlStorage::selectReader(xmlTextReaderPtr reader)
{
	if(getNodeType(reader) == 1) //libxml 1 for start element
	{
		if(getNodeName(reader).IsSameAs(_("VERSION"), false))
		{
			checkVersion(reader);
		}
		if(getNodeName(reader).IsSameAs(_("MODEL"), false))
		{
			//<!ELEMENT MODEL (TABLE+)>
		}

		if(getNodeName(reader).IsSameAs(_("TABLE"), false))
		{
			getTable(reader);
		}

		if(getNodeName(reader).IsSameAs(_("RELATIONSHIP"), false))
		{
			ddRelationshipFigure *r = getRelationship(reader);
			design->addTableToModel(r);
		}

		if(getNodeName(reader).IsSameAs(_("DIAGRAMS"), false))
		{
			initDiagrams(reader);
		}
	}
}

//Check if version of modeler with xml input is compatible
void ddXmlStorage::checkVersion(xmlTextReaderPtr reader)
{
	/*
	<!--Version Element-->
	<!ELEMENT VERSIONXL EMPTY>
	<!ATTLIST VERSION VERIXLU CDATA #REQUIRED>
	*/
	int tmp;
	wxString Number;
	xmlChar *value;
	//<!ATTLIST  VERSIONXL NUMBER CDATA #REQUIRED>
	tmp = xmlTextReaderHasAttributes(reader);
	Number = _("VERIXLU");
	if(tmp)
	{
		value = xmlTextReaderGetAttribute(reader, XML_FROM_WXSTRING(Number));
	}

	if(value)
	{
		Number = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}
	if(!Number.IsSameAs(_("1.0")))
	{
		wxMessageBox(_("Invalid version of XML file for pgAdmin database designer found"), _("Invalid XML"), wxOK | wxICON_ERROR);
	}
}

//Table isn't create here because fk needs all tables created before assign it
ddTableFigure *ddXmlStorage::getTable(xmlTextReaderPtr reader)
{
	/*
	<!ELEMENT POINTS (POINT*)>
	<!-- POINT Element -->
	<!ELEMENT POINT (X,Y)>
	<!ELEMENT X (#PCDATA)>
	<!ELEMENT Y (#PCDATA)>


	<!ELEMENT TABLE (Attribute*,POINTS,TITLE,UKNAMES?, PKNAME, COLUMNS, BEGINDRAWCOLS, BEGINDRAWIDXS, MAXCOLINDEX, MINIDXINDEX, MAXIDXINDEX, COLSROWSSIZE, COLSWINDOW,  IDXSROWSSIZE, IDXSWINDOW )>
	<!ELEMENT POINTS (POINT*)>
	<!ELEMENT COLUMNS (COLUMN*)>
	<!ELEMENT UKNAMES (UKNAME+)>
	<!ELEMENT UKNAME (#PCDATA)>
	<!ELEMENT PKNAME (#PCDATA)>
	<!ELEMENT BEGINDRAWCOLS (#PCDATA)>
	<!ELEMENT BEGINDRAWIDXS (#PCDATA)>
	<!ELEMENT MAXCOLINDEX (#PCDATA)>
	<!ELEMENT MINIDXINDEX (#PCDATA)>
	<!ELEMENT MAXIDXINDEX (#PCDATA)>
	<!ELEMENT COLSROWSSIZE (#PCDATA)>
	<!ELEMENT COLSWINDOW (#PCDATA)>
	<!ELEMENT IDXSROWSSIZE (#PCDATA)>
	<!ELEMENT IDXSWINDOW (#PCDATA)>

	<!ATTLIST TABLE	TableID ID #REQUIRED >
	*/

	int tmp;
	wxString TableID, node, tmpInt;
	xmlChar *value;

	//<!ATTLIST TABLE	TableID ID #REQUIRED >
	tmp = xmlTextReaderHasAttributes(reader);
	TableID = _("TableID");
	if(tmp)
	{
		value = xmlTextReaderGetAttribute(reader, XML_FROM_WXSTRING(TableID));
	}

	if(value)
	{
		TableID = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}

	// --> ATTRIBUTE*
	//Element(s) Attribute*
	// NEED TO IMPLEMENT THIS


	// -->POINTS
	// <!ELEMENT POINTS (POINT*)>
	tmp = xmlTextReaderRead(reader);	//go to POINTS
	wxArrayInt x, y;
	if(getNodeName(reader).IsSameAs(_("POINTS")) && getNodeType(reader) == 1 && !xmlTextReaderIsEmptyElement(reader) )
	{
		tmp = xmlTextReaderRead(reader);	//go POINT
		do
		{
			// --> POINT
			//<!ELEMENT POINT (X,Y)>
			tmp = xmlTextReaderRead(reader);	//go X
			tmp = xmlTextReaderRead(reader);	//go X Value
			value = xmlTextReaderValue(reader);  //Value of X
			if(value)
			{
				tmpInt = WXSTRING_FROM_XML(value);
				x.Add(wxAtoi(tmpInt));
				xmlFree(value);
			}
			tmp = xmlTextReaderRead(reader);	//go to /X

			tmp = xmlTextReaderRead(reader);	//go Y
			tmp = xmlTextReaderRead(reader);	//go Y Value
			value = xmlTextReaderValue(reader);  //Value of Y
			if(value)
			{
				tmpInt = WXSTRING_FROM_XML(value);
				y.Add(wxAtoi(tmpInt));
				xmlFree(value);
			}
			tmp = xmlTextReaderRead(reader);	//go to /Y
			tmp = xmlTextReaderRead(reader);	//go /POINT
			tmp = xmlTextReaderRead(reader);	//go POINT or /POINTS ?
		}
		while(getNodeName(reader).IsSameAs(_("POINT"), false));
	}

	if(!getNodeName(reader).IsSameAs(_("POINTS"), false))
		processResult(-1);

	// --> TITLE
	//<!ELEMENT TITLE (NAME,ALIAS?)>
	//<!ELEMENT NAME (#PCDATA)>
	//<!ELEMENT ALIAS (#PCDATA)>
	wxString tableName = wxEmptyString;
	wxString tableAlias = wxEmptyString;

	tmp = xmlTextReaderRead(reader);	//go to TITLE
	tmp = xmlTextReaderRead(reader);	//go to NAME
	tmp = xmlTextReaderRead(reader);	//go to NAME Value
	value = xmlTextReaderValue(reader);  //Value of NAME

	if(value)
	{
		tableName = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /NAME
	tmp = xmlTextReaderRead(reader);	//go to ALIAS or /TITLE
	if(getNodeName(reader).IsSameAs(_("ALIAS"), false))
	{
		tmp = xmlTextReaderRead(reader);	//go to ALIAS Value
		value = xmlTextReaderValue(reader);  //Value of ALIAS
		if(value)
		{
			tableAlias = WXSTRING_FROM_XML(value);
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /ALIAS
		tmp = xmlTextReaderRead(reader);	//go to /TITLE
	}

	// --> UKNAMES?
	//<!ELEMENT UKNAMES (UKNAME+)>
	//<!ELEMENT UKNAME (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to UKNAMES or PKNAMES?
	wxArrayString ukNames;
	if(getNodeName(reader).IsSameAs(_("UKNAMES"), false))
	{
		tmp = xmlTextReaderRead(reader);	//go UKNAME
		do
		{
			tmp = xmlTextReaderRead(reader);	//go UKNAME Value
			value = xmlTextReaderValue(reader);  //Value of UKNAME
			if(value)
			{
				ukNames.Add(WXSTRING_FROM_XML(value));
				xmlFree(value);
			}
			tmp = xmlTextReaderRead(reader);	//go to /UKNAME
			tmp = xmlTextReaderRead(reader);	//go to UKNAME or /UKNAMES ?
		}
		while(getNodeName(reader).IsSameAs(_("UKNAME"), false));
		tmp = xmlTextReaderRead(reader);	//go to PKNAME
	}

	// --> PKNAME
	//<!ELEMENT PKNAME (#PCDATA)>
	wxString pkName;
	if(getNodeName(reader).IsSameAs(_("PKNAME")))
	{
		tmp = xmlTextReaderRead(reader);	//go to PKNAME Value or PKNAME node?
		if(!getNodeName(reader).IsSameAs(_("PKNAME")))
		{
			value = xmlTextReaderValue(reader);  //Value of PKNAME
			if(value)
			{
				pkName = WXSTRING_FROM_XML(value);
				xmlFree(value);
			}
			tmp = xmlTextReaderRead(reader);	//go to /PKNAME
		}
	}

	int beginDrawCols, beginDrawIdxs, maxColIndex, minIdxIndex, maxIdxIndex, colsRowsSize, colsWindow, idxsRowsSize, idxsWindow;
	// --> BEGINDRAWCOLS
	//<!ELEMENT BEGINDRAWCOLS (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to BEGINDRAWCOLS
	tmp = xmlTextReaderRead(reader);	//go to BEGINDRAWCOLS Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		beginDrawCols = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /BEGINDRAWCOLS
	tmp = xmlTextReaderRead(reader);	//go to BEGINDRAWIDXS

	// --> BEGINDRAWIDXS
	//<!ELEMENT BEGINDRAWIDXS (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to BEGINDRAWIDXS Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		beginDrawIdxs = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /BEGINDRAWIDXS
	tmp = xmlTextReaderRead(reader);	//go to MAXCOLINDEX

	// --> MAXCOLINDEX
	//<!ELEMENT MAXCOLINDEX (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to MAXCOLINDEX Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		maxColIndex = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /MAXCOLINDEX
	tmp = xmlTextReaderRead(reader);	//go to MINIDXINDEX

	// --> MINIDXINDEX
	//<!ELEMENT MINIDXINDEX (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to MINIDXINDEX Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		minIdxIndex = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /MINIDXINDEX
	tmp = xmlTextReaderRead(reader);	//go to MAXIDXINDEX

	// --> MAXIDXINDEX
	//<!ELEMENT MAXIDXINDEX (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to MAXIDXINDEX Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		maxIdxIndex = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /MAXIDXINDEX
	tmp = xmlTextReaderRead(reader);	//go to COLSROWSSIZE

	// --> COLSROWSSIZE
	//<!ELEMENT COLSROWSSIZE (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to COLSROWSSIZE Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		colsRowsSize = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /COLSROWSSIZE
	tmp = xmlTextReaderRead(reader);	//go to COLSWINDOW

	// --> COLSWINDOW
	//<!ELEMENT COLSWINDOW (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to COLSWINDOW Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		colsWindow = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /COLSWINDOW
	tmp = xmlTextReaderRead(reader);	//go to IDXSROWSSIZE

	// --> IDXSROWSSIZE
	//<!ELEMENT IDXSROWSSIZE (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to IDXSROWSSIZE Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		idxsRowsSize = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /IDXSROWSSIZE
	tmp = xmlTextReaderRead(reader);	//go to IDXSWINDOW

	// --> IDXSWINDOW
	//<!ELEMENT IDXSWINDOW (#PCDATA)>*/
	tmp = xmlTextReaderRead(reader);	//go to IDXSWINDOW Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		idxsWindow = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /IDXSWINDOW
	// --> COLUMNS
	//<!ELEMENT COLUMNS (COLUMN*)>

	tmp = xmlTextReaderRead(reader);	//go to COLUMNS

	//Use empty table without columns created at preparsing to fill it with metadata

	ddTableFigure *t = design->getTable(tableName);
	if(t != NULL)
		t->InitTableValues(ukNames, pkName, beginDrawCols, beginDrawIdxs, maxColIndex, minIdxIndex, maxIdxIndex, colsRowsSize, colsWindow, idxsRowsSize, idxsWindow);
	else
		wxMessageBox(_("Table initial metadata info not found"));


	//CHANGE 300,300 for right value when displaybox metadata will be added


	//COLUMNS node have COLUMN children?
	if(getNodeName(reader).IsSameAs(_("COLUMNS")) && getNodeType(reader) == 1 && !xmlTextReaderIsEmptyElement(reader) )
	{
		ddColumnFigure *c;
		do
		{
			c = getColumn(reader, t);
			if(c)
			{
				t->addColumnFromStorage(c);
				c->setRightIconForColumn();
			}
		}
		while(c != NULL);
		//now at </COLUMNS>
	}

	tmp = xmlTextReaderRead(reader);	//go to </TABLE>
	t->syncInternalsPosAt(x, y); //synchronize positions
	t->updateTableSize();
	return t;
}

ddColumnFigure *ddXmlStorage::getColumn(xmlTextReaderPtr reader, ddTableFigure *colOwner)
{
	/*
	<!ELEMENT COLUMN (Attribute*,NAME,OPTION,UKINDEX,ISPK,COLUMNTYPE,PRECISION?,SCALE?,GENERATEFKNAME)>
	<!ELEMENT NAME (#PCDATA)>
	<!ELEMENT OPTION (#PCDATA)>
	<!ELEMENT UKINDEX (#PCDATA)>
	<!ELEMENT ISPK (#PCDATA)>
	<!ELEMENT COLUMNTYPE (#PCDATA)>
	<!ELEMENT PRECISION (#PCDATA)>
	<!ELEMENT SCALE (#PCDATA)>
	<!ELEMENT GENERATEFKNAME (#PCDATA)>
	*/

	xmlChar *value;
	int tmp;
	tmp = xmlTextReaderRead(reader);	//go to COLUMN
	if(getNodeName(reader).IsSameAs(_("COLUMN")) && getNodeType(reader) == 1 && !xmlTextReaderIsEmptyElement(reader) )
	{
		// --> ATTRIBUTE*
		//Element(s) Attribute*
		// NEED TO IMPLEMENT THIS

		// --> NAME
		//<!ELEMENT NAME (#PCDATA)>
		wxString columnName;
		tmp = xmlTextReaderRead(reader);	//go to NAME
		tmp = xmlTextReaderRead(reader);	//go to NAME Value
		value = xmlTextReaderValue(reader);  //Value of NAME
		if(value)
		{
			columnName = WXSTRING_FROM_XML(value);
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /NAME
		tmp = xmlTextReaderRead(reader);	//go to OPTION

		// --> OPTION
		//<!ELEMENT OPTION (#PCDATA)>
		int option;
		wxString tmpInt;
		tmp = xmlTextReaderRead(reader);	//go to OPTION Value
		value = xmlTextReaderValue(reader);  //Value
		if(value)
		{
			tmpInt = WXSTRING_FROM_XML(value);
			option = wxAtoi(tmpInt);
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /OPTION
		tmp = xmlTextReaderRead(reader);	//go to UKINDEX

		// --> UKINDEX
		//<!ELEMENT UKINDEX (#PCDATA)>
		int ukindex;
		tmp = xmlTextReaderRead(reader);	//go to UKINDEX Value
		value = xmlTextReaderValue(reader);  //Value
		if(value)
		{
			tmpInt = WXSTRING_FROM_XML(value);
			ukindex = wxAtoi(tmpInt);
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /UKINDEX
		tmp = xmlTextReaderRead(reader);	//go to ISPK

		// --> ISPK
		//<!ELEMENT ISPK (#PCDATA)>
		bool isPk = false;
		tmp = xmlTextReaderRead(reader);	//go to ISPK Value
		value = xmlTextReaderValue(reader);  //Value of ISPK
		if(value)
		{
			tmpInt = WXSTRING_FROM_XML(value);
			isPk = tmpInt.IsSameAs(_("T"));
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /ISPK

		// --> COLUMNTYPE
		//<!ELEMENT COLUMNTYPE (#PCDATA)>
		wxString columnType;
		tmp = xmlTextReaderRead(reader);	//go to COLUMNTYPE
		tmp = xmlTextReaderRead(reader);	//go to COLUMNTYPE Value
		value = xmlTextReaderValue(reader);  //Value of COLUMNTYPE
		if(value)
		{
			columnType = WXSTRING_FROM_XML(value);
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /COLUMNTYPE

		// --> PRECISION?
		//<!ELEMENT PRECISION (#PCDATA)>
		tmp = xmlTextReaderRead(reader);	//go to PRECISION? or SCALE? or ALIAS? or GENERATEFKNAME
		int precision = -1;
		if(getNodeName(reader).IsSameAs(_("PRECISION")))
		{
			tmp = xmlTextReaderRead(reader);	//go to PRECISION Value
			value = xmlTextReaderValue(reader);  //Value of PRECISION
			if(value)
			{
				tmpInt = WXSTRING_FROM_XML(value);
				precision = wxAtoi(tmpInt);
				xmlFree(value);
			}
			tmp = xmlTextReaderRead(reader);	//go to /PRECISION
			tmp = xmlTextReaderRead(reader);	//go to SCALE? or ALIAS? or GENERATEFKNAME
		}

		// --> 	SCALE?
		//<!ELEMENT SCALE (#PCDATA)>

		int scale = -1;
		if(getNodeName(reader).IsSameAs(_("SCALE")))
		{
			tmp = xmlTextReaderRead(reader);	//go to SCALE Value
			value = xmlTextReaderValue(reader);  //Value of SCALE
			if(value)
			{
				tmpInt = WXSTRING_FROM_XML(value);
				scale = wxAtoi(tmpInt);
				xmlFree(value);
			}
			tmp = xmlTextReaderRead(reader);	//go to /SCALE
			tmp = xmlTextReaderRead(reader);	//go to ALIAS? or GENERATEFKNAME
		}

		// --> GENERATEFKNAME
		//<!ELEMENT GENERATEFKNAME (#PCDATA)>
		bool generateFkName = false;
		tmp = xmlTextReaderRead(reader);	//go to GENERATEFKNAME Value
		value = xmlTextReaderValue(reader);  //Value of GENERATEFKNAME
		if(value)
		{
			tmpInt = WXSTRING_FROM_XML(value);
			generateFkName = tmpInt.IsSameAs(_("T"));
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /GENERATEFKNAME
		tmp = xmlTextReaderRead(reader);	//go to /COLUMN

		return new ddColumnFigure(columnName, colOwner, (ddColumnOptionType)option, generateFkName, isPk, columnType, precision, scale, ukindex, NULL, NULL);
	}
	return NULL;   //found /COLUMNS node
}

ddRelationshipFigure *ddXmlStorage::getRelationship(xmlTextReaderPtr reader)
{
	/*
	<!-- POINTSRELATION Element -->
	<!ELEMENT POINTSRELATION (POINTS+)>
	<!-- POINTS Element -->
	<!ELEMENT POINTS (POINT*)>
	<!-- POINT Element -->
	<!ELEMENT POINT (X,Y)>
	<!ELEMENT X (#PCDATA)>
	<!ELEMENT Y (#PCDATA)>


	<!-- Relationship Element -->
	<!ELEMENT RELATIONSHIP (Attribute*,POINTSRELATION, UKINDEX, NAME?, ONUPDATE, ONDELETE, MATCHSIMPLE, IDENTIFYING, ONETOMANY, MANDATORY, FKFROMPK,RELATIONITEMS)>
	<!ELEMENT RELATIONITEMS (RELATIONITEM*)>
	<!ELEMENT ONUPDATE (#PCDATA)>
	<!ELEMENT ONDELETE (#PCDATA)>
	<!ELEMENT MATCHSIMPLE (#PCDATA)>
	<!ELEMENT IDENTIFYING (#PCDATA)>
	<!ELEMENT ONETOMANY (#PCDATA)>
	<!ELEMENT MANDATORY (#PCDATA)>
	<!ELEMENT FKFROMPK (#PCDATA)>

	<!ATTLIST RELATIONSHIP	SourceTableID IDREF #REQUIRED >
	<!ATTLIST RELATIONSHIP	DestTableID IDREF #REQUIRED >
	*/

	xmlChar *value;
	wxString tmpInt;
	int tmp;

	//<!ATTLIST RELATIONSHIP SourceTableID IDREF #REQUIRED >
	tmp = xmlTextReaderHasAttributes(reader);
	wxString SourceTableID = _("SourceTableID");
	if(tmp)
	{
		value = xmlTextReaderGetAttribute(reader, XML_FROM_WXSTRING(SourceTableID));
	}

	if(value)
	{
		SourceTableID = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}

	//<!ATTLIST RELATIONSHIP DestTableID IDREF #REQUIRED >
	tmp = xmlTextReaderHasAttributes(reader);
	wxString DestTableID = _("DestTableID");
	if(tmp)
	{
		value = xmlTextReaderGetAttribute(reader, XML_FROM_WXSTRING(DestTableID));
	}

	if(value)
	{
		DestTableID = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}

	ddTableFigure *source = design->getTable(design->getTableName(SourceTableID));
	ddTableFigure *destination = design->getTable(design->getTableName(DestTableID));

	ddRelationshipFigure *relation = new ddRelationshipFigure();
	relation->setStartTerminal(new ddRelationshipTerminal(relation, false));
	relation->setEndTerminal(new ddRelationshipTerminal(relation, true));
	relation->clearPoints(0);
	bool firstPoint = true;
	// --> ATTRIBUTE*
	//Element(s) Attribute*
	// NEED TO IMPLEMENT THIS

	// --> POINTSRELATION
	//<!ELEMENT POINTSRELATION (POINTS+)>
	int x, y, posIdx = 0;

	tmp = xmlTextReaderRead(reader);	//go to POINTSRELATION
	if(getNodeName(reader).IsSameAs(_("POINTSRELATION"), false))
	{
		//only first time inside POINTSRELATION this is needed
		tmp = xmlTextReaderRead(reader);	//go POINTS
		do
		{

			if(!firstPoint)  //because at first time position for diagram exists at relationship yet
			{
				relation->AddPosForNewDiagram();
			}
			else
			{
				firstPoint = false;
			}

			if(getNodeName(reader).IsSameAs(_("POINTS")) && getNodeType(reader) == 1 && !xmlTextReaderIsEmptyElement(reader) )
			{
				//only first time inside POINTS this is needed
				tmp = xmlTextReaderRead(reader);	//go POINT
				do
				{
					tmp = xmlTextReaderRead(reader);	//go X
					tmp = xmlTextReaderRead(reader);	//go X Value
					value = xmlTextReaderValue(reader);  //Value of X
					if(value)
					{
						tmpInt = WXSTRING_FROM_XML(value);
						x = wxAtoi(tmpInt);
						xmlFree(value);
					}
					tmp = xmlTextReaderRead(reader);	//go to /X

					tmp = xmlTextReaderRead(reader);	//go Y
					tmp = xmlTextReaderRead(reader);	//go Y Value
					value = xmlTextReaderValue(reader);  //Value of Y
					if(value)
					{
						tmpInt = WXSTRING_FROM_XML(value);
						y = wxAtoi(tmpInt);
						xmlFree(value);
					}
					tmp = xmlTextReaderRead(reader);	//go to /Y
					tmp = xmlTextReaderRead(reader);	//go /POINT
					relation->addPoint(posIdx, x, y);

					tmp = xmlTextReaderRead(reader);	//go POINT or /POINTS?
				}
				while(getNodeName(reader).IsSameAs(_("POINT"), false) && getNodeType(reader) == 1);
			}
			else
			{
				if(! (getNodeName(reader).IsSameAs(_("POINTS")) && xmlTextReaderIsEmptyElement(reader))  )
				{
					processResult(-1);
				}
			}
			posIdx++;  //change of points array then change view position index
			tmp = xmlTextReaderRead(reader);	//go POINTS or /POINTSRELATION?
		}
		while(getNodeName(reader).IsSameAs(_("POINTS"), false) && getNodeType(reader) == 1);
	}

	if(!getNodeName(reader).IsSameAs(_("POINTSRELATION"), false))
	{
		processResult(-1);
	}

	// --> UKINDEX
	//<!ELEMENT UKINDEX (#PCDATA)>
	int ukindex;
	tmp = xmlTextReaderRead(reader);	//go to UKINDEX
	tmp = xmlTextReaderRead(reader);	//go to UKINDEX Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		ukindex = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /UKINDEX

	// --> NAME
	//<!ELEMENT NAME (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to NAME or ONUPDATE
	wxString RelationshipName = wxEmptyString;
	if(getNodeName(reader).IsSameAs(_("NAME"), false))
	{

		tmp = xmlTextReaderRead(reader);	//go to NAME Value
		value = xmlTextReaderValue(reader);  //Value of NAME
		if(value)
		{
			RelationshipName = WXSTRING_FROM_XML(value);
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /NAME
		tmp = xmlTextReaderRead(reader);	//go to ONUPDATE
	}

	// --> ONUPDATE
	//<!ELEMENT ONUPDATE (#PCDATA)>
	int onUpdate;

	tmp = xmlTextReaderRead(reader);	//go to ONUPDATE Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		onUpdate = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /ONUPDATE
	tmp = xmlTextReaderRead(reader);	//go to ONDELETE

	// --> ONDELETE
	//<!ELEMENT ONDELETE (#PCDATA)>
	int onDelete;
	tmp = xmlTextReaderRead(reader);	//go to ONDELETE Value
	value = xmlTextReaderValue(reader);  //Value
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		onDelete = wxAtoi(tmpInt);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /ONDELETE
	tmp = xmlTextReaderRead(reader);	//go to MATCHSIMPLE

	// --> MATCHSIMPLE
	//<!ELEMENT MATCHSIMPLE (#PCDATA)>
	bool matchSimple = false;
	tmp = xmlTextReaderRead(reader);	//go to MATCHSIMPLE Value
	value = xmlTextReaderValue(reader);  //Value of MATCHSIMPLE
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		matchSimple = tmpInt.IsSameAs(_("T"));
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /MATCHSIMPLE
	tmp = xmlTextReaderRead(reader);	//go to IDENTIFYING

	// --> IDENTIFYING
	//<!ELEMENT IDENTIFYING (#PCDATA)>
	bool identifying = false;
	tmp = xmlTextReaderRead(reader);	//go to IDENTIFYING Value
	value = xmlTextReaderValue(reader);  //Value of IDENTIFYING
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		identifying = tmpInt.IsSameAs(_("T"));
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /IDENTIFYING
	tmp = xmlTextReaderRead(reader);	//go to ONETOMANY

	// --> ONETOMANY
	//<!ELEMENT ONETOMANY (#PCDATA)>
	bool oneToMany = false;
	tmp = xmlTextReaderRead(reader);	//go to ONETOMANY Value
	value = xmlTextReaderValue(reader);  //Value of ONETOMANY
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		oneToMany = tmpInt.IsSameAs(_("T"));
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /ONETOMANY
	tmp = xmlTextReaderRead(reader);	//go to MANDATORY

	// --> MANDATORY
	//<!ELEMENT MANDATORY (#PCDATA)>
	bool mandatory = false;
	tmp = xmlTextReaderRead(reader);	//go to MANDATORY Value
	value = xmlTextReaderValue(reader);  //Value of MANDATORY
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		mandatory = tmpInt.IsSameAs(_("T"));
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /MANDATORY
	tmp = xmlTextReaderRead(reader);	//go to FKFROMPK

	// --> FKFROMPK
	//<!ELEMENT FKFROMPK (#PCDATA)>
	bool fkFromPk = false;
	tmp = xmlTextReaderRead(reader);	//go to FKFROMPK Value
	value = xmlTextReaderValue(reader);  //Value of FKFROMPK
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		fkFromPk = tmpInt.IsSameAs(_("T"));
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /FKFROMPK


	//refresh relationship after all values were setted
	relation->initRelationValues(source, destination, ukindex, RelationshipName, (actionKind) onUpdate, (actionKind) onDelete, matchSimple, identifying, oneToMany, mandatory, fkFromPk);


	// --> RELATIONITEMS
	//<!ELEMENT RELATIONITEMS (RELATIONITEM*)>
	tmp = xmlTextReaderRead(reader);	//go to RELATIONITEMS
	if(getNodeName(reader).IsSameAs(_("RELATIONITEMS")) && getNodeType(reader) == 1 && !xmlTextReaderIsEmptyElement(reader) )
	{
		tmp = xmlTextReaderRead(reader);	//go RELATIONITEM
		do
		{
			//Now only add item to relationship
			ddRelationshipItem *item = getRelationshipItem(reader, relation, source, destination);
			relation->getItemsHashMap()[item->original->getColumnName()] = item;

			tmp = xmlTextReaderRead(reader);	//go to RELATIONITEM or /RELATIONITEMS
		}
		while(getNodeName(reader).IsSameAs(_("RELATIONITEM"), false));
	}

	tmp = xmlTextReaderRead(reader);	//go to /RELATIONSHIP

	relation->updateConnection(0);
	return relation;
}

ddRelationshipItem *ddXmlStorage::getRelationshipItem(xmlTextReaderPtr reader, ddRelationshipFigure *itemOwner, ddTableFigure *source, ddTableFigure *destination)
{
	/*
	<!ELEMENT RELATIONITEM (AUTOGENFK, FKCOLNAME,SOURCECOLNAME, INITIALCOLNAME, INITIALALIASNAME )>
	<!ELEMENT AUTOGENFK (#PCDATA)>
	<!ELEMENT FKCOLNAME (#PCDATA)>
	<!ELEMENT SOURCECOLNAME (#PCDATA)>
	<!ELEMENT INITIALCOLNAME (#PCDATA)>
	<!ELEMENT INITIALALIASNAME (#PCDATA)>
	*/
	xmlChar *value;
	wxString tmpInt;
	int tmp;

	// --> AUTOGENFK
	//<!ELEMENT AUTOGENFK (#PCDATA)>
	bool autoGenFk = false;
	tmp = xmlTextReaderRead(reader);	//go to AUTOGENFK
	tmp = xmlTextReaderRead(reader);	//go to AUTOGENFK Value
	value = xmlTextReaderValue(reader);  //Value of AUTOGENFK
	if(value)
	{
		tmpInt = WXSTRING_FROM_XML(value);
		autoGenFk = tmpInt.IsSameAs(_("T"));
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /AUTOGENFK

	//<!ELEMENT FKCOLNAME (#PCDATA)>
	wxString fkColName;
	tmp = xmlTextReaderRead(reader);	//go to FKCOLNAME
	tmp = xmlTextReaderRead(reader);	//go to FKCOLNAME Value
	value = xmlTextReaderValue(reader);  //Value of FKCOLNAME
	if(value)
	{
		fkColName = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /FKCOLNAME

	//<!ELEMENT SOURCECOLNAME (#PCDATA)>
	wxString sourceColName;
	tmp = xmlTextReaderRead(reader);	//go to SOURCECOLNAME
	tmp = xmlTextReaderRead(reader);	//go to SOURCECOLNAME Value
	value = xmlTextReaderValue(reader);  //Value of SOURCECOLNAME
	if(value)
	{
		sourceColName = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /SOURCECOLNAME

	//<!ELEMENT INITIALCOLNAME (#PCDATA)>
	wxString initialColName;
	tmp = xmlTextReaderRead(reader);	//go to INITIALCOLNAME
	tmp = xmlTextReaderRead(reader);	//go to INITIALCOLNAME Value
	value = xmlTextReaderValue(reader);  //Value of INITIALCOLNAME
	if(value)
	{
		initialColName = WXSTRING_FROM_XML(value);
		xmlFree(value);
	}
	tmp = xmlTextReaderRead(reader);	//go to /INITIALCOLNAME

	//<!ELEMENT INITIALALIASNAME (#PCDATA)>
	tmp = xmlTextReaderRead(reader);	//go to INITIALALIASNAME
	wxString initialAliasName = wxEmptyString;
	if(getNodeName(reader).IsSameAs(_("INITIALALIASNAME"), false))
	{
		tmp = xmlTextReaderRead(reader);	//go to INITIALALIASNAME Value
		value = xmlTextReaderValue(reader);  //Value of INITIALALIASNAME
		if(value)
		{
			initialAliasName = WXSTRING_FROM_XML(value);
			xmlFree(value);
		}
		tmp = xmlTextReaderRead(reader);	//go to /INITIALALIASNAME
		tmp = xmlTextReaderRead(reader);	//go to /RELATIONITEM
	}

	ddRelationshipItem *item = new ddRelationshipItem();
	ddColumnFigure *sourceCol = source->getColByName(sourceColName);
	ddColumnFigure *destinationCol = destination->getColByName(fkColName);
	item->initRelationshipItemValues(itemOwner, destination, autoGenFk, destinationCol, sourceCol, initialColName);

	return item;

}

void ddXmlStorage::initDiagrams(xmlTextReaderPtr reader)
{
	/*
	<!-- DIAGRAMS Element -->
	<!ELEMENT DIAGRAMS (DIAGRAM)>
	<!-- DIAGRAM Element -->
	<!ELEMENT DIAGRAM (NAME, TABLEREF*)>
	<!-- TABLEREF Element -->
	<!ELEMENT TABLEREF EMPTY>
	<!ATTLIST TABLEREF TableID IDREF #REQUIRED >
	*/

	//At DIAGRAMS Element
	xmlChar *value = NULL;
	wxString diagramName;
	hdDrawing *newDiagram;
	int tmp;

	//<!ELEMENT DIAGRAM (NAME, TABLEREF*)>
	tmp = xmlTextReaderRead(reader);	//go to DIAGRAM
	if(getNodeName(reader).IsSameAs(_("DIAGRAM")) && getNodeType(reader) == 1 && !xmlTextReaderIsEmptyElement(reader) )
	{
		do
		{
			//<!ELEMENT NAME (#PCDATA)>
			tmp = xmlTextReaderRead(reader);	//go to NAME
			tmp = xmlTextReaderRead(reader);	//go to NAME Value
			value = xmlTextReaderValue(reader);  //Value of NAME
			if(value)
			{
				diagramName = WXSTRING_FROM_XML(value);
				xmlFree(value);
			}
			tmp = xmlTextReaderRead(reader);	//go to /NAME

			if(tabs && design)
			{
				newDiagram = design->createDiagram(tabs, diagramName, true);
				tabs->AddPage(newDiagram->getView(), diagramName);
			}
			else
			{
				processResult(-1);
			}
			//<!-- TABLEREF Element -->
			//<!ELEMENT TABLEREF EMPTY>
			tmp = xmlTextReaderRead(reader);	//go to TABLEREF
			bool firstTime = true;
			if(getNodeName(reader).IsSameAs(_("TABLEREF")) && xmlTextReaderIsEmptyElement(reader))
			{
				wxString TableID, tableName;
				if(firstTime)
				{
					firstTime = false;
				}
				else
				{
					tmp = xmlTextReaderRead(reader);	//go to TABLEREF
				}
				do
				{
					//<!ATTLIST TABLEREF TableID IDREF #REQUIRED >
					tmp = xmlTextReaderHasAttributes(reader);
					TableID = _("TableID");
					if(tmp)
					{
						value = xmlTextReaderGetAttribute(reader, XML_FROM_WXSTRING(TableID));
					}

					if(value)
					{
						TableID = WXSTRING_FROM_XML(value);
						xmlFree(value);
					}

					tableName = design->getTableName(TableID);
					//Add table to diagram
					newDiagram->add(design->getTable(tableName));
					tmp = xmlTextReaderRead(reader);	//go to TABLEREF or /DIAGRAM?
				}
				while(getNodeName(reader).IsSameAs(_("TABLEREF"), false));
			}
			//After adding a new diagram check for all needed relationships at diagram and add it.
			if(design)
			{
				design->getEditor()->checkRelationshipsConsistency(newDiagram->getView()->getIdx());
				newDiagram->getView()->Refresh();
			}
			else
			{
				processResult(-1);
			}
			tmp = xmlTextReaderRead(reader);	//go to DIAGRAM or /DIAGRAMS?
		}
		while(getNodeName(reader).IsSameAs(_("DIAGRAM"), false));
	}



}

//Temporary DTD will change later
wxString ddXmlStorage::getModelDTD()
{
	wxString dtd = wxEmptyString;
	dtd += _("<!ELEMENT MODEL (VERSION,TABLE+,RELATIONSHIP*,DIAGRAMS)>");
	dtd += _("<!--Version Element-->");
	dtd += _("<!ELEMENT VERSION EMPTY>");
	dtd += _("<!ATTLIST VERSION VERIXLU CDATA #REQUIRED>");
	dtd += _(" ");
	dtd += _("<!--Atribute Element-->");
	dtd += _("<!ELEMENT Attribute (#PCDATA)>");
	dtd += _(" ");
	dtd += _("<!ATTLIST Attribute");
	dtd += _("	Name CDATA #REQUIRED");
	dtd += _("	Kind (normal | selected) #REQUIRED");
	dtd += _(">");
	dtd += _("<!--Rectangle Element-->");
	dtd += _("<!ELEMENT Rect (x,y,w,h)>");
	dtd += _("<!ATTLIST Name");
	dtd += _("	Name CDATA #REQUIRED");
	dtd += _(">");
	dtd += _("<!ELEMENT 	x (#PCDATA)>");
	dtd += _("<!ELEMENT 	y (#PCDATA)>");
	dtd += _("<!ELEMENT 	w (#PCDATA)>");
	dtd += _("<!ELEMENT 	h (#PCDATA)>");
	dtd += _(" ");
	dtd += _("<!--Column Element-->");
	dtd += _("<!ELEMENT COLUMN (Attribute*,NAME,OPTION,UKINDEX,ISPK,COLUMNTYPE,PRECISION?,SCALE?,GENERATEFKNAME)>");
	dtd += _("<!ELEMENT NAME (#PCDATA)>");
	dtd += _("<!ELEMENT OPTION (#PCDATA)>");
	dtd += _("<!ELEMENT UKINDEX (#PCDATA)>");
	dtd += _("<!ELEMENT ISPK (#PCDATA)>");
	dtd += _("<!ELEMENT COLUMNTYPE (#PCDATA)>");
	dtd += _("<!ELEMENT PRECISION (#PCDATA)>");
	dtd += _("<!ELEMENT SCALE (#PCDATA)>");
	dtd += _("<!ELEMENT GENERATEFKNAME (#PCDATA)>");
	dtd += _(" ");
	dtd += _(" ");
	dtd += _("<!ELEMENT TITLE (NAME,ALIAS?)>");
	dtd += _("<!ELEMENT ALIAS (#PCDATA)>");
	dtd += _(" ");
	dtd += _("<!-- Table Element -->");
	dtd += _("<!ELEMENT TABLE (Attribute*,POINTS,TITLE,UKNAMES?, PKNAME, BEGINDRAWCOLS, BEGINDRAWIDXS, MAXCOLINDEX, MINIDXINDEX, MAXIDXINDEX, COLSROWSSIZE,");
	dtd += _("COLSWINDOW,IDXSROWSSIZE, IDXSWINDOW, COLUMNS )>");
	dtd += _("<!ELEMENT COLUMNS (COLUMN*)>");
	dtd += _("<!ELEMENT UKNAMES (UKNAME+)>");
	dtd += _("<!ELEMENT UKNAME (#PCDATA)>");
	dtd += _("<!ELEMENT PKNAME (#PCDATA)>");
	dtd += _("<!ELEMENT BEGINDRAWCOLS (#PCDATA)>");
	dtd += _("<!ELEMENT BEGINDRAWIDXS (#PCDATA)>");
	dtd += _("<!ELEMENT MAXCOLINDEX (#PCDATA)>");
	dtd += _("<!ELEMENT MINIDXINDEX (#PCDATA)>");
	dtd += _("<!ELEMENT MAXIDXINDEX (#PCDATA)>");
	dtd += _("<!ELEMENT COLSROWSSIZE (#PCDATA)>");
	dtd += _("<!ELEMENT COLSWINDOW (#PCDATA)>");
	dtd += _("<!ELEMENT IDXSROWSSIZE (#PCDATA)>");
	dtd += _("<!ELEMENT IDXSWINDOW (#PCDATA)>");
	dtd += _(" ");
	dtd += _("<!ATTLIST TABLE");
	dtd += _("	TableID ID #REQUIRED");
	dtd += _("	>");
	dtd += _(" ");
	dtd += _(" ");
	dtd += _("<!-- Relationship Element -->");
	dtd += _("<!ELEMENT RELATIONITEMS (RELATIONITEM*)>");
	dtd += _("<!ELEMENT RELATIONITEM (AUTOGENFK, FKCOLNAME,SOURCECOLNAME, INITIALCOLNAME, INITIALALIASNAME? )>");
	dtd += _("<!ELEMENT AUTOGENFK (#PCDATA)>");
	dtd += _("<!ELEMENT FKCOLNAME (#PCDATA)>");
	dtd += _("<!ELEMENT SOURCECOLNAME (#PCDATA)>");
	dtd += _("<!ELEMENT INITIALCOLNAME (#PCDATA)>");
	dtd += _("<!ELEMENT INITIALALIASNAME (#PCDATA)>");
	dtd += _(" ");
	dtd += _(" ");
	dtd += _("<!-- POINTSRELATION Element -->");
	dtd += _("<!ELEMENT POINTSRELATION (POINTS+)>");
	dtd += _("<!-- POINTS Element -->");
	dtd += _("<!ELEMENT POINTS (POINT*)>");
	dtd += _("<!-- POINT Element -->");
	dtd += _("<!ELEMENT POINT (X,Y)>");
	dtd += _("<!ELEMENT X (#PCDATA)>");
	dtd += _("<!ELEMENT Y (#PCDATA)>");
	dtd += _(" ");
	dtd += _(" ");
	dtd += _("<!ELEMENT RELATIONSHIP (Attribute*, POINTSRELATION, UKINDEX, NAME?, ONUPDATE, ONDELETE, MATCHSIMPLE, IDENTIFYING, ONETOMANY, MANDATORY, FKFROMPK, RELATIONITEMS)>");
	dtd += _("<!ELEMENT ONUPDATE (#PCDATA)>");
	dtd += _("<!ELEMENT ONDELETE (#PCDATA)>");
	dtd += _("<!ELEMENT MATCHSIMPLE (#PCDATA)>");
	dtd += _("<!ELEMENT IDENTIFYING (#PCDATA)>");
	dtd += _("<!ELEMENT ONETOMANY (#PCDATA)>");
	dtd += _("<!ELEMENT MANDATORY (#PCDATA)>");
	dtd += _("<!ELEMENT FKFROMPK (#PCDATA)>");
	dtd += _(" ");
	dtd += _("<!ATTLIST RELATIONSHIP SourceTableID IDREF #REQUIRED >");
	dtd += _("<!ATTLIST RELATIONSHIP DestTableID IDREF #REQUIRED >");
	dtd += _(" ");
	dtd += _(" ");
	dtd += _("<!-- DIAGRAMS Element -->");
	dtd += _("<!ELEMENT DIAGRAMS (DIAGRAM*)>");
	dtd += _("<!-- DIAGRAM Element -->");
	dtd += _("<!ELEMENT DIAGRAM (NAME, TABLEREF*)>");
	dtd += _("<!-- TABLEREF Element -->");
	dtd += _("<!ELEMENT TABLEREF EMPTY>");
	dtd += _("<!ATTLIST TABLEREF TableID IDREF #REQUIRED >");

	return dtd;
}
