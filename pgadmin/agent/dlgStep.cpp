//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgStep.cpp - PostgreSQL Step Property
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "agent/dlgStep.h"
#include "agent/pgaStep.h"
#include "dlg/dlgSelectDatabase.h"
#include "schema/pgTable.h"


// pointer to controls
#define txtID               CTRL_TEXT("txtID")
#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define rbxKind             CTRL_RADIOBOX("rbxKind")
#define rbxOnError          CTRL_RADIOBOX("rbxOnError")
#define pnlDefinition       CTRL_PANEL("pnlDefinition")
#define txtSqlBox           CTRL_TEXT("txtSqlBox")
#define cbDatabase          CTRL_COMBOBOX2("cbDatabase")
#define txtConnStr          CTRL_TEXT("txtConnStr")
#define btnSelDatabase      CTRL_BUTTON("btnSelDatabase")
#define rbRemoteConn        CTRL_RADIOBUTTON("rbRemoteConn")
#define rbLocalConn         CTRL_RADIOBUTTON("rbLocalConn")

#define CTL_SQLBOX  188

BEGIN_EVENT_TABLE(dlgStep, dlgAgentProperty)
	EVT_CHECKBOX(XRCID("chkEnabled"),               dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbDatabase"),               dlgProperty::OnChange)
	EVT_RADIOBOX(XRCID("rbxKind"),                  dlgProperty::OnChange)
	EVT_RADIOBOX(XRCID("rbxOnError"),               dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtConnStr"),                   dlgProperty::OnChange)
	EVT_STC_MODIFIED(CTL_SQLBOX,                    dlgProperty::OnChangeStc)
	EVT_BUTTON(XRCID("btnSelDatabase"),             dlgStep::OnSelectDatabase)
	EVT_RADIOBUTTON(XRCID("rbRemoteConn"),          dlgStep::OnSelRemoteConn)
	EVT_RADIOBUTTON(XRCID("rbLocalConn"),           dlgStep::OnSelLocalConn)
END_EVENT_TABLE();


dlgProperty *pgaStepFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgStep(this, frame, (pgaStep *)node, (pgaJob *)parent);
}


dlgStep::dlgStep(pgaFactory *f, frmMain *frame, pgaStep *node, pgaJob *j)
	: dlgAgentProperty(f, frame, wxT("dlgStep"))
{
	step = node;
	job = j;
	if (job)
		jobId = job->GetRecId();
	else
		jobId = 0;

	sqlBox = new ctlSQLBox(pnlDefinition, CTL_SQLBOX, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);

	wxWindow *placeholder = CTRL_TEXT("txtSqlBox");
	wxSizer *sizer = placeholder->GetContainingSizer();
	sizer->Add(sqlBox, 1, wxRIGHT | wxGROW, 5);
	sizer->Detach(placeholder);
	delete placeholder;
	sizer->Layout();


	txtID->Disable();
}


pgObject *dlgStep::GetObject()
{
	return step;
}


int dlgStep::Go(bool modal)
{
	int returncode;

	hasConnStrSupport = connection->TableHasColumn(wxT("pgagent"), wxT("pga_jobstep"), wxT("jstconnstr"));
	cbDatabase->Append(wxT(" "));
	cbDatabase->SetSelection(0);

	pgSet *db = connection->ExecuteSet(wxT("SELECT datname FROM pg_database"));
	if (db)
	{
		while (!db->Eof())
		{
			cbDatabase->Append(db->GetVal(0));
			db->MoveNext();
		}
		delete db;
	}

	if (step)
	{
		// edit mode
		recId = step->GetRecId();
		txtID->SetValue(NumToStr(recId));

		if (step->HasConnectionString())
		{
			rbRemoteConn->SetValue(true);
			txtConnStr->Enable(true);
			txtConnStr->ChangeValue(step->GetConnStr());
			btnSelDatabase->Enable(true);
			cbDatabase->Enable(false);
		}
		else
		{
			rbLocalConn->SetValue(true);
			if (step->GetDbname().IsEmpty())
				cbDatabase->SetSelection(0);
			else
				cbDatabase->SetValue(step->GetDbname());
		}

		rbxKind->SetSelection(wxString(wxT("sb")).Find(step->GetKindChar()));
		rbxOnError->SetSelection(wxString(wxT("fsi")).Find(step->GetOnErrorChar()));
		sqlBox->SetText(step->GetCode());

		chkEnabled->SetValue(step->GetEnabled());
	}
	else
	{
		// create mode
		rbLocalConn->SetValue(true);
		cbDatabase->Enable(true);
		btnSelDatabase->Enable(false);
		txtConnStr->Enable(false);
		if (!hasConnStrSupport)
			rbLocalConn->Enable(false);
	}

	returncode = dlgProperty::Go(modal);

	SetSqlReadOnly(true);

	return returncode;
}


pgObject *dlgStep::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = stepFactory.CreateObjects(collection, 0, wxT("   AND jstid=") + NumToStr(recId) + wxT("\n"));
	return obj;
}


