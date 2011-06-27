//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCompositeFigure.cpp - Base class for all figures composite with figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdCompositeFigure.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/tools/wxhdCompositeFigureTool.h"

wxhdCompositeFigure::wxhdCompositeFigure()
{
	figureFigures = new wxhdCollection(new wxhdArrayCollection());
	figureHandles = new wxhdCollection(new wxhdArrayCollection());
}

wxhdCompositeFigure::~wxhdCompositeFigure()
{
	wxhdIHandle *tmpH;
	//Handles should be delete by their owner (figure)
	while(figureHandles->count()>0)
	{
		tmpH = (wxhdIHandle*) figureHandles->getItemAt(0);
		figureHandles->removeItemAt(0);
		delete tmpH;
	}
	if(figureHandles)
		delete figureHandles;  

	wxhdIFigure *tmp;
	while(figureFigures->count()>0)
	{
		tmp = (wxhdIFigure*) figureFigures->getItemAt(0);
		figureFigures->removeItemAt(0);
		delete tmp;
	}
	if(figureFigures)
		delete figureFigures;
}

void wxhdCompositeFigure::basicMoveBy(int x, int y)
{
	wxhdIteratorBase *iterator=figuresEnumerator();
	while(iterator->HasNext())
    {
		wxhdIFigure *f = (wxhdIFigure *) iterator->Next();
		f->moveBy(x,y);
	}
	delete iterator;
}

bool wxhdCompositeFigure::containsPoint(int x, int y)
{
	bool out = false;
	wxhdIteratorBase *iterator=figuresEnumerator();
	while(iterator->HasNext())
    {
		wxhdIFigure *f = (wxhdIFigure *) iterator->Next();
		if(f->containsPoint(x,y))
		{
			out=true;  //avoid memory leak
		}
	}
	delete iterator;
	return out;
}

wxhdIteratorBase* wxhdCompositeFigure::figuresEnumerator()
{
	return figureFigures->createIterator();
}

wxhdIteratorBase* wxhdCompositeFigure::figuresInverseEnumerator()
{
	return figureFigures->createDownIterator();
}

//Ignore figures at negative positions
wxhdRect& wxhdCompositeFigure::getBasicDisplayBox()
{
	basicDisplayBox.SetPosition(wxPoint(0,0));
	basicDisplayBox.SetSize(wxSize(0,0));
	bool firstFigure = true;

	wxhdIteratorBase *iterator=figuresEnumerator();
	while(iterator->HasNext())
    {
		wxhdIFigure *f = (wxhdIFigure *) iterator->Next();
		if(firstFigure)
		{
			basicDisplayBox.SetPosition( f->displayBox().GetPosition());
			basicDisplayBox.SetSize( f->displayBox().GetSize());
			if(f->displayBox().GetPosition().x > 0 && f->displayBox().GetPosition().y > 0)
				firstFigure = false;
		}
		else
		{
			if(f->displayBox().GetPosition().x > 0 && f->displayBox().GetPosition().y > 0)
				basicDisplayBox.add( f->displayBox() );
		}
	}
	delete iterator;
	return basicDisplayBox;

}

wxhdCollection* wxhdCompositeFigure::handlesEnumerator()
{
	return figureHandles;
}

void wxhdCompositeFigure::add(wxhdIFigure *figure)
{
	if(includes(figure))
		return;
	
	//Add figure
	figureFigures->addItem(figure);
	//Add figure handles
	wxhdIteratorBase *handlesIterator = figure->handlesEnumerator()->createIterator();
	while(handlesIterator->HasNext())
	{
		wxhdIHandle *h = (wxhdIHandle *) handlesIterator->Next();
		figureHandles->addItem(h);
	}
	delete handlesIterator;
}

void wxhdCompositeFigure::remove(wxhdIFigure *figure)
{
	if(!includes(figure))
		return;
	
	//Remove figure handles
	wxhdIteratorBase *handlesIterator = figure->handlesEnumerator()->createIterator();
	while(handlesIterator->HasNext())
	{
		wxhdIHandle *h = (wxhdIHandle *) handlesIterator->Next();
		figureHandles->removeItem(h);
	}
	delete handlesIterator;
	//Remove figure
	figureFigures->removeItem(figure);
}

bool wxhdCompositeFigure::includes(wxhdIFigure *figure)
{
	if(wxhdAbstractFigure::includes(figure))
		return true;
	
	bool out = false;

	wxhdIteratorBase *iterator=figuresEnumerator();
	while(iterator->HasNext())
    {
		wxhdIFigure *f = (wxhdIFigure *) iterator->Next();
		if(f->includes(figure))
			out = true;
	}
	delete iterator;
	return out;
}

void wxhdCompositeFigure::basicDraw(wxBufferedDC& context, wxhdDrawingView *view)
{
	wxhdIteratorBase *iterator = figuresEnumerator();
	wxhdIFigure *f=NULL;
	while(iterator->HasNext())
    {
		f = (wxhdIFigure *) iterator->Next();
		f->draw(context,view);
	}
	delete iterator;
}

void wxhdCompositeFigure::basicDrawSelected(wxBufferedDC& context, wxhdDrawingView *view)
{
	wxhdIteratorBase *iterator = figuresEnumerator();
	wxhdIFigure *f = NULL;
	while(iterator->HasNext())
    {
        f = (wxhdIFigure *) iterator->Next();
		f->drawSelected(context,view);
	}
	delete iterator;
}

wxhdIFigure* wxhdCompositeFigure::findFigure(int x, int y)
{
	wxhdIFigure *tmp=NULL, *out=NULL;
	wxhdIteratorBase *iterator=figuresInverseEnumerator();
	while(iterator->HasNext())
    {
		 tmp=(wxhdIFigure *)iterator->Next();
		 if(tmp->containsPoint(x,y))
         {
			out=tmp;
			break;
		 }
	}

	delete iterator;

	return out;
}

wxhdITool* wxhdCompositeFigure::CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool)
{
	return new wxhdCompositeFigureTool(editor, this, defaultTool);
}

wxhdIFigure* wxhdCompositeFigure::getFigureAt(int pos)
{
	return (wxhdIFigure*) figureFigures->getItemAt(pos);
}
