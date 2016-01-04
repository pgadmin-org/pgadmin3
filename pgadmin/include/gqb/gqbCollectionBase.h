//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbCollectionBase.h - A Collection Interface for GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBCOLLECTIONFACTORY_H
#define GQBCOLLECTIONFACTORY_H

// App headers
#include "gqb/gqbObject.h"

// This class it's like an interface (but with not all advantages of this at runtime)
// If in a future I just don't want to use an array, simple implement this abstract class again
// with the new data structure.

class gqbIteratorBase : wxObject
{
public:
	gqbIteratorBase() {};
	virtual gqbObject *Next() = 0;
	virtual bool HasNext() = 0;
	virtual void ResetIterator() = 0;
};
// OR probably let to this class to
class gqbCollectionBase : wxObject        // GQB-TODO: Change to the class because probably it's not adapted to the actual use of this class
{
public:
	gqbCollectionBase() {};
	virtual ~gqbCollectionBase() {};
	virtual void addItem(gqbObject *item) = 0;
	virtual void removeItem(gqbObject *item) = 0;
	virtual gqbObject *getItemAt(int index) = 0;
	virtual gqbIteratorBase *createIterator() = 0;
	virtual gqbIteratorBase *createDownIterator() = 0;
	virtual int count() = 0;
	virtual bool existsObject(gqbObject *item) = 0;
	virtual int getIndex(gqbObject *item) = 0;
	virtual void insertAtIndex(gqbObject *item, int index) = 0;
	virtual void deleteAll() = 0;
	virtual void removeAll() = 0;         //remove all items from collection without deleting.
};
#endif
