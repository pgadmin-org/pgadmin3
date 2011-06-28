//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCollection.cpp - Generic implementation of a Collection used by dd
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/utilities/wxhdCollection.h"
#include "dd/wxhotdraw/main/wxhdObject.h"

wxhdCollection::wxhdCollection(wxhdCollectionBase *collectionBase)
{
	collection = collectionBase;
}

wxhdCollection::~wxhdCollection()
{
	if(collection)
		delete collection;
}

void wxhdCollection::addItem(wxhdObject *item)
{
	collection->addItem(item);
}

void wxhdCollection::removeItem(wxhdObject *item)
{
	collection->removeItem(item);
}


wxhdIteratorBase *wxhdCollection::createIterator()
{
	if(collection)
		return collection->createIterator();
	return NULL;
}

wxhdIteratorBase *wxhdCollection::createDownIterator()
{
	if(collection)
		return collection->createDownIterator();
	return NULL;
}

int wxhdCollection::count()
{
	return collection->count();
}

bool wxhdCollection::existsObject(wxhdObject *item)
{
	return collection->existsObject(item);
}

wxhdObject *wxhdCollection::getItemAt(int index)
{
	return collection->getItemAt(index);
}

void wxhdCollection::removeItemAt(int index)
{
	collection->removeItemAt(index);
}

// Remove all items from collection without deleting each one.
void wxhdCollection::removeAll()
{
	collection->removeAll();
}

void wxhdCollection::deleteAll()
{
	collection->deleteAll();
}

int wxhdCollection::getIndex(wxhdObject *item)
{
	return collection->getIndex(item);
}

void wxhdCollection::insertAtIndex(wxhdObject *item, int index)
{
	collection->insertAtIndex(item, index);
}

void wxhdCollection::replaceAtIndex(wxhdObject *item, int index)
{
	collection->replaceAtIndex(item, index);
}

void wxhdCollection::bringToFront(wxhdObject *item)
{
	collection->bringToFront(item);
}

void wxhdCollection::sendToBack(wxhdObject *item)
{
	collection->sendToBack(item);
}