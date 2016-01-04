//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgForeignKey.cpp - ForeignKey class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "schema/pgForeignKey.h"
#include "schema/pgConstraints.h"

pgForeignKey::pgForeignKey(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, foreignKeyFactory, newName)
{
}

pgForeignKey::~pgForeignKey()
{
}


wxString pgForeignKey::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign key");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign key");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for foreign key");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign key \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign key \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop foreign key cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop foreign key?");
			break;
		case PROPERTIESREPORT:
			message = _("Foreign key properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Foreign key properties");
			break;
		case DDLREPORT:
			message = _("Foreign key DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Foreign key DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Foreign key dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Foreign key dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Foreign key dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Foreign key dependents");
			break;
	}

	return message;
}


int pgForeignKey::GetIconId()
{
	if (!GetDatabase()->BackendMinimumVersion(9, 1) || GetValid())
		return foreignKeyFactory.GetIconId();
	else
		return foreignKeyFactory.GetClosedIconId();
}


bool pgForeignKey::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("ALTER TABLE ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
	               + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgForeignKey::GetDefinition()
{
	wxString sql;

	sql = wxT("(") + GetQuotedFkColumns()
	      +  wxT(")\n      REFERENCES ") + GetQuotedSchemaPrefix(GetRefSchema()) + qtIdent(GetReferences())
	      +  wxT(" (") + GetQuotedRefColumns()
	      +  wxT(")");

	if (GetDatabase()->BackendMinimumVersion(7, 4) || GetMatch() == wxT("FULL"))
		sql += wxT(" MATCH ") + GetMatch();

	sql += wxT("\n      ON UPDATE ") + GetOnUpdate()
	       +  wxT(" ON DELETE ") + GetOnDelete();
	if (GetDeferrable())
	{
		sql += wxT(" DEFERRABLE INITIALLY ");
		if (GetDeferred())
			sql += wxT("DEFERRED");
		else
			sql += wxT("IMMEDIATE");
	}

	if (GetDatabase()->BackendMinimumVersion(9, 1) && !GetValid())
		sql += wxT("\n      NOT VALID");

	return sql;
}


wxString pgForeignKey::GetConstraint()
{
	wxString sql;
	sql = GetQuotedIdentifier()
	      +  wxT(" FOREIGN KEY ") + GetDefinition();

	return sql;
}


wxString pgForeignKey::GetSql(ctlTree *browser)
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
			       +  wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
	}

	return sql;
}


wxString pgForeignKey::GetFullName()
{
	return GetName() + wxT(" -> ") + GetReferences();
}

void pgForeignKey::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		wxTreeItemId id = browser->GetItemParent(GetId());
		pgTable *table = (pgTable *)browser->GetObject(id);

		wxStringTokenizer c1l(GetConkey(), wxT(","));
		wxStringTokenizer c2l(GetConfkey(), wxT(","));
		wxString c1, c2;

		// resolve column names
		while (c1l.HasMoreTokens())
		{
			c1 = c1l.GetNextToken();
			c2 = c2l.GetNextToken();
			pgSet *set = ExecuteSet(
			                 wxT("SELECT a1.attname as conattname, a2.attname as confattname\n")
			                 wxT("  FROM pg_attribute a1, pg_attribute a2\n")
			                 wxT(" WHERE a1.attrelid=") + NumToStr(table->GetOid()) + wxT("::oid") + wxT(" AND a1.attnum=") + c1 + wxT("\n")
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
		wxTreeItemId item = browser->GetItemParent(GetId());
		while (item)
		{
			pgTable *table = (pgTable *)browser->GetObject(item);
			if (table && table->IsCreatedBy(tableFactory))
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
		                       + wxT("(") + GetRefColumns() + wxT(")"));

		properties->AppendItem(_("Covering index"), GetCoveringIndex());
		properties->AppendItem(_("Match type"), GetMatch());
		properties->AppendItem(_("On update"), GetOnUpdate());
		properties->AppendItem(_("On delete"), GetOnDelete());
		properties->AppendItem(_("Deferrable?"), BoolToYesNo(GetDeferrable()));
		if (GetDeferrable())
			properties->AppendItem(_("Initially?"),
			                       GetDeferred() ? wxT("DEFERRED") : wxT("IMMEDIATE"));
		if (GetDatabase()->BackendMinimumVersion(9, 1))
			properties->AppendItem(_("Valid?"), BoolToYesNo(GetValid()));
		properties->AppendItem(_("System foreign key?"), BoolToYesNo(GetSystemObject()));
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}


pgObject *pgForeignKey::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *foreignKey = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		foreignKey = foreignKeyFactory.CreateObjects(coll, 0, wxT("\n   AND ct.oid=") + GetOidStr());

	return foreignKey;
}


