//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdCollectionBase.h - A Collection Interface for ERD
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDCOLLECTIONBASE_H
#define WXHDCOLLECTIONBASE_H

// App headers
#include "dd/wxhotdraw/main/wxhdObject.h"

// This class it's like an interface (but with not all advantages of this at runtime)
// If in a future I just don't want to use an array, simple implement this abstract class again
// with the new data structure.

class wxhdIteratorBase : wxObject
{
public:
    wxhdIteratorBase() {};
    virtual wxhdObject* Current() = 0;
    virtual wxhdObject* Next() = 0;
    virtual bool HasNext() = 0;
    virtual void ResetIterator() = 0;
};


class wxhdCollectionBase : wxObject        
{
public:
    wxhdCollectionBase() {};
    virtual ~wxhdCollectionBase() {};
    virtual void addItem(wxhdObject *item) = 0;
    virtual void removeItem(wxhdObject *item) = 0;
	virtual void removeItemAt(int index) = 0;
    virtual wxhdObject* getItemAt(int index) = 0;
    virtual wxhdIteratorBase* createIterator() = 0;
	virtual wxhdIteratorBase* createDownIterator() = 0;
    virtual int count() = 0;
    virtual bool existsObject(wxhdObject *item) = 0;
    virtual int getIndex(wxhdObject *item) = 0;
    virtual void insertAtIndex(wxhdObject *item, int index) = 0;
	virtual void replaceAtIndex(wxhdObject *item, int index) = 0;
	virtual void bringToFront(wxhdObject *item)=0;
	virtual void sendToBack(wxhdObject *item)=0;
    virtual void deleteAll()=0;
    virtual void removeAll()=0;           //remove all items from collection without deleting.
};
#endif
