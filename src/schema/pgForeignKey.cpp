//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "pgTable.h"
#include "pgForeignKey.h"
#include "pgCollection.h"


pgForeignKey::pgForeignKey(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_FOREIGNKEY, newName)
{
}

pgForeignKey::~pgForeignKey()
{
}


bool pgForeignKey::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT(
        "ALTER TABLE ") + GetSchemaPrefix(fkSchema) + qtIdent(fkTable)
        + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier());
}


wxString pgForeignKey::GetDefinition()
{
    wxString sql;
    // MATCH FULL/PARTIAL missing; where is this stored?!?

    sql = wxT("(") + GetQuotedFkColumns()
        +  wxT(") REFERENCES ") + GetQuotedSchemaPrefix(GetRefSchema()) + qtIdent(GetReferences()) 
        +  wxT(" (") + GetQuotedRefColumns()
        +  wxT(") ON UPDATE ") + GetOnUpdate()
        +  wxT(" ON DELETE ") + GetOnDelete();
    if (GetDeferrable())
    {
        sql += wxT(" DEFERRABLE INITIALLY ");
        if (GetDeferred())
            sql += wxT("DEFERRED");
        else
            sql += wxT("IMMEDIATE");
    }
    return sql;
}


wxString pgForeignKey::GetConstraint()
{
    wxString sql;
    sql = GetQuotedIdentifier() 
        +  wxT(" FOREIGN KEY ") + GetDefinition();

    return sql;
}


wxString pgForeignKey::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Foreign Key: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- ALTER TABLE ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() + wxT(";")
            + wxT("\n\nALTER TABLE ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
            + wxT("\n  ADD CONSTRAINT ") + GetConstraint() 
            + wxT(";\n");
        if (!GetComment().IsEmpty())
            sql += wxT("COMMENT ON CONSTRAINT ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
                +  wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
    }

    return sql;
}


wxString pgForeignKey::GetFullName() const
{
    return GetName() + wxT(" -> ") + GetReferences();
}

void pgForeignKey::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        wxStringTokenizer c1l(GetConkey(), wxT(","));
        wxStringTokenizer c2l(GetConfkey(), wxT(","));
        wxString c1, c2;

        // resolve column names
        while (c1l.HasMoreTokens())
        {
            c1=c1l.GetNextToken();
            c2=c2l.GetNextToken();
            pgSet *set=ExecuteSet(
                wxT("SELECT a1.attname as conattname, a2.attname as confattname\n")
                wxT("  FROM pg_attribute a1, pg_attribute a2\n")
                wxT(" WHERE a1.attrelid=") + GetTableOidStr() + wxT(" AND a1.attnum=") + c1 + wxT("\n")
                wxT("   AND a2.attrelid=") + GetRelTableOidStr() + wxT(" AND a2.attnum=") + c2);
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
        wxTreeItemId item=browser->GetItemParent(GetId());
        while (item)
        {
            pgTable *table=(pgTable*)browser->GetItemData(item);
            if (table->GetType() == PG_TABLE)
            {
                coveringIndex = table->GetCoveringIndex(browser, fkColumns);
                break;
            }
            item = browser->GetItemParent(item);
        }
    }

    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), NumToStr(GetOid()));
        properties->AppendItem(_("Child columns"), GetFkColumns());
        properties->AppendItem(_("References"), GetReferences() 
            + wxT("(") +GetRefColumns() + wxT(")"));

        properties->AppendItem(_("Covering index"), GetCoveringIndex());
        properties->AppendItem(_("On update"), GetOnUpdate());
        properties->AppendItem(_("On delete"), GetOnDelete());
        properties->AppendItem(_("Deferrable?"), BoolToYesNo(GetDeferrable()));
        properties->AppendItem(_("Initially?"), 
            GetDeferred() ? wxT("DEFERRED") : wxT("IMMEDIATE"));
        properties->AppendItem(_("System foreign key?"), BoolToYesNo(GetSystemObject()));
        properties->AppendItem(_("Comment"), GetComment());
    }
}




pgObject *pgForeignKey::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *foreignKey=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_CONSTRAINTS)
            foreignKey = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND ct.oid=") + GetOidStr());
    }
    return foreignKey;
}



pgObject *pgForeignKey::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgForeignKey *foreignKey=0;

    pgSet *foreignKeys= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT ct.oid, conname, condeferrable, condeferred, confupdtype, confdeltype, confmatchtype, ")
               wxT("conkey, confkey, confrelid, nl.nspname as fknsp, cl.relname as fktab, ")
               wxT("nr.nspname as refnsp, cr.relname as reftab, description\n")
        wxT("  FROM pg_constraint ct\n")
        wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
        wxT("  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n")
        wxT("  JOIN pg_class cr ON cr.oid=confrelid\n")
        wxT("  JOIN pg_namespace nr ON nr.oid=cr.relnamespace\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=ct.oid\n")
        wxT(" WHERE contype='f' AND conrelid = ") + collection->GetOidStr()
        + restriction + wxT("\n")
        wxT(" ORDER BY conname"));

    if (foreignKeys)
    {
        while (!foreignKeys->Eof())
        {
            foreignKey = new pgForeignKey(collection->GetSchema(), foreignKeys->GetVal(wxT("conname")));

            foreignKey->iSetOid(foreignKeys->GetOid(wxT("oid")));
            foreignKey->iSetTableOid(collection->GetOid());
            foreignKey->iSetRelTableOid(foreignKeys->GetOid(wxT("confrelid")));
            foreignKey->iSetFkSchema(foreignKeys->GetVal(wxT("fknsp")));
            foreignKey->iSetComment(foreignKeys->GetVal(wxT("description")));
            foreignKey->iSetFkTable(foreignKeys->GetVal(wxT("fktab")));
            foreignKey->iSetRefSchema(foreignKeys->GetVal(wxT("refnsp")));
            foreignKey->iSetReferences(foreignKeys->GetVal(wxT("reftab")));
            wxString onUpd=foreignKeys->GetVal(wxT("confupdtype"));
            wxString onDel=foreignKeys->GetVal(wxT("confdeltype"));
            foreignKey->iSetOnUpdate(
                onUpd.IsSameAs('a') ? wxT("NO ACTION") :
                onUpd.IsSameAs('r') ? wxT("RESTRICT") :
                onUpd.IsSameAs('c') ? wxT("CASCADE") :
                onUpd.IsSameAs('d') ? wxT("SET DEFAULT") :
                onUpd.IsSameAs('n') ? wxT("SET NULL") : wxT("Unknown"));
            foreignKey->iSetOnDelete(
                onDel.IsSameAs('a') ? wxT("NO ACTION") :
                onDel.IsSameAs('r') ? wxT("RESTRICT") :
                onDel.IsSameAs('c') ? wxT("CASCADE") :
                onDel.IsSameAs('d') ? wxT("SET DEFAULT") :
                onDel.IsSameAs('n') ? wxT("SET NULL") : wxT("Unknown"));
            wxString cn=foreignKeys->GetVal(wxT("conkey"));
            cn = cn.Mid(1, cn.Length()-2);
            foreignKey->iSetConkey(cn);
            cn=foreignKeys->GetVal(wxT("confkey"));
            cn = cn.Mid(1, cn.Length()-2);
            foreignKey->iSetConfkey(cn);

            foreignKey->iSetDeferrable(foreignKeys->GetBool(wxT("condeferrable")));
            foreignKey->iSetDeferred(foreignKeys->GetBool(wxT("condeferred")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, foreignKey);
	    		foreignKeys->MoveNext();
            }
            else
                break;
        }

		delete foreignKeys;
    }
    return foreignKey;
}
