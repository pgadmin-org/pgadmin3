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
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/utilities/wxhdCollection.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"

class wxhdITool;
class wxhdDrawingEditor;
class wxhdIConnector;
class wxhdITool;

class wxhdIFigure : public wxhdObject
{
public:
	wxhdIFigure();
    ~wxhdIFigure();

	virtual wxhdRect& displayBox();
	virtual wxhdRect& getBasicDisplayBox();
	virtual void draw (wxBufferedDC& context, wxhdDrawingView *view);
	virtual void drawSelected (wxBufferedDC& context, wxhdDrawingView *view);
	virtual wxhdCollection* handlesEnumerator();
	virtual void addHandle (wxhdIHandle *handle);
	virtual void removeHandle (wxhdIHandle *handle);
	virtual wxhdIConnector* connectorAt (int x, int y);
	virtual void moveBy(int x, int y);
	virtual void moveTo(int x, int y);
	virtual bool containsPoint(int x, int y);
	virtual bool isSelected();
	virtual void setSelected(bool value);
	virtual bool includes(wxhdIFigure *figure);
	virtual bool canConnect()=0;
	virtual void onFigureChanged(wxhdIFigure *figure)=0;
	virtual void addObserver (wxhdIFigure *observer);
	virtual void removeObserver (wxhdIFigure *observer);	
	virtual wxhdIteratorBase* observersEnumerator();
	virtual void setKindId(int objectId=-1);
	virtual int getKindId();
	virtual wxhdITool* CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool);

protected:
	wxhdRect basicDisplayBox;
	wxhdCollection *handles;
	wxhdCollection *observers;
	wxhdIConnector *connector;
private:
	bool selected;
	int kindHiddenId;

};
#endif