void dlgStep::CheckChange()
{
	wxString name = GetName();
	bool enable;
	if (step)
	{
		enable  =  name != step->GetName()
		           || chkEnabled->GetValue() != step->GetEnabled()
		           || rbxKind->GetSelection() != wxString(wxT("sb")).Find(step->GetKindChar())
		           || rbxOnError->GetSelection() != wxString(wxT("fsi")).Find(step->GetOnErrorChar())
		           || txtComment->GetValue() != step->GetComment()
		           || sqlBox->GetText() != step->GetCode();

		if (!enable && rbxKind->GetSelection() == 0)
		{
			if (hasConnStrSupport)
			{
				if (step->HasConnectionString())
				{
					if (rbRemoteConn->GetValue())
						enable = txtConnStr->GetValue().Trim() != step->GetConnStr();
					else
						enable = true;
				}
				else
				{
					if (rbRemoteConn->GetValue())
						enable = true;
					else
						enable = cbDatabase->GetValue().Trim() != step->GetDbname();
				}
			}
			else
			{
				enable = cbDatabase->GetValue().Trim() != step->GetDbname();
			}
		}
	}
	else
	{
		enable = true;
	}

	if (statusBar)
		statusBar->SetStatusText(wxEmptyString);

	CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
	CheckValid(enable, sqlBox->GetLength() > 0, _("Please specify code to execute."));

	// Disable/enable the database combo
	if (rbxKind->GetSelection() == 1)
	{
		rbRemoteConn->Enable(false);
		rbLocalConn->Enable(false);
		// I don't see any reason to make
		// the database combobox selection to 0
		//cbDatabase->SetSelection(0);
		txtConnStr->Enable(false);
		btnSelDatabase->Enable(false);
		cbDatabase->Enable(false);
	}
	else
	{
		if (hasConnStrSupport)
		{
			rbRemoteConn->Enable(true);
			rbLocalConn->Enable(true);
			if (rbRemoteConn->GetValue())
			{
				wxString validConnStr;

				btnSelDatabase->Enable(true);
				txtConnStr->Enable(true);
				cbDatabase->Enable(false);
				CheckValid(enable, !txtConnStr->GetValue().Trim().IsEmpty(), _("Please select a connection string."));
				CheckValid(enable, dlgSelectDatabase::getValidConnectionString(txtConnStr->GetValue().Trim(), validConnStr), _("Please enter a valid connection string"));
			}
			else
			{
				cbDatabase->Enable(true);
				btnSelDatabase->Enable(false);
				txtConnStr->Enable(false);
				CheckValid(enable, !cbDatabase->GetValue().Trim().IsEmpty(), _("Please select a database."));
			}
		}
		else
		{
			cbDatabase->Enable(true);
			// Make sure both radio buttons are disabled
			rbRemoteConn->Enable(false);
			rbLocalConn->Enable(false);
			CheckValid(enable, !cbDatabase->GetValue().Trim().IsEmpty(), _("Please select a database."));
		}
	}

	EnableOK(enable);
}



wxString dlgStep::GetComment()
{
	return txtComment->GetValue();
}



wxString dlgStep::GetInsertSql()
{
	wxString sql;

	if (!step)
	{
		wxString name = GetName();
		wxString kind = wxT("sb")[rbxKind->GetSelection()];
		wxString onerror = wxT("fsi")[rbxOnError->GetSelection()];
		wxString db, connstr;
		wxString jstjobid;
		if (jobId)
			jstjobid = NumToStr(jobId);
		else
			jstjobid = wxT("<JobId>");
		// SQL script expected
		if (rbxKind->GetSelection() == 0)
		{
			if (hasConnStrSupport && rbRemoteConn->GetValue())
			{
				connstr = qtDbString(txtConnStr->GetValue().Trim());
				db = wxT("''");
			}
			else
			{
				db = qtDbString(cbDatabase->GetValue().Trim());
				connstr = wxT("''");
			}
		}
		else
		{
			db = wxT("''");
			connstr = wxT("''");
		}

		sql = wxT("INSERT INTO pgagent.pga_jobstep (jstid, jstjobid, jstname, jstdesc, jstenabled, jstkind, jstonerror, jstcode, jstdbname");
		if (hasConnStrSupport)
			sql += wxT(", jstconnstr");
		sql += wxT(")\n ") \
		       wxT("SELECT <StpId>, ") + jstjobid + wxT(", ") + qtDbString(name) + wxT(", ") + qtDbString(txtComment->GetValue()) + wxT(", ")
		       + BoolToStr(chkEnabled->GetValue()) + wxT(", ") + qtDbString(kind) + wxT(", ")
		       + qtDbString(onerror) + wxT(", ") + qtDbString(sqlBox->GetText()) + wxT(", ") + db;
		if (hasConnStrSupport)
		{
			sql += wxT(", ") + connstr;
		}
		sql += wxT(";\n");
	}
	return sql;
}


