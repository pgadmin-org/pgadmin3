//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractFigure.h - Base class for all figures with attributes (line size, fonts and others)
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDXMLSTORAGE_H
#define DDXMLSTORAGE_H

#include "hotdraw/figures/xml/hdStorage.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include <ctl/ctlAuiNotebook.h>

class ddXmlStorage : public hdStorage
{
public:
	ddXmlStorage();

	//Generic part
	static bool Read(xmlTextReaderPtr reader);
	static bool Write(xmlTextWriterPtr writer, hdIFigure *figure);
	static void setModel(ddDatabaseDesign *sourceDesign);
	static void setNotebook(ctlAuiNotebook *notebook);
	static wxString getModelDTD();

	//Write xml info Database Designer Related
	static void WriteLocal( xmlTextWriterPtr writer, ddColumnOptionIcon *figure);
	static void WriteLocal( xmlTextWriterPtr writer, ddColumnKindIcon *figure);
	static void WriteLocal( xmlTextWriterPtr writer, ddTextTableItemFigure *figure);
	static void WriteLocal( xmlTextWriterPtr writer, ddColumnFigure *figure);
	static void WriteLocal( xmlTextWriterPtr writer, ddTableFigure *figure);
	static void WriteLocal( xmlTextWriterPtr writer, ddRelationshipFigure *figure);
	static void WriteLocal( xmlTextWriterPtr writer, ddRelationshipItem *item);
	static void WriteLocal(	xmlTextWriterPtr writer, hdDrawing *diagram);
	static void StarDiagrams( xmlTextWriterPtr writer);
	static void EndDiagrams( xmlTextWriterPtr writer);
	static void StartModel( xmlTextWriterPtr writer, ddDatabaseDesign *design);
	static void EndModel( xmlTextWriterPtr writer);
	static bool processResult(int value);

	//Generic node processing functions
	static wxString getNodeName(xmlTextReaderPtr reader);
	static int getNodeType(xmlTextReaderPtr reader);
	static wxString getNodeValue(xmlTextReaderPtr reader);

	//Read xml info Database Designer Related
	static void selectReader(xmlTextReaderPtr reader);
	static void checkVersion(xmlTextReaderPtr reader);
	static ddTableFigure *getTable(xmlTextReaderPtr reader);
	static ddColumnFigure *getColumn(xmlTextReaderPtr reader, ddTableFigure *colOwner);
	static ddRelationshipFigure *getRelationship(xmlTextReaderPtr reader);
	static ddRelationshipItem *getRelationshipItem(xmlTextReaderPtr reader, ddRelationshipFigure *itemOwner, ddTableFigure *source, ddTableFigure *destination);
	static void initDiagrams(xmlTextReaderPtr reader);
	static void initialModelParse(xmlTextReaderPtr reader);

private:

	static ddDatabaseDesign *design;
	static ctlAuiNotebook *tabs;
};
#endif
