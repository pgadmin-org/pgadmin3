//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddDatabaseDesign.h - Manages all design related info and contains all model(s) and tables.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDDATABASEDESIGN_H
#define DDDATABASEDESIGN_H

#include <libxml/xmlwriter.h>

#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/dditems/figures/ddTableFigure.h"

WX_DECLARE_STRING_HASH_MAP( wxString , tablesMappingHashMap );

class ddDatabaseDesign : public wxObject
{
public:
	ddDatabaseDesign(wxWindow *parent);
	~ddDatabaseDesign();
	wxhdDrawingView *getView();
	wxhdDrawingEditor *getEditor();
	void addTable(wxhdIFigure *figure);
	void removeTable(wxhdIFigure *figure);
	void setTool(wxhdITool *tool);
	void refreshDraw();
	void eraseModel();
	wxString generateModel();
	bool validateModel(wxString &errors);
	wxString getNewTableName();
	ddTableFigure *getSelectedTable();
	ddTableFigure *getTable(wxString tableName);
	bool writeXmlModel(wxString file);
	bool readXmlModel(wxString file);

	wxString getTableId(wxString tableName);
	void addTableToMapping(wxString IdKey, wxString tableName);
	wxString getTableName(wxString Id);

protected:
	tablesMappingHashMap mappingNameToId;
	tablesMappingHashMap mappingIdToName;
private:
	wxhdDrawingEditor *draw;
	wxhdITool *tool;
	xmlTextWriterPtr xmlWriter;

};
#endif
