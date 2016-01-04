//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsDeclareRecordStmt.h"

#include "pgscript/objects/pgsRecord.h"

pgsDeclareRecordStmt::pgsDeclareRecordStmt(const wxString &rec,
        const wxArrayString &columns, pgsThread *app) :
	pgsStmt(app), m_rec(rec), m_columns(columns)
{

}

pgsDeclareRecordStmt::~pgsDeclareRecordStmt()
{

}

void pgsDeclareRecordStmt::eval(pgsVarMap &vars) const
{
	pgsRecord *rec = pnew pgsRecord(m_columns.size());
	for (size_t i = 0; i < m_columns.GetCount(); i++)
	{
		rec->set_column_name(i, m_columns.Item(i));
	}
	vars[m_rec] = rec;
}
