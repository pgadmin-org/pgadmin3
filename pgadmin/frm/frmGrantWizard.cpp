//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmGrantWizard.cpp - Grant Wizard dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frm/frmGrantWizard.h"
#include "frm/frmMain.h"
#include "utils/sysLogger.h"
#include "ctl/ctlSecurityPanel.h"
#include "schema/pgFunction.h"
#include "schema/pgSequence.h"
#include "schema/pgTable.h"
#include "schema/pgView.h"
#include "schema/gpExtTable.h"

// Icons
#include "images/index.pngc"


#define chkList     CTRL_CHECKLISTBOX("chkList")



BEGIN_EVENT_TABLE(frmGrantWizard, ExecutionDialog)
	EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  frmGrantWizard::OnPageSelect)
	EVT_BUTTON(XRCID("btnChkAll"),                  frmGrantWizard::OnCheckAll)
	EVT_BUTTON(XRCID("btnUnchkAll"),                frmGrantWizard::OnUncheckAll)
	EVT_CHECKLISTBOX(XRCID("chkList"),              frmGrantWizard::OnChange)
END_EVENT_TABLE()


frmGrantWizard::frmGrantWizard(frmMain *form, pgObject *obj) : ExecutionDialog(form, obj)
{
	nbNotebook = 0;

	SetFont(settings->GetSystemFont());
	LoadResource(form, wxT("frmGrantWizard"));
	RestorePosition();

	SetTitle(object->GetTranslatedMessage(GRANTWIZARDTITLE));

	// Icon
	SetIcon(*index_png_ico);
	nbNotebook = CTRL_NOTEBOOK("nbNotebook");
	sqlPane = 0;

	Restore();
	EnableOK(false);
}


frmGrantWizard::~frmGrantWizard()
{
	SavePosition();
	Abort();
}


wxString frmGrantWizard::GetHelpPage() const
{
	wxString page = wxT("pg/sql-grant");

	return page;
}

void frmGrantWizard::OnChange(wxCommandEvent &event)
{
	sqlPane->SetReadOnly(false);
	sqlPane->SetText(GetSql());
	sqlPane->SetReadOnly(true);

	if (sqlPane->GetText().IsEmpty())
		EnableOK(false);
	else
		EnableOK(true);
}


void frmGrantWizard::OnUncheckAll(wxCommandEvent &event)
{
	unsigned int i;
	for (i = 0 ; i < chkList->GetCount() ; i++)
		chkList->Check(i, false);

	OnChange(event);
}



void frmGrantWizard::OnCheckAll(wxCommandEvent &event)
{
	unsigned int i;
	for (i = 0 ; i < chkList->GetCount() ; i++)
		chkList->Check(i, true);

	OnChange(event);
}

void frmGrantWizard::OnPageSelect(wxNotebookEvent &event)
{
	if (nbNotebook && sqlPane && event.GetSelection() == (int)nbNotebook->GetPageCount() - 2)
	{
		sqlPane->SetReadOnly(false);
		sqlPane->SetText(GetSql());
		sqlPane->SetReadOnly(true);
	}
}


void frmGrantWizard::AddObjects(pgCollection *collection)
{
	bool traverseKids = (!collection->IsCollection() || collection->GetMetaType() == PGM_SCHEMA);

	if (!traverseKids)
	{
		pgaFactory *factory = collection->GetFactory();
		if (!factory->IsCollectionFor(tableFactory) &&
		        !factory->IsCollectionFor(functionFactory) &&
		        !factory->IsCollectionFor(triggerFunctionFactory) &&
		        !factory->IsCollectionFor(procedureFactory) &&
		        !factory->IsCollectionFor(viewFactory) &&
		        !factory->IsCollectionFor(extTableFactory) &&
		        !factory->IsCollectionFor(sequenceFactory))
			return;
	}

	wxCookieType cookie;
	wxTreeItemId item = mainForm->GetBrowser()->GetFirstChild(collection->GetId(), cookie);

	while (item)
	{
		pgObject *obj = mainForm->GetBrowser()->GetObject(item);
		if (obj)
		{
			if (traverseKids)
				AddObjects((pgCollection *)obj);
			else
			{
				if (obj->CanEdit())
				{
					objectArray.Add(obj);
					chkList->Append((wxString)wxGetTranslation(obj->GetTypeName()) + wxT(" ") + obj->GetFullIdentifier());
				}
			}
		}
		item = mainForm->GetBrowser()->GetNextChild(collection->GetId(), cookie);
	}
}

