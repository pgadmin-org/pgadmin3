//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgQuery.cpp - Property Dialog
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/button.h>

// App headers
#include "pgAdmin3.h"
#include "ctl/ctlMenuToolbar.h"
#include "ctl/ctlSQLBox.h"
#include "schema/pgCollection.h"
#include "schema/pgDatatype.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"
#include "ctl/ctlSecurityPanel.h"
#include "ctl/ctlDefaultSecurityPanel.h"

// Images
#include "images/properties.pngc"

#include "frm/frmMain.h"
#include "frm/frmHint.h"

// Property dialogs
#include "dlg/dlgProperty.h"
#include "dlg/dlgServer.h"
#include "dlg/dlgAggregate.h"
#include "dlg/dlgColumn.h"
#include "dlg/dlgIndex.h"
#include "dlg/dlgIndexConstraint.h"
#include "dlg/dlgForeignKey.h"
#include "dlg/dlgCheck.h"
#include "dlg/dlgRule.h"
#include "dlg/dlgTrigger.h"
#include "dlg/dlgEventTrigger.h"
#include "agent/dlgJob.h"
#include "agent/dlgStep.h"
#include "agent/dlgSchedule.h"

#include "slony/dlgRepCluster.h"
#include "slony/dlgRepNode.h"
#include "slony/dlgRepPath.h"
#include "slony/dlgRepListen.h"
#include "slony/dlgRepSet.h"
#include "slony/dlgRepSequence.h"
#include "slony/dlgRepTable.h"
#include "slony/dlgRepSubscription.h"
#include "schema/pgTable.h"
#include "schema/pgColumn.h"
#include "schema/pgTrigger.h"
#include "schema/pgGroup.h"
#include "schema/pgUser.h"
#include "schema/pgEventTrigger.h"

void dataType::SetOid(OID id)
{
	oid = id;
}

void dataType::SetTypename(wxString name)
{
	typeName = name;
}

OID dataType::GetOid()
{
	return oid;
}

wxString dataType::GetTypename()
{
	return typeName;
}

#define CTRLID_CHKSQLTEXTFIELD 1000


