//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgDomain.h PostgreSQL Domain
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGDOMAIN_H
#define PGDOMAIN_H

#include "pgSchema.h"

class pgCollection;

class pgDomainFactory : public pgSchemaObjFactory
{
public:
    pgDomainFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgDomainFactory domainFactory;


class pgDomain : public pgSchemaObject
{
public:
    pgDomain(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgDomain();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return GetSchema()->GetMetaType() != PGM_CATALOG; }

    wxString GetBasetype() const { return basetype; }
    void iSetBasetype(const wxString& s) { basetype = s; }
    wxString GetQuotedBasetype() const { return quotedBasetype; }
    void iSetQuotedBasetype(const wxString& s) { quotedBasetype = s; }
    void iSetIsDup(bool b) { isDup = b; }
    long GetLength() const { return length; }
    void iSetLength(long l) { length=l; }
    long GetPrecision() const { return precision; }
    void iSetPrecision(long l) { precision = l; }
    wxString GetCheck() const { return check; }
    void iSetCheck(const wxString &s) { check=s; }
    wxString GetDefault() const { return defaultVal; }
    void iSetDefault(const wxString& s) { defaultVal = s; }
    bool GetNotNull() const { return notNull; }
    void iSetNotNull(bool b) { notNull = b; }
    long GetDimensions() const { return dimensions; }
    void iSetDimensions(long l) { dimensions=l; }
    wxString GetDelimiter() const { return delimiter; }
    void iSetDelimiter(const wxString& s) { delimiter = s; }
    OID GetBasetypeOid() const { return basetypeOid; }
    void iSetBasetypeOid(OID d) { basetypeOid = d; }
    long GetTyplen() const { return typlen; }
    void iSetTyplen(const long l) { typlen=l; }
    long GetTypmod() const { return typmod; }
    void iSetTypmod(const long l) { typmod=l; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString basetype, quotedBasetype, defaultVal, delimiter, check;
    long length, precision, dimensions;
    long typlen, typmod;
    bool notNull, isDup;
    OID basetypeOid;
};

#endif
