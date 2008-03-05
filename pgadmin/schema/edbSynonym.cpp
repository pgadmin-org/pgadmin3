//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgsynonym.cpp 5828 2007-01-04 16:41:08Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// edbSynonym.cpp - EnterpriseDB Synonym class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/edbSynonym.h"

edbSynonym::edbSynonym(const wxString& newName)
: pgDatabaseObject(synonymFactory, newName)
{
}

bool edbSynonym::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql=wxT("DROP PUBLIC SYNONYM ") + GetQuotedIdentifier();

    return GetDatabase()->ExecuteVoid(sql);
}

wxString edbSynonym::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Public synonym: ") + GetQuotedIdentifier() + wxT("\n\n")
              wxT("-- DROP PUBLIC SYNONYM ") + GetQuotedIdentifier();

        sql += wxT("\n\nCREATE OR REPLACE PUBLIC SYNONYM ") + GetQuotedIdentifier() + wxT(" FOR ");

        if (GetTargetSchema() != wxEmptyString)
            sql += qtIdent(GetTargetSchema()) + wxT(".");

        sql += qtIdent(GetTargetObject()) + wxT(";\n");
    }

    return sql;
}

void edbSynonym::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Target type"), GetTargetType());
        properties->AppendItem(_("Target schema"), GetTargetSchema());
        properties->AppendItem(_("Target object"), GetTargetObject());
        properties->AppendItem(_("System synonym?"), GetSystemObject());
    }
}



pgObject *edbSynonym::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *synonym=0;

    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        synonym = synonymFactory.CreateObjects(coll, 0, wxT(" WHERE synname=") + qtDbString(GetName()));

    return synonym;
}



pgObject *edbSynonymFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    edbSynonym *synonym=0;

    wxString sql = wxT("SELECT *, pg_get_userbyid(synowner) AS owner,\n") 
                   wxT("  COALESCE((SELECT \n")
                   wxT("    CASE WHEN relkind = 'r' THEN '");
             sql += _("Table");
             sql += wxT("'\n         WHEN relkind = 'S' THEN '");
             sql += _("Sequence");
             sql += wxT("'\n         WHEN relkind = 'v' THEN '");
             sql += _("View");
             sql += wxT("'\n    END\n")
                    wxT("  FROM pg_class c, pg_namespace n\n")
                    wxT("  WHERE c.relnamespace = n.oid\n") 
                    wxT("    AND n.nspname = synobjschema\n")
                    wxT("    AND c.relname = synobjname), '");
             sql += _("Public synonym");
             sql += wxT("') AS targettype\n")
                    wxT("  FROM pg_synonym")
                    + restriction +
                    wxT("  ORDER BY synname;");

    pgSet *synonyms = collection->GetDatabase()->ExecuteSet(sql);

    if (synonyms)
    {
        while (!synonyms->Eof())
        {
            wxString name=synonyms->GetVal(wxT("synname"));
            synonym = new edbSynonym(name);

            synonym->iSetDatabase(collection->GetDatabase());
            synonym->iSetOwner(synonyms->GetVal(wxT("owner")));
            synonym->iSetTargetType(synonyms->GetVal(wxT("targettype")));
            synonym->iSetTargetSchema(synonyms->GetVal(wxT("synobjschema")));
            synonym->iSetTargetObject(synonyms->GetVal(wxT("synobjname")));

            if (browser)
            {
                browser->AppendObject(collection, synonym);
			    synonyms->MoveNext();
            }
            else
                break;
        }
		delete synonyms;
    }
    return synonym;
}


/////////////////////////////

#include "images/synonym.xpm"
#include "images/synonyms.xpm"

edbSynonymFactory::edbSynonymFactory()
: pgDatabaseObjFactory(__("Synonym"), __("New Public Synonym..."), __("Create a new Public Synonym."), synonym_xpm)
{
}


edbSynonymFactory synonymFactory;
static pgaCollectionFactory cf(&synonymFactory, __("Public Synonyms"), synonyms_xpm);
