//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdIFigure.h - Base class for all figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDIFIGURE_H
#define WXHDIFIGURE_H
#include "dd/wxhotdraw/utilities/wxhdMultiPosRect.h"
#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/utilities/wxhdCollection.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/utilities/wxhdMultiPosRect.h"

class wxhdITool;
class wxhdDrawingEditor;
class wxhdIConnector;
class wxhdITool;

WX_DEFINE_ARRAY_INT(bool, wxArrayBool);

class wxhdIFigure : public wxhdObject
{
public:
	wxhdIFigure();
	~wxhdIFigure();

	virtual wxhdMultiPosRect &displayBox();
	virtual wxhdMultiPosRect &getBasicDisplayBox();
	virtual void AddPosForNewDiagram();
	virtual void RemovePosOfDiagram(int posIdx);
	virtual void draw (wxBufferedDC &context, wxhdDrawingView *view);
	virtual void drawSelected (wxBufferedDC &context, wxhdDrawingView *view);
	virtual wxhdCollection *handlesEnumerator();
	virtual void addHandle (wxhdIHandle *handle);
	virtual void removeHandle (wxhdIHandle *handle);
	virtual wxhdIConnector *connectorAt (int posIdx, int x, int y);
	virtual void moveBy(int posIdx, int x, int y) = 0;
	virtual void moveTo(int posIdx, int x, int y) = 0;
	virtual bool containsPoint(int posIdx, int x, int y) = 0;
	virtual bool isSelected(int posIdx);
	virtual void setSelected(int posIdx, bool value);
	virtual bool includes(wxhdIFigure *figure);
	virtual bool canConnect() = 0;
	virtual void onFigureChanged(int posIdx, wxhdIFigure *figure) = 0;
	virtual void addObserver (wxhdIFigure *observer);
	virtual void removeObserver (wxhdIFigure *observer);
	virtual wxhdIteratorBase *observersEnumerator();
	virtual void setKindId(int objectId = -1);
	virtual int getKindId();
	virtual wxhdITool *CreateFigureTool(wxhdDrawingView *view, wxhdITool *defaultTool);

protected:
	wxhdMultiPosRect basicDisplayBox;
	wxhdCollection *handles;
	wxhdCollection *observers;
	wxhdIConnector *connector;
private:
	//bool selected;
	wxArrayBool selected;
	int kindHiddenId;

};
#endif
