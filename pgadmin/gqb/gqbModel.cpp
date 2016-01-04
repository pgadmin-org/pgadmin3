//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbModel.cpp - Model of MVC Pattern for GQB
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbModel.h"
#include "gqb/gqbQueryObjs.h"

gqbModel::gqbModel():
	wxObject()
{
// here store all queryObjects
// GQB-TODO: allow different names for each model
	queryCollection = new gqbQueryObjs();
	restrictions = new gqbRestrictions();
	columnsAlias = new  wxArrayString();
}


// GQB-TODO: check this destructor is not complete
gqbModel::~gqbModel()
{
	if(queryCollection)
		delete queryCollection;

	// Don't owns objects only remove then in both
	colsPosition.Empty();
	colsParents.Empty();
	if(columnsAlias)
		delete columnsAlias;

	//GQB-TODO: delete restrictions
	if(restrictions)
		delete restrictions;
}


gqbQueryObject *gqbModel::addTable(gqbTable *table, wxPoint p)
{
	// Get a table but introduce a QueryObject
	gqbQueryObject *tmp = new gqbQueryObject(table);
	tmp->position = p;

	// Now use insert the new object in the collection of the model
	queryCollection->addTable(tmp);

	// Columns of added table should be possible to use on Order By Clause
	gqbIteratorBase *iterator = tmp->parent->createColumnsIterator();
	while(iterator->HasNext())
	{
		gqbColumn *col = (gqbColumn *)iterator->Next();
		AvailableColumns.Add(col);
		ColumnAvailParent.Add(tmp);
	}
	delete iterator;

	return tmp;
}


gqbIteratorBase  *gqbModel::createQueryIterator()
{
	return queryCollection->createQueryIterator();
}

gqbIteratorBase  *gqbModel::createDownQueryIterator()
{
	return queryCollection->createDownQueryIterator();
}

void gqbModel::deleteTable(gqbQueryObject *modelTable)
{
	if(modelTable)
	{
		queryCollection->removeTable(modelTable);
		delete modelTable;
		modelTable = NULL;
	}
}


int gqbModel::tablesCount()
{
	return queryCollection->tablesCount();
}


void gqbModel::emptyAll()
{
	colsPosition.Empty();
	colsParents.Empty();
	queryCollection->removeAllQueryObjs();

}


gqbQueryRestriction *gqbModel::addRestriction()
{
	gqbQueryRestriction *r = new gqbQueryRestriction();
	restrictions->addRestriction(r);
	return r;
}