void frmGrantWizard::Go()
{
	chkList->SetFocus();

	wxString privList = wxT("INSERT,SELECT,UPDATE,DELETE,TRUNCATE,REFERENCES,TRIGGER");
	const char *privChar = "arwdDxt";

	switch (object->GetMetaType())
	{
		case PGM_DATABASE:
		case PGM_SCHEMA:
			privList.Append(wxT(",EXECUTE,USAGE"));
			privChar = "arwdDxtXU";
			break;
		case PGM_FUNCTION:
			privList = wxT("EXECUTE");
			privChar = "X";
			break;
		case PGM_SEQUENCE:
			privList = wxT("SELECT,UPDATE,USAGE");
			privChar = "rwU";
			break;
		case GP_EXTTABLE:
			privList = wxT("SELECT");
			privChar = "r";
		default:
			break;
	}

	securityPage = new ctlSecurityPanel(nbNotebook, privList, privChar, mainForm->GetImageList());
	securityPage->SetConnection(object->GetConnection());
	this->Connect(EVT_SECURITYPANEL_CHANGE, wxCommandEventHandler(frmGrantWizard::OnChange));

	sqlPane = new ctlSQLBox(nbNotebook, CTL_PROPSQL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY | wxTE_RICH2);
	nbNotebook->AddPage(sqlPane, wxT("SQL"));

	txtMessages = new wxTextCtrl(nbNotebook, CTL_MSG, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
	nbNotebook->AddPage(txtMessages, _("Messages"));

	AddObjects((pgCollection *)object);


	if (securityPage->cbGroups)
	{
		pgSet *set = object->GetConnection()->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

		if (set)
		{
			while (!set->Eof())
			{
				securityPage->cbGroups->Append(wxT("group ") + set->GetVal(0));
				set->MoveNext();
			}
			delete set;
		}

		if (settings->GetShowUsersForPrivileges())
		{
			set = object->GetConnection()->ExecuteSet(wxT("SELECT usename FROM pg_user ORDER BY usename"));

			if (set)
			{
				securityPage->stGroup->SetLabel(_("Group/User"));

				while (!set->Eof())
				{
					securityPage->cbGroups->Append(set->GetVal(0));
					set->MoveNext();
				}
				delete set;
				Layout();
			}
		}
	}

	Layout();
	Show(true);

	// Work around a weird display bug in wx2.7
	this->Refresh();
}


wxString frmGrantWizard::GetSql()
{
	wxString sql;

	unsigned int i;
	for (i = 0 ; i < chkList->GetCount() ; i++)
	{
		if (chkList->IsChecked(i))
		{
			wxString tmp;

			pgObject *obj = (pgObject *)objectArray.Item(i);

			switch (obj->GetMetaType())
			{
				case PGM_FUNCTION:
				{
					if (((pgFunction *)obj)->GetIsProcedure())
					{
						tmp = securityPage->GetGrant(wxT("X"), wxT("PROCEDURE ")
						                             + obj->GetQuotedFullIdentifier() + wxT("(")
						                             + ((pgProcedure *)obj)->GetArgSigList() + wxT(")"));
					}
					else
					{
						tmp = securityPage->GetGrant(wxT("X"), wxT("FUNCTION ")
						                             + obj->GetQuotedFullIdentifier() + wxT("(")
						                             + ((pgFunction *)obj)->GetArgSigList() + wxT(")"));
					}
					break;
				}
				case PGM_VIEW:
					tmp = securityPage->GetGrant(wxT("arwdxt"), wxT("TABLE ") + obj->GetQuotedFullIdentifier());
					break;
				case PGM_SEQUENCE:
					tmp = securityPage->GetGrant(wxT("rwU"), wxT("SEQUENCE ") + obj->GetQuotedFullIdentifier());
					break;
				case GP_EXTTABLE:
					tmp = securityPage->GetGrant(wxT("r"), wxT("TABLE ") + obj->GetQuotedFullIdentifier());
				default:
					if (obj->GetTypeName().Upper() == wxT("EXTERNAL TABLE")) // somewhat of a hack
					{
						tmp = securityPage->GetGrant(wxT("r"), wxT("TABLE ") + obj->GetQuotedFullIdentifier());
					}
					else
						tmp = securityPage->GetGrant(wxT("arwdDxt"), obj->GetTypeName().Upper() + wxT(" ") + obj->GetQuotedFullIdentifier());
					break;
			}

			if (!tmp.IsEmpty())
				sql.Append(tmp);
		}
	}
	return sql;
}


grantWizardFactory::grantWizardFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Grant Wizard..."), _("Grants rights to multiple objects"));
}


wxWindow *grantWizardFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmGrantWizard *frm = new frmGrantWizard(form, obj);
	frm->Go();
	return 0;
}


bool grantWizardFactory::CheckEnable(pgObject *obj)
{
	if (obj)
	{
		switch (obj->GetMetaType())
		{
			case PGM_TABLE:
			case PGM_FUNCTION:
			case PGM_SEQUENCE:
			case PGM_VIEW:
			case GP_EXTTABLE:
				if (obj->IsCollection() && obj->GetSchema()->GetMetaType() != PGM_CATALOG)
					return obj->GetSchema()->CanEdit();
				break;

			case PGM_SCHEMA:
				return obj->CanEdit();
			default:
				break;
		}
	}
	return false;
}
