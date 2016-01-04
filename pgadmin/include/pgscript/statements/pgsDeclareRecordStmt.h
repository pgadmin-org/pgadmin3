//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSDECLARERECORDSTMT_H_
#define PGSDECLARERECORDSTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsDeclareRecordStmt : public pgsStmt
{

private:

	const wxString m_rec;
	const wxArrayString m_columns;

public:

	pgsDeclareRecordStmt(const wxString &rec,
	                     const wxArrayString &columns, pgsThread *app = 0);

	virtual ~pgsDeclareRecordStmt();

	virtual void eval(pgsVarMap &vars) const;

private:

	pgsDeclareRecordStmt(const pgsDeclareRecordStmt &that);

	pgsDeclareRecordStmt &operator=(const pgsDeclareRecordStmt &that);

};

#endif /*PGSDECLARERECORDSTMT_H_*/
