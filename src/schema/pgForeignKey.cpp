//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgForeignKey.cpp - ForeignKey class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgForeignKey.h"
#include "pgCollection.h"


pgForeignKey::pgForeignKey(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_FOREIGNKEY, newName)
{
}

pgForeignKey::~pgForeignKey()
{
}



wxString pgForeignKey::GetConstraint()
{
    wxString con;
    // MATCH FULL/PARTIAL missing
    con = GetQuotedIdentifier() 
        +  wxT(" FOREIGN KEY (") + GetQuotedFkColumns()
//        +  wxT(") REFERENCES ") + qtIdent(GetRefSchema()) + wxT(".") + qtIdent(GetReferences()) 
        +  wxT(") REFERENCES ") + qtIdent(GetReferences()) 
        +  wxT(" (") + GetQuotedRefColumns()
        +  wxT(")\n        ON UPDATE ") + GetOnUpdate()
        +  wxT(" ON DELETE ") + GetOnDelete();
    if (GetDeferrable())
    {
        con += wxT(" DEFERRABLE INITIALLY ");
        if (GetDeferred())
            con += wxT("DEFERRED");
        else
            con += wxT("IMMEDIATE");
    }

    return con;
}


wxString pgForeignKey::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- ALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() 
            + wxT(";\nALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
            + wxT(" ADD CONSTRAINT ") + GetConstraint() 
            + wxT(";\n");
    }

    return sql;
}


wxString pgForeignKey::GetFullName() const
{
    return GetName() + wxT(" -> ") + GetReferences();
}

void pgForeignKey::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);


    if (!expandedKids)
    {
        expandedKids=true;

        wxStringTokenizer c1l=GetConkey();
        wxStringTokenizer c2l=GetConfkey();
        wxString c1, c2;

        while (c1l.HasMoreTokens())
        {
            c1=c1l.GetNextToken();
            c2=c2l.GetNextToken();
            pgSet *set=ExecuteSet(wxT(
                "SELECT a1.attname as conattname, a2.attname as confattname\n"
                "  FROM pg_attribute a1, pg_attribute a2\n"
                " WHERE a1.attrelid=") + GetTableOidStr() + wxT(" AND a1.attnum=") + c1 + wxT("\n"
                "   AND a2.attrelid=") + GetRelTableOidStr() + wxT(" AND a2.attnum=") + c2);
            if (set)
            {
                if (!fkColumns.IsNull())
                {
                    fkColumns += wxT(", ");
                    refColumns += wxT(", ");
                    quotedFkColumns += wxT(", ");
                    quotedRefColumns += wxT(", ");
                }
                fkColumns += set->GetVal(0);
                refColumns += set->GetVal(1);
                quotedFkColumns += qtIdent(set->GetVal(0));
                quotedRefColumns += qtIdent(set->GetVal(1));
                delete set;
            }
        }
    }

    if (properties)
    {
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
        InsertListItem(properties, pos++, wxT("Child Columns"), GetFkColumns());
        InsertListItem(properties, pos++, wxT("References"), GetReferences() 
            + wxT("(") +GetRefColumns() + wxT(")"));

/*      wxStringTokenizer c1l=GetConkey();
        wxStringTokenizer c2l=GetConfkey();
        wxString c1, c2;

        while (c1l.HasMoreTokens())
        {
            c1=c1l.GetNextToken();
            c2=c2l.GetNextToken();
            pgSet *set=ExecuteSet(wxT(
                "SELECT a1.attname as conattname, a2.attname as confattname\n"
                "  FROM pg_attribute a1, pg_attribute a2\n"
                " WHERE a1.attrelid=") + GetTableOidStr() + wxT(" AND a1.attnum=") + c1 + wxT("\n"
                "   AND a2.attrelid=") + GetRelTableOidStr() + wxT(" AND a2.attnum=") + c2);
            if (set)
            {
                InsertListItem(properties, pos++, wxT(""), set->GetVal(0) + wxT(" -> ")+ set->GetVal(1));
                delete set;
            }
        }
*/

        InsertListItem(properties, pos++, wxT("On Update"), GetOnUpdate());
        InsertListItem(properties, pos++, wxT("On Delete"), GetOnDelete());
        InsertListItem(properties, pos++, wxT("Deferrable?"), BoolToYesNo(GetDeferrable()));
        InsertListItem(properties, pos++, wxT("Initially?"), 
            GetDeferred() ? wxT("DEFERRED") : wxT("IMMEDIATE"));
        InsertListItem(properties, pos++, wxT("System Foreign Key?"), BoolToYesNo(GetSystemObject()));
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}


