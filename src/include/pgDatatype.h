//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgDatatype.h - PostgreSQL Datatypes
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DATATYPE_INC
#define __DATATYPE_INC

#include <wx/wx.h>

// App headers

#include "pgSet.h"
#include "pgConn.h"

class pgDatatype
{
public:
    pgDatatype(const wxString &typname, long numdims=0, long typmod=-1);
    wxString Name() const { return name; };
    wxString LengthString() const { return length; }
    wxString Array() const { return array; }
    wxString FullName() const { return name + length + array; }
    wxString QuotedFullName() const { return qtIdent(name) + length + array; }
    long Length() const { return len; }
    long Precision() const { return prec; }
private:
    wxString name;
    wxString length;
    wxString array;
    long len, prec;
};


class DatatypeReader
{
public:
    DatatypeReader(pgConn *conn, bool withDomains=true);
    ~DatatypeReader() { if (set) delete set; }
    bool HasMore() const { return set && !set->Eof(); }
    void MoveNext() { if (set)  set->MoveNext(); }

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

private:
    pgSet *set;
};

#endif
