//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCollection.cpp - Generic implementation of a Collection used by dd
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/utilities/hdCollection.h"
#include "hotdraw/main/hdObject.h"

hdCollection::hdCollection(hdCollectionBase *collectionBase)
{
	collection = collectionBase;
}

hdCollection::~hdCollection()
{
	if(collection)
		delete collection;
}

void hdCollection::addItem(hdObject *item)
{
	collection->addItem(item);
}

void hdCollection::removeItem(hdObject *item)
{
	collection->removeItem(item);
}


hdIteratorBase *hdCollection::createIterator()
{
	if(collection)
		return collection->createIterator();
	return NULL;
}

hdIteratorBase *hdCollection::createDownIterator()
{
	if(collection)
		return collection->createDownIterator();
	return NULL;
}

int hdCollection::count()
{
	return collection->count();
}

bool hdCollection::existsObject(hdObject *item)
{
	return collection->existsObject(item);
}

hdObject *hdCollection::getItemAt(int index)
{
	return collection->getItemAt(index);
}

void hdCollection::removeItemAt(int index)
{
	collection->removeItemAt(index);
}

// Remove all items from collection without deleting each one.
void hdCollection::removeAll()
{
	collection->removeAll();
}

void hdCollection::deleteAll()
{
	collection->deleteAll();
}

int hdCollection::getIndex(hdObject *item)
{
	return collection->getIndex(item);
}

void hdCollection::insertAtIndex(hdObject *item, int index)
{
	collection->insertAtIndex(item, index);
}

void hdCollection::replaceAtIndex(hdObject *item, int index)
{
	collection->replaceAtIndex(item, index);
}

void hdCollection::bringToFront(hdObject *item)
{
	collection->bringToFront(item);
}

void hdCollection::sendToBack(hdObject *item)
{
	collection->sendToBack(item);
}
