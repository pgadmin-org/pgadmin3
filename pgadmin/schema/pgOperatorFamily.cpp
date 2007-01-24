//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgOperatorFamily.cpp 5884 2007-01-23 12:22:27Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgOperatorFamily.cpp - OperatorFamily class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgOperatorFamily.h"
#include "schema/pgFunction.h"


pgOperatorFamily::pgOperatorFamily(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, operatorFamilyFactory, newName)
{
}

pgOperatorFamily::~pgOperatorFamily()
{
}

bool pgOperatorFamily::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql=wxT("DROP OPERATOR FAMILY ") + GetQuotedFullIdentifier() + wxT(" USING ") + GetAccessMethod();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}

wxString pgOperatorFamily::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Operator Family: \"") + GetName() + wxT("\"\n\n")
            + wxT("-- DROP OPERATOR FAMILY ") + GetQuotedFullIdentifier() + wxT(" USING ") + GetAccessMethod() + wxT(";")
            + wxT("\n\nCREATE OPERATOR FAMILY ") + GetQuotedFullIdentifier()
            + wxT(" USING ") + GetAccessMethod()
			+ wxT(";");
    }

    return sql;
}


void pgOperatorFamily::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Access method"), GetAccessMethod());
        properties->AppendItem(_("System operator family?"), GetSystemObject());
        properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
    }
}



pgObject *pgOperatorFamily::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *operatorFamily=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        operatorFamily = operatorFamilyFactory.CreateObjects(coll, 0, wxT("\n   AND opf.oid=") + GetOidStr());

    return operatorFamily;
}


///////////////////////////////////////////////////


pgObject *pgOperatorFamilyFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgOperatorFamily *operatorFamily=0;

	pgSet *operatorFamilies;

	operatorFamilies = collection->GetDatabase()->ExecuteSet(
		wxT("SELECT opf.oid, opf.*, pg_get_userbyid(opf.opfowner) as opowner, amname\n")
		wxT("  FROM pg_opfamily opf\n")
		wxT("  JOIN pg_am am ON am.oid=opf.opfmethod\n")
		wxT(" WHERE opfnamespace = ") + collection->GetSchema()->GetOidStr()
		+ restriction + wxT("\n")
		wxT(" ORDER BY opfname"));


    if (operatorFamilies)
    {
        while (!operatorFamilies->Eof())
        {
            operatorFamily = new pgOperatorFamily(
                        collection->GetSchema(), operatorFamilies->GetVal(wxT("opfname")));

            operatorFamily->iSetOid(operatorFamilies->GetOid(wxT("oid")));
            operatorFamily->iSetOwner(operatorFamilies->GetVal(wxT("opowner")));
            operatorFamily->iSetAccessMethod(operatorFamilies->GetVal(wxT("amname")));

            if (browser)
            {
                browser->AppendObject(collection, operatorFamily);
				operatorFamilies->MoveNext();
            }
            else
                break;
        }

		delete operatorFamilies;
    }
    return operatorFamily;
}


#include "images/operatorfamily.xpm"
#include "images/operatorfamilies.xpm"

pgOperatorFamilyFactory::pgOperatorFamilyFactory() 
: pgSchemaObjFactory(__("Operator Family"), __("New Operator Family..."), __("Create a new Operator Family."), operatorfamily_xpm)
{
    metaType = PGM_OPFAMILY;
}

dlgProperty *pgOperatorFamilyFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return 0; // not implemented
}

pgOperatorFamilyFactory operatorFamilyFactory;
static pgaCollectionFactory cf(&operatorFamilyFactory, __("Operator Families"), operatorfamilies_xpm);
