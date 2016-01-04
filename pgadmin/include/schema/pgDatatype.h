//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgDatatype.h - PostgreSQL Datatypes
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DATATYPE_INC
#define __DATATYPE_INC

#include <wx/wx.h>

// App headers

#include "db/pgSet.h"
#include "db/pgConn.h"

class pgDatabase;

class pgDatatype
{
public:
	pgDatatype(const wxString &nsp, const wxString &typname, bool isduplicate, long numdims = 0, long typmod = -1);
	wxString Name() const
	{
		return name;
	};
	wxString LengthString() const
	{
		return length;
	}
	wxString Array() const
	{
		return array;
	}
	wxString FullName() const;
	wxString QuotedFullName() const;
	wxString GetSchemaPrefix(pgDatabase *db) const;
	wxString GetQuotedSchemaPrefix(pgDatabase *db) const;
	long Length() const
	{
		return len;
	}
	long Precision() const
	{
		return prec;
	}
	static long GetTypmod(const wxString &name, const wxString &len, const wxString &prec);

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}

private:
	wxString schema;
	wxString name;
	wxString length;
	wxString array;
	long len, prec;
	bool needSchema;
};


class DatatypeReader
{
public:
	DatatypeReader(pgDatabase *db, bool withDomains = true, bool addSerials = false);
	DatatypeReader(pgDatabase *db, const wxString &condition, bool addSerials = false);
	~DatatypeReader()
	{
		if (set) delete set;
	}
	bool HasMore() const
	{
		return set && !set->Eof();
	}
	void MoveNext()
	{
		if (set)  set->MoveNext();
	}

	bool IsDomain() const;
	bool IsVarlen() const;
	bool MaySpecifyLength() const;
	bool MaySpecifyPrecision() const;
	pgDatatype GetDatatype() const;
	wxString GetTypename() const;
	wxString GetSchema() const;
	wxString GetSchemaPrefix() const;
	wxString GetQuotedSchemaPrefix() const;
	wxString GetOidStr() const;
	OID GetOid() const;

private:
	pgSet *set;
	pgDatabase *database;
	void init(pgDatabase *db, const wxString &condition, bool addSerials = false);
};

#endif
