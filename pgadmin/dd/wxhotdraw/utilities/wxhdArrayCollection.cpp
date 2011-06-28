//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdArrayCollection.cpp - Implementation of Collection Using Arrays
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/main/wxhdObject.h"

// Destructor
wxhdArrayCollection::~wxhdArrayCollection()
{
	WX_CLEAR_ARRAY(ddArray);
}

// Add item to array
void wxhdArrayCollection::addItem(wxhdObject *item)
{
	ddArray.Add(item);
}

// Remove item from array but don't delete it.
void wxhdArrayCollection::removeItem(wxhdObject *item)
{
	ddArray.Remove(item);
}

void wxhdArrayCollection::removeItemAt(int index)
{
	ddArray.RemoveAt(index);
}

// Create an iterator for the objects inside the array
wxhdIteratorBase *wxhdArrayCollection::createIterator()
{
	return (new wxhdArrayIterator(&ddArray));
}

// Create a Down to iterator for the objects inside the array
wxhdIteratorBase *wxhdArrayCollection::createDownIterator()
{
	return (new wxhdArrayDownIterator(&ddArray));
}

// Return the number of elements inside the array
int wxhdArrayCollection::count()
{
	return ddArray.Count();
}

// Return true if an element pointer is found inside array
bool wxhdArrayCollection::existsObject(wxhdObject *item)
{
	wxhdObject *found = NULL;
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
void wxhdArrayCollection::deleteAll()
{
	WX_CLEAR_ARRAY(ddArray);
}

// Removes all elements inside array without deleting
void wxhdArrayCollection::removeAll()
{
	ddArray.Empty();
}

// Get Item at certain position at Collection
wxhdObject *wxhdArrayCollection::getItemAt(int index)
{
	if(!ddArray.IsEmpty())
		return ddArray[index];
	else
		return NULL;
}

//Bring item to start of array
void wxhdArrayCollection::bringToFront(wxhdObject *item)
{
	wxhdObject *tmp = ddArray[0];
	int index = getIndex(item);
	ddArray[0] = ddArray[index];
	ddArray[index] = tmp;
}

//Bring item to end of array
void wxhdArrayCollection::sendToBack(wxhdObject *item)
{
	int end = count() - 1;
	wxhdObject *tmp = ddArray[end];
	int index = getIndex(item);
	ddArray[end] = ddArray[index];
	ddArray[index] = tmp;
}


int wxhdArrayCollection::getIndex(wxhdObject *item)
{
	return ddArray.Index(item);
}

// Insert item into the array before the index
void wxhdArrayCollection::insertAtIndex(wxhdObject *item, int index)
{
	ddArray.Insert(item, index);
}

// Replace item into the array at index (if overwrite user should delete manually previous object at index)
void wxhdArrayCollection::replaceAtIndex(wxhdObject *item, int index)
{
	ddArray.RemoveAt(index);
	ddArray.Insert(item, index);
}


//
// wxhdArrayIterator - Manages iterator for the array collection concrete class, from first to last element
//


// Constructor
wxhdArrayIterator::wxhdArrayIterator(ddObjsArray *ddPtrsArray)
{
	position = 0;
	internalArray = ddPtrsArray;
}

// Get current item in the array for the iterator
wxhdObject *wxhdArrayIterator::Current()
{
	wxhdObject *obj = internalArray->Item(position);
	return obj;
}

// Get next item in the array for the iterator
wxhdObject *wxhdArrayIterator::Next()
{
	wxhdObject *obj = internalArray->Item(position);
	position++;
	return obj;
}

// Return true if the array has more elements to return
bool wxhdArrayIterator::HasNext()
{
	int size = internalArray->GetCount();
	if( (size > 0) && (position <= (size - 1)) )
		return true;
	else
		return false;
}

void wxhdArrayIterator::ResetIterator()
{
	position = 0;
}


//
// wxhdArrayDownIterator - Manages iterator for the array collection concrete class from last to first element
//


// Constructor
wxhdArrayDownIterator::wxhdArrayDownIterator(ddObjsArray *ddPtrsArray)
{
	internalArray = ddPtrsArray;
	position = internalArray->GetCount() - 1;
}

// Get current item in the array for the iterator
wxhdObject *wxhdArrayDownIterator::Current()
{
	wxhdObject *obj = internalArray->Item(position);
	return obj;
}

// Get next item in the array for the iterator
wxhdObject *wxhdArrayDownIterator::Next()
{
	wxhdObject *obj = internalArray->Item(position);
	position--;
	return obj;
}

// Return true if the array has more elements to return
bool wxhdArrayDownIterator::HasNext()
{
	int size = internalArray->GetCount();
	if( (size > 0) && (position <= (size - 1) && position >= 0) )
		return true;
	else
		return false;
}

void wxhdArrayDownIterator::ResetIterator()
{
	position = internalArray->GetCount() - 1;
}