wxString dlgStep::GetUpdateSql()
{
	wxString sql;

	if (step)
	{
		// edit mode
		wxString name = GetName();
		wxString kind = wxT("sb")[rbxKind->GetSelection()];

		wxString vars;
		if (name != step->GetName())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jstname=") + qtDbString(name));
		}
		if (chkEnabled->GetValue() != step->GetEnabled())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jstenabled=") + BoolToStr(chkEnabled->GetValue()));
		}
		if (hasConnStrSupport && kind == wxT("s"))
		{
			if (rbRemoteConn->GetValue())
			{
				if (step->HasConnectionString())
				{
					if (txtConnStr->GetValue().Trim() != step->GetConnStr())
					{
						if (!vars.IsEmpty())
							vars.Append(wxT(", "));
						vars.Append(wxT("jstconnstr=") + qtDbString(txtConnStr->GetValue().Trim()));
					}
				}
				else
				{
					if (!vars.IsEmpty())
						vars.Append(wxT(", "));
					vars.Append(wxT("jstconnstr=") + qtDbString(txtConnStr->GetValue().Trim()) + wxT(", "));
					vars.Append(wxT("jstdbname=''"));
				}
			}
			else
			{
				if (step->HasConnectionString())
				{
					if (!vars.IsEmpty())
						vars.Append(wxT(", "));
					vars.Append(wxT("jstdbname=") + qtDbString(cbDatabase->GetValue().Trim()) + wxT(", "));
					vars.Append(wxT("jstconnstr=''"));
				}
				else
				{
					if (cbDatabase->GetValue().Trim() != step->GetDbname())
					{
						if (!vars.IsEmpty())
							vars.Append(wxT(", "));
						vars.Append(wxT("jstdbname=") + qtDbString(cbDatabase->GetValue().Trim()));
					}
				}
			}
		}
		else if (kind == wxT("s"))
		{
			if (cbDatabase->GetValue().Trim() != step->GetDbname())
			{
				if (!vars.IsEmpty())
					vars.Append(wxT(", "));
				vars.Append(wxT("jstdbname=") + qtDbString(cbDatabase->GetValue().Trim()));
			}
		}
		else
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jstdbname=''"));
			if (hasConnStrSupport)
				vars.Append(wxT(", jstconnstr=''"));
		}
		if (rbxKind->GetSelection() != kind)
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jstkind=") + qtDbString(kind));
		}
		if (rbxOnError->GetSelection() != wxString(wxT("fsi")).Find(step->GetOnErrorChar()))
		{
			wxString onerror = wxT("fsi")[rbxOnError->GetSelection()];
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jstonerror='") + onerror + wxT("'"));
		}
		if (txtComment->GetValue() != step->GetComment())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jstdesc=") + qtDbString(txtComment->GetValue()));
		}
		if (sqlBox->GetText() != step->GetCode())
		{
			{
				if (!vars.IsEmpty())
					vars.Append(wxT(", "));
				vars.Append(wxT("jstcode=") + qtDbString(sqlBox->GetText()));
			}
		}

		if (!vars.IsEmpty())
			sql = wxT("UPDATE pgagent.pga_jobstep\n")
			      wxT("   SET ") + vars + wxT("\n")
			      wxT(" WHERE jstid=") + NumToStr(step->GetRecId()) +
			      wxT(";\n");
	}
	else
	{
		// create mode; handled by GetInsertSql()
	}
	return sql;
}

bool dlgStep::IsUpToDate()
{
	if (step && !step->IsUpToDate())
		return false;
	else
		return true;
}

void dlgStep::OnSelectDatabase(wxCommandEvent &ev)
{
	dlgSelectDatabase dlgSD(this, wxID_ANY);
	if (dlgSD.ShowModal() == wxID_OK)
	{
		wxString strConnStr = dlgSD.getConnInfo();
		if (!strConnStr.IsEmpty())
			txtConnStr->SetValue(strConnStr);
	}
}

void dlgStep::OnSelRemoteConn(wxCommandEvent &ev)
{
	if (rbRemoteConn->GetValue())
	{
		cbDatabase->Enable(false);
		btnSelDatabase->Enable(true);
		txtConnStr->Enable(true);
	}
	dlgProperty::OnChange(ev);
}

void dlgStep::OnSelLocalConn(wxCommandEvent &ev)
{
	if (rbLocalConn->GetValue())
	{
		cbDatabase->Enable(true);
		btnSelDatabase->Enable(false);
		txtConnStr->Enable(false);
	}
	dlgProperty::OnChange(ev);
}