BEGIN_EVENT_TABLE(dlgProperty, DialogWithHelp)
	EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  dlgProperty::OnPageSelect)

	EVT_TEXT(XRCID("txtName"),                      dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbOwner"),                      dlgProperty::OnChangeOwner)
	EVT_COMBOBOX(XRCID("cbOwner"),                  dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbSchema"),                     dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbSchema"),                 dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtComment"),                   dlgProperty::OnChange)

	EVT_CHECKBOX(CTRLID_CHKSQLTEXTFIELD,            dlgProperty::OnChangeReadOnly)

	EVT_BUTTON(wxID_HELP,                           dlgProperty::OnHelp)
	EVT_BUTTON(wxID_OK,                             dlgProperty::OnOK)
END_EVENT_TABLE();


dlgProperty::dlgProperty(pgaFactory *f, frmMain *frame, const wxString &resName) : DialogWithHelp(frame)
{
	readOnly = false;
	sqlPane = 0;
	sqlTextField1 = 0;
	sqlTextField2 = 0;
	processing = false;
	mainForm = frame;
	database = 0;
	connection = 0;
	factory = f;
	item = (void *)NULL;
	owneritem = (void *)NULL;
	chkReadOnly = (wxCheckBox *)NULL;
	SetFont(settings->GetSystemFont());
	LoadResource(frame, resName);

#ifdef __WXMSW__
	SetWindowStyleFlag(GetWindowStyleFlag() & ~wxMAXIMIZE_BOX);
#endif

	nbNotebook = CTRL_NOTEBOOK("nbNotebook");
	if (!nbNotebook)
	{
		wxMessageBox(wxString::Format(_("Problem with resource %s: Notebook not found.\nPrepare to crash!"), resName.c_str()));
		return;
	}

	// Set the icon
	wxBitmap bm(factory->GetImage());
	wxIcon ico;
	ico.CopyFromBitmap(bm);
	SetIcon(ico);

	txtName = CTRL_TEXT("txtName");
	txtOid = CTRL_TEXT("txtOID");
	txtComment = CTRL_TEXT("txtComment");
	cbOwner = CTRL_COMBOBOX2("cbOwner");
	cbSchema = CTRL_COMBOBOX2("cbSchema");
	cbClusterSet = CTRL_COMBOBOX1("cbClusterSet");

	wxString db = wxT("Database");
	wxString ts = wxT("Tablespace");
	wxString rg = wxT("Resource Group");
	enableSQL2 = db.Cmp(factory->GetTypeName()) == 0
	             || ts.Cmp(factory->GetTypeName()) == 0
	             || rg.Cmp(factory->GetTypeName()) == 0;

	wxNotebookPage *page = nbNotebook->GetPage(0);
	wxASSERT(page != NULL);
	page->GetClientSize(&width, &height);

	numericValidator.SetStyle(wxFILTER_NUMERIC);
	btnOK->Disable();

	statusBar = XRCCTRL(*this, "unkStatusBar", wxStatusBar);
}


dlgProperty::~dlgProperty()
{
	wxString prop = wxT("Properties/") + wxString(factory->GetTypeName());
	settings->WritePoint(prop, GetPosition());

	if (GetWindowStyle() & wxRESIZE_BORDER)
		settings->WriteSize(prop, GetSize());

	if (obj)
		obj->SetWindowPtr(NULL);
}


wxString dlgProperty::GetHelpPage() const
{
	wxString page;

	pgObject *obj = ((dlgProperty *)this)->GetObject();
	if (obj)
		page = obj->GetHelpPage(false);
	else
	{
		// Attempt to get he page from the dialogue, otherwise, take a shot at it!
		page = this->GetHelpPage(true);
		if (page.Length() == 0)
		{
			page = wxT("pg/sql-create");
			page += wxString(factory->GetTypeName()).Lower();
		}
	}

	return page;
}


void dlgProperty::CheckValid(bool &enable, const bool condition, const wxString &msg)
{
	if (enable)
	{
		if (!condition)
		{
			if (statusBar)
				statusBar->SetStatusText(msg);
			enable = false;
		}
	}
}


void dlgProperty::SetDatabase(pgDatabase *db)
{
	database = db;
	if (db)
		connection = db->GetConnection();
}

void dlgProperty::SetDatatypeCache(dataTypeCache cache)
{
	dtCache = cache;
}

void dlgProperty::EnableOK(bool enable)
{
	btnOK->Enable(enable);
	if (enable)
	{
		if (statusBar)
			statusBar->SetStatusText(wxEmptyString);
	}
}


void dlgProperty::SetSqlReadOnly(bool readonly)
{
	if (chkReadOnly)
		chkReadOnly->Enable(!readonly);
}


int dlgProperty::Go(bool modal)
{
	wxASSERT(factory != 0);

	if(GetObject())
		obj = GetObject();
	else
		obj = mainForm->GetBrowser()->GetObject(mainForm->GetBrowser()->GetSelection());

	// restore previous position and size, if applicable
	wxString prop = wxT("Properties/") + wxString(factory->GetTypeName());

	wxSize origSize = GetSize();

	if (GetWindowStyle() & wxRESIZE_BORDER)
		SetSize(settings->Read(prop, GetSize()));

	wxPoint pos = settings->Read(prop, GetPosition());
	if (pos.x < 0)
		pos.x = 0;
	if (pos.y < 0)
		pos.y = 0;
	Move(pos);

	wxSize size = GetSize();
	CheckOnScreen(this, pos, size, origSize.GetWidth(), origSize.GetHeight());
	Move(pos);

	ctlComboBoxFix *cbowner = (ctlComboBoxFix *)cbOwner;
	ctlComboBoxFix *cbschema = (ctlComboBoxFix *)cbSchema;

	if (cbClusterSet)
	{
		cbClusterSet->Append(wxEmptyString);
		cbClusterSet->SetSelection(0);

		if (mainForm && database)
		{
			wxArrayString clusters = database->GetSlonyClusters(mainForm->GetBrowser());

			size_t i;
			for (i = 0 ; i < clusters.GetCount() ; i++)
			{
				wxString cluster = wxT("_") + clusters.Item(i);
				pgSetIterator sets(connection,
				                   wxT("SELECT set_id, ") + qtIdent(cluster) + wxT(".slonyversionmajor(), ") + qtIdent(cluster) + wxT(".slonyversionminor()\n")
				                   wxT("  FROM ") + qtIdent(cluster) + wxT(".sl_set\n")
				                   wxT(" WHERE set_origin = ") + qtIdent(cluster) +
				                   wxT(".getlocalnodeid(") + qtDbString(cluster) + wxT(");"));

				while (sets.RowsLeft())
				{
					wxString str;
					long setId = sets.GetLong(wxT("set_id"));
					long majorVer = sets.GetLong(wxT("slonyversionmajor"));
					long minorVer = sets.GetLong(wxT("slonyversionminor"));
					str.Printf(_("Cluster \"%s\", set %ld"), clusters.Item(i).c_str(), setId);
					cbClusterSet->Append(str, static_cast<void *>(new replClientData(cluster, setId, majorVer, minorVer)));
				}
			}
		}
		if (cbClusterSet->GetCount() < 2)
			cbClusterSet->Disable();
	}

	if (cbowner && !cbowner->GetCount())
	{
		if (!GetObject())
			cbOwner->Append(wxEmptyString);
		AddGroups(cbowner);
		AddUsers(cbowner);
	}
	if (txtOid)
		txtOid->Disable();

	if (cbschema && !cbschema->GetCount())
		AddSchemas(cbschema);

	if (GetObject())
	{
		if (txtName)
			txtName->SetValue(GetObject()->GetName());
		if (txtOid)
			txtOid->SetValue(NumToStr((unsigned long)GetObject()->GetOid()));
		if (cbOwner)
			cbOwner->SetValue(GetObject()->GetOwner());
		if (cbSchema)
			cbSchema->SetValue(GetObject()->GetSchema()->GetName());
		if (txtComment)
			txtComment->SetValue(GetObject()->GetComment());


		if (!readOnly && !GetObject()->CanCreate())
		{
			// users who can't create will usually not be allowed to change either.
			readOnly = false;
		}

		wxString typeName = factory->GetTypeName();
		SetTitle(wxString(wxGetTranslation(typeName)) + wxT(" ") + GetObject()->GetFullIdentifier());
	}
	else
	{
		if (factory)
			SetTitle(wxGetTranslation(factory->GetNewString()));
		if (cbSchema)
		{
			if (obj->GetMetaType() == PGM_SCHEMA)
				cbSchema->SetValue(obj->GetName());
			else
				cbSchema->SetValue(obj->GetSchema()->GetName());
		}
	}
	if (statusBar)
		statusBar->SetStatusText(wxEmptyString);

	if (nbNotebook)
	{
		wxNotebookPage *pg = nbNotebook->GetPage(0);
		if (pg)
			pg->SetFocus();
	}

	// This fixes a UI glitch on MacOS X and Windows
	// Because of the new layout code, the Privileges pane don't size itself properly
	SetSize(GetSize().GetWidth() + 1, GetSize().GetHeight());
	SetSize(GetSize().GetWidth() - 1, GetSize().GetHeight());

	if (modal)
		return ShowModal();
	else
		Show(true);

	return 0;
}


void dlgProperty::CreateAdditionalPages()
{
	if (wxString(factory->GetTypeName()).Cmp(wxT("Server")))
	{
		// create a panel
		sqlPane = new wxPanel(nbNotebook);

		// add panel to the notebook
		nbNotebook->AddPage(sqlPane, wxT("SQL"));

		// create a flex grid sizer
		wxFlexGridSizer *fgsizer = new wxFlexGridSizer(1, 5, 5);

		// add checkbox to the panel
		chkReadOnly = new wxCheckBox(sqlPane, CTRLID_CHKSQLTEXTFIELD, _("Read only"));
		chkReadOnly->SetValue(true);
		fgsizer->Add(chkReadOnly, 1, wxALL | wxALIGN_LEFT, 5);

		// text entry box
		sqlTextField1 = new ctlSQLBox(sqlPane, CTL_PROPSQL,
		                              wxDefaultPosition, wxDefaultSize,
		                              wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);
		fgsizer->Add(sqlTextField1, 1, wxALL | wxEXPAND, 5);

		// text entry box
		if (enableSQL2)
		{
			sqlTextField2 = new ctlSQLBox(sqlPane, CTL_PROPSQL,
			                              wxDefaultPosition, wxDefaultSize,
			                              wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);
			fgsizer->Add(sqlTextField2, 1, wxALL | wxEXPAND, 5);
		}

		fgsizer->AddGrowableCol(0);
		fgsizer->AddGrowableRow(1);
		if (fgsizer->GetRows() > 1)
		{
			fgsizer->AddGrowableRow(2);
		}

		sqlPane->SetAutoLayout(true);
		sqlPane->SetSizer(fgsizer);
	}
}


wxString dlgProperty::GetName()
{
	if (txtName)
	{
		if (GetObject())
		{
			// If there is an existing object name with a leading or trailing
			// space, don't try to remove it.
			if (GetObject()->GetName() == txtName->GetValue())
				return txtName->GetValue();
			else
				return txtName->GetValue().Strip(wxString::both);
		}
		else
			return txtName->GetValue().Strip(wxString::both);
	}
	return wxEmptyString;
}


void dlgProperty::AppendNameChange(wxString &sql, const wxString &objName)
{
	if (GetObject()->GetName() != GetName())
	{
		if (objName.Length() > 0)
		{
			sql += wxT("ALTER ") + objName
			       +  wxT("\n  RENAME TO ") + qtIdent(GetName())
			       +  wxT(";\n");
		}
		else
		{
			sql += wxT("ALTER ") + GetObject()->GetTypeName().MakeUpper()
			       +  wxT(" ") + GetObject()->GetQuotedFullIdentifier()
			       +  wxT("\n  RENAME TO ") + qtIdent(GetName())
			       +  wxT(";\n");
		}
	}
}


void dlgProperty::AppendOwnerChange(wxString &sql, const wxString &objName)
{
	if (!GetObject() || GetObject()->GetOwner() != cbOwner->GetValue())
	{
		sql += wxT("ALTER ") + objName
		       +  wxT("\n  OWNER TO ") + qtIdent(cbOwner->GetValue())
		       +  wxT(";\n");
	}
}


void dlgProperty::AppendOwnerNew(wxString &sql, const wxString &objName)
{
	if (cbOwner->GetGuessedSelection() > 0)
		sql += wxT("ALTER ") + objName
		       +  wxT("\n  OWNER TO ") + qtIdent(cbOwner->GetValue())
		       +  wxT(";\n");
}


void dlgProperty::AppendSchemaChange(wxString &sql, const wxString &objName)
{
	wxString currentschema;

	if (GetObject()->GetMetaType() == PGM_SCHEMA)
	{
		currentschema = GetObject()->GetName();
	}
	else
	{
		currentschema = GetObject()->GetSchema()->GetName();
	}

	if (currentschema != cbSchema->GetValue())
	{
		sql += wxT("ALTER ") + objName
		       +  wxT("\n  SET SCHEMA ") + qtIdent(cbSchema->GetValue())
		       +  wxT(";\n");
	}
}


void dlgProperty::AppendComment(wxString &sql, const wxString &objName, pgObject *obj)
{
	wxString comment = txtComment->GetValue();
	if ((!obj && !comment.IsEmpty()) || (obj && obj->GetComment() != comment))
	{
		sql += wxT("COMMENT ON ") + objName
		       + wxT("\n  IS ") + qtDbString(comment) + wxT(";\n");
	}
}


void dlgProperty::AppendComment(wxString &sql, const wxString &objType, pgSchema *schema, pgObject *obj)
{
	wxString comment = txtComment->GetValue();
	if ((!obj && !comment.IsEmpty()) || (obj && obj->GetComment() != comment))
	{
		sql += wxT("COMMENT ON ") + objType + wxT(" ");
		if (schema)
			sql += schema->GetQuotedPrefix();
		sql += qtIdent(GetName()) + wxT("\n  IS ") + qtDbString(comment) + wxT(";\n");
	}
}


void dlgProperty::AppendQuoted(wxString &sql, const wxString &name)
{
	// quick and quite dirty:
	// !!! this is unsafe if the name itself contains a dot which isn't meant as separator between schema and object
	if (name.First('.') >= 0)
	{
		sql += qtIdent(name.BeforeFirst('.')) + wxT(".") + qtIdent(name.AfterFirst('.'));
	}
	else
		sql += qtIdent(name);
}

void dlgProperty::AppendQuotedType(wxString &sql, const wxString &name)
{
	// see AppendQuoted()
	if (name.First('.') >= 0)
	{
		sql += qtIdent(name.BeforeFirst('.')) + wxT(".") + qtTypeIdent(name.AfterFirst('.'));
	}
	else
		sql += qtTypeIdent(name);
}


void dlgProperty::FillCombobox(const wxString &query, ctlComboBoxFix *cb1, ctlComboBoxFix *cb2)
{
	if (!cb1 && !cb2)
		return;

	pgSet *set = connection->ExecuteSet(query);
	if (set)
	{
		while (!set->Eof())
		{
			if (cb1)
				cb1->Append(set->GetVal(0));
			if (cb2)
				cb2->Append(set->GetVal(0));
			set->MoveNext();
		}
		delete set;
	}

}


void dlgProperty::AddDatabases(ctlComboBoxFix *cb)
{
	FillCombobox(wxT("SELECT datname FROM pg_database ORDER BY 1"), cb);
}


void dlgProperty::AddUsers(ctlComboBoxFix *cb1, ctlComboBoxFix *cb2)
{
	if (connection->BackendMinimumVersion(8, 1))
	{
		FillCombobox(wxT("SELECT rolname FROM pg_roles WHERE rolcanlogin ORDER BY 1"), cb1, cb2);
	}
	else
	{
		FillCombobox(wxT("SELECT usename FROM pg_user ORDER BY 1"), cb1, cb2);
	}
}


void dlgProperty::AddGroups(ctlComboBoxFix *combo)
{
	if (connection->BackendMinimumVersion(8, 1))
	{
		FillCombobox(wxT("SELECT rolname FROM pg_roles WHERE NOT rolcanlogin ORDER BY 1"), combo);
	}
	else
	{
		FillCombobox(wxT("SELECT groname FROM pg_group ORDER BY 1"), combo);
	}
}


void dlgProperty::AddSchemas(ctlComboBoxFix *combo)
{
	if (connection->BackendMinimumVersion(8, 1))
	{
		FillCombobox(wxT("SELECT nspname FROM pg_namespace WHERE nspname NOT LIKE E'pg\\\\_%' AND nspname != 'information_schema' ORDER BY nspname"),
		             combo);
	}
}


void dlgProperty::PrepareTablespace(ctlComboBoxFix *cb, const OID current)
{
	wxASSERT(cb != 0);

	if (connection->BackendMinimumVersion(8, 0))
	{
		// Populate the combo
		cb->FillOidKey(connection, wxT("SELECT oid, spcname FROM pg_tablespace WHERE spcname <> 'pg_global' ORDER BY spcname"));

		if (current)
			cb->SetKey(current);
		else
		{
			if (database)
				cb->SetValue(database->GetDefaultTablespace());
			else
			{
				wxString def = connection->ExecuteScalar(wxT("SELECT current_setting('default_tablespace');"));
				if (def == wxEmptyString || def == wxT("unset"))
					def = wxT("pg_default");
				cb->SetValue(def);
			}
		}
	}
	else
		cb->Disable();
}


void dlgProperty::OnChangeStc(wxStyledTextEvent &ev)
{
	CheckChange();
}


void dlgProperty::OnChange(wxCommandEvent &ev)
{
	CheckChange();
}


void dlgProperty::OnChangeOwner(wxCommandEvent &ev)
{
	ctlComboBox *cb = cbOwner;
	if (cb)
		cb->GuessSelection(ev);
	CheckChange();
}


void dlgProperty::OnChangeReadOnly(wxCommandEvent &ev)
{
	size_t pos;
	bool showmessage;

	showmessage = chkReadOnly->GetValue()
	              && ! (!enableSQL2 && GetSql().Length() == 0 && sqlTextField1->GetText().Cmp(_("-- nothing to change")) == 0)
	              && ! (!enableSQL2 && GetSql().Length() == 0 && sqlTextField1->GetText().Cmp(_("-- definition incomplete")) == 0)
	              && ! (enableSQL2 && GetSql().Length() == 0 && GetSql2().Length() == 0 && sqlTextField1->GetText().Cmp(_("-- nothing to change")) == 0 && sqlTextField2->GetText().Length() == 0)
	              && ! (enableSQL2 && GetSql().Length() == 0 && GetSql2().Length() == 0 && sqlTextField1->GetText().Cmp(_("-- definition incomplete")) == 0 && sqlTextField2->GetText().Length() == 0)
	              && (sqlTextField1->GetText().Cmp(GetSql()) != 0 || (enableSQL2 && sqlTextField2->GetText().Cmp(GetSql2()) != 0));

	if (showmessage)
	{
		if (wxMessageBox(_("Are you sure you wish to cancel your edit?"), _("SQL editor"), wxYES_NO | wxNO_DEFAULT) != wxYES)
		{
			chkReadOnly->SetValue(false);
			return;
		}
	}

	sqlTextField1->SetReadOnly(chkReadOnly->GetValue());
	if (enableSQL2)
	{
		sqlTextField2->SetReadOnly(chkReadOnly->GetValue());
	}
	for (pos = 0; pos < nbNotebook->GetPageCount() - 1; pos++)
	{
		nbNotebook->GetPage(pos)->Enable(chkReadOnly->GetValue());
	}

	if (chkReadOnly->GetValue())
	{
		FillSQLTextfield();
	}
}


void dlgProperty::FillSQLTextfield()
{
	// create a function because this is a duplicated code
	sqlTextField1->SetReadOnly(false);
	if (enableSQL2)
	{
		sqlTextField2->SetReadOnly(false);
	}
	if (btnOK->IsEnabled())
	{
		wxString tmp;
		if (cbClusterSet && cbClusterSet->GetSelection() > 0)
		{
			replClientData *data = (replClientData *)cbClusterSet->wxItemContainer::GetClientData(cbClusterSet->GetSelection());
			if(data)
				tmp.Printf(_("-- Execute replicated using cluster \"%s\", set %ld\n"), data->cluster.c_str(), data->setId);
		}
		sqlTextField1->SetText(tmp + GetSql());
		if (enableSQL2)
		{
			sqlTextField2->SetText(GetSql2());
		}
	}
	else
	{
		if (GetObject())
			sqlTextField1->SetText(_("-- nothing to change"));
		else
			sqlTextField1->SetText(_("-- definition incomplete"));
		if (enableSQL2)
		{
			sqlTextField2->SetText(wxT(""));
		}
	}
	sqlTextField1->SetReadOnly(true);
	if (enableSQL2)
	{
		sqlTextField2->SetReadOnly(true);
	}
}


bool dlgProperty::tryUpdate(wxTreeItemId collectionItem)
{
	ctlTree *browser = mainForm->GetBrowser();
	pgCollection *collection = (pgCollection *)browser->GetObject(collectionItem);
	if (collection && collection->IsCollection() && factory->GetCollectionFactory() == collection->GetFactory())
	{
		pgObject *data = CreateObject(collection);
		if (data)
		{

			wxString nodeName = this->GetDisplayName();
			if (nodeName.IsEmpty())
				nodeName = data->GetDisplayName();

			size_t pos = 0;
			wxTreeItemId newItem;

			if (!data->IsCreatedBy(columnFactory))
			{
				// columns should be appended, not inserted alphabetically

				wxCookieType cookie;
				newItem = browser->GetFirstChild(collectionItem, cookie);
				while (newItem)
				{
					if (browser->GetItemText(newItem) > nodeName)
						break;
					pos++;
					newItem = browser->GetNextChild(collectionItem, cookie);
				}
			}

			if (newItem)
				browser->InsertItem(collectionItem, pos, nodeName, data->GetIconId(), -1, data);
			else
				browser->AppendItem(collectionItem, nodeName, data->GetIconId(), -1, data);

			if (data->WantDummyChild())
				browser->AppendItem(data->GetId(), wxT("Dummy"));

			if (browser->GetSelection() == item)
				collection->ShowTreeDetail(browser, 0, mainForm->GetProperties());
			else
				collection->UpdateChildCount(browser);
		}
		else
		{
			// CreateObject didn't return a new pgObject; refresh the complete collection
			mainForm->Refresh(collection);
		}
		return true;
	}
	return false;
}



void dlgProperty::ShowObject()
{
	mainForm->ObjectBrowserRefreshing(true);
	pgObject *data = GetObject();

	// We might have a parent to refresh. If so, the children will
	// inherently get refreshed as well. Yay :-)
	if (owneritem)
	{
		// Get the object node in case we need it later
		wxTreeItemId objectnode = mainForm->GetBrowser()->GetItemParent(owneritem);

		// Stash the selected items path
		wxString currentPath = mainForm->GetCurrentNodePath();

		pgObject *tblobj = mainForm->GetBrowser()->GetObject(owneritem);

		if (tblobj)
		{
			dlgProperty *ownDialog = NULL;
			if (data)
			{
				ownDialog = data->GetWindowPtr();
				data->SetWindowPtr(NULL);
			}
			mainForm->Refresh(tblobj);
			if (data)
			{
				data->SetWindowPtr(ownDialog);
			}
		}

		// Restore the previous selection...
		mainForm->SetCurrentNode(mainForm->GetBrowser()->GetRootItem(), currentPath);
	}
	else if (data)
	{
		pgObject *newData = data->Refresh(mainForm->GetBrowser(), item);
		if (newData && newData != data)
		{
			mainForm->SetCurrentObject(newData);
			mainForm->GetBrowser()->SetItemData(item, newData);

			newData->SetId(item);
			delete data;
			SetObject(newData);

			newData->UpdateIcon(mainForm->GetBrowser());
		}
		if (newData)
		{
			mainForm->GetBrowser()->DeleteChildren(newData->GetId());

			if (item == mainForm->GetBrowser()->GetSelection())
				newData->ShowTree(mainForm, mainForm->GetBrowser(), mainForm->GetProperties(), 0);
			mainForm->GetBrowser()->SetItemText(item, newData->GetFullName());
			mainForm->GetSqlPane()->SetReadOnly(false);
			mainForm->GetSqlPane()->SetText(newData->GetSql(mainForm->GetBrowser()));
			mainForm->GetSqlPane()->SetReadOnly(true);
		}
	}
	else if (item && chkReadOnly->GetValue())
	{
		wxTreeItemId collectionItem = item;

		while (collectionItem)
		{
			// search up the tree for our collection
			if (tryUpdate(collectionItem))
				break;
			collectionItem = mainForm->GetBrowser()->GetItemParent(collectionItem);
		}
	}
	else // Brute force update the current item
	{
		pgObject *currobj = mainForm->GetBrowser()->GetObject(mainForm->GetBrowser()->GetSelection());

		if (currobj)
			mainForm->Refresh(currobj);
	}
	mainForm->ObjectBrowserRefreshing(false);
}


bool dlgProperty::apply(const wxString &sql, const wxString &sql2)
{
	wxString tmp;
	pgConn *myConn = connection;

	if (GetDisconnectFirst())
	{
		myConn = database->GetServer()->GetConnection();
		database->Disconnect();
	}

	if (!sql.IsEmpty())
	{
		wxArrayString queries;

		if (WannaSplitQueries())
			queries = SplitQueries(BuildSql(sql));
		else
			queries.Add(BuildSql(sql));

		for (size_t index = 0; index < queries.GetCount(); index++)
		{
			tmp = queries.Item(index);
			if (!myConn->ExecuteVoid(tmp))
			{
				// error message is displayed inside ExecuteVoid
				return false;
			}

			if (database)
				database->AppendSchemaChange(tmp);
		}
	}

	// Process the second SQL statement. This is primarily only used by
	// CREATE DATABASE which cannot be run in a multi-statement query in
	// PostgreSQL 8.3+
	if (!sql2.IsEmpty())
	{
		tmp = BuildSql(sql2);

		if (!myConn->ExecuteVoid(tmp))
		{
			// error message is displayed inside ExecuteVoid
			// Warn the user about partially applied changes, but don't bail out.
			// Carry on as if everything was successful (because the most important
			// change was!!
			wxMessageBox(_("An error occurred executing the second stage SQL statement.\n\nChanges may have been partially applied."), _("Warning"), wxICON_EXCLAMATION | wxOK, this);
		}
		else // Only apend schema changes if there was no error.
		{
			if (database)
				database->AppendSchemaChange(tmp);
		}
	}

	ShowObject();

	return true;
}


wxString dlgProperty::BuildSql(const wxString &sql)
{
	wxString tmp;

	if (cbClusterSet && cbClusterSet->GetSelection() > 0)
	{
		replClientData *data = (replClientData *)cbClusterSet->wxItemContainer::GetClientData(cbClusterSet->GetSelection());
		if (data)
		{
			if (data->majorVer > 1 || (data->majorVer == 1 && data->minorVer >= 2))
			{
				// From slony version 2.2.0 onwards ddlscript_prepare() method is removed and
				// ddlscript_complete() method arguments got changed so we have to use ddlcapture() method
				// instead of ddlscript_prepare() and changed the argument of ddlscript_complete() method
				if ((data->majorVer == 2 && data->minorVer >= 2) || (data->majorVer > 2))
				{
					tmp = wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlcapture(") + qtDbString(sql) + wxT(", ") + wxT("NULL::text") + wxT(" );\n")
					      + wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlscript_complete(") + wxT("NULL::text") + wxT(" );\n");
				}
				else
				{
					tmp = wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlscript_prepare(") + NumToStr(data->setId) + wxT(", -1);\n")
					      + sql + wxT(";\n")
					      + wxT("SELECT ") + qtIdent(data->cluster)
					      + wxT(".ddlscript_complete(") + NumToStr(data->setId) + wxT(", ")
					      + qtDbString(sql) + wxT(", -1);\n");
				}
			}
			else
			{
				tmp = wxT("SELECT ") + qtIdent(data->cluster)
				      + wxT(".ddlscript(") + NumToStr(data->setId) + wxT(", ")
				      + qtDbString(sql) + wxT(", 0);\n");
			}
		}
	}
	else
		tmp = sql;

	return tmp;
}


wxArrayString dlgProperty::SplitQueries(const wxString &sql)
{
	wxArrayString queries;
	wxString query;
	wxString c;

	bool antislash = false;
	bool quote_string = false;
	bool doublequote_string = false;

	for (size_t item = 0; item < sql.Length(); item++)
	{
		c = sql.GetChar(item);

		if (c == wxT("\\"))
			antislash = true;

		if (c == wxT("'"))
		{
			if (antislash)
				antislash = false;
			else if (quote_string)
				quote_string = false;
			else if (!doublequote_string)
				quote_string = true;
		}

		if (c == wxT("\""))
		{
			if (antislash)
				antislash = false;
			else if (doublequote_string)
				doublequote_string = false;
			else if (!quote_string)
				doublequote_string = true;
		}

		query = query + c;

		if (c == wxT(";") && !antislash && !quote_string && !doublequote_string)
		{
			queries.Add(query);
			query = wxEmptyString;
		}
	}

	return queries;
}


void dlgProperty::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	if (!IsUpToDate())
	{
		if (wxMessageBox(wxT("The object has been changed by another user. Do you wish to continue to try to update it?"), wxT("Overwrite changes?"), wxYES_NO) != wxYES)
			return;
	}

	EnableOK(false);

	if (IsModal())
	{
		EndModal(0);
		return;
	}

	wxString sql;
	wxString sql2;
	if (chkReadOnly->GetValue())
	{
		sql = GetSql();
		sql2 = GetSql2();
	}
	else
	{
		sql = sqlTextField1->GetText();
		if (enableSQL2)
		{
			sql2 = sqlTextField2->GetText();
		}
		else
		{
			sql2 = wxT("");
		}
	}

	if (!apply(sql, sql2))
	{
		EnableOK(true);
		return;
	}

	Destroy();
}


