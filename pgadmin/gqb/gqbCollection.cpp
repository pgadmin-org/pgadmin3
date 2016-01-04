//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbCollection.cpp - Generic implementation of a Collection used by GQB.
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbCollection.h"
#include "gqb/gqbObject.h"

gqbCollection::gqbCollection(gqbCollectionBase *collectionBase)
{
	collection = collectionBase;
}


gqbCollection::~gqbCollection()
{
	if(collection)
		delete collection;
}


void gqbCollection::addItem(gqbObject *item)
{
	collection->addItem(item);
}


void gqbCollection::removeItem(gqbObject *item)
{
	collection->removeItem(item);
}


gqbIteratorBase *gqbCollection::createIterator()
{
	return collection->createIterator();
}

gqbIteratorBase *gqbCollection::createDownIterator()
{
	return collection->createDownIterator();
}


int gqbCollection::count()
{
	return collection->count();
}


bool gqbCollection::existsObject(gqbObject *item)
{
	return collection->existsObject(item);
}


gqbObject *gqbCollection::getItemAt(int index)
{
	return collection->getItemAt(index);
}


// Remove all items from collection without deleting each one.
void gqbCollection::removeAll()
{
	collection->removeAll();
}


void gqbCollection::deleteAll()
{
	collection->deleteAll();
}


int gqbCollection::getIndex(gqbObject *item)
{
	return collection->getIndex(item);
}


void gqbCollection::insertAtIndex(gqbObject *item, int index)
{
	collection->insertAtIndex(item, index);
}
