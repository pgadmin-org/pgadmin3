//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCollectionBase.h - A Collection Interface for ERD
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCOLLECTIONBASE_H
#define HDCOLLECTIONBASE_H

// App headers
#include "hotdraw/main/hdObject.h"

// This class it's like an interface (but with not all advantages of this at runtime)
// If in a future I just don't want to use an array, simple implement this abstract class again
// with the new data structure.

class hdIteratorBase : wxObject
{
public:
	hdIteratorBase() {};
	virtual hdObject *Current() = 0;
	virtual hdObject *Next() = 0;
	virtual bool HasNext() = 0;
	virtual void ResetIterator() = 0;
};


class hdCollectionBase : wxObject
{
public:
	hdCollectionBase() {};
	virtual ~hdCollectionBase() {};
	virtual void addItem(hdObject *item) = 0;
	virtual void removeItem(hdObject *item) = 0;
	virtual void removeItemAt(int index) = 0;
	virtual hdObject *getItemAt(int index) = 0;
	virtual hdIteratorBase *createIterator() = 0;
	virtual hdIteratorBase *createDownIterator() = 0;
	virtual int count() = 0;
	virtual bool existsObject(hdObject *item) = 0;
	virtual int getIndex(hdObject *item) = 0;
	virtual void insertAtIndex(hdObject *item, int index) = 0;
	virtual void replaceAtIndex(hdObject *item, int index) = 0;
	virtual void bringToFront(hdObject *item) = 0;
	virtual void sendToBack(hdObject *item) = 0;
	virtual void deleteAll() = 0;
	virtual void removeAll() = 0;         //remove all items from collection without deleting.
};
#endif
