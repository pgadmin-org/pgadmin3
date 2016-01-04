//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbObject.h - Main basic object used by GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBOBJECT_H
#define GQBOBJECT_H

enum type_gqbObject
{
	GQB_DATABASE,
	GQB_SCHEMA,
	GQB_TABLE,
	GQB_VIEW,
	GQB_COLUMN,
	GQB_QUERYOBJ,
	GQB_QUERY,
	GQB_JOIN,
	GQB_RESTRICTION
};

// Create Array Objects used as base for gqbCollections
class gqbObject : public wxTreeItemData
{
public:
	gqbObject(wxString name, wxTreeItemData *owner, pgConn *connection, OID oid = 0);
	virtual ~gqbObject();
	const wxString &getName()
	{
		return Name;
	}
	wxTreeItemData *getOwner()
	{
		return Owner;
	}
	const type_gqbObject getType()
	{
		return Type;
	}
	void setType(const type_gqbObject type)
	{
		Type = type;
	}
	pgConn *getConnection()
	{
		return conn;
	}
	OID getOid()
	{
		return Oid;
	}

protected:
	pgConn *conn;

private:
	wxString Name;
	wxTreeItemData *Owner;
	type_gqbObject Type;
	OID Oid;
};
#endif
