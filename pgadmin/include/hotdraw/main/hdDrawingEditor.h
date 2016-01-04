//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDrawingEditor.h - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDDRAWINGEDITOR_H
#define HDDRAWINGEDITOR_H

#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/utilities/hdArrayCollection.h"

class hdDrawingEditor : public hdObject
{
public:
	hdDrawingEditor(wxWindow *owner, bool defaultView = true );
	~hdDrawingEditor();
	hdDrawingView *getExistingView(int diagramIndex);
	hdDrawing *getExistingDiagram(int diagramIndex);
	virtual hdDrawing *createDiagram(wxWindow *owner, bool fromXml);
	virtual void deleteDiagram(int diagramIndex, bool deleteView = true);
	virtual void addDiagramFigure(int diagramIndex, hdIFigure *figure);
	virtual void removeDiagramFigure(int diagramIndex, hdIFigure *figure);
	virtual void addModelFigure(hdIFigure *figure);
	virtual void deleteModelFigure(hdIFigure *figure);
	virtual void removeFromAllSelections(hdIFigure *figure);
	virtual void removeAllDiagramsFigures();
	virtual void deleteAllModelFigures();
	virtual bool modelIncludes(hdIFigure *figure);
	virtual void remOrDelSelFigures(int diagramIndex);
	virtual void changeDefaultFiguresFont();
	int modelCount();
	hdIteratorBase *modelFiguresEnumerator();
	hdIteratorBase *diagramsEnumerator();
	bool modelHasChanged();
	virtual void notifyChanged();

protected:
	hdArrayCollection *_diagrams;
	hdArrayCollection *_model;
	wxWindow *editorOwner;
	bool modelChanged;

private:
};
#endif
