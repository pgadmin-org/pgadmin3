//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdArrayCollection.cpp - Implementation of Collection Using Arrays
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/main/hdObject.h"

// Destructor
hdArrayCollection::~hdArrayCollection()
{
	WX_CLEAR_ARRAY(ddArray);
}

// Add item to array
void hdArrayCollection::addItem(hdObject *item)
{
	ddArray.Add(item);
}

// Remove item from array but don't delete it.
void hdArrayCollection::removeItem(hdObject *item)
{
	ddArray.Remove(item);
}

void hdArrayCollection::removeItemAt(int index)
{
	ddArray.RemoveAt(index);
}

// Create an iterator for the objects inside the array
hdIteratorBase *hdArrayCollection::createIterator()
{
	return (new hdArrayIterator(&ddArray));
}

// Create a Down to iterator for the objects inside the array
hdIteratorBase *hdArrayCollection::createDownIterator()
{
	return (new hdArrayDownIterator(&ddArray));
}

// Return the number of elements inside the array
int hdArrayCollection::count()
{
	return ddArray.Count();
}

// Return true if an element pointer is found inside array
bool hdArrayCollection::existsObject(hdObject *item)
{
	hdObject *found = NULL;
	int size = ddArray.GetCount();
	for(int i = 0; i < size; i++)
	{
		if (ddArray.Item(i) == item)
		{
			found = ddArray.Item(i);
			break;
		}
	}
	return (found != NULL);
}

// Delete all elements inside array
void hdArrayCollection::deleteAll()
{
	WX_CLEAR_ARRAY(ddArray);
}

// Removes all elements inside array without deleting
void hdArrayCollection::removeAll()
{
	ddArray.Empty();
}

// Get Item at certain position at Collection
hdObject *hdArrayCollection::getItemAt(int index)
{
	if(!ddArray.IsEmpty())
		return ddArray[index];
	else
		return NULL;
}

//Bring item to start of array
void hdArrayCollection::bringToFront(hdObject *item)
{
	hdObject *tmp = ddArray[0];
	int index = getIndex(item);
	ddArray[0] = ddArray[index];
	ddArray[index] = tmp;
}

//Bring item to end of array
void hdArrayCollection::sendToBack(hdObject *item)
{
	int end = count() - 1;
	hdObject *tmp = ddArray[end];
	int index = getIndex(item);
	ddArray[end] = ddArray[index];
	ddArray[index] = tmp;
}


int hdArrayCollection::getIndex(hdObject *item)
{
	return ddArray.Index(item);
}

// Insert item into the array before the index
void hdArrayCollection::insertAtIndex(hdObject *item, int index)
{
	ddArray.Insert(item, index);
}

// Replace item into the array at index (if overwrite user should delete manually previous object at index)
void hdArrayCollection::replaceAtIndex(hdObject *item, int index)
{
	ddArray.RemoveAt(index);
	ddArray.Insert(item, index);
}


//
// hdArrayIterator - Manages iterator for the array collection concrete class, from first to last element
//


// Constructor
hdArrayIterator::hdArrayIterator(ddObjsArray *ddPtrsArray)
{
	position = 0;
	internalArray = ddPtrsArray;
}

// Get current item in the array for the iterator
hdObject *hdArrayIterator::Current()
{
	hdObject *obj = internalArray->Item(position);
	return obj;
}

// Get next item in the array for the iterator
hdObject *hdArrayIterator::Next()
{
	hdObject *obj = internalArray->Item(position);
	position++;
	return obj;
}

// Return true if the array has more elements to return
bool hdArrayIterator::HasNext()
{
	int size = internalArray->GetCount();
	if( (size > 0) && (position <= (size - 1)) )
		return true;
	else
		return false;
}

void hdArrayIterator::ResetIterator()
{
	position = 0;
}


//
// hdArrayDownIterator - Manages iterator for the array collection concrete class from last to first element
//


// Constructor
hdArrayDownIterator::hdArrayDownIterator(ddObjsArray *ddPtrsArray)
{
	internalArray = ddPtrsArray;
	position = internalArray->GetCount() - 1;
}

// Get current item in the array for the iterator
hdObject *hdArrayDownIterator::Current()
{
	hdObject *obj = internalArray->Item(position);
	return obj;
}

// Get next item in the array for the iterator
hdObject *hdArrayDownIterator::Next()
{
	hdObject *obj = internalArray->Item(position);
	position--;
	return obj;
}

// Return true if the array has more elements to return
bool hdArrayDownIterator::HasNext()
{
	int size = internalArray->GetCount();
	if( (size > 0) && (position <= (size - 1) && position >= 0) )
		return true;
	else
		return false;
}

void hdArrayDownIterator::ResetIterator()
{
	position = internalArray->GetCount() - 1;
}
