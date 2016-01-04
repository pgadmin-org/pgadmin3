//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgJob.cpp - PostgreSQL Job Property
//
//////////////////////////////////////////////////////////////////////////


// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "agent/dlgJob.h"
#include "agent/dlgStep.h"
#include "agent/dlgSchedule.h"
#include "agent/pgaJob.h"
#include "agent/pgaStep.h"
#include "agent/pgaSchedule.h"
#include "frm/frmMain.h"


// pointer to controls
#define	txtID				CTRL_TEXT("txtID")
#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define cbJobclass          CTRL_COMBOBOX("cbJobclass")
#define txtHostAgent        CTRL_TEXT("txtHostAgent")
#define txtCreated          CTRL_TEXT("txtCreated")
#define txtChanged          CTRL_TEXT("txtChanged")
#define txtNextrun          CTRL_TEXT("txtNextrun")
#define txtLastrun          CTRL_TEXT("txtLastrun")
#define txtLastresult       CTRL_TEXT("txtLastResult")

#define lstSteps            CTRL_LISTVIEW("lstSteps")
#define btnChangeStep       CTRL_BUTTON("btnChangeStep")
#define btnAddStep          CTRL_BUTTON("btnAddStep")
#define btnRemoveStep       CTRL_BUTTON("btnRemoveStep")

#define lstSchedules        CTRL_LISTVIEW("lstSchedules")
#define btnChangeSchedule   CTRL_BUTTON("btnChangeSchedule")
#define btnAddSchedule      CTRL_BUTTON("btnAddSchedule")
#define btnRemoveSchedule   CTRL_BUTTON("btnRemoveSchedule")


BEGIN_EVENT_TABLE(dlgJob, dlgAgentProperty)
	EVT_CHECKBOX(XRCID("chkEnabled"),               dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbJobclass"),               dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtHostAgent"),                 dlgProperty::OnChange)

	EVT_LIST_ITEM_SELECTED(XRCID("lstSteps"),       dlgJob::OnSelChangeStep)
	EVT_BUTTON(XRCID("btnChangeStep"),              dlgJob::OnChangeStep)
	EVT_BUTTON(XRCID("btnAddStep"),                 dlgJob::OnAddStep)
	EVT_BUTTON(XRCID("btnRemoveStep"),              dlgJob::OnRemoveStep)

	EVT_LIST_ITEM_SELECTED(XRCID("lstSchedules"),   dlgJob::OnSelChangeSchedule)
	EVT_BUTTON(XRCID("btnChangeSchedule"),          dlgJob::OnChangeSchedule)
	EVT_BUTTON(XRCID("btnAddSchedule"),             dlgJob::OnAddSchedule)
	EVT_BUTTON(XRCID("btnRemoveSchedule"),          dlgJob::OnRemoveSchedule)

#ifdef __WXMAC__
	EVT_SIZE(                                       dlgJob::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgProperty *pgaJobFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgJob(this, frame, (pgaJob *)node);
}


dlgJob::dlgJob(pgaFactory *f, frmMain *frame, pgaJob *node)
	: dlgAgentProperty(f, frame, wxT("dlgJob"))
{
	job = node;

	txtID->Disable();
	txtCreated->Disable();
	txtChanged->Disable();
	txtNextrun->Disable();
	txtLastrun->Disable();
	txtLastresult->Disable();
	lstSteps->CreateColumns(0, _("Step"), _("Comment"), 90);
	lstSteps->AddColumn(wxT("cmd"), 0);
	lstSteps->AddColumn(wxT("id"), 0);

	lstSchedules->CreateColumns(0, _("Schedule"), _("Comment"), 90);
	lstSchedules->AddColumn(wxT("cmd"), 0);
	lstSchedules->AddColumn(wxT("id"), 0);
	btnChangeStep->Disable();
	btnRemoveStep->Disable();
	btnChangeSchedule->Disable();
	btnRemoveSchedule->Disable();
}


pgObject *dlgJob::GetObject()
{
	return job;
}