void dlgProperty::OnPageSelect(wxNotebookEvent &event)
{
	if (sqlTextField1 && chkReadOnly->GetValue() &&
	        event.GetSelection() == (int)nbNotebook->GetPageCount() - 1)
	{
		FillSQLTextfield();
	}
}



void dlgProperty::InitDialog(frmMain *frame, pgObject *node)
{
	CenterOnParent();
	if (!connection)
		connection = node->GetConnection();
	database = node->GetDatabase();

	if (factory != node->GetFactory() && !node->IsCollection())
	{
		wxCookieType cookie;
		wxTreeItemId collectionItem = frame->GetBrowser()->GetFirstChild(node->GetId(), cookie);
		while (collectionItem)
		{
			pgCollection *collection = (pgCollection *)frame->GetBrowser()->GetObject(collectionItem);
			if (collection && collection->IsCollection() && collection->IsCollectionFor(node))
				break;

			collectionItem = frame->GetBrowser()->GetNextChild(node->GetId(), cookie);
		}
		item = collectionItem;
	}
	else
		item = node->GetId();

	// Additional hacks to get the table to refresh when modifying sub-objects
	if (!item && (node->GetMetaType() == PGM_TABLE || node->GetMetaType() == PGM_VIEW
	              || node->GetMetaType() == GP_PARTITION || node->GetMetaType() == PGM_DOMAIN))
		owneritem = node->GetId();

	int metatype = node->GetMetaType();

	switch (metatype)
	{
		case PGM_COLUMN:
			owneritem = node->GetTable()->GetId();
			break;

		case PGM_CHECK:
		case PGM_FOREIGNKEY:
		case PGM_CONSTRAINT:
		case PGM_EXCLUDE:
		case PGM_INDEX:
		case PGM_PRIMARYKEY:
		case PGM_UNIQUE:
		case PGM_TRIGGER:
		case PGM_RULE: // Rules are technically table objects! Yeuch
		case EDB_PACKAGEFUNCTION:
		case EDB_PACKAGEVARIABLE:
		case PGM_SCHEDULE:
		case PGM_STEP:
			if (node->IsCollection())
				owneritem = frame->GetBrowser()->GetParentObject(node->GetId())->GetId();
			else
				owneritem = frame->GetBrowser()->GetParentObject(frame->GetBrowser()->GetParentObject(node->GetId())->GetId())->GetId();
			break;

		default:
			// we want to do this as objects can change schema
			owneritem = node->GetId();
			break;
	}
}


