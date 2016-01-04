//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCompositeFigure.cpp - Base class for all figures composite with figures
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "hotdraw/figures/hdCompositeFigure.h"
#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/tools/hdCompositeFigureTool.h"

hdCompositeFigure::hdCompositeFigure()
{
	figureFigures = new hdCollection(new hdArrayCollection());
	figureHandles = new hdCollection(new hdArrayCollection());
}

void hdCompositeFigure::AddPosForNewDiagram()
{
	//Add position for new displaybox at new diagram
	hdAttributeFigure::AddPosForNewDiagram();
	//Add position to each figure inside this composite figure
	hdIteratorBase *iterator = figuresEnumerator();
	while(iterator->HasNext())
	{
		hdIFigure *f = (hdIFigure *) iterator->Next();
		f->AddPosForNewDiagram();
	}
	delete iterator;
}

void hdCompositeFigure::RemovePosOfDiagram(int posIdx)
{
	hdAttributeFigure::RemovePosOfDiagram(posIdx);
	hdIteratorBase *iterator = figuresEnumerator();
	while(iterator->HasNext())
	{
		hdIFigure *f = (hdIFigure *) iterator->Next();
		f->RemovePosOfDiagram(posIdx);
	}
	delete iterator;
}

hdCompositeFigure::~hdCompositeFigure()
{
	hdIHandle *tmpH;
	//Handles should be delete by their owner (figure)
	while(figureHandles->count() > 0)
	{
		tmpH = (hdIHandle *) figureHandles->getItemAt(0);
		figureHandles->removeItemAt(0);
		delete tmpH;
	}
	if(figureHandles)
		delete figureHandles;

	hdIFigure *tmp;
	while(figureFigures->count() > 0)
	{
		tmp = (hdIFigure *) figureFigures->getItemAt(0);
		figureFigures->removeItemAt(0);
		delete tmp;
	}
	if(figureFigures)
		delete figureFigures;
}

void hdCompositeFigure::basicMoveBy(int posIdx, int x, int y)
{
	hdIteratorBase *iterator = figuresEnumerator();
	while(iterator->HasNext())
	{
		hdIFigure *f = (hdIFigure *) iterator->Next();
		f->moveBy(posIdx, x, y);
	}

	basicDisplayBox.x[posIdx] += x;
	basicDisplayBox.y[posIdx] += y;
	delete iterator;
}

bool hdCompositeFigure::containsPoint(int posIdx, int x, int y)
{
	bool out = false;
	hdIteratorBase *iterator = figuresEnumerator();
	while(iterator->HasNext())
	{
		hdIFigure *f = (hdIFigure *) iterator->Next();
		if(f->containsPoint(posIdx, x, y))
		{
			out = true; //avoid memory leak
		}
	}
	delete iterator;
	return out;
}

hdIteratorBase *hdCompositeFigure::figuresEnumerator()
{
	return figureFigures->createIterator();
}

hdIteratorBase *hdCompositeFigure::figuresInverseEnumerator()
{
	return figureFigures->createDownIterator();
}

//Ignore figures at negative positions
hdMultiPosRect &hdCompositeFigure::getBasicDisplayBox()
{
	basicDisplayBox.SetSize(wxSize(0, 0));

	int posIdx;
	hdIteratorBase *iterator = figuresEnumerator();
	for(posIdx = 0; posIdx < basicDisplayBox.CountPositions(); posIdx++)
	{
		basicDisplayBox.SetPosition(posIdx, wxPoint(0, 0));
		bool firstFigure = true;

		while(iterator->HasNext())
		{
			hdIFigure *f = (hdIFigure *) iterator->Next();
			if(firstFigure)
			{
				basicDisplayBox.SetPosition(posIdx, f->displayBox().GetPosition(posIdx));
				basicDisplayBox.SetSize( f->displayBox().GetSize());
				if(f->displayBox().GetPosition(posIdx).x > 0 && f->displayBox().GetPosition(posIdx).y > 0)
					firstFigure = false;
			}
			else
			{
				if(f->displayBox().GetPosition(posIdx).x > 0 && f->displayBox().GetPosition(posIdx).y > 0)
					basicDisplayBox.add(posIdx, f->displayBox().gethdRect(posIdx) );
			}
		}
		iterator->ResetIterator();
	}
	delete iterator;
	return basicDisplayBox;

}

hdCollection *hdCompositeFigure::handlesEnumerator()
{
	return figureHandles;
}

void hdCompositeFigure::add(hdIFigure *figure)
{
	if(includes(figure))
		return;

	//Add figure
	figureFigures->addItem(figure);
	//Check figure available positions for diagrams.
	int i, start;
	start = figure->displayBox().CountPositions();
	for(i = start; i < basicDisplayBox.CountPositions(); i++)
	{
		figure->AddPosForNewDiagram();
	}
	//Add figure handles
	hdIteratorBase *handlesIterator = figure->handlesEnumerator()->createIterator();
	while(handlesIterator->HasNext())
	{
		hdIHandle *h = (hdIHandle *) handlesIterator->Next();
		figureHandles->addItem(h);
	}
	delete handlesIterator;
}

void hdCompositeFigure::remove(hdIFigure *figure)
{
	if(!includes(figure))
		return;

	//Remove figure handles
	hdIteratorBase *handlesIterator = figure->handlesEnumerator()->createIterator();
	while(handlesIterator->HasNext())
	{
		hdIHandle *h = (hdIHandle *) handlesIterator->Next();
		figureHandles->removeItem(h);
	}
	delete handlesIterator;
	//Remove figure
	figureFigures->removeItem(figure);
}

bool hdCompositeFigure::includes(hdIFigure *figure)
{
	if(hdAbstractFigure::includes(figure))
		return true;

	bool out = false;

	hdIteratorBase *iterator = figuresEnumerator();
	while(iterator->HasNext())
	{
		hdIFigure *f = (hdIFigure *) iterator->Next();
		if(f->includes(figure))
			out = true;
	}
	delete iterator;
	return out;
}

void hdCompositeFigure::basicDraw(wxBufferedDC &context, hdDrawingView *view)
{
	hdIteratorBase *iterator = figuresEnumerator();
	hdIFigure *f = NULL;
	while(iterator->HasNext())
	{
		f = (hdIFigure *) iterator->Next();
		f->draw(context, view);
	}
	delete iterator;
}

void hdCompositeFigure::basicDrawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	hdIteratorBase *iterator = figuresEnumerator();
	hdIFigure *f = NULL;
	while(iterator->HasNext())
	{
		f = (hdIFigure *) iterator->Next();
		f->drawSelected(context, view);
	}
	delete iterator;
}

hdIFigure *hdCompositeFigure::findFigure(int posIdx, int x, int y)
{
	hdIFigure *tmp = NULL, *out = NULL;
	hdIteratorBase *iterator = figuresInverseEnumerator();
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->containsPoint(posIdx, x, y))
		{
			out = tmp;
			break;
		}
	}

	delete iterator;

	return out;
}

hdITool *hdCompositeFigure::CreateFigureTool(hdDrawingView *view, hdITool *defaultTool)
{
	return new hdCompositeFigureTool(view, this, defaultTool);
}

hdIFigure *hdCompositeFigure::getFigureAt(int indexOfCollection)
{
	return (hdIFigure *) figureFigures->getItemAt(indexOfCollection);
}
