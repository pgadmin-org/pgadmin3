//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdArrayCollection.h - Implementation of Collection Using Arrays
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDARRAYCOLLECTION_H
#define HDARRAYCOLLECTION_H

// App headers
#include "hotdraw/utilities/hdCollectionBase.h"
#include "hotdraw/main/hdObject.h"


WX_DEFINE_ARRAY_PTR(hdObject *, ddObjsArray);

class hdArrayIterator : public hdIteratorBase
{
public:
	hdArrayIterator(ddObjsArray *ddPtrsArray);
	hdObject *Next();
	hdObject *Current();
	bool HasNext();
	void ResetIterator();

private:
	int position;
	ddObjsArray *internalArray;
};

class hdArrayDownIterator : public hdIteratorBase
{
public:
	hdArrayDownIterator(ddObjsArray *ddPtrsArray);
	hdObject *Next();
	hdObject *Current();
	bool HasNext();
	void ResetIterator();

private:
	int position;
	ddObjsArray *internalArray;
};

//Create Array Objects used as base for hdCollections
class hdArrayCollection : public hdCollectionBase
{
public:
	~hdArrayCollection();
	void addItem(hdObject *item);
	void removeItem(hdObject *item);
	virtual void removeItemAt(int index);
	hdIteratorBase *createIterator();
	hdIteratorBase *createDownIterator();
	hdObject *getItemAt(int index);
	void bringToFront(hdObject *item);
	void sendToBack(hdObject *item);
	int count();
	bool existsObject(hdObject *item);
	int getIndex(hdObject *item);
	void insertAtIndex(hdObject *item, int index);
	void replaceAtIndex(hdObject *item, int index);
	void deleteAll();
	void removeAll();
	hdObject *&operator[](size_t index)
	{
		return ddArray[index];
	}
private:
	ddObjsArray ddArray;
};
#endif
