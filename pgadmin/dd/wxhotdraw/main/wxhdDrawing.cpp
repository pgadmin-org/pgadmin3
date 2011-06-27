//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawing.cpp - Main storage class for all objects of the model
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/main/wxhdDrawing.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"


wxhdDrawing::wxhdDrawing()
{
	figures = new wxhdCollection(new wxhdArrayCollection());
}

wxhdDrawing::~wxhdDrawing()
{
	wxhdIFigure *tmp;
	while(figures->count()>0)
	{
		tmp = (wxhdIFigure*) figures->getItemAt(0);
		figures->removeItemAt(0);
		delete tmp;
	}
	if(figures)
		delete figures;
}

void wxhdDrawing::add(wxhdIFigure *figure)
{
	if(figures)
		figures->addItem(figure);	
}

void wxhdDrawing::remove(wxhdIFigure *figure)
{
    if(figures)
		figures->removeItem(figure);
}

bool wxhdDrawing::includes(wxhdIFigure *figure)
{
	if(figures)
		return figures->existsObject(figure);
	return false;
}

wxhdIFigure* wxhdDrawing::findFigure(int x, int y)
{
	wxhdIFigure *tmp=NULL, *out=NULL;
	wxhdIteratorBase *iterator=figures->createIterator();
	while(iterator->HasNext())
    {
		 tmp=(wxhdIFigure *)iterator->Next();
		 if(tmp->containsPoint(x,y))
         {
			out=tmp;
			break;
		 }
	}

	delete iterator;;

	return out;
}

void wxhdDrawing::recalculateDisplayBox(){
	bool first=true;
	wxhdIFigure *figure=NULL;

	wxhdIteratorBase *iterator = figures->createIterator();
	while(iterator->HasNext())
	{
		figure=(wxhdIFigure *)iterator->Next();
		if(first)
		{
			displayBox=figure->displayBox();
			first=false;
		}
		else
		{
			displayBox.add(figure->displayBox());
		}
	}

	delete iterator;	
}

void wxhdDrawing::bringToFront(wxhdIFigure *figure)
{
	//To bring to front this figure need to be at last position when is draw
	//because this reason sendToBack (last position) is used.
	figures->sendToBack(figure);
}

void wxhdDrawing::sendToBack(wxhdIFigure *figure)
{
	//To send to back this figure need to be at first position when is draw
	//because this reason bringToFront (1st position) is used.
	figures->bringToFront(figure);
}

wxhdRect& wxhdDrawing::DisplayBox()
{
	return displayBox;
}

wxhdIteratorBase* wxhdDrawing::figuresEnumerator()
{
	return figures->createIterator();
}

wxhdIteratorBase* wxhdDrawing::figuresInverseEnumerator()
{
	return figures->createDownIterator();
}

void wxhdDrawing::deleteAllFigures()
{
	wxhdIFigure *tmp;
	while(figures->count()>0)
	{
		tmp = (wxhdIFigure*) figures->getItemAt(0);
		figures->removeItemAt(0);
		delete tmp;
	}
	//handles delete it together with figures
}

