//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDrawing.cpp - Main storage class for all objects of the diagram,
// their functions are used by model and shouldn't be called directly
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/main/hdDrawing.h"
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/main/hdDrawingEditor.h"
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/figures/hdIFigure.h"


hdDrawing::hdDrawing(hdDrawingEditor *owner)
{
	figures = new hdCollection(new hdArrayCollection());
	selection =  new hdCollection(new hdArrayCollection());
	usedView = NULL;
	ownerEditor = owner;
	drawingName = wxEmptyString;
}

hdDrawing::~hdDrawing()
{
	//clear selection
	if(selection)
	{
		selection->removeAll();
		delete selection;
	}

	//Cannot delete figures, because they belong to model not to diagram
	hdIFigure *tmp;
	while(figures->count() > 0)
	{
		tmp = (hdIFigure *) figures->getItemAt(0);
		figures->removeItemAt(0);
	}

	if(figures)
		delete figures;
}

void hdDrawing::add(hdIFigure *figure)
{
	if(figures)
		figures->addItem(figure);
}

void hdDrawing::remove(hdIFigure *figure)
{
	if(figures)
	{
		figures->removeItem(figure);
		if(usedView)
			figure->moveTo(usedView->getIdx(), -1, -1);
	}
}

bool hdDrawing::includes(hdIFigure *figure)
{
	if(figures)
		return figures->existsObject(figure);
	return false;
}

hdIFigure *hdDrawing::findFigure(int posIdx, int x, int y)
{
	hdIFigure *tmp = NULL, *out = NULL;
	hdIteratorBase *iterator = figures->createIterator();
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		if(tmp->containsPoint(posIdx, x, y))
		{
			out = tmp;
			break;
		}
	}

	delete iterator;;

	return out;
}

void hdDrawing::recalculateDisplayBox(int posIdx)
{
	bool first = true;
	hdIFigure *figure = NULL;

	hdIteratorBase *iterator = figures->createIterator();
	while(iterator->HasNext())
	{
		figure = (hdIFigure *)iterator->Next();
		if(first)
		{
			displayBox = figure->displayBox().gethdRect(posIdx);
			first = false;
		}
		else
		{
			displayBox.add(figure->displayBox().gethdRect(posIdx));
		}
	}

	delete iterator;
}

void hdDrawing::bringToFront(hdIFigure *figure)
{
	//To bring to front this figure need to be at last position when is draw
	//because this reason sendToBack (last position) is used.
	figures->sendToBack(figure);
}

void hdDrawing::sendToBack(hdIFigure *figure)
{
	//To send to back this figure need to be at first position when is draw
	//because this reason bringToFront (1st position) is used.
	figures->bringToFront(figure);
}

hdRect &hdDrawing::DisplayBox()
{
	return displayBox;
}

hdIteratorBase *hdDrawing::figuresEnumerator()
{
	return figures->createIterator();
}

hdIteratorBase *hdDrawing::figuresInverseEnumerator()
{
	return figures->createDownIterator();
}

void hdDrawing::deleteAllFigures()
{
	selection->removeAll();

	hdIFigure *tmp;
	while(figures->count() > 0)
	{
		tmp = (hdIFigure *) figures->getItemAt(0);
		figures->removeItemAt(0);
		delete tmp;
	}
	//handles delete it together with figures
}

void hdDrawing::removeAllFigures()
{
	selection->removeAll();

	hdIFigure *tmp;
	while(figures->count() > 0)
	{
		tmp = (hdIFigure *) figures->getItemAt(0);
		figures->removeItemAt(0);
		if(usedView)
			tmp->moveTo(usedView->getIdx(), -1, -1);
	}
}

void hdDrawing::deleteSelectedFigures()
{
	//Allow to customize delete dialog at Editor
	ownerEditor->remOrDelSelFigures(usedView->getIdx());
}

void hdDrawing::addToSelection(hdIFigure *figure)
{
	if(!selection)
	{
		selection = new hdCollection(new hdArrayCollection());
	}
	if(figure)
	{
		figure->setSelected(usedView->getIdx(), true);
		selection->addItem(figure);
	}
}

void hdDrawing::addToSelection(hdCollection *figures)
{
}

void hdDrawing::removeFromSelection(hdIFigure *figure)
{
	figure->setSelected(usedView->getIdx(), false);
	if(selection)
		selection->removeItem(figure);
}


void hdDrawing::toggleSelection(hdIFigure *figure)
{
	if(figure->isSelected(usedView->getIdx()) &&	selection->existsObject(figure))
		selection->removeItem(figure);
	else if(!figure->isSelected(usedView->getIdx()) && this->includes(figure))
		selection->addItem(figure);

	figure->setSelected(usedView->getIdx(), !figure->isSelected(usedView->getIdx()));
}

void hdDrawing::clearSelection()
{
	hdIFigure *tmp = NULL;
	hdIteratorBase *iterator = selection->createIterator();
	while(iterator->HasNext())
	{
		tmp = (hdIFigure *)iterator->Next();
		tmp->setSelected(usedView->getIdx(), false);
	}
	selection->removeAll();
	delete iterator;
}

bool hdDrawing::isFigureSelected(hdIFigure *figure)
{
	return selection->existsObject(figure);
}

hdIteratorBase *hdDrawing::selectionFigures()
{
	return selection->createIterator();
}

int hdDrawing::countSelectedFigures()
{
	return selection->count();
}
