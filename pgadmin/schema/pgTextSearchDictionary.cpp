//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// pgTextSearchDictionary.cpp - Text Search Dictionary class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgTextSearchDictionary.h"


pgTextSearchDictionary::pgTextSearchDictionary(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, textSearchDictionaryFactory, newName)
{
}

pgTextSearchDictionary::~pgTextSearchDictionary()
{
}

bool pgTextSearchDictionary::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql = wxT("DROP TEXT SEARCH DICTIONARY ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetIdentifier();

    if (cascaded)
        sql += wxT(" CASCADE");

    return GetDatabase()->ExecuteVoid(sql);
}


wxString pgTextSearchDictionary::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Text Search Dictionary: ") + GetFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP TEXT SEARCH DICTIONARY ") + GetFullIdentifier() + wxT("\n\n")
            + wxT("CREATE TEXT SEARCH DICTIONARY ") + GetFullIdentifier() + wxT(" (")
            + wxT("\n   TEMPLATE = ") + qtTypeIdent(GetTemplate());

        if (options.Length() > 0)
            sql += wxT(",\n   ") + options;

        sql += wxT("\n);\n");

	if (!GetComment().IsNull())
	    sql += wxT("COMMENT ON TEXT SEARCH DICTIONARY ") + GetFullIdentifier()
	    + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
    }

    return sql;
}


void pgTextSearchDictionary::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Template"), GetTemplate());
        properties->AppendItem(_("Options"), GetOptions());
        properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
    }
}



pgObject *pgTextSearchDictionary::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *dict=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        dict = textSearchDictionaryFactory.CreateObjects(coll, 0, wxT("\n   AND dict.oid=") + GetOidStr());

    return dict;
}



//////////////////////////////////////////////////////


pgObject *pgTextSearchDictionaryFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgTextSearchDictionary *dict=0;

	pgSet *dictionaries;
	dictionaries = collection->GetDatabase()->ExecuteSet(
		wxT("SELECT dict.oid, dict.dictname, pg_get_userbyid(dict.dictowner) as dictowner, t.tmplname, dict.dictinitoption, description\n")
		wxT("  FROM pg_ts_dict dict\n")
		wxT("  LEFT OUTER JOIN pg_ts_template t ON t.oid=dict.dicttemplate\n")
		wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=dict.oid\n")
		wxT(" WHERE dict.dictnamespace = ") + collection->GetSchema()->GetOidStr() 
		+ restriction + wxT("\n")
		wxT(" ORDER BY dict.dictname"));

    if (dictionaries)
    {
        while (!dictionaries->Eof())
        {
            dict = new pgTextSearchDictionary(collection->GetSchema(), dictionaries->GetVal(wxT("dictname")));
            dict->iSetOid(dictionaries->GetOid(wxT("oid")));
            dict->iSetOwner(dictionaries->GetVal(wxT("dictowner")));
            dict->iSetComment(dictionaries->GetVal(wxT("description")));
            dict->iSetTemplate(dictionaries->GetVal(wxT("tmplname")));
            dict->iSetOptions(dictionaries->GetVal(wxT("dictinitoption")));

            if (browser)
            {
                browser->AppendObject(collection, dict);
		dictionaries->MoveNext();
            }
            else
                break;
        }

		delete dictionaries;
    }
    return dict;
}


#include "images/dictionary.xpm"
#include "images/dictionaries.xpm"

pgTextSearchDictionaryFactory::pgTextSearchDictionaryFactory() 
: pgSchemaObjFactory(__("FTS Dictionary"), __("New FTS Dictionary..."), __("Create a new FTS Dictionary."), dictionary_xpm)
{
}


pgTextSearchDictionaryFactory textSearchDictionaryFactory;
static pgaCollectionFactory cf(&textSearchDictionaryFactory, __("FTS Dictionaries"), dictionaries_xpm);
