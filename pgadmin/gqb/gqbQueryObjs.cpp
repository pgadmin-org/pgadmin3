//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbQueryObjs.cpp - All objects used by a model of a query in the MVC Pattern model.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/regex.h>

// App headers
#include "gqb/gqbTable.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbObjectCollection.h"
#include "gqb/gqbViewPanels.h"

//
// Collections of Tables inside a Query, data structured used for query storage & later generation of SQL sentence
//

gqbQueryObjs::gqbQueryObjs()
	: gqbObjectCollection(wxT(""), NULL, NULL)
{
	setType(GQB_QUERY);
}


void gqbQueryObjs::addTable(gqbQueryObject *mtable)
{
	this->addObject(mtable);
}


void gqbQueryObjs::removeTable(gqbQueryObject *mtable)
{
	this->removeObject(mtable);
}


gqbIteratorBase  *gqbQueryObjs::createQueryIterator()
{
	return this->createIterator();
}

gqbIteratorBase  *gqbQueryObjs::createDownQueryIterator()
{
	return this->createDownIterator();
}


int gqbQueryObjs::tablesCount()
{
	return this->countObjects();
}


void gqbQueryObjs::removeAllQueryObjs()
{
	this->removeAll();
}


//
// An Object inside a query (A table object in the query), not equal to gqbTable
// Reason: Sometimes a table can be used twice or more times in a query with different columns selected
//         Because this we can not use directly the base table object

gqbQueryObject::gqbQueryObject(gqbTable *table)
	: gqbObjectCollection(table->getName(), table, table->getConnection())
{
	selected = false;
	parent = table;

	//GQB-TODO: Calculate a good initial position
	position.x = 20;
	position.y = 20;
	haveJoins = false;
	haveRegisteredJoins = false;
	registeredCollection = NULL;
	joinsCollection = NULL;
	setType(GQB_QUERYOBJ);
}


// Destructor must empty collection to don't allow deleting of column items from tree
// because this collection doesn't owns it, and then shouldn't destroy it.
gqbQueryObject::~gqbQueryObject()
{
	this->removeAll();

	// Remove item registered at this Query Object
	gqbQueryJoin *tmp;
	if(registeredCollection)
	{
		gqbIteratorBase *r = createRegJoinsIterator();
		while(r->HasNext())
		{
			tmp = (gqbQueryJoin *)r->Next();
			this->unregisterJoin(tmp, true);      // remove and unregister every join in every query object
			// which have registered at this query object

			// On each iteration the structure of iterator change because
			// modified his own collection & should be reset
			r->ResetIterator();
		}
		delete r;
	}

	if(joinsCollection)
	{
		gqbIteratorBase *j = createJoinsIterator();
		while(j->HasNext())
		{
			tmp = (gqbQueryJoin *)j->Next();
			this->removeJoin(tmp, true);          // removes & unregister Join which have like origin this
			// query object
			// On each iteration the structure of iterator change because
			// modified his own collection & should be reset
			j->ResetIterator();
		}
		delete j;

	}

	// removeJoin & unregisterJoin delete the collections where there aren't any items inside.
}


void gqbQueryObject::setSelected(bool value)
{
	this->selected = value;
}


bool gqbQueryObject::getSelected()
{
	return this->selected;
}


void gqbQueryObject::setWidth(int value)
{
	width = value;
}


int gqbQueryObject::getWidth()
{
	return width;
}


void gqbQueryObject::setHeight(int value)
{
	height = value;
}


int gqbQueryObject::getHeight()
{
	return height;
}


void gqbQueryObject::addColumn(gqbColumn *column)
{
	this->addObject(column);
}


void gqbQueryObject::removeColumn(gqbColumn *column)
{
	this->removeObject(column);
}


bool gqbQueryObject::existsColumn(gqbColumn *column)
{
	return this->existsObject(column);
}


gqbIteratorBase *gqbQueryObject::createQueryTableIterator()
{
	return this->createIterator();
}


gqbIteratorBase *gqbQueryObject::createJoinsIterator()
{
	return joinsCollection->createIterator();
}


gqbIteratorBase *gqbQueryObject::createRegJoinsIterator()
{
	return registeredCollection->createIterator();
}


// Create a Join from this table [owner] column to other table [observable] column
gqbQueryJoin *gqbQueryObject::addJoin(gqbQueryObject *owner, gqbQueryObject *observable, gqbColumn *source, gqbColumn *destination, type_Join kind)
{
	if(!haveJoins)
	{
		implementationj = new gqbArrayCollection();
		joinsCollection =  new gqbCollection(implementationj);
		haveJoins = true;
	}

	gqbQueryJoin *join = new gqbQueryJoin(owner, observable, source, destination, kind);
	joinsCollection->addItem(join);
	observable->registerJoin(join);
	return join;
}


// Remove the join from this query object [source table]
void gqbQueryObject::removeJoin(gqbQueryJoin *join, bool unRegister = false)
{
	// Notify to observable that the join this object owns will be removed & then remove the join
	if(unRegister)
		join->getDestQTable()->unregisterJoin(join, false);
	joinsCollection->removeItem(join);
	if(join)
		delete join;                              // Join can be only delete Here by his owner

	if(joinsCollection->count() <= 0)
	{
		delete joinsCollection;                   // implementation it's delete too inside collection.
		haveJoins = false;
		joinsCollection = NULL;
	}
}