#ifdef __WXMAC__
void dlgJob::OnChangeSize(wxSizeEvent &ev)
{
	lstSteps->SetSize(wxDefaultCoord, wxDefaultCoord,
	                  ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	lstSchedules->SetSize(wxDefaultCoord, wxDefaultCoord,
	                      ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


int dlgJob::Go(bool modal)
{
	int returncode;

	pgSet *jcl = connection->ExecuteSet(wxT("SELECT jclname FROM pgagent.pga_jobclass"));
	if (jcl)
	{
		while (!jcl->Eof())
		{
			cbJobclass->Append(jcl->GetVal(0));
			jcl->MoveNext();
		}
		delete jcl;
	}

	if (job)
	{
		// edit mode
		recId = job->GetRecId();
		txtID->SetValue(NumToStr(recId));
		cbJobclass->SetValue(job->GetJobclass());
		chkEnabled->SetValue(job->GetEnabled());
		txtHostAgent->SetValue(job->GetHostAgent());
		txtCreated->SetValue(DateToStr(job->GetCreated()));
		txtChanged->SetValue(DateToStr(job->GetChanged()));
		txtNextrun->SetValue(DateToStr(job->GetNextrun()));
		txtLastrun->SetValue(DateToStr(job->GetLastrun()));
		txtLastresult->SetValue(job->GetLastresult());

		wxCookieType cookie;
		pgObject *data = 0;

		wxTreeItemId item, stepsItem, schedulesItem;
		item = mainForm->GetBrowser()->GetFirstChild(job->GetId(), cookie);
		while (item)
		{
			data = mainForm->GetBrowser()->GetObject(item);
			if (data->GetMetaType() == PGM_STEP)
				stepsItem = item;
			else if (data->GetMetaType() == PGM_SCHEDULE)
				schedulesItem = item;

			item = mainForm->GetBrowser()->GetNextChild(job->GetId(), cookie);
		}

		if (stepsItem)
		{
			pgCollection *coll = (pgCollection *)data;
			// make sure all columns are appended
			coll->ShowTreeDetail(mainForm->GetBrowser());
			// this is the columns collection
			item = mainForm->GetBrowser()->GetFirstChild(stepsItem, cookie);

			// add columns
			while (item)
			{
				data = mainForm->GetBrowser()->GetObject(item);
				if (data->IsCreatedBy(stepFactory))
				{
					pgaStep *step = (pgaStep *)data;
					int pos = lstSteps->AppendItem(stepFactory.GetIconId(), step->GetName(), step->GetComment());
					lstSteps->SetItem(pos, 3, NumToStr((long)step));
					previousSteps.Add(NumToStr((long)step));
				}
				item = mainForm->GetBrowser()->GetNextChild(stepsItem, cookie);
			}
		}

		if (schedulesItem)
		{
			pgCollection *coll = (pgCollection *)data;
			// make sure all columns are appended
			coll->ShowTreeDetail(mainForm->GetBrowser());
			// this is the columns collection
			item = mainForm->GetBrowser()->GetFirstChild(schedulesItem, cookie);

			// add columns
			while (item)
			{
				data = mainForm->GetBrowser()->GetObject(item);
				if (data->IsCreatedBy(scheduleFactory))
				{
					pgaSchedule *schedule = (pgaSchedule *)data;
					int pos = lstSchedules->AppendItem(scheduleFactory.GetIconId(), schedule->GetName(), schedule->GetComment());
					lstSchedules->SetItem(pos, 3, NumToStr((long)schedule));
					previousSchedules.Add(NumToStr((long)schedule));
				}
				item = mainForm->GetBrowser()->GetNextChild(schedulesItem, cookie);
			}
		}
	}
	else
	{
		// create mode
		cbJobclass->SetSelection(0);
		btnChangeStep->Hide();
		btnChangeSchedule->Hide();
	}

	returncode = dlgProperty::Go(modal);

	SetSqlReadOnly(true);

	// This fixes a UI glitch on MacOS X
	// Because of the new layout code, the Columns pane doesn't size itself properly
	SetSize(GetSize().GetWidth() + 1, GetSize().GetHeight());
	SetSize(GetSize().GetWidth() - 1, GetSize().GetHeight());

	return returncode;
}


pgObject *dlgJob::CreateObject(pgCollection *collection)
{
	pgObject *obj = jobFactory.CreateObjects(collection, 0, wxT("   WHERE jobid=") + NumToStr(recId) + wxT("\n"));
	return obj;
}


void dlgJob::CheckChange()
{
	bool enable = true;
	wxString name = GetName();
	if (job)
	{
		enable  =  txtComment->GetValue() != job->GetComment()
		           || name != job->GetName()
		           || chkEnabled->GetValue() != job->GetEnabled()
		           || txtHostAgent->GetValue() != job->GetHostAgent();
		if (!enable)
		{
			enable = !GetUpdateSql().IsEmpty();
		}
	}

	if (statusBar)
		statusBar->SetStatusText(wxEmptyString);

	CheckValid(enable, !txtName->GetValue().IsEmpty(), _("Please specify name."));

	EnableOK(enable);
}


void dlgJob::OnChangeStep(wxCommandEvent &ev)
{
	long pos = lstSteps->GetSelection();
	pgaStep *obj = (pgaStep *) StrToLong(lstSteps->GetText(pos, 3));

	dlgStep step(&stepFactory, mainForm, obj, job);
	step.CenterOnParent();
	step.SetConnection(connection);

	if (step.Go(true) != wxID_CANCEL)
	{
		lstSteps->SetItem(pos, 0, step.GetName());
		lstSteps->SetItem(pos, 1, step.GetComment());

		if (lstSteps->GetText(pos, 3).IsEmpty())
		{
			wxString *stepSql = new wxString(step.GetInsertSql());
			lstSteps->SetItemData(pos, (long)stepSql);
		}
		else
		{
			wxString *stepSql = new wxString(step.GetUpdateSql());
			lstSteps->SetItemData(pos, (long)stepSql);
		}

		CheckChange();
	}
}


void dlgJob::OnSelChangeStep(wxListEvent &ev)
{
	btnChangeStep->Enable();
	btnRemoveStep->Enable();
}


void dlgJob::OnAddStep(wxCommandEvent &ev)
{
	dlgStep step(&stepFactory, mainForm, NULL, job);
	step.CenterOnParent();
	step.SetConnection(connection);
	if (step.Go(true) != wxID_CANCEL)
	{
		int pos = lstSteps->AppendItem(stepFactory.GetIconId(), step.GetName(), step.GetComment());
		wxString *stepSql = new wxString(step.GetInsertSql());
		lstSteps->SetItemData(pos, (long)stepSql);
		CheckChange();
	}
}


void dlgJob::OnRemoveStep(wxCommandEvent &ev)
{
	delete (wxString *)lstSteps->GetItemData(lstSteps->GetSelection());
	lstSteps->DeleteCurrentItem();

	btnChangeStep->Disable();
	btnRemoveStep->Disable();

	CheckChange();
}


void dlgJob::OnSelChangeSchedule(wxListEvent &ev)
{
	btnChangeSchedule->Enable();
	btnRemoveSchedule->Enable();
}


void dlgJob::OnChangeSchedule(wxCommandEvent &ev)
{
	long pos = lstSchedules->GetSelection();
	pgaSchedule *obj = (pgaSchedule *) StrToLong(lstSchedules->GetText(pos, 3));

	dlgSchedule schedule(&scheduleFactory, mainForm, obj, job);
	schedule.CenterOnParent();
	schedule.SetConnection(connection);

	if (schedule.Go(true) != wxID_CANCEL)
	{
		lstSchedules->SetItem(pos, 0, schedule.GetName());
		lstSchedules->SetItem(pos, 1, schedule.GetComment());

		if (lstSchedules->GetText(pos, 3).IsEmpty())
		{
			wxString *scheduleSql = new wxString(schedule.GetInsertSql());
			lstSchedules->SetItemData(pos, (long)scheduleSql);
		}
		else
		{
			wxString *scheduleSql = new wxString(schedule.GetUpdateSql());
			lstSchedules->SetItemData(pos, (long)scheduleSql);
		}

		CheckChange();
	}
}


void dlgJob::OnAddSchedule(wxCommandEvent &ev)
{
	dlgSchedule schedule(&scheduleFactory, mainForm, NULL, job);
	schedule.CenterOnParent();
	schedule.SetConnection(connection);
	if (schedule.Go(true) != wxID_CANCEL)
	{
		int pos = lstSchedules->AppendItem(scheduleFactory.GetIconId(), schedule.GetName(), schedule.GetComment());
		wxString *scheduleSql = new wxString(schedule.GetInsertSql());
		lstSchedules->SetItemData(pos, (long)scheduleSql);
		CheckChange();
	}
}


void dlgJob::OnRemoveSchedule(wxCommandEvent &ev)
{
	delete (wxString *)lstSchedules->GetItemData(lstSchedules->GetSelection());
	lstSchedules->DeleteCurrentItem();

	btnChangeSchedule->Disable();
	btnRemoveSchedule->Disable();

	CheckChange();
}


wxString dlgJob::GetInsertSql()
{
	wxString sql;

	if (!job)
	{
		sql = wxT("INSERT INTO pgagent.pga_job (jobid, jobjclid, jobname, jobdesc, jobenabled, jobhostagent)\n")
		      wxT("SELECT <JobId>, jcl.jclid, ") + qtDbString(GetName()) +
		      wxT(", ") + qtDbString(txtComment->GetValue()) + wxT(", ") + BoolToStr(chkEnabled->GetValue()) +
		      wxT(", ") + qtDbString(txtHostAgent->GetValue()) + wxT("\n")
		      wxT("  FROM pgagent.pga_jobclass jcl WHERE jclname=") + qtDbString(cbJobclass->GetValue()) + wxT(";\n");
	}
	return sql;
}


wxString dlgJob::GetUpdateSql()
{
	wxString sql, name;
	name = GetName();

	if (job)
	{
		// edit mode
		wxString vars;

		if (name != job->GetName())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jobname = ") + qtDbString(name));
		}
		if (cbJobclass->GetValue().Trim() != job->GetJobclass())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jobjclid= (SELECT jclid FROM pgagent.pga_jobclass WHERE jclname=") + qtDbString(cbJobclass->GetValue()) + wxT(")"));
		}
		if (chkEnabled->GetValue() != job->GetEnabled())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jobenabled = ") + BoolToStr(chkEnabled->GetValue()));
		}
		if (txtHostAgent->GetValue() != job->GetHostAgent())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jobhostagent = ") + qtDbString(txtHostAgent->GetValue()));
		}
		if (txtComment->GetValue() != job->GetComment())
		{
			if (!vars.IsEmpty())
				vars.Append(wxT(", "));
			vars.Append(wxT("jobdesc = ") + qtDbString(txtComment->GetValue()));
		}

		if (!vars.IsEmpty())
			sql = wxT("UPDATE pgagent.pga_job SET ") + vars + wxT("\n")
			      wxT(" WHERE jobid=") + NumToStr(recId) + wxT(";\n");

	}
	else
	{
		// create mode
		// done by GetInsertSql
	}

	int pos, index;

	wxArrayString tmpSteps = previousSteps;
	for (pos = 0 ; pos < lstSteps->GetItemCount() ; pos++)
	{
		wxString str = lstSteps->GetText(pos, 3);
		if (!str.IsEmpty())
		{
			index = tmpSteps.Index(str);
			if (index >= 0)
				tmpSteps.RemoveAt(index);
		}

		if(lstSteps->GetItemData(pos))
		{
			str = *(wxString *)lstSteps->GetItemData(pos);
			if (!str.IsEmpty())
				sql += str;
		}
	}

	for (index = 0 ; index < (int)tmpSteps.GetCount() ; index++)
	{
		sql += wxT("DELETE FROM pgagent.pga_jobstep WHERE jstid=")
		       + NumToStr(((pgaStep *)StrToLong(tmpSteps.Item(index)))->GetRecId()) + wxT(";\n");
	}

	wxArrayString tmpSchedules = previousSchedules;
	for (pos = 0 ; pos < lstSchedules->GetItemCount() ; pos++)
	{
		wxString str = lstSchedules->GetText(pos, 3);
		if (!str.IsEmpty())
		{
			index = tmpSchedules.Index(str);
			if (index >= 0)
				tmpSchedules.RemoveAt(index);
		}
		if(lstSchedules->GetItemData(pos))
		{
			str = *(wxString *)lstSchedules->GetItemData(pos);
			if (!str.IsEmpty())
				sql += str;
		}
	}

	for (index = 0 ; index < (int)tmpSchedules.GetCount() ; index++)
	{
		sql += wxT("DELETE FROM pgagent.pga_schedule WHERE jscid=")
		       + NumToStr(((pgaStep *)StrToLong(tmpSchedules.Item(index)))->GetRecId()) + wxT(";\n");
	}

	return sql;

}
