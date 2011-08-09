//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawing.h - Main storage class for all objects of the model
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDDRAWING_H
#define WXHDDRAWING_H

#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"


// Main model of drawing
class wxhdDrawing : public wxObject
{
public:
	wxhdDrawing(wxhdDrawingEditor *owner);
	virtual ~wxhdDrawing();
	virtual void add(wxhdIFigure *figure);
	virtual void remove(wxhdIFigure *figure);
	virtual bool includes(wxhdIFigure *figure);
	virtual wxhdIFigure *findFigure(int posIdx, int x, int y);
	virtual void recalculateDisplayBox(int posIdx);
	virtual void bringToFront(wxhdIFigure *figure);
	virtual void sendToBack(wxhdIFigure *figure);
	virtual wxhdRect &DisplayBox();
	virtual wxhdIteratorBase *figuresEnumerator();
	virtual wxhdIteratorBase *figuresInverseEnumerator();
	virtual void removeAllFigures();
	virtual void deleteAllFigures();
	virtual void registerView(wxhdDrawingView *view)
	{
		usedView = view;
	};
	virtual wxhdDrawingView *getView()
	{
		return usedView;
	};
	virtual void addToSelection(wxhdIFigure *figure);
	virtual void addToSelection(wxhdCollection *figures);
	virtual void removeFromSelection(wxhdIFigure *figure);
	virtual void deleteSelectedFigures();
	virtual void toggleSelection(wxhdIFigure *figure);
	virtual void clearSelection();
	virtual bool isFigureSelected(wxhdIFigure *figure);
	virtual wxhdIteratorBase *selectionFigures();
	virtual int countSelectedFigures();
	wxhdCollection *selectedFigures()
	{
		return selection;
	};
	wxhdCollection *getFiguresCollection()
	{
		return figures;
	};
	wxhdDrawingEditor *getOwnerEditor()
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
	wxhdDrawingEditor *ownerEditor;
	wxhdDrawingView *usedView;
	wxhdCollection *selection;
	wxhdCollection *figures;
	wxhdCollection *handles;
	wxhdRect displayBox;
	wxString drawingName;
};
#endif