void pgForeignKey::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgForeignKey *foreignKey;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding ForeignKeys to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Get the ForeignKeys
        pgSet *foreignKeys= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT ct.oid, conname, condeferrable, condeferred, confupdtype, confdeltype, confmatchtype, "
                   "conkey, confkey, confrelid, nl.nspname as fknsp, cl.relname as fktab, "
                   "nr.nspname as refnsp, cr.relname as reftab\n"
            "  FROM pg_constraint ct\n"
            "  JOIN pg_class cl ON cl.oid=conrelid\n"
            "  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n"
            "  JOIN pg_class cr ON cr.oid=confrelid\n"
            "  JOIN pg_namespace nr ON nr.oid=cr.relnamespace\n"
            " WHERE contype='f' AND conrelid = ") + collection->GetOidStr() + wxT("\n"
            " ORDER BY conname"));

        if (foreignKeys)
        {
            while (!foreignKeys->Eof())
            {
                foreignKey = new pgForeignKey(collection->GetSchema(), foreignKeys->GetVal(wxT("conname")));

                foreignKey->iSetOid(StrToDouble(foreignKeys->GetVal(wxT("oid"))));
                foreignKey->iSetTableOid(collection->GetOid());
                foreignKey->iSetRelTableOid(StrToDouble(foreignKeys->GetVal(wxT("confrelid"))));
                foreignKey->iSetFkSchema(foreignKeys->GetVal(wxT("fknsp")));
                foreignKey->iSetFkTable(foreignKeys->GetVal(wxT("fktab")));
                foreignKey->iSetRefSchema(foreignKeys->GetVal(wxT("refnsp")));
                foreignKey->iSetReferences(foreignKeys->GetVal(wxT("reftab")));
                wxString onUpd=foreignKeys->GetVal(wxT("confupdtype"));
                wxString onDel=foreignKeys->GetVal(wxT("confdeltype"));
                foreignKey->iSetOnUpdate(
                    onUpd.IsSameAs('a') ? wxT("NO ACTION") :
                    onUpd.IsSameAs('r') ? wxT("RESTRICT") :
                    onUpd.IsSameAs('c') ? wxT("CASCADE") :
                    onUpd.IsSameAs('d') ? wxT("DEFAULT") :
                    onUpd.IsSameAs('n') ? wxT("SET NULL") : wxT("Unknown"));
                foreignKey->iSetOnDelete(
                    onDel.IsSameAs('a') ? wxT("NO ACTION") :
                    onDel.IsSameAs('r') ? wxT("RESTRICT") :
                    onDel.IsSameAs('c') ? wxT("CASCADE") :
                    onDel.IsSameAs('d') ? wxT("DEFAULT") :
                    onDel.IsSameAs('n') ? wxT("SET NULL") : wxT("Unknown"));
                wxString cn=foreignKeys->GetVal(wxT("conkey"));
                cn = cn.Mid(1, cn.Length()-2);
                foreignKey->iSetConkey(cn);
                cn=foreignKeys->GetVal(wxT("confkey"));
                cn = cn.Mid(1, cn.Length()-2);
                foreignKey->iSetConfkey(cn);

                foreignKey->iSetDeferrable(StrToBool(foreignKeys->GetVal(wxT("condeferrable"))));
                foreignKey->iSetDeferred(StrToBool(foreignKeys->GetVal(wxT("condeferred"))));

                browser->AppendItem(collection->GetId(), foreignKey->GetFullName(), PGICON_KEY, -1, foreignKey);
	    
			    foreignKeys->MoveNext();
            }

		    delete foreignKeys;
        }
    }
}