// Register a Join created from other table [source] to this table [destination]
void gqbQueryObject::registerJoin(gqbQueryJoin *join)
{
	if(!haveRegisteredJoins)
	{
		implementationr = new gqbArrayCollection();
		registeredCollection =  new gqbCollection(implementationr);
		haveRegisteredJoins = true;
	}
	registeredCollection->addItem(join);
}


// Unregister a Join create from other table [source] to this table [destination] delete the join if need it..
void gqbQueryObject::unregisterJoin(gqbQueryJoin *join, bool removeIt = false)
{
	// Notify to source/owner object of join about join removing & then remove
	registeredCollection->removeItem(join);
	if(removeIt)
		join->getSourceQTable()->removeJoin(join, false);
	if(registeredCollection->count() <= 0)
	{
		delete registeredCollection;              //implementation it's delete too inside collection.
		haveRegisteredJoins = false;
		registeredCollection = NULL;
	}
}


int gqbQueryObject::getColumnIndex(gqbColumn *column)
{
	return parent->indexColumn(column);
}


bool gqbQueryObject::getHaveJoins()
{
	return haveJoins;
}


bool gqbQueryObject::getHaveRegJoins()
{
	return haveRegisteredJoins;
}

// GQB-TODO if last join it's delete I MUST delete implementation & collection & put haveJoins in false;
// Same for registered joins

//
//  A Join inside a query Object like Table or view [Stored at source, registered at destination]
//  I need to store the owner, destination because columns it's share between multiple joins
gqbQueryJoin::gqbQueryJoin(gqbQueryObject *_owner, gqbQueryObject *_destination, gqbColumn *sourceCol, gqbColumn *destCol, type_Join joinKind)
	: gqbObject(wxT(""), _owner, NULL)
{
	kindofJoin = joinKind;
	sCol = sourceCol;
	dCol = destCol;
	owner = _owner;
	selected = false;
	destination = _destination;
	setType(GQB_JOIN);
}


void gqbQueryJoin::setKindofJoin(type_Join kind)
{
	kindofJoin = kind;
}


type_Join gqbQueryJoin::getKindofJoin()
{
	return kindofJoin;
}


// Return the object where the join is stored
gqbQueryObject *gqbQueryJoin::getSourceQTable()
{
	return owner;
}


// Return the object where the join point to.
gqbQueryObject *gqbQueryJoin::getDestQTable()
{
	return destination;
}


// Return the gqbObject of Destination Column
gqbColumn *gqbQueryJoin::getDCol()
{
	return dCol;
}


// Return the gqbObject of Source Column
gqbColumn *gqbQueryJoin::getSCol()
{
	return sCol;
}


wxString gqbQueryJoin::getSourceTable()
{
	if (!owner)
		return wxEmptyString;

	gqbTable *s = (gqbTable *)sCol->getOwner();
	return s->getName();
}


wxString gqbQueryJoin::getDestTable()
{
	if (!destination)
		return wxEmptyString;

	gqbTable *d = (gqbTable *)dCol->getOwner();
	return d->getName();
}


wxString gqbQueryJoin::getSourceCol()
{
	if (!sCol)
		return wxEmptyString;

	return sCol->getName();
}


wxString gqbQueryJoin::getDestCol()
{
	if (!dCol)
		return wxEmptyString;

	return dCol->getName();
}


void gqbQueryJoin::setSourceAnchor(wxPoint pt)
{
	sAnchor = pt;
}


void gqbQueryJoin::setDestAnchor(wxPoint pt)
{
	dAnchor = pt;
}


wxPoint &gqbQueryJoin::getSourceAnchor()
{
	return sAnchor;
}


wxPoint &gqbQueryJoin::getDestAnchor()
{
	return dAnchor;
}


void gqbQueryJoin::setSelected(bool value)
{
	this->selected = value;
}


bool gqbQueryJoin::getSelected()
{
	return this->selected;
}


void gqbQueryJoin::setAnchorsUsed(wxPoint pt)
{
	anchorsUsed = pt;
}


wxPoint &gqbQueryJoin::getAnchorsUsed()
{
	return anchorsUsed;
}


//
// A query restriction
//

enum
{
	QRButton = 9000,
	QRValue,
	QRConnector,
	QRtype
};

gqbQueryRestriction::gqbQueryRestriction()
	: gqbObject(wxT(""), NULL, NULL)
{
	leftPart = wxT("");
	value_s = wxT("");
	connector = wxT("AND");
	restriction = wxT("=");
	setType(GQB_RESTRICTION);
}


gqbRestrictions::gqbRestrictions()
	: gqbObjectCollection(wxT(""), NULL, NULL)
{
	setType(GQB_RESTRICTION);
}


gqbRestrictions::~gqbRestrictions()
{
	this->removeAll();
}


void gqbRestrictions::addRestriction(gqbQueryRestriction *r)
{
	this->addObject(r);
}


void gqbRestrictions::addRestrictionAt(gqbQueryRestriction *r, int index)
{
	this->insertObjectAt(r, index);
}


// Remove but don't delete restriction
void gqbRestrictions::removeRestriction(gqbQueryRestriction *r)
{
	this->removeObject(r);
}


void gqbRestrictions::deleteAllRestrictions()
{
	this->deleteAll();
}


gqbIteratorBase *gqbRestrictions::createRestrictionsIterator()
{
	return this->createIterator();
}


int gqbRestrictions::restrictionsCount()
{
	return this->getCount();
}


gqbQueryRestriction *gqbRestrictions::getRestrictionAt(int index)
{
	return (gqbQueryRestriction *)this->getObjectAtIndex(index);
}
