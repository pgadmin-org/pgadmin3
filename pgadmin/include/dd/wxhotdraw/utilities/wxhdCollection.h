//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCollection.h - Generic implementation of a Collection used by dd
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCOLLECTION_H
#define WXHDCOLLECTION_H

// App headers
#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/utilities/wxhdCollectionBase.h"

class wxhdCollection : public wxObject
{
public:
	wxhdCollection(wxhdCollectionBase *collectionBase);
	virtual ~wxhdCollection();
	void addItem(wxhdObject *item);
	void removeItem(wxhdObject *item);
	void removeItemAt(int index);
	void deleteAll();
	void removeAll();
	int count();
	bool existsObject(wxhdObject *item);
	int getIndex(wxhdObject *item);
	wxhdObject *getItemAt(int index);
	void insertAtIndex(wxhdObject *item, int index);
	void replaceAtIndex(wxhdObject *item, int index);
	void bringToFront(wxhdObject *item);
	void sendToBack(wxhdObject *item);
	wxhdIteratorBase *createIterator();
	wxhdIteratorBase *createDownIterator();

private:
	wxhdCollectionBase *collection;
};
#endif
