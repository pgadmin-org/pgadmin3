//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgColumn.cpp - Column class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgColumn.h"
#include "pgCollection.h"


pgColumn::pgColumn(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_COLUMN, newName)
{
    isFK=false;
    isPK=false;
}

pgColumn::~pgColumn()
{
}


wxString pgColumn::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        if (GetInheritedCount())
            sql = wxT("-- Column inherited; cannot be changed");
        else
        {
            sql = wxT("-- Just a proposal; indexes, foreign keys and trigger might prevent this\n\n"
                      "ALTER TABLE ") + GetQuotedFullTable()
                + wxT(" ADD COLUMN pgadmin_tmpcol ") + GetFullType();
            if (GetStorage() != "PLAIN")
                sql += wxT(";\nALTER TABLE ")+ GetQuotedFullTable()
                    +  wxT(" ALTER COLUMN pgadmin_tmpcol SET STORAGE ") + GetStorage();
            sql +=wxT(";\nUPDATE ") + GetQuotedFullTable()
                + wxT(" SET pgadmin_tmpcol=") + GetQuotedIdentifier()
                + wxT(";\nALTER TABLE ") + GetQuotedFullIdentifier()
                + wxT(" DROP COLUMN ") + GetQuotedIdentifier()
                + wxT(";\nALTER TABLE ") + GetQuotedFullTable()
                + wxT(" RENAME COLUMN pgadmin_tmpcol TO ") + GetQuotedIdentifier()
                + wxT(";\n");
            if (GetNotNull())
                sql += wxT("ALTER TABLE ") + GetQuotedFullTable()
                    + wxT(" ALTER COLUMN ") + GetQuotedIdentifier()
                    + wxT(" SET NOT NULL;\n");
        }
    }

    return sql;
}

wxString pgColumn::GetFullType()
{
    wxString tn=qtIdent(GetVarTypename());
    if (typlen == -1 && typmod > 0)
    {
        tn += wxT("(") + NumToStr(length);
        if (precision >= 0)
            tn += wxT(", ") + NumToStr(precision);
        tn += wxT(")");
    }
    if (isArray)
        tn += wxT("[]");

    return tn;
}


