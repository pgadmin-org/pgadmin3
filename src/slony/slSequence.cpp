//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSequence.cpp PostgreSQL Slony-I sequence
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "slSequence.h"
#include "slObject.h"
#include "slSet.h"
#include "slCluster.h"
#include "frmMain.h"


slSequence::slSequence(slSet *s, const wxString& newName)
: slSetObject(s, SL_SEQUENCE, newName)
{
    wxLogInfo(wxT("Creating a slSequence object"));
}

slSequence::~slSequence()
{
    wxLogInfo(wxT("Destroying a slSequence object"));
}


bool slSequence::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("setdropsequence(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slSequence::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Register sequence ") + GetName() + wxT(" for replication.\n\n")
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("setaddsequence(") 
                    + NumToStr(GetSet()->GetSlId()) + wxT(", ") 
                    + NumToStr(GetSlId()) + wxT(", ")
                    + qtString(GetName()) + wxT(", ")
                    + qtString(GetComment()) + wxT(");\n");
    }
    return sql;
}


void slSequence::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for sequence ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("ID"), GetSlId());
        properties->AppendItem(_("Active"), GetActive());
    }
}



pgObject *slSequence::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *sequence=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        slSetCollection *coll=(slSetCollection*)browser->GetItemData(parentItem);
        if (coll->GetType() == SL_SEQUENCES)
            sequence = ReadObjects(coll, 0, wxT(" WHERE seq_id=") + NumToStr(GetSlId()) + wxT("\n"));
    }
    return sequence;
}



pgObject *slSequence::ReadObjects(slSetCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slSequence *sequence=0;

    pgSet *sequences = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT seq_id, seq_set, nspname, relname, seq_comment\n")
        wxT("  FROM ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_sequence\n")
        wxT("  JOIN ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_set ON set_id=seq_set\n")
        wxT("  JOIN pg_class cl ON cl.oid=seq_reloid\n")
        wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
         + restriction +
        wxT(" ORDER BY seq_id"));

    if (sequences)
    {
        while (!sequences->Eof())
        {
            sequence = new slSequence(coll->GetSet(), sequences->GetVal(wxT("nspName")) + wxT(".") + sequences->GetVal(wxT("relname")));
            sequence->iSetSlId(sequences->GetLong(wxT("seq_id")));
            sequence->iSetComment(sequences->GetVal(wxT("seq_comment")));

            if (browser)
            {
                coll->AppendBrowserItem(browser, sequence);
				sequences->MoveNext();
            }
            else
                break;
        }

		delete sequences;
    }
    return sequence;
}


    
pgObject *slSequence::ReadObjects(slSetCollection *coll, wxTreeCtrl *browser)
{
    // Get the sequences
    wxString restriction = wxT(" WHERE seq_set = ") + NumToStr(coll->GetSet()->GetSlId());
    return ReadObjects(coll, browser, restriction);
}

