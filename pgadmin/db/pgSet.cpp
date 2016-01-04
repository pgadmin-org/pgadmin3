//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgSet.cpp - PostgreSQL ResultSet class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// App headers
#include "db/pgSet.h"
#include "db/pgConn.h"
#include "utils/sysLogger.h"
#include "utils/pgDefs.h"

pgSet::pgSet()
	: conv(wxConvLibc)
{
	conn = 0;
	res = 0;
	nCols = 0;
	nRows = 0;
	pos = 0;
}

pgSet::pgSet(PGresult *newRes, pgConn *newConn, wxMBConv &cnv, bool needColQt)
	: conv(cnv)
{
	needColQuoting = needColQt;

	conn = newConn;
	res = newRes;

	// Make sure we have tuples
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		nCols = 0;
		nRows = 0;
		pos = 0;
	}
	else
	{
		nCols = PQnfields(res);
		for (int x = 0; x < nCols + 1; x++)
		{
			colTypes.Add(wxT(""));
			colFullTypes.Add(wxT(""));
			colClasses.Add(0);
		}

		nRows = PQntuples(res);
		MoveFirst();
	}
}


pgSet::~pgSet()
{
	PQclear(res);
}



OID pgSet::ColTypeOid(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	return PQftype(res, col);
}

long pgSet::ColTypeMod(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	return PQfmod(res, col);
}


long pgSet::GetInsertedCount() const
{
	char *cnt = PQcmdTuples(res);
	if (!*cnt)
		return -1;
	else
		return atol(cnt);
}


pgTypClass pgSet::ColTypClass(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	if (colClasses[col] != 0)
		return (pgTypClass)colClasses[col];

	wxString typoid = ExecuteScalar(
	                      wxT("SELECT CASE WHEN typbasetype=0 THEN oid else typbasetype END AS basetype\n")
	                      wxT("  FROM pg_type WHERE oid=") + NumToStr(ColTypeOid(col)));

	switch (StrToLong(typoid))
	{
		case PGOID_TYPE_BOOL:
			colClasses[col] = PGTYPCLASS_BOOL;
			break;
		case PGOID_TYPE_INT8:
		case PGOID_TYPE_INT2:
		case PGOID_TYPE_INT4:
		case PGOID_TYPE_OID:
		case PGOID_TYPE_XID:
		case PGOID_TYPE_TID:
		case PGOID_TYPE_CID:
		case PGOID_TYPE_FLOAT4:
		case PGOID_TYPE_FLOAT8:
		case PGOID_TYPE_MONEY:
		case PGOID_TYPE_BIT:
		case PGOID_TYPE_NUMERIC:
			colClasses[col] = PGTYPCLASS_NUMERIC;
			break;
		case PGOID_TYPE_BYTEA:
		case PGOID_TYPE_CHAR:
		case PGOID_TYPE_NAME:
		case PGOID_TYPE_TEXT:
		case PGOID_TYPE_VARCHAR:
			colClasses[col] = PGTYPCLASS_STRING;
			break;
		case PGOID_TYPE_TIMESTAMP:
		case PGOID_TYPE_TIMESTAMPTZ:
		case PGOID_TYPE_TIME:
		case PGOID_TYPE_TIMETZ:
		case PGOID_TYPE_INTERVAL:
			colClasses[col] = PGTYPCLASS_DATE;
			break;
		default:
			colClasses[col] = PGTYPCLASS_OTHER;
			break;
	}

	return (pgTypClass)colClasses[col];
}


wxString pgSet::ColType(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	if (!colTypes[col].IsEmpty())
		return colTypes[col];

	wxString szSQL, szResult;
	szSQL.Printf(wxT("SELECT format_type(oid,NULL) as typname FROM pg_type WHERE oid = %d"), (int)ColTypeOid(col));
	szResult = ExecuteScalar(szSQL);
	colTypes[col] = szResult;

	return szResult;
}

wxString pgSet::ColFullType(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	if (!colFullTypes[col].IsEmpty())
		return colFullTypes[col];

	wxString szSQL, szResult;
	szSQL.Printf(wxT("SELECT format_type(oid,%d) as typname FROM pg_type WHERE oid = %d"), (int)ColTypeMod(col), (int)ColTypeOid(col));
	szResult = ExecuteScalar(szSQL);
	colFullTypes[col] = szResult;

	return szResult;
}

int pgSet::ColScale(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	// TODO
	return 0;
}
wxString pgSet::ColName(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	return wxString(PQfname(res, col), conv);
}