void pgColumn::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    if (!expandedKids)
    {
        expandedKids = true;
        // append type here
        // fk, pk lesen
        pgSet *set = ExecuteSet(wxT(
            "SELECT indkey FROM pg_index\n"
            " WHERE indrelid=") + GetTableOidStr());
        if (set)
        {
            wxString indkey, str;
            while (!isPK && !set->Eof())
            {
                wxStringTokenizer indkey(set->GetVal(0));
                while (indkey.HasMoreTokens())
                {
                    str=indkey.GetNextToken();
                    if (StrToLong(str) == GetColNumber())
                    {
                        isPK = true;
                        break;
                    }
                }

                set->MoveNext();
            }
            delete set;
        }

        set=ExecuteSet(wxT(
            "SELECT conkey\n"
            "  FROM pg_constraint ct\n"
            "  JOIN pg_class cl on cl.oid=confrelid\n"
            " WHERE contype='f' AND conrelid = ") + GetTableOidStr() + wxT("\n"
            " ORDER BY conname"));
        if (set)
        {
            wxString conkey, str;
            while (!isFK && !set->Eof())
            {
                wxStringTokenizer conkey(set->GetVal(0));

                while (conkey.HasMoreTokens())
                {
                    str=conkey.GetNextToken();
                    if (atol(str.c_str()+1) == GetColNumber())
                    {
                        isFK = true;
                        break;
                    }
                }

                set->MoveNext();
            }
            delete set;
        }
    }

    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("Position"), GetColNumber());
        InsertListItem(properties, pos++, wxT("Data Type"), GetVarTypename());
        if (GetLength() == -2)
            InsertListItem(properties, pos++, wxT("Length"), wxT("null-terminated"));
        else if (GetLength() < 0)
            InsertListItem(properties, pos++, wxT("Length"), wxT("variable"));
        else
            InsertListItem(properties, pos++, wxT("Length"), GetLength());
        if (GetPrecision() >= 0)
            InsertListItem(properties, pos++, wxT("Base Type"), GetPrecision());
        InsertListItem(properties, pos++, wxT("Default"), GetDefault());
        InsertListItem(properties, pos++, wxT("Not Null?"), GetNotNull());
        InsertListItem(properties, pos++, wxT("Primary Key?"), GetIsPK());
        InsertListItem(properties, pos++, wxT("Foreign Key?"), GetIsFK());
        InsertListItem(properties, pos++, wxT("Storage"), GetStorage());
        InsertListItem(properties, pos++, wxT("Storage"), GetStorage());
        InsertListItem(properties, pos++, wxT("Inherits Count"), GetInheritedCount());

        InsertListItem(properties, pos++, wxT("System Domain?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }

    // statistic 
    DisplayStatistics(statistics, wxT(
        "SELECT null_frac AS \"Null Fraction\", avg_width AS \"Average Width\", n_distinct AS \"Distinct Values\", "
        "most_common_vals AS \"Most Common Values\", most_common_freqs AS \"Most Common Frequencies\", "
        " histogram_bounds AS \"Histogram Bounds\", correlation AS \"Correlation\"\n"
        "  FROM pg_stats WHERE tablename = 'pt_partner' AND attname = 'partner_nr'"));
}



void pgColumn::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgColumn *column;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding Columns to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Get the Columns

        wxString systemRestriction;
        if (!settings->GetShowSystemObjects())
            systemRestriction = "\n   AND attnum > 0 AND attisdropped IS FALSE";


        pgSet *columns= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT att.*, def.*, ty.typname, et.typname as elemtypname, relname, nspname,\n"
            "       CASE WHEN atttypid IN (1231,1700) OR ty.typbasetype IN (1231,1700) "
                        "THEN 1 ELSE 0 END AS isnumeric\n"
            "  FROM pg_attribute att\n"
            "  JOIN pg_type ty ON ty.oid=atttypid\n"
            "  JOIN pg_class cl ON cl.oid=attrelid\n"
            "  JOIN pg_namespace na ON na.oid=cl.relnamespace\n"
            "  LEFT OUTER JOIN pg_type et ON et.oid=ty.typelem\n"
            "  LEFT OUTER JOIN pg_attrdef def ON adrelid=attrelid AND adnum=attnum\n"

            " WHERE attrelid = ") + collection->GetOidStr() + systemRestriction + wxT("\n"
            " ORDER BY attnum"));

        if (columns)
        {
            while (!columns->Eof())
            {
                column = new pgColumn(collection->GetSchema(), columns->GetVal(wxT("attname")));

                column->iSetTableOid(collection->GetOid());
                column->iSetColNumber(columns->GetLong(wxT("attnum")));
                column->iSetIsArray(columns->GetLong(wxT("attndims")) > 0);
                if (column->GetIsArray())
                    column->iSetVarTypename(columns->GetVal(wxT("elemtypname")));
                else
                    column->iSetVarTypename(columns->GetVal(wxT("typname")));
                column->iSetDefault(columns->GetVal(wxT("adsrc")));
                column->iSetStatistics(columns->GetLong(wxT("attstattarget")));
                column->iSetStorage(columns->GetVal(wxT("attstorage")));

                long typlen=columns->GetLong(wxT("attlen"));
                long typmod=columns->GetLong(wxT("atttypmod"));
                bool isnum=columns->GetBool(wxT("isnumeric"));

                long precision=-1, length=typlen;
                if (length == -1)
                {
                    if (typmod > 0)
                    {
                        if (isnum)
                        {
                            length=(typmod-4) >> 16;
                            precision=(typmod-4) & 0xffff;
                        }
                        else
                            length = typmod-4;
                    }
                }
                column->iSetTyplen(typlen);
                column->iSetTypmod(typmod);
                column->iSetLength(length);
                column->iSetPrecision(precision);
                column->iSetNotNull(columns->GetBool(wxT("attnotnull")));
                column->iSetQuotedFullTable(qtIdent(columns->GetVal(wxT("nspname"))) + wxT(".")
                    + qtIdent(columns->GetVal(wxT("relname"))));
                column->iSetInheritedCount(columns->GetLong(wxT("attinhcount")));

                browser->AppendItem(collection->GetId(), column->GetIdentifier(), PGICON_COLUMN, -1, column);
	    
			    columns->MoveNext();
            }

		    delete columns;
        }
    }
}

