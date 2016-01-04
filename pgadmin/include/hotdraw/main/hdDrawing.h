//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDrawing.h - Main storage class for all objects of the model
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDDRAWING_H
#define HDDRAWING_H

#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/utilities/hdRect.h"


// Main model of drawing
class hdDrawing : public wxObject
{
public:
	hdDrawing(hdDrawingEditor *owner);
	virtual ~hdDrawing();
	virtual void add(hdIFigure *figure);
	virtual void remove(hdIFigure *figure);
	virtual bool includes(hdIFigure *figure);
	virtual hdIFigure *findFigure(int posIdx, int x, int y);
	virtual void recalculateDisplayBox(int posIdx);
	virtual void bringToFront(hdIFigure *figure);
	virtual void sendToBack(hdIFigure *figure);
	virtual hdRect &DisplayBox();
	virtual hdIteratorBase *figuresEnumerator();
	virtual hdIteratorBase *figuresInverseEnumerator();
	virtual void removeAllFigures();
	virtual void deleteAllFigures();
	virtual void registerView(hdDrawingView *view)
	{
		usedView = view;
	};
	virtual hdDrawingView *getView()
	{
		return usedView;
	};
	virtual void addToSelection(hdIFigure *figure);
	virtual void addToSelection(hdCollection *figures);
	virtual void removeFromSelection(hdIFigure *figure);
	virtual void deleteSelectedFigures();
	virtual void toggleSelection(hdIFigure *figure);
	virtual void clearSelection();
	virtual bool isFigureSelected(hdIFigure *figure);
	virtual hdIteratorBase *selectionFigures();
	virtual int countSelectedFigures();
	hdCollection *selectedFigures()
	{
		return selection;
	};
	hdCollection *getFiguresCollection()
	{
		return figures;
	};
	hdDrawingEditor *getOwnerEditor()
	{
		return ownerEditor;
	};
	void setName(wxString name)
	{
		drawingName = name;
	};
	wxString getName()
	{
		return drawingName;
	};
protected:

private:
	hdDrawingEditor *ownerEditor;
	hdDrawingView *usedView;
	hdCollection *selection;
	hdCollection *figures;
	hdCollection *handles;
	hdRect displayBox;
	wxString drawingName;
};
#endif
