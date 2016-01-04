//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbObjectCollection.h - A Collection of simple GQB objects
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBOBJECTCOLLECTION_H
#define GQBOBJECTCOLLECTION_H

// App headers
#include "gqb/gqbObject.h"
#include "gqb/gqbCollection.h"
#include "gqb/gqbArrayCollection.h"

// Create Collections of gqbObjects
class gqbObjectCollection : public gqbObject
{
public:
	gqbObjectCollection(wxString name, wxTreeItemData *owner, pgConn *connection, OID oid = 0);
	virtual ~gqbObjectCollection();

protected:
	void addObject(gqbObject *object);
	void removeObject(gqbObject *object);
	gqbIteratorBase *createIterator();
	gqbIteratorBase *createDownIterator();
	int countObjects();
	gqbObject *getObjectAtIndex(int index);
	bool existsObject(gqbObject *object);
	int indexObject(gqbObject *object);
	void insertObjectAt(gqbObject *object, int index);
	int getCount();
	void removeAll();             // Remove all objects from collection without deleting each one.
	void deleteAll();             // Remove and Delete all objects from collection.

private:
	gqbCollection *objectsCollection;
	gqbArrayCollection *implementation;       // GQB-TODO: DEBO Eliminar esto (que gqbArrayCollection este dentro de la clase) pero como?
};
#endif
