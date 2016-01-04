//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbObjectCollection.cpp - A Collection of simple GQB objects
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbObject.h"
#include "gqb/gqbObjectCollection.h"
#include "gqb/gqbCollection.h"
#include "gqb/gqbArrayCollection.h"

gqbObjectCollection::gqbObjectCollection(wxString name, wxTreeItemData *owner, pgConn *connection, OID oid)
	: gqbObject(name, owner, connection, oid)
{
	// Create the concrete implementation of the Collection, right now only one implementation not need parameter
	implementation = new gqbArrayCollection();

	// Create the collection using the concrete implementation
	// use the array implementation of the collection
	objectsCollection =  new gqbCollection(implementation);
}


gqbObjectCollection::~gqbObjectCollection()
{
	if(objectsCollection)  // Implementation is deleted when delete the collection & shouldn't be deleted again
		delete objectsCollection;
}


void gqbObjectCollection::addObject(gqbObject *object)
{
	objectsCollection->addItem(object);
}


void gqbObjectCollection::removeObject(gqbObject *object)
{
	objectsCollection->removeItem(object);
}


gqbIteratorBase *gqbObjectCollection::createIterator()
{
	return objectsCollection->createIterator();
}

gqbIteratorBase *gqbObjectCollection::createDownIterator()
{
	return objectsCollection->createDownIterator();
}

int gqbObjectCollection::countObjects()
{
	return objectsCollection->count();
}


gqbObject *gqbObjectCollection::getObjectAtIndex(int index)
{
	return objectsCollection->getItemAt(index);
}


bool gqbObjectCollection::existsObject(gqbObject *object)
{
	return objectsCollection->existsObject(object);
}


// Remove all objects from collection without deleting each one.
void gqbObjectCollection::removeAll()
{
	objectsCollection->removeAll();
}


int gqbObjectCollection::indexObject(gqbObject *object)
{
	return  objectsCollection->getIndex(object);
}


void gqbObjectCollection::insertObjectAt(gqbObject *object, int index)
{
	objectsCollection->insertAtIndex(object, index);
}


int gqbObjectCollection::getCount()
{
	return objectsCollection->count();
}


// Remove & delete all objects
void gqbObjectCollection::deleteAll()
{
	objectsCollection->deleteAll();
}
