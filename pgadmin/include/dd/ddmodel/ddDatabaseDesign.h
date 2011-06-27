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

#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/dditems/figures/ddTableFigure.h"

class ddDatabaseDesign : public wxObject
{
public:
	ddDatabaseDesign(wxWindow *parent);
	~ddDatabaseDesign();
	wxhdDrawingView* getView();
	wxhdDrawingEditor* getEditor();
	void addTable(wxhdIFigure *figure);
	void removeTable(wxhdIFigure *figure);
	void setTool(wxhdITool* tool);
	void refreshDraw();
	void eraseModel();
	wxString generateModel();
	bool validateModel(wxString& errors);
	wxString getNewTableName();
    ddTableFigure* getSelectedTable();
	bool containsTable(wxString tableName);


protected:

private:
	wxhdDrawingEditor *draw;
	wxhdITool *tool;
};
#endif