dlgProperty *dlgProperty::CreateDlg(frmMain *frame, pgObject *node, bool asNew, pgaFactory *factory)
{
	if (!factory)
	{
		factory = node->GetFactory();
		if (node->IsCollection())
			factory = ((pgaCollectionFactory *)factory)->GetItemFactory();
	}

	pgObject *currentNode, *parentNode;
	if (asNew)
		currentNode = 0;
	else
		currentNode = node;

	if (factory != node->GetFactory())
		parentNode = node;
	else
		parentNode = frame->GetBrowser()->GetObject(
		                 frame->GetBrowser()->GetItemParent(node->GetId()));

	if (parentNode && parentNode->IsCollection() && parentNode->GetMetaType() != PGM_SERVER)
		parentNode = frame->GetBrowser()->GetObject(
		                 frame->GetBrowser()->GetItemParent(parentNode->GetId()));

	dlgProperty *dlg = 0;

	if (factory)
	{
		dlg = factory->CreateDialog(frame, currentNode, parentNode);
		if (dlg)
		{
			if (factory->IsCollection())
				factory = ((pgaCollectionFactory *)factory)->GetItemFactory();
			wxASSERT(factory);

			dlg->InitDialog(frame, node);

			if (currentNode)
				currentNode->SetWindowPtr(dlg);
		}
	}
	return dlg;
}


