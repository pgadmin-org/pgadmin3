//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbArrayCollection.h - Implementation of Collection Using Arrays
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBARRAYCOLLECTION_H
#define GQBARRAYCOLLECTION_H

// App headers
#include "gqb/gqbCollectionBase.h"

WX_DEFINE_ARRAY_PTR(gqbObject *, gqbObjsArray);

class gqbArrayIterator : public gqbIteratorBase
{
public:
	gqbArrayIterator(gqbObjsArray *gqbPtrsArray);
	gqbObject *Next();
	bool HasNext();
	void ResetIterator();

private:
	int position;
	gqbObjsArray *internalArray;
};

class gqbArrayDownIterator : public gqbIteratorBase
{
public:
	gqbArrayDownIterator(gqbObjsArray *gqbPtrsArray);
	gqbObject *Next();
	bool HasNext();
	void ResetIterator();

private:
	int position;
	gqbObjsArray *internalArray;
};

//Create Array Objects used as base for gqbCollections
class gqbArrayCollection : public gqbCollectionBase
{
public:
	~gqbArrayCollection();
	void addItem(gqbObject *item);
	void removeItem(gqbObject *item);
	gqbIteratorBase *createIterator();
	gqbIteratorBase *createDownIterator();
	gqbObject *getItemAt(int index);
	int count();
	bool existsObject(gqbObject *item);
	int getIndex(gqbObject *item);
	void insertAtIndex(gqbObject *item, int index);
	void deleteAll();
	void removeAll();
	gqbObject *&operator[](size_t index)
	{
		return gqbArray[index];
	}

private:
	gqbObjsArray gqbArray;
};
#endif
