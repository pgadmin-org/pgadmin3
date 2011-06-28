//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdArrayCollection.h - Implementation of Collection Using Arrays
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDARRAYCOLLECTION_H
#define WXHDARRAYCOLLECTION_H

// App headers
#include "dd/wxhotdraw/utilities/wxhdCollectionBase.h"
#include "dd/wxhotdraw/main/wxhdObject.h"


WX_DEFINE_ARRAY_PTR(wxhdObject *, ddObjsArray);

class wxhdArrayIterator : public wxhdIteratorBase
{
public:
	wxhdArrayIterator(ddObjsArray *ddPtrsArray);
	wxhdObject *Next();
	wxhdObject *Current();
	bool HasNext();
	void ResetIterator();

private:
	int position;
	ddObjsArray *internalArray;
};

class wxhdArrayDownIterator : public wxhdIteratorBase
{
public:
	wxhdArrayDownIterator(ddObjsArray *ddPtrsArray);
	wxhdObject *Next();
	wxhdObject *Current();
	bool HasNext();
	void ResetIterator();

private:
	int position;
	ddObjsArray *internalArray;
};

//Create Array Objects used as base for wxhdCollections
class wxhdArrayCollection : public wxhdCollectionBase
{
public:
	~wxhdArrayCollection();
	void addItem(wxhdObject *item);
	void removeItem(wxhdObject *item);
	virtual void removeItemAt(int index);
	wxhdIteratorBase *createIterator();
	wxhdIteratorBase *createDownIterator();
	wxhdObject *getItemAt(int index);
	void bringToFront(wxhdObject *item);
	void sendToBack(wxhdObject *item);
	int count();
	bool existsObject(wxhdObject *item);
	int getIndex(wxhdObject *item);
	void insertAtIndex(wxhdObject *item, int index);
	void replaceAtIndex(wxhdObject *item, int index);
	void deleteAll();
	void removeAll();
	wxhdObject * &operator[](size_t index)
	{
		return ddArray[index];
	}
private:
	ddObjsArray ddArray;
};
#endif