void pgForeignKey::Validate(frmMain *form)
{
	wxString sql = wxT("ALTER TABLE ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
	               + wxT("\n  VALIDATE CONSTRAINT ") + GetQuotedIdentifier();
	GetDatabase()->ExecuteVoid(sql);

	iSetValid(true);
	UpdateIcon(form->GetBrowser());
}


pgObject *pgForeignKeyFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgTableObjCollection *collection = (pgTableObjCollection *)coll;
	pgForeignKey *foreignKey = 0;

	sql = wxT("SELECT ct.oid, conname, condeferrable, condeferred, confupdtype, confdeltype, confmatchtype, ")
	      wxT("conkey, confkey, confrelid, nl.nspname as fknsp, cl.relname as fktab, ")
	      wxT("nr.nspname as refnsp, cr.relname as reftab, description");
	if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
		sql += wxT(", convalidated");
	sql += wxT("\n  FROM pg_constraint ct\n")
	       wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
	       wxT("  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n")
	       wxT("  JOIN pg_class cr ON cr.oid=confrelid\n")
	       wxT("  JOIN pg_namespace nr ON nr.oid=cr.relnamespace\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=ct.oid AND des.classoid='pg_constraint'::regclass)\n")
	       wxT(" WHERE contype='f' AND conrelid = ") + collection->GetOidStr()
	       + restriction + wxT("\n")
	       wxT(" ORDER BY conname");

	pgSet *foreignKeys = collection->GetDatabase()->ExecuteSet(sql);

	if (foreignKeys)
	{
		while (!foreignKeys->Eof())
		{
			foreignKey = new pgForeignKey(collection->GetSchema()->GetSchema(), foreignKeys->GetVal(wxT("conname")));

			foreignKey->iSetOid(foreignKeys->GetOid(wxT("oid")));
			foreignKey->iSetRelTableOid(foreignKeys->GetOid(wxT("confrelid")));
			foreignKey->iSetFkSchema(foreignKeys->GetVal(wxT("fknsp")));
			foreignKey->iSetComment(foreignKeys->GetVal(wxT("description")));
			foreignKey->iSetFkTable(foreignKeys->GetVal(wxT("fktab")));
			foreignKey->iSetRefSchema(foreignKeys->GetVal(wxT("refnsp")));
			foreignKey->iSetReferences(foreignKeys->GetVal(wxT("reftab")));
			if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
				foreignKey->iSetValid(foreignKeys->GetBool(wxT("convalidated")));
			wxString onUpd = foreignKeys->GetVal(wxT("confupdtype"));
			wxString onDel = foreignKeys->GetVal(wxT("confdeltype"));
			wxString match = foreignKeys->GetVal(wxT("confmatchtype"));
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
			foreignKey->iSetMatch(
			    match.IsSameAs('f') ? wxT("FULL") :
			    match.IsSameAs('s') ? wxT("SIMPLE") :
			    match.IsSameAs('u') ? wxT("SIMPLE") : wxT("Unknown"));

			wxString cn = foreignKeys->GetVal(wxT("conkey"));
			cn = cn.Mid(1, cn.Length() - 2);
			foreignKey->iSetConkey(cn);
			cn = foreignKeys->GetVal(wxT("confkey"));
			cn = cn.Mid(1, cn.Length() - 2);
			foreignKey->iSetConfkey(cn);

			foreignKey->iSetDeferrable(foreignKeys->GetBool(wxT("condeferrable")));
			foreignKey->iSetDeferred(foreignKeys->GetBool(wxT("condeferred")));

			if (browser)
			{
				browser->AppendObject(collection, foreignKey);
				foreignKeys->MoveNext();
			}
			else
				break;
		}

		delete foreignKeys;
	}
	return foreignKey;
}

/////////////////////////////

wxString pgForeignKeyCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign keys");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign keys");
			break;
		case OBJECTSLISTREPORT:
			message = _("Foreign keys list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/foreignkey.pngc"
#include "images/foreignkeybad.pngc"


pgForeignKeyFactory::pgForeignKeyFactory()
	: pgSchemaObjFactory(__("Foreign Key"), __("New Foreign Key..."), __("Create a new Foreign Key constraint."), foreignkey_png_img)
{
	metaType = PGM_FOREIGNKEY;
	collectionFactory = &constraintCollectionFactory;
	closedId = addIcon(foreignkeybad_png_img);
}


pgForeignKeyFactory foreignKeyFactory;

validateForeignKeyFactory::validateForeignKeyFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Validate foreign key"), _("Validate the selected foreign key."));
}


wxWindow *validateForeignKeyFactory::StartDialog(frmMain *form, pgObject *obj)
{
	((pgForeignKey *)obj)->Validate(form);
	((pgForeignKey *)obj)->SetDirty();

	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
	{
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());
		form->GetSqlPane()->SetReadOnly(false);
		form->GetSqlPane()->SetText(((pgForeignKey *)obj)->GetSql(form->GetBrowser()));
		form->GetSqlPane()->SetReadOnly(true);
	}
	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), (ctlMenuToolbar *)form->GetToolBar());

	return 0;
}


bool validateForeignKeyFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(foreignKeyFactory) && obj->CanEdit()
	       && ((pgForeignKey *)obj)->GetConnection()->BackendMinimumVersion(9, 1)
	       && !((pgForeignKey *)obj)->GetValid();
}