bool dlgProperty::CreateObjectDialog(frmMain *frame, pgObject *node, pgaFactory *factory)
{
	if (node->GetMetaType() != PGM_SERVER)
	{
		pgConn *conn = node->GetConnection();
		if (!conn || conn->GetStatus() != PGCONN_OK || !conn->IsAlive())
			return false;
	}

	dlgProperty *dlg = NULL;

	if (node)
		dlg = node->GetWindowPtr();

	if (dlg)
		dlg->Raise();
	else
	{
		dlg = CreateDlg(frame, node, true, factory);

		if (dlg)
		{
			dlg->SetTitle(wxGetTranslation(dlg->factory->GetNewString()));

			dlg->CreateAdditionalPages();
			dlg->Go();
			dlg->CheckChange();
		}
		else
			wxMessageBox(_("Not implemented."));
	}

	return true;
}


bool dlgProperty::EditObjectDialog(frmMain *frame, ctlSQLBox *sqlbox, pgObject *node)
{
	if (node->GetMetaType() != PGM_SERVER)
	{
		pgConn *conn = node->GetConnection();
		if (!conn || conn->GetStatus() != PGCONN_OK || !conn->IsAlive())
			return false;
	}

	// If this is a function or view, hint that the user might want to edit the object in
	// the query tool.
	if (node->GetMetaType() == PGM_FUNCTION || node->GetMetaType() == PGM_VIEW)
	{
		if (frmHint::ShowHint(frame, HINT_OBJECT_EDITING) == wxID_CANCEL)
			return false;
	}

	dlgProperty *dlg = NULL;

	if (node)
		dlg = node->GetWindowPtr();

	if (dlg)
		dlg->Raise();
	else
	{
		dlg = CreateDlg(frame, node, false);

		if (dlg)
		{
			wxString typeName = dlg->factory->GetTypeName();
			dlg->SetTitle(wxString(wxGetTranslation(typeName)) + wxT(" ") + node->GetFullIdentifier());

			dlg->CreateAdditionalPages();
			dlg->Go();

			dlg->CheckChange();
		}
		else
			wxMessageBox(_("Not implemented."));
	}

	return true;
}

wxString dlgProperty::qtDbString(const wxString &str)
{
	// Use the server aware version if possible
	if (connection)
		return connection->qtDbString(str);
	else if (database)
		return database->GetConnection()->qtDbString(str);
	else
	{
		wxString ret = str;
		ret.Replace(wxT("\\"), wxT("\\\\"));
		ret.Replace(wxT("'"), wxT("''"));
		ret.Append(wxT("'"));
		ret.Prepend(wxT("'"));
		return ret;
	}
}

void dlgProperty::OnHelp(wxCommandEvent &ev)
{
	wxString page = GetHelpPage();

	if (!page.IsEmpty())
	{
		if (page.StartsWith(wxT("pg/")))
		{
			if (connection)
			{
				if (connection->GetIsEdb())
					DisplayHelp(page.Mid(3), HELP_ENTERPRISEDB);
				else if (connection->GetIsGreenplum())
					DisplayHelp(page.Mid(3), HELP_GREENPLUM);
				else
					DisplayHelp(page.Mid(3), HELP_POSTGRESQL);
			}
			else
				DisplayHelp(page.Mid(3), HELP_POSTGRESQL);
		}
		else if (page.StartsWith(wxT("slony/")))
			DisplayHelp(page.Mid(6), HELP_SLONY);
		else
			DisplayHelp(page, HELP_PGADMIN);
	}
}

/////////////////////////////////////////////////////////////////////////////


dlgTypeProperty::dlgTypeProperty(pgaFactory *f, frmMain *frame, const wxString &resName)
	: dlgProperty(f, frame, resName)
{
	isVarLen = false;
	isVarPrec = false;
	if (wxWindow::FindWindow(XRCID("txtLength")))
	{
		txtLength = CTRL_TEXT("txtLength");
		txtLength->SetValidator(numericValidator);
		txtLength->Disable();
	}
	else
		txtLength = 0;
	if (wxWindow::FindWindow(XRCID("txtPrecision")))
	{
		txtPrecision = CTRL_TEXT("txtPrecision");
		txtPrecision->SetValidator(numericValidator);
		txtPrecision->Disable();
	}
	else
		txtPrecision = 0;
}


void dlgTypeProperty::FillDatatype(ctlComboBox *cb, bool withDomains, bool addSerials)
{
	FillDatatype(cb, 0, withDomains, addSerials);
}

void dlgTypeProperty::FillDatatype(ctlComboBox *cb, ctlComboBox *cb2, bool withDomains, bool addSerials)
{

	if (dtCache.IsEmpty())
	{
		// A column dialog is directly called, no datatype caching is done.
		// Fetching datatypes from server.
		DatatypeReader tr(database, withDomains, addSerials);
		while (tr.HasMore())
		{
			pgDatatype dt = tr.GetDatatype();

			AddType(wxT("?"), tr.GetOid(), dt.GetQuotedSchemaPrefix(database) + dt.QuotedFullName());
			cb->Append(dt.GetQuotedSchemaPrefix(database) + dt.QuotedFullName());
			if (cb2)
				cb2->Append(dt.GetQuotedSchemaPrefix(database) + dt.QuotedFullName());
			tr.MoveNext();
		}
	}
	else
	{
		// A column dialog is called from a table dialog where we have already cached the datatypes.
		// Using cached datatypes.
		size_t i;
		for (i = 0; i < dtCache.GetCount(); i++)
		{
			AddType(wxT("?"), dtCache.Item(i)->GetOid(), dtCache.Item(i)->GetTypename());
			cb->Append(dtCache.Item(i)->GetTypename());
			if (cb2)
				cb2->Append(dtCache.Item(i)->GetTypename());
		}
	}

}


int dlgTypeProperty::Go(bool modal)
{
	if (GetObject())
	{
		if (txtLength)
			txtLength->SetValidator(numericValidator);
		if (txtPrecision)
			txtPrecision->SetValidator(numericValidator);
	}
	return dlgProperty::Go(modal);
}



