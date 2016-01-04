//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbModel.h - Model of MVC Pattern for GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBMODEL_H
#define GQBMODEL_H

// App headers
#include "gqb/gqbQueryObjs.h"

#define MAXRECTANGLES 10

WX_DEFINE_ARRAY_CHAR(char, charArray);

class gqbModel : public wxObject
{
public:
	gqbModel();
	~gqbModel();
	void emptyAll();

	// Tables
	gqbQueryObject *addTable(gqbTable *table, wxPoint p);
	void deleteTable(gqbQueryObject *table);
	gqbIteratorBase *createQueryIterator();
	gqbIteratorBase *createDownQueryIterator();
	int tablesCount();

	// Projection Panel
	gqbObjsArray *getOrderedColumns()
	{
		return &colsPosition;
	};
	gqbObjsArray *getColumnsParents()
	{
		return &colsParents;
	};
	wxArrayString *getColumnsAlias()
	{
		return columnsAlias;
	};

	// Restrictions Panel
	gqbQueryRestriction *addRestriction();    // GQB-TODO: delete if not use this function
	gqbRestrictions *getRestrictions()
	{
		return restrictions;
	};

	// Order By Panel
	gqbObjsArray *getOrdByAvailColumns()
	{
		return &AvailableColumns;
	};
	gqbObjsArray *getOrdByAvailParents()
	{
		return &ColumnAvailParent;
	};
	gqbObjsArray *getOrdByColumns()
	{
		return &OrderedColumns;
	};
	gqbObjsArray *getOrdByParents()
	{
		return &ColumnOrdParent;
	};
	charArray *getOrdByKind()
	{
		return &orderBy;
	};

private:
	// query objects [tables] with joins inside
	gqbQueryObjs *queryCollection;

	// projection Panel
	gqbObjsArray colsPosition;          // Here store position of the columns at Select projection clause
	// [Select c1,c2,c3...,cn from...]
	gqbObjsArray colsParents;			// Because above array only store a column object cannot be recovered
	// the object that store it (gqbQueryObject) [remember can be use same
	// table twice on a query].
	wxArrayString *columnsAlias;

	// restrictions Panel
	gqbRestrictions *restrictions;

	// order by Panel
	// For left grid [available columns to order clause]
	gqbObjsArray AvailableColumns;
	gqbObjsArray ColumnAvailParent;

	// For right grid [used columns on order clause]
	gqbObjsArray OrderedColumns;
	gqbObjsArray ColumnOrdParent;
	charArray orderBy;            // D or A
};
#endif
