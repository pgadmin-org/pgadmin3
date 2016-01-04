//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gpPartition.h Greenplum Partitioned Table Partition
//
//////////////////////////////////////////////////////////////////////////

#ifndef gpPartition_H
#define gpPartition_H

#include "pgSchema.h"
#include "pgTable.h"



class gpPartitionFactory : public pgTableObjFactory
{
public:
	gpPartitionFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent) ;
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	virtual void AppendMenu(wxMenu *menu);
};

extern gpPartitionFactory partitionFactory;

class gpPartition : public pgTable
{
public:
	gpPartition(pgSchema *newSchema, const wxString &newName = wxT(""));
	~gpPartition();
	bool CanCreate();
	wxMenu *GetNewMenu();
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	wxString GetPartitionName()
	{
		return partitionname;
	}
	void iSetPartitionName(const wxString &pn)
	{
		partitionname = pn;
	}

private:
	wxString partitionname;
};


class gpPartitionObject : public pgTableObject
{
public:
	gpPartitionObject(gpPartition *newTable, pgaFactory &factory, const wxString &newName = wxT(""))
		: pgTableObject(newTable, factory, newName) { };
	virtual gpPartition *GetTable() const
	{
		return dynamic_cast<gpPartition *>(table);
	}
	OID GetTableOid() const
	{
		return table->GetOid();
	}
	wxString GetTableOidStr() const
	{
		return NumToStr(table->GetOid()) + wxT("::oid");
	}
};


class gpPartitionCollection : public pgTableCollection
{
public:
	gpPartitionCollection(pgaFactory *factory, gpPartition *_table);
	virtual bool CanCreate()
	{
		return false;
	};
};


class gpPartitionObjCollection : public pgTableObjCollection
{
public:
	gpPartitionObjCollection(pgaFactory *factory, gpPartition *_table)
		: pgTableObjCollection(factory, (pgTable *)_table ) { };
	virtual pgTable *GetTable() const
	{
		return table;
	}
	virtual bool CanCreate();
};



#endif
