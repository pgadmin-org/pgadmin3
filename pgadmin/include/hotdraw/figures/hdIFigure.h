//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdIFigure.h - Base class for all figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDIFIGURE_H
#define HDIFIGURE_H
#include "hotdraw/utilities/hdMultiPosRect.h"
#include "hotdraw/main/hdObject.h"
#include "hotdraw/utilities/hdCollection.h"
#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/utilities/hdMultiPosRect.h"

class hdITool;
class hdDrawingEditor;
class hdIConnector;
class hdITool;

WX_DEFINE_ARRAY_INT(bool, wxArrayBool);

class hdIFigure : public hdObject
{
public:
	hdIFigure();
	~hdIFigure();

	virtual hdMultiPosRect &displayBox();
	virtual hdMultiPosRect &getBasicDisplayBox();
	virtual void AddPosForNewDiagram();
	virtual void RemovePosOfDiagram(int posIdx);
	virtual void draw (wxBufferedDC &context, hdDrawingView *view);
	virtual void drawSelected (wxBufferedDC &context, hdDrawingView *view);
	virtual hdCollection *handlesEnumerator();
	virtual void addHandle (hdIHandle *handle);
	virtual void removeHandle (hdIHandle *handle);
	virtual hdIConnector *connectorAt (int posIdx, int x, int y);
	virtual void moveBy(int posIdx, int x, int y) = 0;
	virtual void moveTo(int posIdx, int x, int y) = 0;
	virtual bool containsPoint(int posIdx, int x, int y) = 0;
	virtual bool isSelected(int posIdx);
	virtual void setSelected(int posIdx, bool value);
	virtual bool includes(hdIFigure *figure);
	virtual bool canConnect() = 0;
	virtual void onFigureChanged(int posIdx, hdIFigure *figure) = 0;
	virtual void addObserver (hdIFigure *observer);
	virtual void removeObserver (hdIFigure *observer);
	virtual hdIteratorBase *observersEnumerator();
	virtual void setKindId(int objectId = -1);
	virtual int getKindId();
	virtual hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);

protected:
	hdMultiPosRect basicDisplayBox;
	hdCollection *handles;
	hdCollection *observers;
	hdIConnector *connector;
private:
	//bool selected;
	wxArrayBool selected;
	int kindHiddenId;

};
#endif