int pgSet::ColNumber(const wxString &colname) const
{
	int col;

	if (needColQuoting)
	{
		wxString quotedColName = colname;
		quotedColName.Replace(wxT("\""), wxT("\"\""));
		col = PQfnumber(res, (wxT("\"") + quotedColName + wxT("\"")).mb_str(conv));
	}
	else
		col = PQfnumber(res, colname.mb_str(conv));

	if (col < 0)
	{
		wxLogError(__("Column not found in pgSet: %s"), colname.c_str());
	}
	return col;
}

bool pgSet::HasColumn(const wxString &colname) const
{
	if (needColQuoting)
	{
		wxString quotedColName = colname;
		quotedColName.Replace(wxT("\""), wxT("\"\""));
		return (PQfnumber(res, (wxT("\"") + quotedColName + wxT("\"")).mb_str(conv)) < 0 ? false : true);
	}
	else
		return (PQfnumber(res, colname.mb_str(conv)) < 0 ? false : true);

}



char *pgSet::GetCharPtr(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	return PQgetvalue(res, pos - 1, col);
}


char *pgSet::GetCharPtr(const wxString &col) const
{
	return PQgetvalue(res, pos - 1, ColNumber(col));
}


wxString pgSet::GetVal(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	return wxString(GetCharPtr(col), conv);
}


wxString pgSet::GetVal(const wxString &colname) const
{
	return GetVal(ColNumber(colname));
}


long pgSet::GetLong(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	char *c = PQgetvalue(res, pos - 1, col);
	if (c)
		return atol(c);
	else
		return 0;
}


long pgSet::GetLong(const wxString &col) const
{
	char *c = PQgetvalue(res, pos - 1, ColNumber(col));
	if (c)
		return atol(c);
	else
		return 0;
}


bool pgSet::GetBool(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	char *c = PQgetvalue(res, pos - 1, col);
	if (c)
	{
		if (*c == 't' || *c == '1' || !strcmp(c, "on"))
			return true;
	}
	return false;
}


bool pgSet::GetBool(const wxString &col) const
{
	return GetBool(ColNumber(col));
}


wxDateTime pgSet::GetDateTime(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	wxDateTime dt;
	wxString str = GetVal(col);
	/* This hasn't just been used. ( Is not infinity ) */
	if (!str.IsEmpty())
		dt.ParseDateTime(str);
	return dt;
}


wxDateTime pgSet::GetDateTime(const wxString &col) const
{
	return GetDateTime(ColNumber(col));
}


wxDateTime pgSet::GetDate(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	wxDateTime dt;
	wxString str = GetVal(col);
	/* This hasn't just been used. ( Is not infinity ) */
	if (!str.IsEmpty())
		dt.ParseDate(str);
	return dt;
}


wxDateTime pgSet::GetDate(const wxString &col) const
{
	return GetDate(ColNumber(col));
}


double pgSet::GetDouble(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	return StrToDouble(GetVal(col));
}


double pgSet::GetDouble(const wxString &col) const
{
	return GetDouble(ColNumber(col));
}


wxULongLong pgSet::GetLongLong(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	char *c = PQgetvalue(res, pos - 1, col);
	if (c)
		return atolonglong(c);
	else
		return 0;
}

wxULongLong pgSet::GetLongLong(const wxString &col) const
{
	return GetLongLong(ColNumber(col));
}


OID pgSet::GetOid(const int col) const
{
	wxASSERT(col < nCols && col >= 0);

	char *c = PQgetvalue(res, pos - 1, col);
	if (c)
		return (OID)strtoul(c, 0, 10);
	else
		return 0;
}


OID pgSet::GetOid(const wxString &col) const
{
	return GetOid(ColNumber(col));
}


wxString pgSet::ExecuteScalar(const wxString &sql) const
{
	return conn->ExecuteScalar(sql);
}


//////////////////////////////////////////////////////////////////

pgSetIterator::pgSetIterator(pgConn *conn, const wxString &qry)
{
	set = conn->ExecuteSet(qry);
	first = true;
}


pgSetIterator::pgSetIterator(pgSet *s)
{
	set = s;
	first = true;
}


pgSetIterator::~pgSetIterator()
{
	if (set)
		delete set;
}


bool pgSetIterator::RowsLeft()
{
	if (!set)
		return false;

	if (first)
	{
		if (!set->NumRows())
			return false;
		first = false;
	}
	else
		set->MoveNext();

	return !set->Eof();
}


bool pgSetIterator::MovePrev()
{
	if (!set)
		return false;

	set->MovePrevious();
	return true;
}
