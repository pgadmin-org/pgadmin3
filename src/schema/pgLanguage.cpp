//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgLanguage.cpp - Language class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgLanguage.h"
#include "pgCollection.h"


pgLanguage::pgLanguage(const wxString& newName)
: pgDatabaseObject(PG_LANGUAGE, newName)
{
    wxLogInfo(wxT("Creating a pgLanguage object"));
}

pgLanguage::~pgLanguage()
{
    wxLogInfo(wxT("Destroying a pgLanguage object"));
}

bool pgLanguage::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP LANGUAGE ") + GetQuotedFullIdentifier() + wxT(";"));
}

wxString pgLanguage::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Language: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP LANGUAGE ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\n CREATE ");
        if (GetTrusted())
            sql += wxT("TRUSTED ");
        sql += wxT("PROCEDURAL LANGUAGE '") + GetName() 
            +  wxT("'\n  HANDLER ") + GetHandlerProc() + wxT(";\n")
            +  GetGrant(wxT("-"), GetTypeName(), true);

    }
    return sql;
}

void pgLanguage::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("ACL"), GetAcl());
        InsertListItem(properties, pos++, _("Trusted?"), GetTrusted());
        InsertListItem(properties, pos++, _("Handler"), GetHandlerProc());
        InsertListItem(properties, pos++, _("Validator"), GetValidatorProc());
        InsertListItem(properties, pos++, _("System language?"), GetSystemObject());
    }
}



pgObject *pgLanguage::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *language=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_LANGUAGES)
            language = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND lan.oid=") + GetOidStr());
    }
    return language;
}



pgObject *pgLanguage::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgLanguage *language=0;

        pgSet *languages= collection->GetDatabase()->ExecuteSet(
       wxT("SELECT lan.oid, lan.lanname, lanpltrusted, lanacl, hp.proname as lanproc, vp.proname as lanval\n")
       wxT("  FROM pg_language lan\n")
       wxT("  JOIN pg_proc hp on hp.oid=lanplcallfoid\n")
       wxT("  LEFT OUTER JOIN pg_proc vp on vp.oid=lanvalidator\n")
       wxT(" WHERE lanispl IS TRUE")
       + restriction + wxT("\n")
       wxT(" ORDER BY lanname"));

    if (languages)
    {
        while (!languages->Eof())
        {

            language = new pgLanguage(languages->GetVal(wxT("lanname")));
            language->iSetDatabase(collection->GetDatabase());
            language->iSetOid(languages->GetOid(wxT("oid")));
            language->iSetAcl(languages->GetVal(wxT("lanacl")));
            language->iSetHandlerProc(languages->GetVal(wxT("lanproc")));
            language->iSetValidatorProc(languages->GetVal(wxT("lanval")));
            language->iSetTrusted(languages->GetBool(wxT("lanpltrusted")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, language);
	
			    languages->MoveNext();
            }
            else
                break;
        }

		delete languages;
    }
    return language;
}
