//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
#include "frmMain.h"

pgLanguage::pgLanguage(const wxString& newName)
: pgObject(PG_LANGUAGE, newName)
{
    wxLogInfo(wxT("Creating a pgLanguage object"));
}

pgLanguage::~pgLanguage()
{
    wxLogInfo(wxT("Destroying a pgLanguage object"));
}



wxString pgLanguage::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE ");
        if (GetTrusted())
            sql += wxT("TRUSTED ");
        sql += wxT("PROCEDURAL LANGUAGE '") + GetName() 
            +  wxT("' HANDLER ") + GetHandlerProc() + wxT(";\n")
            +  GetGrant(GetTypeName(), true);

    }
    return sql;
}

void pgLanguage::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (form)
        form->SetButtons(true, true, true, true, false, false, false);

    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

    int pos=0;

    InsertListItem(properties, pos++, wxT("Name"), GetName());
    InsertListItem(properties, pos++, wxT("OID"), GetOid());
    InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
    InsertListItem(properties, pos++, wxT("Trusted?"), GetTrusted());
    InsertListItem(properties, pos++, wxT("Handler"), GetHandlerProc());
    InsertListItem(properties, pos++, wxT("Validator"), GetValidatorProc());
    InsertListItem(properties, pos++, wxT("System Object?"), GetSystemObject());
}



void pgLanguage::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgLanguage *language;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {

        // Log
        msg.Printf(wxT("Adding Languages to database %s"), collection->GetDatabase()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Get the Languages
        pgSet *languages= collection->GetDatabase()->ExecuteSet(wxT(
           "SELECT lan.oid, lan.lanname, lanpltrusted, lanacl, hp.proname as lanproc, vp.proname as lanval\n"
           "  FROM pg_language lan\n"
           "  JOIN pg_proc hp on hp.oid=lanplcallfoid\n"
           "  LEFT OUTER JOIN pg_proc vp on vp.oid=lanvalidator\n"
           " WHERE lanispl IS TRUE"
           " ORDER BY lanname"));

        if (languages)
        {
            while (!languages->Eof())
            {

                language = new pgLanguage(languages->GetVal(wxT("lanname")));
                language->SetDatabase(collection->GetDatabase());
                language->iSetOid(languages->GetOid(wxT("oid")));
                language->iSetAcl(languages->GetVal(wxT("lanacl")));
                language->iSetHandlerProc(languages->GetVal(wxT("lanproc")));
                language->iSetValidatorProc(languages->GetVal(wxT("lanval")));
                language->iSetTrusted(languages->GetBool(wxT("lanpltrusted")));

                browser->AppendItem(collection->GetId(), language->GetIdentifier(), PGICON_LANGUAGE, -1, language);
	    
			    languages->MoveNext();
            }

		    delete languages;
        }
    }
}

