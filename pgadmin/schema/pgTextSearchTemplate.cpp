//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgTextSearchTemplate.cpp - Text Search Template class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgTextSearchTemplate.h"


pgTextSearchTemplate::pgTextSearchTemplate(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, textSearchTemplateFactory, newName)
{
}

pgTextSearchTemplate::~pgTextSearchTemplate()
{
}

bool pgTextSearchTemplate::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql = wxT("DROP TEXT SEARCH TEMPLATE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetIdentifier();

    if (cascaded)
        sql += wxT(" CASCADE");

    return GetDatabase()->ExecuteVoid(sql);
}


wxString pgTextSearchTemplate::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Text Search Template: ") + GetFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP TEXT SEARCH TEMPLATE ") + GetFullIdentifier() + wxT("\n\n")
              wxT("CREATE TEXT SEARCH TEMPLATE ") + GetFullIdentifier() + wxT(" (");
        AppendIfFilled(sql, wxT("\n  INIT = "), GetInit());
        AppendIfFilled(sql, wxT(",\n  LEXIZE = "), GetLexize());
        sql += wxT("\n);\n");

	if (!GetComment().IsNull())
	    sql += wxT("COMMENT ON TEXT SEARCH TEMPLATE ") + GetFullIdentifier()
	    + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
    }

    return sql;
}


void pgTextSearchTemplate::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Init"), GetInit());
        properties->AppendItem(_("Lexize"), GetLexize());
        properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
    }
}



pgObject *pgTextSearchTemplate::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *tstemplate=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        tstemplate = textSearchTemplateFactory.CreateObjects(coll, 0, wxT("\n   AND tmpl.oid=") + GetOidStr());

    return tstemplate;
}



//////////////////////////////////////////////////////


pgObject *pgTextSearchTemplateFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgTextSearchTemplate *tmpl=0;

	pgSet *templates;
	templates = collection->GetDatabase()->ExecuteSet(
		wxT("SELECT tmpl.oid, tmpl.tmplname, tmpl.tmplinit, tmpl.tmpllexize, description\n")
		wxT("  FROM pg_ts_template tmpl\n")
		wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=tmpl.oid\n")
		wxT(" WHERE tmpl.tmplnamespace = ") + collection->GetSchema()->GetOidStr() 
		+ restriction + wxT("\n")
		wxT(" ORDER BY tmpl.tmplname"));

    if (templates)
    {
        while (!templates->Eof())
        {
            tmpl = new pgTextSearchTemplate(collection->GetSchema(), templates->GetVal(wxT("tmplname")));
            tmpl->iSetOid(templates->GetOid(wxT("oid")));
            tmpl->iSetComment(templates->GetVal(wxT("description")));
            if (templates->GetVal(wxT("tmplinit")).Cmp(wxT("-")) != 0)
            {
                tmpl->iSetInit(templates->GetVal(wxT("tmplinit")));
            }
            else
            {
                tmpl->iSetInit(wxT(""));
            }
            tmpl->iSetLexize(templates->GetVal(wxT("tmpllexize")));

            if (browser)
            {
                browser->AppendObject(collection, tmpl);
		templates->MoveNext();
            }
            else
                break;
        }

		delete templates;
    }
    return tmpl;
}


#include "images/template.xpm"
#include "images/templates.xpm"

pgTextSearchTemplateFactory::pgTextSearchTemplateFactory() 
: pgSchemaObjFactory(__("FTS Template"), __("New FTS Template..."), __("Create a new FTS Template."), template_xpm)
{
}


pgTextSearchTemplateFactory textSearchTemplateFactory;
static pgaCollectionFactory cf(&textSearchTemplateFactory, __("FTS Templates"), templates_xpm);
