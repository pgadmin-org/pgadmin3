//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSequence.cpp PostgreSQL Slony-I sequence
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgObject.h"
#include "slony/slSequence.h"
#include "slony/slSet.h"
#include "slony/slCluster.h"
#include "frm/frmMain.h"


slSequence::slSequence(slSet *s, const wxString& newName)
: slSetObject(s, slSequenceFactory, newName)
{
}

bool slSequence::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("setdropsequence(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slSequence::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Register sequence ") + GetName() + wxT(" for replication.\n\n")
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("setaddsequence(") 
                    + NumToStr(GetSet()->GetSlId()) + wxT(", ") 
                    + NumToStr(GetSlId()) + wxT(", ")
                    + qtDbString(GetName()) + wxT(", ")
                    + qtDbString(GetComment()) + wxT(");\n");
    }
    return sql;
}


void slSequence::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        browser->RemoveDummyChild(this);
    }


    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("ID"), GetSlId());
        properties->AppendItem(_("Active"), GetActive());
    }
}



pgObject *slSequence::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *sequence=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        sequence = slSequenceFactory.CreateObjects(coll, 0, wxT(" WHERE seq_id=") + NumToStr(GetSlId()) + wxT("\n"));

    return sequence;
}



pgObject *slSlSequenceFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restr)
{
    slSetObjCollection *collection=(slSetObjCollection*)coll;
    slSequence *sequence=0;
    wxString restriction;
    if (restr.IsEmpty())
        restriction = wxT(" WHERE seq_set = ") + NumToStr(collection->GetSlId());
    else
        restriction = restr;

    pgSet *sequences = collection->GetDatabase()->ExecuteSet(
        wxT("SELECT seq_id, seq_set, nspname, relname, seq_comment\n")
        wxT("  FROM ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_sequence\n")
        wxT("  JOIN ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_set ON set_id=seq_set\n")
        wxT("  JOIN pg_class cl ON cl.oid=seq_reloid\n")
        wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
         + restriction +
        wxT(" ORDER BY seq_id"));

    if (sequences)
    {
        while (!sequences->Eof())
        {
            sequence = new slSequence(collection->GetSet(), sequences->GetVal(wxT("nspname")) + wxT(".") + sequences->GetVal(wxT("relname")));
            sequence->iSetSlId(sequences->GetLong(wxT("seq_id")));
            sequence->iSetComment(sequences->GetVal(wxT("seq_comment")));

            if (browser)
            {
                browser->AppendObject(collection, sequence);
				sequences->MoveNext();
            }
            else
                break;
        }

		delete sequences;
    }
    return sequence;
}



///////////////////////////////////////////////////

#include "images/sequence-repl.xpm"
#include "images/sequences.xpm"

slSlSequenceFactory::slSlSequenceFactory() 
: slSetObjFactory(__("Sequence"), __("New Sequence"), __("Create a new Sequence."), sequence_repl_xpm)
{
    metaType = SLM_SEQUENCE;
}


slSlSequenceFactory slSequenceFactory;
static pgaCollectionFactory cf(&slSequenceFactory, __("Sequences"), sequences_xpm);