void dlgTypeProperty::AddType(const wxString &typ, const OID oid, const wxString quotedName)
{
	wxString vartyp;
	if (typ == wxT("?"))
	{
		switch ((long)oid)
		{
			case PGOID_TYPE_BIT:
			case PGOID_TYPE_BIT_ARRAY:
			case PGOID_TYPE_VARBIT:
			case PGOID_TYPE_VARBIT_ARRAY:
			case PGOID_TYPE_BPCHAR:
			case PGOID_TYPE_BPCHAR_ARRAY:
			case PGOID_TYPE_VARCHAR:
			case PGOID_TYPE_VARCHAR_ARRAY:
				vartyp = wxT("L");
				break;
			case PGOID_TYPE_TIME:
			case PGOID_TYPE_TIME_ARRAY:
			case PGOID_TYPE_TIMETZ:
			case PGOID_TYPE_TIMETZ_ARRAY:
			case PGOID_TYPE_TIMESTAMP:
			case PGOID_TYPE_TIMESTAMP_ARRAY:
			case PGOID_TYPE_TIMESTAMPTZ:
			case PGOID_TYPE_TIMESTAMPTZ_ARRAY:
			case PGOID_TYPE_INTERVAL:
			case PGOID_TYPE_INTERVAL_ARRAY:
				vartyp = wxT("D");
				break;
			case PGOID_TYPE_NUMERIC:
			case PGOID_TYPE_NUMERIC_ARRAY:
				vartyp = wxT("P");
				break;
			default:
				vartyp = wxT(" ");
				break;
		}
	}
	else
		vartyp = typ;

	types.Add(vartyp + NumToStr(oid) + wxT(":") + quotedName);
}


wxString dlgTypeProperty::GetTypeInfo(int sel)
{
	wxString str;
	if (sel >= 0)
		str = types.Item(sel);

	return str;
}


wxString dlgTypeProperty::GetTypeOid(int sel)
{
	wxString str;
	if (sel >= 0)
		str = types.Item(sel).Mid(1).BeforeFirst(':');

	return str;
}


wxString dlgTypeProperty::GetQuotedTypename(int sel)
{
	wxString sql, suffix;
	bool isArray = false;

	if (sel >= 0)
	{
		sql = types.Item(sel).AfterFirst(':');

		// Deal with time/timestamp first as they're special cases
		if (sql.Left(19) == wxT("time with time zone"))
		{
			if (sql.Right(2) == wxT("[]"))
				isArray = true;
			sql = wxT("time");
			suffix = wxT("with time zone");
		}
		else if (sql.Left(21) == wxT("time without time zone"))
		{
			if (sql.Right(2) == wxT("[]"))
				isArray = true;
			sql = wxT("time");
			suffix = wxT("without time zone");
		}
		else if (sql.Left(24) == wxT("timestamp with time zone"))
		{
			if (sql.Right(2) == wxT("[]"))
				isArray = true;
			sql = wxT("timestamp");
			suffix = wxT("with time zone");
		}
		else if (sql.Left(27) == wxT("timestamp without time zone"))
		{
			if (sql.Right(2) == wxT("[]"))
				isArray = true;
			sql = wxT("timestamp");
			suffix = wxT("without time zone");
		}
		else if (sql.Right(2) == wxT("[]"))
		{
			sql = sql.SubString(0, sql.Len() - 3);
			isArray = true;
		}
		else if (sql.Right(3) == wxT("[]\""))
		{
			sql = sql.SubString(1, sql.Len() - 4);
			isArray = true;
		}

		// Stick the length on
		if (isVarLen && txtLength)
		{
			wxString varlen = txtLength->GetValue();
			if (!varlen.IsEmpty() && NumToStr(StrToLong(varlen)) == varlen && StrToLong(varlen) >= minVarLen)
			{
				sql += wxT("(") + varlen;
				if (isVarPrec && txtPrecision)
				{
					wxString varprec = txtPrecision->GetValue();
					if (!varprec.IsEmpty())
						sql += wxT(",") + varprec;
				}
				sql += wxT(")");
			}
		}
	}

	// Append any post-length suffix
	if (suffix.length())
		sql += wxT(" ") + suffix;

	// Append any array decoration
	if (isArray)
		sql += wxT("[]");

	return sql;
}


void dlgTypeProperty::CheckLenEnable()
{
	int sel = cbDatatype->GetGuessedSelection();
	if (sel >= 0)
	{
		wxString info = types.Item(sel);
		isVarPrec = info.StartsWith(wxT("P"));
		isVarLen =  isVarPrec || info.StartsWith(wxT("L")) || info.StartsWith(wxT("D"));
		minVarLen = (info.StartsWith(wxT("D")) ? 0 : 1);
		maxVarLen = isVarPrec ? 1000 :
		            minVarLen ? 0x7fffffff : 10;
	}
}


/////////////////////////////////////////////////////////////////////////////


dlgCollistProperty::dlgCollistProperty(pgaFactory *f, frmMain *frame, const wxString &resName, pgTable *parentNode)
	: dlgProperty(f, frame, resName)
{
	columns = 0;
	table = parentNode;
}


dlgCollistProperty::dlgCollistProperty(pgaFactory *f, frmMain *frame, const wxString &resName, ctlListView *colList)
	: dlgProperty(f, frame, resName)
{
	columns = colList;
	table = 0;
}


int dlgCollistProperty::Go(bool modal)
{
	if (columns)
	{
		int pos;
		// iterate cols
		for (pos = 0 ; pos < columns->GetItemCount() ; pos++)
		{
			wxString col = columns->GetItemText(pos);
			if (cbColumns->FindString(col) < 0)
			{
				cbColumns->Append(col, StrToOid(columns->GetText(pos, 7)));
			}
		}
	}
	if (table)
	{
		wxCookieType cookie;
		pgObject *data;
		wxTreeItemId columnsItem = mainForm->GetBrowser()->GetFirstChild(table->GetId(), cookie);
		while (columnsItem)
		{
			data = mainForm->GetBrowser()->GetObject(columnsItem);
			if (data->GetMetaType() == PGM_COLUMN && data->IsCollection())
				break;
			columnsItem = mainForm->GetBrowser()->GetNextChild(table->GetId(), cookie);
		}

		if (columnsItem)
		{
			wxCookieType cookie;
			pgColumn *column;
			wxTreeItemId item = mainForm->GetBrowser()->GetFirstChild(columnsItem, cookie);

			// check columns
			while (item)
			{
				column = (pgColumn *)mainForm->GetBrowser()->GetObject(item);
				if (column->IsCreatedBy(columnFactory))
				{
					if (column->GetColNumber() > 0)
					{
						cbColumns->Append(column->GetName(), column->GetAttTypId());
					}
				}

				item = mainForm->GetBrowser()->GetNextChild(columnsItem, cookie);
			}
		}
	}

	return dlgProperty::Go(modal);
}



/////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(dlgSecurityProperty, dlgProperty)
	EVT_BUTTON(CTL_ADDPRIV,             dlgSecurityProperty::OnAddPriv)
	EVT_BUTTON(CTL_DELPRIV,             dlgSecurityProperty::OnDelPriv)
#ifdef __WXMAC__
	EVT_SIZE(                           dlgSecurityProperty::OnChangeSize)
#endif
END_EVENT_TABLE();

void dlgSecurityProperty::SetPrivilegesLayout()
{
	securityPage->lbPrivileges->GetParent()->Layout();
}

dlgSecurityProperty::dlgSecurityProperty(pgaFactory *f, frmMain *frame, pgObject *obj, const wxString &resName, const wxString &privList, const char *privChar)
	: dlgProperty(f, frame, resName)
{
	securityChanged = false;


	if (!privList.IsEmpty() && (!obj || obj->CanCreate()))
	{
		securityPage = new ctlSecurityPanel(nbNotebook, privList, privChar, frame->GetImageList());

		if (obj)
		{

			wxArrayString groups;
			// Fetch Groups Information
			pgSet *setGrp = obj->GetConnection()->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

			if (setGrp)
			{
				while (!setGrp->Eof())
				{
					groups.Add(setGrp->GetVal(0));
					setGrp->MoveNext();
				}
				delete setGrp;
			}

			wxString str = obj->GetAcl();
			if (!str.IsEmpty())
			{
				str = str.Mid(1, str.Length() - 2);
				wxStringTokenizer tokens(str, wxT(","));

				while (tokens.HasMoreTokens())
				{
					wxString str = tokens.GetNextToken();
					if (str[0U] == '"')
						str = str.Mid(1, str.Length() - 2);

					wxString name = str.BeforeLast('=');
					wxString value;

					connection = obj->GetConnection();
					if (connection->BackendMinimumVersion(7, 4))
						value = str.Mid(name.Length() + 1).BeforeLast('/');
					else
						value = str.Mid(name.Length() + 1);

					int icon = userFactory.GetIconId();

					if (name.Left(6).IsSameAs(wxT("group "), false))
					{
						icon = groupFactory.GetIconId();
						name = wxT("group ") + qtStrip(name.Mid(6));
					}
					else if (name.IsEmpty())
					{
						icon = PGICON_PUBLIC;
						name = wxT("public");
					}
					else
					{
						name = qtStrip(name);
						for (unsigned int index = 0; index < groups.Count(); index++)
							if (name == groups[index])
							{
								name = wxT("group ") + name;
								icon = groupFactory.GetIconId();
								break;
							}
					}

					securityPage->lbPrivileges->AppendItem(icon, name, value);
					currentAcl.Add(name + wxT("=") + value);
				}
			}
			else
			{
				int icon = PGICON_PUBLIC;
				wxString name = wxT("public");
				wxString value;
				if (obj->GetMetaType() == PGM_DATABASE)
					value = wxT("Tc");
				else if (obj->GetMetaType() == PGM_FUNCTION)
					value = wxT("X");
				else if (obj->GetMetaType() == PGM_LANGUAGE)
					value = wxT("U");

				if (value != wxEmptyString)
				{
					securityPage->lbPrivileges->AppendItem(icon, name, value);
					currentAcl.Add(name + wxT("=") + value);
				}
			}
		}
	}
	else
		securityPage = NULL;
}


