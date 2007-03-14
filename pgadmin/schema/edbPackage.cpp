//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgpackage.cpp 5828 2007-01-04 16:41:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// edbPackage.cpp - EnterpriseDB Package class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/edbPackage.h"

edbPackage::edbPackage(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, packageFactory, newName)
{
}

bool edbPackage::IsUpToDate()
{
    wxString sql = wxT("SELECT xmin FROM edb_package WHERE oid = ") + this->GetOidStr();
	if (!this->GetDatabase()->GetConnection() || this->GetDatabase()->ExecuteScalar(sql) != NumToStr(GetXid()))
		return false;
	else
		return true;
}

bool edbPackage::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql=wxT("DROP PACKAGE ") + GetQuotedFullIdentifier();

    return GetDatabase()->ExecuteVoid(sql);
}

wxString edbPackage::GetSql(ctlTree *browser)
{
    wxString qtName = GetQuotedFullIdentifier();

    if (sql.IsNull())
    {
        sql = wxT("-- Package: ") + qtName + wxT("\n\n")
              wxT("-- DROP PACKAGE ") + qtName;

        sql += wxT("\n\n");
        sql += GetHeader();
        sql += wxT("\n\n");
        sql += GetBody();

        sql += wxT("\n\n");
        sql += GetGrant(wxT("X"), wxT("PACKAGE ") + qtName);

        sql += wxT("\n");
    }

    return sql;
}

wxString edbPackage::GetHeaderInner()
{
    return GetInner(GetHeader());
}

wxString edbPackage::GetBodyInner()
{
    return GetInner(GetBody());
}

wxString edbPackage::GetInner(const wxString &def)
{
    long start=0, end=0;

    wxStringTokenizer tkz(def, wxT("\t\r\n "));

    // Find the opening AS/IF keyword
    while ( tkz.HasMoreTokens() )
    {
        wxString token = tkz.GetNextToken();
        if (token.Lower() == wxT("as") || token.Lower() == wxT("is"))
        {
            start = tkz.GetPosition();
            break;
        }
    }

    // Find the closing END keyword
    wxString tmp = def;
    tmp.Replace(wxT("\n"), wxT(" "));
    tmp.Replace(wxT("\r"), wxT(" "));
    tmp.Replace(wxT("\t"), wxT(" "));

    int e1 = tmp.Lower().rfind(wxT(" end;"));
    int e2 = tmp.Lower().rfind(wxT(" end "));

    end = (e1 > e2 ? e1 : e2);

    return def.Mid(start, end - start);
}

void edbPackage::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        browser->RemoveDummyChild(this);

        // Log
        wxLogInfo(wxT("Adding child object to package ") + GetIdentifier());

        browser->AppendCollection(this, packageFunctionFactory);
        browser->AppendCollection(this, packageProcedureFactory);
        browser->AppendCollection(this, packageVariableFactory);
    }


    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Header"), firstLineOnly(GetHeader()));
        properties->AppendItem(_("Body"), firstLineOnly(GetBody()));
        properties->AppendItem(_("ACL"), GetAcl());
        properties->AppendItem(_("System package?"), GetSystemObject());
    }
}



pgObject *edbPackage::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *package=0;

    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
    {
        if (coll->GetConnection()->EdbMinimumVersion(8, 2))
            package = packageFactory.CreateObjects(coll, 0, wxT(" AND nspname='") + GetName() + wxT("'"));
        else
            package = packageFactory.CreateObjects(coll, 0, wxT(" AND pkgname='") + GetName() + wxT("'"));
    }

    return package;
}



pgObject *edbPackageFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    edbPackage *package=0;

    wxString sql;
    
    if (collection->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2))
    {
        sql = wxT("SELECT oid, xmin, nspname AS pkgname, nspbodysrc AS pkgbodysrc, nspheadsrc AS pkgheadsrc,\n")
              wxT("       nspacl AS pkgacl, pg_get_userbyid(nspowner) AS owner\n") 
              wxT("  FROM pg_namespace")
              wxT("  WHERE nspparent = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n")
              + restriction +
              wxT("  ORDER BY nspname;");
    }
    else
    {

        sql = wxT("SELECT oid, xmin, *, pg_get_userbyid(pkgowner) AS owner\n") 
              wxT("  FROM edb_package")
              wxT("  WHERE pkgnamespace = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n")
              + restriction +
              wxT("  ORDER BY pkgname;");
    }

    pgSet *packages = collection->GetDatabase()->ExecuteSet(sql);

    if (packages)
    {
        while (!packages->Eof())
        {
            wxString name=packages->GetVal(wxT("pkgname"));
            package = new edbPackage(collection->GetSchema(), name);

            package->iSetOid(packages->GetOid(wxT("oid")));
            package->iSetXid(packages->GetOid(wxT("xmin")));
            package->iSetDatabase(collection->GetDatabase());
            package->iSetOwner(packages->GetVal(wxT("owner")));
            package->iSetHeader(packages->GetVal(wxT("pkgheadsrc")));
            package->iSetBody(packages->GetVal(wxT("pkgbodysrc")));

            package->iSetAcl(packages->GetVal(wxT("pkgacl")));

            if (browser)
            {
                browser->AppendObject(collection, package);
			    packages->MoveNext();
            }
            else
                break;
        }
		delete packages;
    }
    return package;
}


/////////////////////////////

#include "images/package.xpm"
#include "images/packages.xpm"

edbPackageFactory::edbPackageFactory()
: pgSchemaObjFactory(__("Package"), __("New Package..."), __("Create a new package."), package_xpm)
{
}


edbPackageFactory packageFactory;
static pgaCollectionFactory cf(&packageFactory, __("Packages"), packages_xpm);
