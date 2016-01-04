//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbQueryObjs.h - All objects used by a model of a query in the MVC Pattern model.
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBQUERYOBJS_H
#define GQBQUERYOBJS_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/textctrl.h>

// App headers
#include "gqb/gqbTable.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbObjectCollection.h"

#define MAXRECTANGLES 10

class gqbQueryObject;
class gqbQueryJoin;

// GQB-TODO: Add all kinds of joins
enum type_Join
{
	_equally,
	_lesser,
	_greater,
	_equlesser,
	_equgreater

};

// Collection of main Query Objects [Tables]
class gqbQueryObjs : public gqbObjectCollection
{
public:
	gqbQueryObjs();								// No destructor, the base destructor destroy collection
	// the unique dynamic object in this class
	void addTable(gqbQueryObject *mtable);		// Uses alias to only allow operations I want to do it.
	void removeTable(gqbQueryObject *mtable);
	gqbIteratorBase *createQueryIterator();
	gqbIteratorBase *createDownQueryIterator();
	int tablesCount();
	void removeAllQueryObjs();
};

// Collection of main Table Objects [Columns] have joins too but in a new variable
class gqbQueryObject : public gqbObjectCollection
{
public:
	gqbQueryObject(gqbTable *table);
	~gqbQueryObject();
	gqbTable *parent;
	wxPoint position;
	void setSelected(bool value);
	bool getSelected();
	void setWidth(int value);
	int getWidth();
	void setHeight(int value);
	int getHeight();
	void removeColumn(gqbColumn *column);		// Used only as synonym for gqbObjectCollection removeObject
	void addColumn(gqbColumn *column);			// Used only as synonym for gqbObjectCollection addObject
	int getColumnIndex(gqbColumn *column);
	bool existsColumn(gqbColumn *column);
	gqbIteratorBase *createQueryTableIterator();
	gqbIteratorBase *createJoinsIterator();
	gqbIteratorBase *createRegJoinsIterator();
	gqbQueryJoin *addJoin(gqbQueryObject *owner, gqbQueryObject *observable, gqbColumn *source, gqbColumn *destination, type_Join kind);
	void removeJoin(gqbQueryJoin *join, bool unRegister);
	void registerJoin(gqbQueryJoin *join);
	void unregisterJoin(gqbQueryJoin *join, bool removeIt);
	bool getHaveJoins();
	bool getHaveRegJoins();
	void setAlias(wxString name)
	{
		alias = name;
	};
	wxString getAlias()
	{
		return alias;
	};

private:
	bool selected;
	wxString alias;
	int width;
	int height;
	bool haveJoins, haveRegisteredJoins;
	gqbCollection *joinsCollection, *registeredCollection;
	gqbArrayCollection *implementationj, *implementationr ;

};

// A Join Object
class gqbQueryJoin : public gqbObject
{
public:
	gqbQueryJoin(gqbQueryObject *_owner, gqbQueryObject *_destination, gqbColumn *sourceCol, gqbColumn *destCol, type_Join joinKind);
	void setKindofJoin(type_Join join);
	type_Join getKindofJoin();
	gqbQueryObject *getSourceQTable();
	gqbQueryObject *getDestQTable();
	gqbColumn *getDCol();
	gqbColumn *getSCol();
	wxString getSourceTable();
	wxString getDestTable();
	wxString getSourceCol();
	wxString getDestCol();
	void setSourceAnchor(wxPoint pt);
	void setDestAnchor(wxPoint pt);
	wxPoint &getSourceAnchor();
	wxPoint &getDestAnchor();
	void setSelected(bool value);
	bool getSelected();
	void setAnchorsUsed(wxPoint pt);
	wxPoint &getAnchorsUsed();

private:
	bool selected;
	type_Join kindofJoin;
	gqbColumn *sCol, *dCol;
	gqbQueryObject *owner, *destination;
	wxPoint sAnchor, dAnchor;         // The source/destination anchor points of the join (for same join)
	wxPoint anchorsUsed;
};

// A Restriction Object
class gqbQueryRestriction : public gqbObject
{
public:
	gqbQueryRestriction();
	wxString &getLeft()
	{
		return leftPart;
	};
	wxString &getRestriction()
	{
		return restriction;
	};
	wxString &getValue_s()
	{
		return value_s;
	};
	wxString &getConnector()
	{
		return connector;
	};
	void setLeft(const wxString &value)
	{
		leftPart = value;
	};
	void setRestriction(const wxString &value)
	{
		restriction = value;
	};
	void setValue_s(const wxString &value)
	{
		value_s = value;
	};
	void setConnector(const wxString &value)
	{
		connector = value;
	};

private:
	wxString leftPart;
	wxString restriction;
	wxString value_s;
	wxString connector;
};

// Collection of restrictions for a where clause
class gqbRestrictions : public gqbObjectCollection
{
public:
	gqbRestrictions();
	~gqbRestrictions();

	void addRestriction(gqbQueryRestriction *r);
	void removeRestriction(gqbQueryRestriction *r);
	void deleteAllRestrictions();
	gqbIteratorBase *createRestrictionsIterator();
	void addRestrictionAt(gqbQueryRestriction *r, int index);
	int restrictionsCount();
	gqbQueryRestriction *getRestrictionAt(int index);

};
#endif