dlgSecurityProperty::~dlgSecurityProperty()
{
}



#ifdef __WXMAC__
void dlgSecurityProperty::OnChangeSize(wxSizeEvent &ev)
{
	if (securityPage)
		securityPage->lbPrivileges->SetSize(wxDefaultCoord, wxDefaultCoord,
		                                    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 550);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


int dlgSecurityProperty::Go(bool modal)
{
	if (securityPage)
	{
		if (cbOwner && !cbOwner->GetCount())
		{
			if (!GetObject())
				cbOwner->Append(wxEmptyString);
			AddGroups(cbOwner);
			AddUsers(cbOwner);
		}

		securityPage->SetConnection(connection);
		//securityPage->Layout();
	}

	return dlgProperty::Go(modal);
}


void dlgSecurityProperty::AddGroups(ctlComboBox *comboBox)
{
	if (!((securityPage && securityPage->cbGroups) || comboBox))
		return;

	pgSet *set = connection->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

	if (set)
	{
		while (!set->Eof())
		{
			if (securityPage && securityPage->cbGroups)
				securityPage->cbGroups->Append(wxT("group ") + set->GetVal(0));
			if (comboBox)
				comboBox->Append(set->GetVal(0));
			set->MoveNext();
		}
		delete set;
	}
}


void dlgSecurityProperty::AddUsers(ctlComboBox *combobox)
{
	if (securityPage && securityPage->cbGroups && settings->GetShowUsersForPrivileges())
	{
		securityPage->stGroup->SetLabel(_("Group/User"));
		dlgProperty::AddUsers(securityPage->cbGroups, combobox);
		Layout();
	}
	else
		dlgProperty::AddUsers(combobox);
}


void dlgSecurityProperty::OnAddPriv(wxCommandEvent &ev)
{
	securityChanged = true;
	EnableOK(btnOK->IsEnabled());
}


void dlgSecurityProperty::OnDelPriv(wxCommandEvent &ev)
{
	securityChanged = true;
	EnableOK(btnOK->IsEnabled());
}


wxString dlgSecurityProperty::GetHelpPage() const
{
	if (nbNotebook->GetSelection() == (int)nbNotebook->GetPageCount() - 2)
		return wxT("pg/sql-grant");
	else
		return dlgProperty::GetHelpPage();
}


void dlgSecurityProperty::EnableOK(bool enable, bool ignoreSql)
{
	// Don't enable the OK button if the object isn't yet created,
	// leave that to the object dialog.
	if (securityChanged && GetObject() && !ignoreSql)
	{
		wxString sql = GetSql();
		if (sql.IsEmpty())
		{
			enable = false;
			securityChanged = false;
		}
		else
			enable = true;
	}
	dlgProperty::EnableOK(enable);
}


wxString dlgSecurityProperty::GetGrant(const wxString &allPattern, const wxString &grantObject)
{
	if (securityPage)
		return securityPage->GetGrant(allPattern, grantObject, &currentAcl);
	else
		return wxString();
}

bool dlgSecurityProperty::DisablePrivilege(const wxString &priv)
{
	if (securityPage)
		return securityPage->DisablePrivilege(priv);
	else
		return true;
}

void dlgSecurityProperty::AppendCurrentAcl(const wxString &name, const wxString &value)
{
	if (!(name.IsEmpty() && value.IsEmpty()))
		currentAcl.Add(name + wxT("=") + value);
}


/////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(dlgDefaultSecurityProperty, dlgSecurityProperty)
	EVT_BUTTON(CTL_DEFADDPRIV, dlgDefaultSecurityProperty::OnAddPriv)
	EVT_BUTTON(CTL_DEFDELPRIV, dlgDefaultSecurityProperty::OnDelPriv)
#ifdef __WXMAC__
	EVT_SIZE(                  dlgDefaultSecurityProperty::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgDefaultSecurityProperty::dlgDefaultSecurityProperty(pgaFactory *f, frmMain *frame, pgObject *obj, const wxString &resName, const wxString &privList, const char *privChar, bool createDefPrivPanel)
	: dlgSecurityProperty(f, frame, obj, resName, privList, privChar), defaultSecurityChanged(false)
{
	pgConn *l_conn = obj ? obj->GetConnection() : connection;
	if ((!obj || obj->CanCreate()) && createDefPrivPanel)
		defaultSecurityPage = new ctlDefaultSecurityPanel(l_conn, nbNotebook, frame->GetImageList());
	else
		defaultSecurityPage = NULL;
}


void dlgDefaultSecurityProperty::AddGroups(ctlComboBox *comboBox)
{
	if (!((securityPage && securityPage->cbGroups) || comboBox || defaultSecurityPage))
		return;

	pgSet *set = connection->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

	if (set)
	{
		while (!set->Eof())
		{
			if (securityPage && securityPage->cbGroups)
				securityPage->cbGroups->Append(wxT("group ") + set->GetVal(0));

			if (comboBox)
				comboBox->Append(set->GetVal(0));

			if (defaultSecurityPage)
				defaultSecurityPage->m_groups.Add(wxT("group ") + set->GetVal(0));

			set->MoveNext();
		}
		delete set;
	}
}


void dlgDefaultSecurityProperty::AddUsers(ctlComboBox *combobox)
{
	if ((securityPage && securityPage->cbGroups) || defaultSecurityPage || combobox)
	{
		wxString strFetchUserQuery =
		    connection->BackendMinimumVersion(8, 1) ?
		    wxT("SELECT rolname FROM pg_roles WHERE rolcanlogin ORDER BY 1") :
		    wxT("SELECT usename FROM pg_user ORDER BY 1");

		pgSet *set = connection->ExecuteSet(strFetchUserQuery);
		if (set)
		{
			while (!set->Eof())
			{
				if (settings->GetShowUsersForPrivileges())
				{
					if (securityPage && securityPage->cbGroups)
						securityPage->cbGroups->Append(set->GetVal(0));

					if (defaultSecurityPage)
						defaultSecurityPage->m_groups.Add(set->GetVal(0));
				}

				if (combobox)
					combobox->Append(set->GetVal(0));

				set->MoveNext();
			}
			delete set;
		}
	}
}

#ifdef __WXMAC__
void dlgDefaultSecurityProperty::OnChangeSize(wxSizeEvent &ev)
{
	wxSize l_size = ev.GetSize();
	if (defaultSecurityPage && l_size.GetWidth() > 10 && l_size.GetHeight() > 25)
		defaultSecurityPage->SetSize(l_size.GetWidth() - 10, l_size.GetHeight() - 25);
	dlgSecurityProperty::OnChangeSize(ev);
}
#endif


void dlgDefaultSecurityProperty::EnableOK(bool enable, bool ignoreSql)
{
	// Don't enable the OK button if the object isn't yet created,
	// leave that to the object dialog.
	if (GetObject() && !ignoreSql)
	{
		wxString sql = GetSql();
		if (sql.IsEmpty())
		{
			enable = false;
		}
		else
			enable = true;
	}
	dlgSecurityProperty::EnableOK(enable, ignoreSql);
}


void dlgDefaultSecurityProperty::OnAddPriv(wxCommandEvent &ev)
{
	defaultSecurityChanged = true;
	EnableOK(btnOK->IsEnabled());
}


void dlgDefaultSecurityProperty::OnDelPriv(wxCommandEvent &ev)
{
	defaultSecurityChanged = true;
	EnableOK(btnOK->IsEnabled());
}

wxString dlgDefaultSecurityProperty::GetDefaultPrivileges(const wxString &schemaName)
{
	if (defaultSecurityChanged)
		return defaultSecurityPage->GetDefaultPrivileges(schemaName);
	return wxT("");
}

int dlgDefaultSecurityProperty::Go(bool modal, bool createDefPrivs, const wxString &defPrivsOnTables,
                                   const wxString &defPrivsOnSeqs, const wxString &defPrivsOnFuncs,
                                   const wxString &defPrivsOnTypes)
{
	if (securityPage)
	{
		if (cbOwner && !cbOwner->GetCount())
		{
			if (!GetObject())
				cbOwner->Append(wxEmptyString);
			AddGroups(cbOwner);
			AddUsers(cbOwner);
		}

		securityPage->SetConnection(connection);
		//securityPage->Layout();
	}

	int res = dlgSecurityProperty::Go(modal);

	if (defaultSecurityPage)
	{
		if (createDefPrivs && connection->BackendMinimumVersion(9, 0))
			defaultSecurityPage->UpdatePrivilegePages(createDefPrivs, defPrivsOnTables,
			        defPrivsOnSeqs, defPrivsOnFuncs, defPrivsOnTypes);
		else
			defaultSecurityPage->Enable(false);
	}

	return res;
}

wxString dlgDefaultSecurityProperty::GetHelpPage() const
{
	int nDiff      = nbNotebook->GetPageCount() - nbNotebook->GetSelection();

	switch (nDiff)
	{
		case 3:
			return wxT("pg/sql-grant");
		case 2:
			return wxT("pg/sql-alterdefaultprivileges");
		default:
			return dlgProperty::GetHelpPage();
	}
}

/////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(dlgAgentProperty, dlgProperty)
	EVT_BUTTON (wxID_OK,                            dlgAgentProperty::OnOK)
END_EVENT_TABLE();

dlgAgentProperty::dlgAgentProperty(pgaFactory *f, frmMain *frame, const wxString &resName)
	: dlgProperty(f, frame, resName)
{
	recId = 0;
}


wxString dlgAgentProperty::GetSql()
{
	wxString str = GetInsertSql();
	if (!str.IsEmpty())
		str += wxT("\n\n");
	return str + GetUpdateSql();
}



bool dlgAgentProperty::executeSql()
{
	wxString sql;
	bool dataChanged = false;

	sql = GetInsertSql();
	if (!sql.IsEmpty())
	{
		int pos;
		long jobId = 0, schId = 0, stpId = 0;
		if (sql.Contains(wxT("<JobId>")))
		{
			recId = jobId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_job_jobid_seq');")));
			while ((pos = sql.Find(wxT("<JobId>"))) >= 0)
				sql = sql.Left(pos) + NumToStr(jobId) + sql.Mid(pos + 7);
		}

		if (sql.Contains(wxT("<SchId>")))
		{
			// Each schedule ID should be unique. This'll need work if a schedule hits more than
			// one table or anything.
			recId = schId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_schedule_jscid_seq');")));
			while ((pos = sql.Find(wxT("<SchId>"))) >= 0)
			{
				sql = sql.Left(pos) + NumToStr(schId) + sql.Mid(pos + 7);
				recId = schId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_schedule_jscid_seq');")));
			}
		}

		if (sql.Contains(wxT("<StpId>")))
		{
			// Each step ID should be unique. This'll need work if a step hits more than
			// one table or anything.
			recId = stpId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_jobstep_jstid_seq');")));
			while ((pos = sql.Find(wxT("<StpId>"))) >= 0)
			{
				sql = sql.Left(pos) + NumToStr(stpId) + sql.Mid(pos + 7);
				recId = stpId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_jobstep_jstid_seq');")));
			}
		}

		pgSet *set = connection->ExecuteSet(sql);
		if (set)
		{
			delete set;
		}
		if (!set)
		{
			return false;
		}
		dataChanged = true;
	}

	sql = GetUpdateSql();
	if (!sql.IsEmpty())
	{
		int pos;
		while ((pos = sql.Find(wxT("<JobId>"))) >= 0)
			sql = sql.Left(pos) + NumToStr(recId) + sql.Mid(pos + 7);

		long newId;
		if (sql.Contains(wxT("<SchId>")))
		{
			// Each schedule ID should be unique. This'll need work if a schedule hits more than
			// one table or anything.
			newId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_schedule_jscid_seq');")));
			while ((pos = sql.Find(wxT("<SchId>"))) >= 0)
			{
				sql = sql.Left(pos) + NumToStr(newId) + sql.Mid(pos + 7);
				newId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_schedule_jscid_seq');")));
			}
		}

		if (sql.Contains(wxT("<StpId>")))
		{
			// Each step ID should be unique. This'll need work if a step hits more than
			// one table or anything.
			newId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_jobstep_jstid_seq');")));
			while ((pos = sql.Find(wxT("<StpId>"))) >= 0)
			{
				sql = sql.Left(pos) + NumToStr(newId) + sql.Mid(pos + 7);
				newId = StrToLong(connection->ExecuteScalar(wxT("SELECT nextval('pgagent.pga_jobstep_jstid_seq');")));
			}
		}

		if (!connection->ExecuteVoid(sql))
		{
			// error message is displayed inside ExecuteVoid
			return false;
		}
		dataChanged = true;
	}

	return dataChanged;
}


void dlgAgentProperty::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	if (!IsUpToDate())
	{
		if (wxMessageBox(wxT("The object has been changed by another user. Do you wish to continue to try to update it?"), wxT("Overwrite changes?"), wxYES_NO) != wxYES)
			return;
	}

	if (IsModal())
	{
		EndModal(0);
		return;
	}

	connection->ExecuteVoid(wxT("BEGIN TRANSACTION"));

	if (executeSql())
	{
		connection->ExecuteVoid(wxT("COMMIT TRANSACTION"));
		ShowObject();
	}
	else
	{
		connection->ExecuteVoid(wxT("ROLLBACK TRANSACTION"));
	}

	Destroy();
}


propertyFactory::propertyFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	if (mnu)
		mnu->Append(id, _("&Properties...\tCtrl-Alt-Enter"), _("Display/edit the properties of the selected object."));
	else
		context = false;
	if (toolbar)
		toolbar->AddTool(id, wxEmptyString, *properties_png_bmp, _("Display/edit the properties of the selected object."), wxITEM_NORMAL);
}


