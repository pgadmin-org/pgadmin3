//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgSequence.cpp - Sequence class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgSequence.h"


pgSequence::pgSequence(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, sequenceFactory, newName)
{
	isReplicated = false;
}

pgSequence::~pgSequence()
{
}

wxString pgSequence::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on sequence");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing sequence");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop sequence \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop sequence \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop sequence cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop sequence?");
			break;
		case PROPERTIESREPORT:
			message = _("Sequence properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Sequence properties");
			break;
		case DDLREPORT:
			message = _("Sequence DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Sequence DDL");
			break;
		case STATISTICSREPORT:
			message = _("Sequence statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Sequence statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Sequence dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Sequence dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Sequence dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Sequence dependents");
			break;
	}

	return message;
}


int pgSequence::GetIconId()
{
	if (isReplicated)
		return sequenceFactory.GetReplicatedIconId();
	else
		return sequenceFactory.GetIconId();
}

bool pgSequence::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP SEQUENCE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


void pgSequence::UpdateValues()
{
	pgSet *sequence = ExecuteSet(
	                      wxT("SELECT last_value, min_value, max_value, cache_value, is_cycled, increment_by, is_called\n")
	                      wxT("  FROM ") + GetQuotedFullIdentifier());
	if (sequence)
	{
		lastValue = sequence->GetLongLong(wxT("last_value"));
		minValue = sequence->GetLongLong(wxT("min_value"));
		maxValue = sequence->GetLongLong(wxT("max_value"));
		cacheValue = sequence->GetLongLong(wxT("cache_value"));
		increment = sequence->GetLongLong(wxT("increment_by"));
		cycled = sequence->GetBool(wxT("is_cycled"));
		called = sequence->GetBool(wxT("is_called"));
		if (called)
			nextValue = lastValue + increment;
		else
			nextValue = lastValue;

		delete sequence;
	}
}


wxString pgSequence::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		UpdateValues();
		sql = wxT("-- Sequence: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP SEQUENCE ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE SEQUENCE ") + GetQuotedFullIdentifier()
		      + wxT("\n  INCREMENT ") + GetIncrement().ToString()
		      + wxT("\n  MINVALUE ") + GetMinValue().ToString()
		      + wxT("\n  MAXVALUE ") + GetMaxValue().ToString()
		      + wxT("\n  START ") + GetLastValue().ToString()
		      + wxT("\n  CACHE ") + GetCacheValue().ToString();
		if (GetCycled())
			sql += wxT("\n  CYCLE");
		sql += wxT(";\n")
		       + GetOwnerSql(7, 3, wxT("TABLE ") + GetQuotedFullIdentifier());

		if (!GetConnection()->BackendMinimumVersion(8, 2))
			sql += GetGrant(wxT("arwdRxt"), wxT("TABLE ") + GetQuotedFullIdentifier());
		else
			sql += GetGrant(wxT("rwU"), wxT("SEQUENCE ") + GetQuotedFullIdentifier());

		sql += GetCommentSql();

		if (GetConnection()->BackendMinimumVersion(9, 1))
			sql += GetSeqLabelsSql();
	}

	return sql;
}

void pgSequence::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	UpdateValues();
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Current value"), GetLastValue());
		properties->AppendItem(_("Next value"), GetNextValue());
		properties->AppendItem(_("Minimum"), GetMinValue());
		properties->AppendItem(_("Maximum"), GetMaxValue());
		properties->AppendItem(_("Increment"), GetIncrement());
		properties->AppendItem(_("Cache"), GetCacheValue());
		properties->AppendYesNoItem(_("Cycled?"), GetCycled());
		properties->AppendYesNoItem(_("Called?"), GetCalled());
		properties->AppendYesNoItem(_("System sequence?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

		if (!GetLabels().IsEmpty())
		{
			wxArrayString seclabels = GetProviderLabelArray();
			if (seclabels.GetCount() > 0)
			{
				for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
				{
					properties->AppendItem(seclabels.Item(index), seclabels.Item(index + 1));
				}
			}
		}
	}
}



void pgSequence::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxLogInfo(wxT("Displaying statistics for sequence on ") + GetSchema()->GetIdentifier());

	// Add the statistics view columns
	CreateListColumns(statistics, _("Statistic"), _("Value"));

	pgSet *stats = GetSchema()->GetDatabase()->ExecuteSet(wxT(
	                   "SELECT blks_read, blks_hit FROM pg_statio_all_sequences WHERE relid = ") + GetOidStr());

	if (stats)
	{
		statistics->InsertItem(0, _("Blocks Read"), PGICON_STATISTICS);
		statistics->SetItem(0l, 1, stats->GetVal(wxT("blks_read")));
		statistics->InsertItem(1, _("Blocks Hit"), PGICON_STATISTICS);
		statistics->SetItem(1, 1, stats->GetVal(wxT("blks_hit")));

		delete stats;
	}
}


pgObject *pgSequence::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *sequence = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		sequence = sequenceFactory.CreateObjects(coll, 0, wxT("\n   AND cl.oid=") + GetOidStr());

	return sequence;
}


///////////////////////////////////////////////////


pgSequenceCollection::pgSequenceCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgSequenceCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on sequences");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing sequences");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for sequences");
			break;
		case OBJECTSLISTREPORT:
			message = _("Sequences list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////////////////////////////////


pgObject *pgSequenceFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgSet *sequences;
	pgSequence *sequence = 0;
	wxString sql;

	sql = wxT("SELECT cl.oid, relname, pg_get_userbyid(relowner) AS seqowner, relacl, description");
	if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
	{
		sql += wxT(",\n(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=cl.oid) AS labels");
		sql += wxT(",\n(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=cl.oid) AS providers");
	}
	sql += wxT("\n  FROM pg_class cl\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=cl.oid AND des.classoid='pg_class'::regclass)\n")
	       wxT(" WHERE relkind = 'S' AND relnamespace  = ") + collection->GetSchema()->GetOidStr()
	       + restriction + wxT("\n")
	       wxT(" ORDER BY relname");

	sequences = collection->GetDatabase()->ExecuteSet(sql);

	if (sequences)
	{
		while (!sequences->Eof())
		{
			sequence = new pgSequence(collection->GetSchema(),
			                          sequences->GetVal(wxT("relname")));

			sequence->iSetOid(sequences->GetOid(wxT("oid")));
			sequence->iSetComment(sequences->GetVal(wxT("description")));
			sequence->iSetOwner(sequences->GetVal(wxT("seqowner")));
			sequence->iSetAcl(sequences->GetVal(wxT("relacl")));

			if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
			{
				sequence->iSetProviders(sequences->GetVal(wxT("providers")));
				sequence->iSetLabels(sequences->GetVal(wxT("labels")));
			}

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


#include "images/sequence.pngc"
#include "images/sequences.pngc"

pgSequenceFactory::pgSequenceFactory()
	: pgSchemaObjFactory(__("Sequence"), __("New Sequence..."), __("Create a new Sequence."), sequence_png_img)
{
	metaType = PGM_SEQUENCE;
}


pgCollection *pgSequenceFactory::CreateCollection(pgObject *obj)
{
	return new pgSequenceCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgSequenceFactory sequenceFactory;
static pgaCollectionFactory cf(&sequenceFactory, __("Sequences"), sequences_png_img);
