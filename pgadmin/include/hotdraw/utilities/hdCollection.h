//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdCollection.h - Generic implementation of a Collection used by dd
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDCOLLECTION_H
#define HDCOLLECTION_H

// App headers
#include "hotdraw/main/hdObject.h"
#include "hotdraw/utilities/hdCollectionBase.h"

class hdCollection : public wxObject
{
public:
	hdCollection(hdCollectionBase *collectionBase);
	virtual ~hdCollection();
	void addItem(hdObject *item);
	void removeItem(hdObject *item);
	void removeItemAt(int index);
	void deleteAll();
	void removeAll();
	int count();
	bool existsObject(hdObject *item);
	int getIndex(hdObject *item);
	hdObject *getItemAt(int index);
	void insertAtIndex(hdObject *item, int index);
	void replaceAtIndex(hdObject *item, int index);
	void bringToFront(hdObject *item);
	void sendToBack(hdObject *item);
	hdIteratorBase *createIterator();
	hdIteratorBase *createDownIterator();

private:
	hdCollectionBase *collection;
};
#endif