wxWindow *propertyFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (!dlgProperty::EditObjectDialog(form, form->GetSqlPane(), obj))
		form->CheckAlive();

	return 0;
}


bool propertyFactory::CheckEnable(pgObject *obj)
{
	return obj && ((obj->GetMetaType() == PGM_DATABASE) ? (obj->GetConnection() != NULL) : true) && obj->CanEdit();
}


#include "images/create.pngc"
createFactory::createFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("&Create..."),  _("Create a new object of the same type as the selected object."));
	toolbar->AddTool(id, wxEmptyString, *create_png_bmp, _("Create a new object of the same type as the selected object."), wxITEM_NORMAL);
}


wxWindow *createFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (!dlgProperty::CreateObjectDialog(form, obj, 0))
		form->CheckAlive();

	return 0;
}


bool createFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->CanCreate();
}


#include "images/drop.pngc"
dropFactory::dropFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Delete/Drop...\tDel"),  _("Delete/Drop the selected object."));
	toolbar->AddTool(id, wxEmptyString, *drop_png_bmp, _("Drop the currently selected object."), wxITEM_NORMAL);
}


wxWindow *dropFactory::StartDialog(frmMain *form, pgObject *obj)
{
	form->ExecDrop(false);
	return 0;
}


bool dropFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->CanDrop();
}


dropCascadedFactory::dropCascadedFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Drop cascaded..."), _("Drop the selected object and all objects dependent on it."));
}


wxWindow *dropCascadedFactory::StartDialog(frmMain *form, pgObject *obj)
{
	form->ExecDrop(true);
	return 0;
}


bool dropCascadedFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->CanDrop() && obj->CanDropCascaded();
}


#include "images/refresh.pngc"
refreshFactory::refreshFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	if (mnu)
		mnu->Append(id, _("Re&fresh\tF5"), _("Refresh the selected object."));
	else
		context = false;
	if (toolbar)
		toolbar->AddTool(id, wxEmptyString, *refresh_png_bmp, _("Refresh the selected object."), wxITEM_NORMAL);
}


wxWindow *refreshFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (form)
		obj = form->GetBrowser()->GetObject(form->GetBrowser()->GetSelection());

	if (obj)
		if (CheckEnable(obj))
			form->Refresh(obj);
	return 0;
}


bool refreshFactory::CheckEnable(pgObject *obj)
{
	// This isn't really clean... But we don't have a pgObject::CanRefresh() so far,
	// so it's Good Enough (tm) for now.
	return obj != 0 && !obj->IsCreatedBy(serverFactory.GetCollectionFactory());
}

