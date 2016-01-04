//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbArrayCollection.cpp - Implementation of Collection Using Arrays
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbArrayCollection.h"
#include "gqb/gqbObject.h"

// Destructor
gqbArrayCollection::~gqbArrayCollection()
{
	WX_CLEAR_ARRAY(gqbArray);
}


// Add item to array
void gqbArrayCollection::addItem(gqbObject *item)
{
	gqbArray.Add(item);
}


// Remove item from array but don't delete it.
void gqbArrayCollection::removeItem(gqbObject *item)
{
	gqbArray.Remove(item);
}


// Create an iterator for the objects inside the array
gqbIteratorBase *gqbArrayCollection::createIterator()
{
	return (new gqbArrayIterator(&gqbArray));
}

// Create a Down to iterator for the objects inside the array
gqbIteratorBase *gqbArrayCollection::createDownIterator()
{
	return (new gqbArrayDownIterator(&gqbArray));
}

// Return the number of elements inside the array
int gqbArrayCollection::count()
{
	return gqbArray.Count();
}


// Return true if an element pointer is found inside array
bool gqbArrayCollection::existsObject(gqbObject *item)
{
	gqbObject *found = NULL;
	int size = gqbArray.GetCount();
	for(int i = 0; i < size; i++)
	{
		if (gqbArray.Item(i) == item)
		{
			found = gqbArray.Item(i);
			break;
		}
	}
	if(found)
		return true;
	else
		return false;
}


// Delete all elements inside array
void gqbArrayCollection::deleteAll()
{
	WX_CLEAR_ARRAY(gqbArray);
}


// Removes all elements inside array without deleting
void gqbArrayCollection::removeAll()
{
	gqbArray.Empty();
}

// Get Item at certain position at Collection
gqbObject *gqbArrayCollection::getItemAt(int index)
{
	if(!gqbArray.IsEmpty())
		return gqbArray.Item(index);
	else
		return NULL;
}


int gqbArrayCollection::getIndex(gqbObject *item)
{
	return gqbArray.Index(item);
}


// Insert item into the array before the index
void gqbArrayCollection:: insertAtIndex(gqbObject *item, int index)
{
	gqbArray.Insert(item, index);
}


//
// gqbArrayIterator - Manages iterator for the array collection concrete class, from first to last element
//

// Constructor
gqbArrayIterator::gqbArrayIterator(gqbObjsArray *gqbPtrsArray)
{
	position = 0;
	internalArray = gqbPtrsArray;
}


// Get next item in the array for the iterator
gqbObject *gqbArrayIterator::Next()
{
	gqbObject *obj = internalArray->Item(position);
	position++;
	return obj;
}


// Return true if the array has more elements to return
bool gqbArrayIterator::HasNext()
{
	int size = internalArray->GetCount();
	if( (size > 0) && (position <= (size - 1)) )
		return true;
	else
		return false;
}


void gqbArrayIterator::ResetIterator()
{
	position = 0;
}


//
// gqbArrayDownIterator - Manages iterator for the array collection concrete class from last to first element
//

// Constructor
gqbArrayDownIterator::gqbArrayDownIterator(gqbObjsArray *gqbPtrsArray)
{
	internalArray = gqbPtrsArray;
	position = internalArray->GetCount() - 1;
}


// Get next item in the array for the iterator
gqbObject *gqbArrayDownIterator::Next()
{
	gqbObject *obj = internalArray->Item(position);
	position--;
	return obj;
}


// Return true if the array has more elements to return
bool gqbArrayDownIterator::HasNext()
{
	int size = internalArray->GetCount();
	if( (size > 0) && (position <= (size - 1) && position >= 0) )
		return true;
	else
		return false;
}


void gqbArrayDownIterator::ResetIterator()
{
	position = internalArray->GetCount() - 1;
}
