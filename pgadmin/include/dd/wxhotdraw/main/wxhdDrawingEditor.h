//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.h - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDDRAWINGEDITOR_H
#define WXHDDRAWINGEDITOR_H

#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"

class wxhdDrawingEditor : public wxhdObject
{
public:
	wxhdDrawingEditor(wxWindow *owner, bool defaultView = true );
	~wxhdDrawingEditor();
	wxhdDrawingView *getExistingView(int diagramIndex);
	wxhdDrawing *getExistingDiagram(int diagramIndex);
	virtual wxhdDrawing *createDiagram(wxWindow *owner, bool fromXml);
	virtual void deleteDiagram(int diagramIndex, bool deleteView = true);
	virtual void addDiagramFigure(int diagramIndex, wxhdIFigure *figure);
	virtual void removeDiagramFigure(int diagramIndex, wxhdIFigure *figure);
	virtual void addModelFigure(wxhdIFigure *figure);
	virtual void deleteModelFigure(wxhdIFigure *figure);
	virtual void removeFromAllSelections(wxhdIFigure *figure);
	virtual void removeAllDiagramsFigures();
	virtual void deleteAllModelFigures();
	virtual bool modelIncludes(wxhdIFigure *figure);
	virtual void remOrDelSelFigures(int diagramIndex);
	virtual void changeDefaultFiguresFont();
	int modelCount();
	wxhdIteratorBase *modelFiguresEnumerator();
	wxhdIteratorBase *diagramsEnumerator();
	bool modelHasChanged();
	virtual void notifyChanged();

protected:
	wxhdArrayCollection *_diagrams;
	wxhdArrayCollection *_model;
	wxWindow *editorOwner;
	bool modelChanged;

private:
};
#endif
