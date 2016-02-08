//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgView.cpp - PostgreSQL View Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "ctl/ctlSQLBox.h"
#include "dlg/dlgView.h"
#include "schema/pgView.h"
#include "schema/pgSchema.h"
#include "ctl/ctlSeclabelPanel.h"


// pointer to controls
#define pnlDefinition       CTRL_PANEL("pnlDefinition")
#define txtSqlBox           CTRL_SQLBOX("txtSqlBox")
#define chkSecurityBarrier  CTRL_CHECKBOX("chkSecurityBarrier")

/* AutoVacuum Settings */
#define nbVaccum            CTRL_NOTEBOOK("nbVacuum")
#define chkCustomVac        CTRL_CHECKBOX("chkCustomVac")
#define chkVacEnabled       CTRL_CHECKBOX("chkVacEnabled")
#define txtBaseVac          CTRL_TEXT("txtBaseVac")
#define stBaseVacCurr       CTRL_STATIC("stBaseVacCurr")
#define txtBaseAn           CTRL_TEXT("txtBaseAn")
#define stBaseAnCurr        CTRL_STATIC("stBaseAnCurr")
#define txtFactorVac        CTRL_TEXT("txtFactorVac")
#define stFactorVacCurr     CTRL_STATIC("stFactorVacCurr")
#define txtFactorAn         CTRL_TEXT("txtFactorAn")
#define stFactorAnCurr      CTRL_STATIC("stFactorAnCurr")
#define txtVacDelay         CTRL_TEXT("txtVacDelay")
#define stVacDelayCurr      CTRL_STATIC("stVacDelayCurr")
#define txtVacLimit         CTRL_TEXT("txtVacLimit")
#define stVacLimitCurr      CTRL_STATIC("stVacLimitCurr")
#define txtFreezeMinAge     CTRL_TEXT("txtFreezeMinAge")
#define stFreezeMinAgeCurr  CTRL_STATIC("stFreezeMinAgeCurr")
#define txtFreezeMaxAge     CTRL_TEXT("txtFreezeMaxAge")
#define stFreezeMaxAgeCurr  CTRL_STATIC("stFreezeMaxAgeCurr")
#define txtFreezeTableAge   CTRL_TEXT("txtFreezeTableAge")
#define stFreezeTableAgeCurr CTRL_STATIC("stFreezeTableAgeCurr")

/* TOAST TABLE AutoVacuum Settings */
#define chkCustomToastVac         CTRL_CHECKBOX("chkCustomToastVac")
#define chkToastVacEnabled        CTRL_CHECKBOX("chkToastVacEnabled")
#define txtBaseToastVac           CTRL_TEXT("txtBaseToastVac")
#define stBaseToastVacCurr        CTRL_STATIC("stBaseToastVacCurr")
#define txtFactorToastVac         CTRL_TEXT("txtFactorToastVac")
#define stFactorToastVacCurr      CTRL_STATIC("stFactorToastVacCurr")
#define txtToastVacDelay          CTRL_TEXT("txtToastVacDelay")
#define stToastVacDelayCurr       CTRL_STATIC("stToastVacDelayCurr")
#define txtToastVacLimit          CTRL_TEXT("txtToastVacLimit")
#define stToastVacLimitCurr       CTRL_STATIC("stToastVacLimitCurr")
#define txtToastFreezeMinAge      CTRL_TEXT("txtToastFreezeMinAge")
#define stToastFreezeMinAgeCurr   CTRL_STATIC("stToastFreezeMinAgeCurr")
#define txtToastFreezeMaxAge      CTRL_TEXT("txtToastFreezeMaxAge")
#define stToastFreezeMaxAgeCurr   CTRL_STATIC("stToastFreezeMaxAgeCurr")
#define txtToastFreezeTableAge    CTRL_TEXT("txtToastFreezeTableAge")
#define stToastFreezeTableAgeCurr CTRL_STATIC("stToastFreezeTableAgeCurr")

/* Materialized view Settings */
#define stMaterializedView        CTRL_STATIC("stMaterializedView")
#define chkMaterializedView       CTRL_CHECKBOX("chkMaterializedView")
#define stTableSpace              CTRL_STATIC("stTableSpace")
#define cboTablespace             CTRL_COMBOBOX("cboTablespace")
#define stFillFactor              CTRL_STATIC("stFillFactor")
#define txtFillFactor             CTRL_TEXT("txtFillFactor")
#define stMatViewWithData         CTRL_STATIC("stMatViewWithData")
#define chkMatViewWithData        CTRL_CHECKBOX("chkMatViewWithData")
#define stCheckOption             CTRL_STATIC("stCheckOption")
#define cbCheckOption             CTRL_COMBOBOX("cbCheckOption")

BEGIN_EVENT_TABLE(dlgView, dlgSecurityProperty)
	EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgProperty::OnChangeStc)
	EVT_CHECKBOX(XRCID("chkSecurityBarrier"),       dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbCheckOption"),            dlgProperty::OnChange)

	/* Materialized view setting */
	EVT_CHECKBOX(XRCID("chkMaterializedView"),      dlgView::OnCheckMaterializedView)
	EVT_TEXT(XRCID("txtFillFactor"),                dlgView::OnChangeVacuum)

	EVT_COMBOBOX(XRCID("cboTablespace"),            dlgView::OnChangeVacuum)

	EVT_CHECKBOX(XRCID("chkMatViewWithData"),       dlgProperty::OnChange)

	/* AutoVacuum Settings */
	EVT_CHECKBOX(XRCID("chkCustomVac"),             dlgView::OnChangeVacuum)
	EVT_CHECKBOX(XRCID("chkVacEnabled"),            dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtBaseVac"),                   dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtBaseAn"),                    dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFactorVac"),                 dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFactorAn"),                  dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtVacDelay"),                  dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtVacLimit"),                  dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFreezeMinAge"),              dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFreezeMaxAge"),              dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFreezeTableAge"),            dlgView::OnChangeVacuum)

	/* TOAST TABLE AutoVacuum Settings */
	EVT_CHECKBOX(XRCID("chkCustomToastVac"),            dlgView::OnChangeVacuum)
	EVT_CHECKBOX(XRCID("chkToastVacEnabled"),           dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtBaseToastVac"),                  dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFactorToastVac"),                dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastVacDelay"),                 dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastVacLimit"),                 dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastFreezeMinAge"),             dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastFreezeMaxAge"),             dlgView::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastFreezeTableAge"),           dlgView::OnChangeVacuum)

END_EVENT_TABLE();


dlgProperty *pgViewFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgView(this, frame, (pgView *)node, (pgSchema *)parent);
}

dlgView::dlgView(pgaFactory *f, frmMain *frame, pgView *node, pgSchema *sch)
	: dlgSecurityProperty(f, frame, node, wxT("dlgView"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
	schema = sch;
	view = node;
	forceSecurityBarrierChanged = false;

	seclabelPage = new ctlSeclabelPanel(nbNotebook);
}


pgObject *dlgView::GetObject()
{
	return view;
}


int dlgView::Go(bool modal)
{
	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(view);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgView::OnChange));
	}
	else
		seclabelPage->Disable();

	chkSecurityBarrier->Enable(connection->BackendMinimumVersion(9, 2));
	cbCheckOption->Enable(connection->BackendMinimumVersion(9, 4));

	if (connection->BackendMinimumVersion(9, 3))
	{
		PrepareTablespace(cboTablespace);
	}

	if (view)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(8, 1));
		oldDefinition = view->GetFormattedDefinition();
		txtSqlBox->SetText(oldDefinition);
		chkSecurityBarrier->SetValue(view->GetSecurityBarrier() == wxT("true"));

		if ((connection->BackendMinimumVersion(9, 3) && view))
		{
			// If it is materialized view than edit it
			if (view->GetMaterializedView())
			{
				// Checked the view as user is in edit materialized view mode
				chkMaterializedView->SetValue(true);

				// Disable the security barrier as user is editing the materailized view
				chkSecurityBarrier->Disable();

				// Disable the check-option as user is editing the materailized view
				cbCheckOption->Disable();

				// Disable the materialized view as user is editing it and not allowed to switch to other view
				chkMaterializedView->Disable();

				if (view->GetTablespaceOid() != 0)
					cboTablespace->SetKey(view->GetTablespaceOid());

				txtFillFactor->SetValue(view->GetFillFactor());

				if (view->GetIsPopulated().Cmp(wxT("t")) == 0)
					chkMatViewWithData->SetValue(true);
				else
					chkMatViewWithData->SetValue(false);

				settingAutoVacuum = false;

				pgSetIterator avSet(connection,
				                    wxT("SELECT name, setting FROM pg_settings WHERE name like '%vacuum%' ORDER BY name"));
				while (avSet.RowsLeft())
				{
					wxString name = avSet.GetVal(wxT("name"));
					wxString setting = avSet.GetVal(wxT("setting"));

					if (name == wxT("autovacuum_vacuum_cost_delay"))
						settingCostDelay = setting;
					else if (name == wxT("vacuum_cost_delay"))
					{
						if (StrToLong(settingCostDelay) < 0)
							settingCostDelay = setting;
					}
					else if (name == wxT("autovacuum_vacuum_cost_limit"))
						settingCostLimit = setting;
					else if (name == wxT("vacuum_cost_limit"))
					{
						if (StrToLong(settingCostLimit) < 0)
							settingCostLimit = setting;
					}
					else if (name == wxT("autovacuum_vacuum_scale_factor"))
						settingVacFactor = setting;
					else if (name == wxT("autovacuum_analyze_scale_factor"))
						settingAnlFactor = setting;
					else if (name == wxT("autovacuum_vacuum_threshold"))
						settingVacBaseThr = setting;
					else if (name == wxT("autovacuum_analyze_threshold"))
						settingAnlBaseThr = setting;
					else if (name == wxT("vacuum_freeze_min_age"))
						settingFreezeMinAge = setting;
					else if (name == wxT("autovacuum_freeze_max_age"))
						settingFreezeMaxAge = setting;
					else if (name == wxT("vacuum_freeze_table_age"))
						settingFreezeTableAge = setting;
					else
						settingAutoVacuum = avSet.GetBool(wxT("setting"));
				}

				tableVacBaseThr = wxT("-1");
				tableAnlBaseThr = wxT("-1");
				tableCostDelay = wxT("-1");
				tableCostLimit = wxT("-1");
				tableFreezeMinAge = wxT("-1");
				tableFreezeMaxAge = wxT("-1");
				tableVacFactor = wxT("-1");
				tableAnlFactor = wxT("-1");
				tableFreezeTableAge = wxT("-1");

				toastTableVacBaseThr = wxT("-1");
				toastTableCostDelay = wxT("-1");
				toastTableCostLimit = wxT("-1");
				toastTableFreezeMinAge = wxT("-1");
				toastTableFreezeMaxAge = wxT("-1");
				toastTableVacFactor = wxT("-1");
				toastTableFreezeTableAge = wxT("-1");

				toastTableHasVacuum = false;
				toastTableVacEnabled = false;

				if (view)
				{
					if (view->GetAutoVacuumEnabled() == 2)
						tableVacEnabled = settingAutoVacuum;
					else
						tableVacEnabled = view->GetAutoVacuumEnabled() == 1;
					if (!view->GetAutoVacuumVacuumThreshold().IsEmpty())
						tableVacBaseThr = view->GetAutoVacuumVacuumThreshold();
					if (!view->GetAutoVacuumAnalyzeThreshold().IsEmpty())
						tableAnlBaseThr = view->GetAutoVacuumAnalyzeThreshold();
					if (!view->GetAutoVacuumVacuumScaleFactor().IsEmpty())
						tableVacFactor = view->GetAutoVacuumVacuumScaleFactor();
					if (!view->GetAutoVacuumAnalyzeScaleFactor().IsEmpty())
						tableAnlFactor = view->GetAutoVacuumAnalyzeScaleFactor();
					if (!view->GetAutoVacuumVacuumCostDelay().IsEmpty())
						tableCostDelay = view->GetAutoVacuumVacuumCostDelay();
					if (!view->GetAutoVacuumVacuumCostLimit().IsEmpty())
						tableCostLimit = view->GetAutoVacuumVacuumCostLimit();
					if (!view->GetAutoVacuumFreezeMinAge().IsEmpty())
						tableFreezeMinAge = view->GetAutoVacuumFreezeMinAge();
					if (!view->GetAutoVacuumFreezeMaxAge().IsEmpty())
						tableFreezeMaxAge = view->GetAutoVacuumFreezeMaxAge();
					if (!view->GetAutoVacuumFreezeTableAge().IsEmpty())
						tableFreezeTableAge = view->GetAutoVacuumFreezeTableAge();

					hasVacuum = view->GetCustomAutoVacuumEnabled();
					chkVacEnabled->SetValue(hasVacuum ? tableVacEnabled : settingAutoVacuum);

					toastTableVacEnabled = false;

					if (!view->GetHasToastTable())
					{
						nbVaccum->GetPage(2)->Enable(false);
					}
					else
					{
						toastTableHasVacuum = view->GetToastCustomAutoVacuumEnabled();
						if (toastTableHasVacuum)
						{
							if (view->GetToastAutoVacuumEnabled() == 2)
								toastTableVacEnabled = settingAutoVacuum;
							else
								toastTableVacEnabled  = view->GetToastAutoVacuumEnabled() == 1;
							if (!view->GetToastAutoVacuumVacuumThreshold().IsEmpty())
								toastTableVacBaseThr = view->GetToastAutoVacuumVacuumThreshold();
							if (!view->GetToastAutoVacuumVacuumScaleFactor().IsEmpty())
								toastTableVacFactor = view->GetToastAutoVacuumVacuumScaleFactor();
							if (!view->GetToastAutoVacuumVacuumCostDelay().IsEmpty())
								toastTableCostDelay = view->GetToastAutoVacuumVacuumCostDelay();
							if (!view->GetToastAutoVacuumVacuumCostLimit().IsEmpty())
								toastTableCostLimit = view->GetToastAutoVacuumVacuumCostLimit();
							if (!view->GetToastAutoVacuumFreezeMinAge().IsEmpty())
								toastTableFreezeMinAge = view->GetToastAutoVacuumFreezeMinAge();
							if (!view->GetToastAutoVacuumFreezeMaxAge().IsEmpty())
								toastTableFreezeMaxAge = view->GetToastAutoVacuumFreezeMaxAge();
							if (!view->GetToastAutoVacuumFreezeTableAge().IsEmpty())
								toastTableFreezeTableAge = view->GetToastAutoVacuumFreezeTableAge();
						}
						chkToastVacEnabled->SetValue(toastTableHasVacuum ? toastTableVacEnabled : settingAutoVacuum);
					}
				}
				else
				{
					hasVacuum = false;
					chkVacEnabled->SetValue(settingAutoVacuum);
				}

				txtBaseVac->SetValue(tableVacBaseThr);
				txtBaseAn->SetValue(tableAnlBaseThr);
				txtFactorVac->SetValue(tableVacFactor);
				txtFactorAn->SetValue(tableAnlFactor);
				txtVacDelay->SetValue(tableCostDelay);
				txtVacLimit->SetValue(tableCostLimit);
				txtFreezeMinAge->SetValue(tableFreezeMinAge);
				txtFreezeMaxAge->SetValue(tableFreezeMaxAge);

				txtFreezeTableAge->SetValue(tableFreezeTableAge);
				txtBaseToastVac->SetValue(toastTableVacBaseThr);
				txtFactorToastVac->SetValue(toastTableVacFactor);
				txtToastVacDelay->SetValue(toastTableCostDelay);
				txtToastVacLimit->SetValue(toastTableCostLimit);
				txtToastFreezeMinAge->SetValue(toastTableFreezeMinAge);
				txtToastFreezeMaxAge->SetValue(toastTableFreezeMaxAge);
				txtToastFreezeTableAge->SetValue(toastTableFreezeTableAge);

				chkCustomToastVac->SetValue(toastTableHasVacuum);
				chkToastVacEnabled->SetValue(toastTableHasVacuum ? toastTableVacEnabled : settingAutoVacuum);

				chkCustomVac->SetValue(hasVacuum);
				wxCommandEvent ev;
				OnChangeVacuum(ev);
			}
			else
			{
				// It is not materialized view so disabling all the controls
				DisableMaterializedView();
			}

			cbCheckOption->SetSelection(GetIndexCheckOption(view->GetCheckOption()));
		}
	}
	else
	{
		// create mode
		cboTablespace->Insert(_("<default tablespace>"), 0, (void *)0);
		cboTablespace->SetSelection(0);
		cbCheckOption->SetSelection(0);
		wxCommandEvent ev;
		OnChangeVacuum(ev);
	}

	// Find, and disable the RULE ACL option if we're 8.2
	if (connection->BackendMinimumVersion(8, 2))
	{
		// Disable the checkbox
		if (!DisablePrivilege(wxT("RULE")))
		{
			wxLogError(_("Failed to disable the RULE privilege checkbox!"));
		}
	}

	return dlgSecurityProperty::Go(modal);
}


pgObject *dlgView::CreateObject(pgCollection *collection)
{
	pgObject *obj = viewFactory.CreateObjects(collection, 0,
	                wxT("\n   AND c.relname=") + qtDbString(txtName->GetValue()) +
	                wxT("\n   AND c.relnamespace=") + schema->GetOidStr());
	return obj;
}


void dlgView::CheckChange()
{
	bool enable = true;
	wxString name = GetName();

	CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
	CheckValid(enable, txtSqlBox->GetText().Trim(true).Trim(false).Length() > 0 , _("Please enter function definition."));

	if (!connection->BackendMinimumVersion(9, 3))
	{
		DisableMaterializedView();
	}

	if(enable)
	{
		if (view)
			enable = txtComment->GetValue() != view->GetComment()
			         || txtSqlBox->GetText().Trim(true).Trim(false) != oldDefinition.Trim(true).Trim(false)
			         || cbOwner->GetValue() != view->GetOwner()
			         || cbSchema->GetValue() != view->GetSchema()->GetName()
			         || name != view->GetName();

		if (connection->BackendMinimumVersion(9, 3))
		{
			enable = !GetSql().IsEmpty();
		}

		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());

		if (connection->BackendMinimumVersion(9, 2))
		{
			if (view)
			{
				if (chkSecurityBarrier->GetValue())
					enable = enable || !(view->GetSecurityBarrier() == wxT("true"));
				else
					enable = enable || (view->GetSecurityBarrier() == wxT("true"));
			}
			else
			{
				enable = enable || (chkSecurityBarrier->GetValue());
			}
		}

		if (connection->BackendMinimumVersion(9, 4) && view)
		{
			enable = enable || (cbCheckOption->GetSelection() != GetIndexCheckOption(view->GetCheckOption()));
		}
	}

	EnableOK(enable);
}


wxString dlgView::GetSql()
{
	wxString sql;
	wxString name;
	wxString withoptions = wxEmptyString;
	bool editQuery = false;

	if (view)
	{
		// edit mode
		name = GetName();

		if (name != view->GetName())
		{
			if (connection->BackendMinimumVersion(8, 3))
			{
				if (connection->BackendMinimumVersion(9, 3))
				{
					if (view->GetMaterializedView())
					{
						AppendNameChange(sql, wxT("MATERIALIZED VIEW ") + view->GetQuotedFullIdentifier());
						editQuery = true;
					}
					else
						AppendNameChange(sql, wxT("VIEW ") + view->GetQuotedFullIdentifier());
				}
				else
					AppendNameChange(sql, wxT("VIEW ") + view->GetQuotedFullIdentifier());
			}
			else
				AppendNameChange(sql, wxT("TABLE ") + view->GetQuotedFullIdentifier());
		}

		if (connection->BackendMinimumVersion(8, 4) && cbSchema->GetName() != view->GetSchema()->GetName())
		{
			if (connection->BackendMinimumVersion(9, 3))
			{
				if (view->GetMaterializedView())
				{
					AppendSchemaChange(sql, wxT("MATERIALIZED VIEW " + qtIdent(view->GetSchema()->GetName()) + wxT(".") + qtIdent(name)));
					editQuery = true;
				}
				else
					AppendSchemaChange(sql, wxT("VIEW " + qtIdent(view->GetSchema()->GetName()) + wxT(".") + qtIdent(name)));
			}
			else
				AppendSchemaChange(sql, wxT("VIEW " + qtIdent(view->GetSchema()->GetName()) + wxT(".") + qtIdent(name)));
		}
		else
			AppendSchemaChange(sql, wxT("TABLE " + qtIdent(view->GetSchema()->GetName()) + wxT(".") + qtIdent(name)));
	}

	name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());
	if (!view || txtSqlBox->GetText().Trim(true).Trim(false) != oldDefinition.Trim(true).Trim(false))
	{
		if (editQuery)
		{
			// Delete the materialized view query
			sql += wxT("DROP MATERIALIZED VIEW ") + name + wxT(";");
		}

		// Check if user creates the materialized view
		if (!chkMaterializedView->GetValue())
		{
			sql += wxT("CREATE OR REPLACE VIEW ") + name;

			if (connection->BackendMinimumVersion(9, 2) && chkSecurityBarrier->GetValue())
				withoptions += wxT("security_barrier=true");
			if (connection->BackendMinimumVersion(9, 4) && cbCheckOption->GetSelection() > 0)
			{
				if (withoptions.Length() > 0)
					withoptions += wxT(", ");
				if (cbCheckOption->GetSelection() == 1)
					withoptions += wxT("check_option=local");
				if (cbCheckOption->GetSelection() == 2)
					withoptions += wxT("check_option=cascaded");
			}

			if (withoptions.Length() > 0)
				sql += wxT(" WITH (") + withoptions + wxT(")");

			sql += wxT(" AS\n")
			       + txtSqlBox->GetText().Trim(true).Trim(false)
			       + wxT(";\n");
		}
		else if (connection->BackendMinimumVersion(9, 3) && chkMaterializedView->GetValue())
		{
			sql += wxT("CREATE MATERIALIZED VIEW ") + name;

			// Add the parameter of tablespace and storage parameter to create the materilized view
			if (txtFillFactor->GetValue().Trim().Length() > 0 || chkVacEnabled->GetValue() == true || chkToastVacEnabled->GetValue() == true)
			{
				bool fillFactorFlag, toastTableFlag;
				fillFactorFlag = false;
				toastTableFlag = false;

				sql += wxT("\nWITH (");

				if (txtFillFactor->GetValue().Trim().Length() > 0)
				{
					sql += wxT("\n  FILLFACTOR = ") + txtFillFactor->GetValue();
					fillFactorFlag = true;
				}

				bool valChanged = false;
				wxString newVal;
				wxString resetStr;

				if (connection->BackendMinimumVersion(9, 3) && chkCustomVac->GetValue())
				{
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_enabled"), BoolToStr(chkVacEnabled->GetValue()));

					if (!fillFactorFlag)
					{
						int position = sql.Find(',', true);
						if (position != wxNOT_FOUND)
							sql.Remove(position, 1);
						toastTableFlag = true;
					}

					newVal =  AppendNum(valChanged, txtBaseVac, tableVacBaseThr);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_threshold"), newVal);
					}

					newVal = AppendNum(valChanged, txtBaseAn, tableAnlBaseThr);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_analyze_threshold"), newVal);
					}

					newVal = AppendNum(valChanged, txtFactorVac, tableVacFactor);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_scale_factor"), newVal);
					}

					newVal = AppendNum(valChanged, txtFactorAn, tableAnlFactor);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_analyze_scale_factor"), newVal);
					}

					newVal = AppendNum(valChanged, txtVacDelay, tableCostDelay);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_cost_delay"), newVal);
					}

					newVal = AppendNum(valChanged, txtVacLimit, tableCostLimit);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_cost_limit"), newVal);
					}

					newVal = AppendNum(valChanged, txtFreezeMinAge, tableFreezeMinAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_freeze_min_age"), newVal);
					}

					newVal = AppendNum(valChanged, txtFreezeMaxAge, tableFreezeMaxAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_freeze_max_age"), newVal);
					}

					newVal = AppendNum(valChanged, txtFreezeTableAge, tableFreezeTableAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_freeze_table_age"), newVal);
					}
				}

				if (connection->BackendMinimumVersion(9, 3) && chkCustomToastVac->GetValue())
				{
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_enabled"), BoolToStr(chkToastVacEnabled->GetValue()));

					if (!fillFactorFlag && !toastTableFlag)
					{
						int position = sql.Find(',', true);
						if (position != wxNOT_FOUND)
							sql.Remove(position, 1);
					}

					newVal =  AppendNum(valChanged, txtBaseToastVac, toastTableVacBaseThr);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_threshold"), newVal);
					}

					newVal = AppendNum(valChanged, txtFactorToastVac, toastTableVacFactor);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_scale_factor"), newVal);
					}

					newVal = AppendNum(valChanged, txtToastVacDelay, toastTableCostDelay);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_cost_delay"), newVal);
					}

					newVal = AppendNum(valChanged, txtToastVacLimit, toastTableCostLimit);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_cost_limit"), newVal);
					}

					newVal = AppendNum(valChanged, txtToastFreezeMinAge, toastTableFreezeMinAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_freeze_min_age"), newVal);
					}

					newVal = AppendNum(valChanged, txtToastFreezeMaxAge, toastTableFreezeMaxAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_freeze_max_age"), newVal);
					}

					newVal = AppendNum(valChanged, txtToastFreezeTableAge, toastTableFreezeTableAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_freeze_table_age"), newVal);
					}
				}

				sql += wxT("\n)\n");
			}

			if (cboTablespace->GetCurrentSelection() > 0 && cboTablespace->GetOIDKey() > 0)
				sql += wxT("\nTABLESPACE ") + qtIdent(cboTablespace->GetValue());

			wxString sqlDefinition;
			bool tmpLoopFlag = true;
			sqlDefinition = txtSqlBox->GetText().Trim(true).Trim(false);

			// Remove semicolon from the end of the string
			while(tmpLoopFlag)
			{
				int length = sqlDefinition.Len();
				int position = sqlDefinition.Find(';', true);
				if ((position != wxNOT_FOUND) && (position = (length - 1)))
					sqlDefinition.Remove(position, 1);
				else
					tmpLoopFlag = false;
			}

			sql += wxT(" AS\n")
			       + sqlDefinition;

			if (chkMatViewWithData->GetValue())
				sql += wxT("\n WITH DATA;\n");
			else
				sql += wxT("\n WITH NO DATA;\n");
		}
	}
	else if (view)
	{
		if (!chkMaterializedView->GetValue())
		{
			if (connection->BackendMinimumVersion(9, 2))
			{
				if (chkSecurityBarrier->GetValue() && view->GetSecurityBarrier() != wxT("true"))
					sql += wxT("ALTER VIEW ") + name + wxT("\n  SET (security_barrier=true);\n");
				else if (!chkSecurityBarrier->GetValue() && view->GetSecurityBarrier() == wxT("true"))
					sql += wxT("ALTER VIEW ") + name + wxT("\n  SET (security_barrier=false);\n");
			}

			if (connection->BackendMinimumVersion(9, 4)
			        && cbCheckOption->GetSelection() != GetIndexCheckOption(view->GetCheckOption()) )
			{
				if ((cbCheckOption->GetSelection()) == 0)
					sql += wxT("ALTER VIEW ") + name + wxT(" RESET (check_option);\n");
				else
				{
					if (cbCheckOption->GetSelection() == 1)
						sql += wxT("ALTER VIEW ") + name + wxT("\n  SET (check_option=local") + wxT(");\n");
					if (cbCheckOption->GetSelection() == 2)
						sql += wxT("ALTER VIEW ") + name + wxT("\n  SET (check_option=cascaded") + wxT(");\n");
				}
			}

			if (withoptions.Length() > 0)
				sql += wxT(" WITH (") + withoptions + wxT(")");
		}
		else if (connection->BackendMinimumVersion(9, 3) && chkMaterializedView->GetValue())
		{
			if (txtFillFactor->GetValue() != view->GetFillFactor())
			{
				// If fill factor value get changed then set the new value
				if (txtFillFactor->GetValue().Trim().Length() > 0)
				{
					sql += wxT("ALTER MATERIALIZED VIEW ") + name
					       +  wxT("\n  SET (FILLFACTOR=")
					       +  txtFillFactor->GetValue() + wxT(");\n");
				}
				else
				{
					// If fill factor value get changed and value is not blank then do the reset
					sql += wxT("ALTER MATERIALIZED VIEW ") + name
					       +  wxT(" RESET(\n")
					       wxT("  FILLFACTOR\n")
					       wxT(");\n");
				}
			}

			bool isPopulatedFlag = false;

			if (view->GetIsPopulated().Cmp(wxT("t")) == 0)
				isPopulatedFlag = true;

			if (chkMatViewWithData->GetValue() != isPopulatedFlag)
			{
				// If checkbox is checked then set WITH NO DATA
				if (isPopulatedFlag)
				{
					sql += wxT("REFRESH MATERIALIZED VIEW ") + name
					       +  wxT(" WITH NO DATA;\n");
				}
				else
				{
					sql += wxT("REFRESH MATERIALIZED VIEW ") + name
					       +  wxT(" WITH DATA;\n");
				}
			}

			// Altered the storage parameters for the materialized view?
			if (!chkCustomVac->GetValue())
			{
				if (hasVacuum)
				{
					sql += wxT("ALTER MATERIALIZED VIEW ") + name
					       +  wxT(" RESET(\n")
					       wxT("  autovacuum_enabled,\n")
					       wxT("  autovacuum_vacuum_threshold,\n")
					       wxT("  autovacuum_analyze_threshold,\n")
					       wxT("  autovacuum_vacuum_scale_factor,\n")
					       wxT("  autovacuum_analyze_scale_factor,\n")
					       wxT("  autovacuum_vacuum_cost_delay,\n")
					       wxT("  autovacuum_vacuum_cost_limit,\n")
					       wxT("  autovacuum_freeze_min_age,\n")
					       wxT("  autovacuum_freeze_max_age,\n")
					       wxT("  autovacuum_freeze_table_age\n")
					       wxT(");\n");
				}
			}
			else
			{
				wxString vacStr;
				bool changed = (chkVacEnabled->GetValue() != tableVacEnabled);

				bool valChanged = false;
				wxString newVal;
				wxString setStr;
				wxString resetStr;

				if (changed)
				{
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_enabled"), BoolToStr(chkVacEnabled->GetValue()));
				}
				newVal =  AppendNum(valChanged, txtBaseVac, tableVacBaseThr);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_threshold"), newVal);
				}

				newVal = AppendNum(valChanged, txtBaseAn, tableAnlBaseThr);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_analyze_threshold"), newVal);
				}

				newVal = AppendNum(valChanged, txtFactorVac, tableVacFactor);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_scale_factor"), newVal);
				}

				newVal = AppendNum(valChanged, txtFactorAn, tableAnlFactor);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_analyze_scale_factor"), newVal);
				}

				newVal = AppendNum(valChanged, txtVacDelay, tableCostDelay);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_cost_delay"), newVal);
				}

				newVal = AppendNum(valChanged, txtVacLimit, tableCostLimit);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_cost_limit"), newVal);
				}

				newVal = AppendNum(valChanged, txtFreezeMinAge, tableFreezeMinAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_freeze_min_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtFreezeMaxAge, tableFreezeMaxAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_freeze_max_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtFreezeTableAge, tableFreezeTableAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_freeze_table_age"), newVal);
				}

				if (!setStr.IsEmpty())
				{
					vacStr = wxT("ALTER MATERIALIZED VIEW ") + name + setStr + wxT("\n);\n");;
					changed = true;
				}
				if (!resetStr.IsEmpty())
				{
					vacStr += wxT("ALTER MATERIALIZED VIEW ") + name + resetStr + wxT("\n);\n");;
					changed = true;
				}
				if (changed)
					sql += vacStr;
			}

			if (!chkCustomToastVac->GetValue())
			{
				if (toastTableHasVacuum)
				{
					sql += wxT("ALTER MATERIALIZED VIEW ") + name
					       +  wxT(" RESET(\n")
					       wxT("  toast.autovacuum_enabled,\n")
					       wxT("  toast.autovacuum_vacuum_threshold,\n")
					       wxT("  toast.autovacuum_analyze_threshold,\n")
					       wxT("  toast.autovacuum_vacuum_scale_factor,\n")
					       wxT("  toast.autovacuum_analyze_scale_factor,\n")
					       wxT("  toast.autovacuum_vacuum_cost_delay,\n")
					       wxT("  toast.autovacuum_vacuum_cost_limit,\n")
					       wxT("  toast.autovacuum_freeze_min_age,\n")
					       wxT("  toast.autovacuum_freeze_max_age,\n")
					       wxT("  toast.autovacuum_freeze_table_age\n")
					       wxT(");\n");
				}
			}
			else
			{
				wxString vacStr;
				bool changed = (chkToastVacEnabled->GetValue() != toastTableVacEnabled);
				bool valChanged = false;
				wxString newVal;
				wxString setStr;
				wxString resetStr;
				if (changed)
				{
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_enabled"), BoolToStr(chkToastVacEnabled->GetValue()));
				}
				newVal =  AppendNum(valChanged, txtBaseToastVac, toastTableVacBaseThr);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_threshold"), newVal);
				}

				newVal = AppendNum(valChanged, txtFactorToastVac, toastTableVacFactor);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_scale_factor"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastVacDelay, toastTableCostDelay);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_cost_delay"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastVacLimit, toastTableCostLimit);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_cost_limit"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeMinAge, toastTableFreezeMinAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_freeze_min_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeMaxAge, toastTableFreezeMaxAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_freeze_max_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeTableAge, toastTableFreezeTableAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_freeze_table_age"), newVal);
				}

				if (!setStr.IsEmpty())
				{
					vacStr = wxT("ALTER MATERIALIZED VIEW ") + name + setStr + wxT("\n);\n");
					changed = true;
				}
				if (!resetStr.IsEmpty())
				{
					vacStr += wxT("ALTER MATERIALIZED VIEW ") + name + resetStr + wxT("\n);\n");
					changed = true;
				}
				if (changed)
					sql += vacStr;
			}

			if (cboTablespace->GetOIDKey() != view->GetTablespaceOid())
			{
				sql += wxT("ALTER MATERIALIZED VIEW ") + name
				       +  wxT("\n  SET TABLESPACE ") + qtIdent(cboTablespace->GetValue())
				       + wxT(";\n");
			}
		}
	}

	if (view)
		AppendOwnerChange(sql, wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));
	else
		AppendOwnerNew(sql, wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));


	sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	if (connection->BackendMinimumVersion(9, 3) && chkMaterializedView->GetValue())
		AppendComment(sql, wxT("MATERIALIZED VIEW ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), view);
	else
		AppendComment(sql, wxT("VIEW ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), view);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("VIEW"), qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	return sql;
}

bool dlgView::IsUpToDate()
{
	if (view && !view->IsUpToDate())
		return false;
	else
		return true;
}

void dlgView::OnChange(wxCommandEvent &event)
{
	CheckChange();
}

void dlgView::OnCheckMaterializedView(wxCommandEvent &ev)
{
	if (chkMaterializedView->GetValue())
	{
		// Security barrier and Check-option, are not applicable to the materialized view
		cbCheckOption->Disable();
		if (chkSecurityBarrier->GetValue())
		{
			wxMessageBox(_("The security barrier option is not applicable to materialized views and has been turned off."), _("View"), wxICON_EXCLAMATION | wxOK, this);
			chkSecurityBarrier->SetValue(false);
			chkSecurityBarrier->Disable();
			forceSecurityBarrierChanged = true;
		}
		else
		{
			chkSecurityBarrier->Disable();
		}

		cboTablespace->Enable();
		txtFillFactor->Enable();
		chkMatViewWithData->Enable();
		chkCustomVac->Enable();
		chkCustomToastVac->Enable();
	}
	else
	{
		chkSecurityBarrier->Enable();
		cbCheckOption->Enable();
		chkSecurityBarrier->SetValue(forceSecurityBarrierChanged);
		forceSecurityBarrierChanged = false;

		DisableStorageParameters();
	}
}

void dlgView::FillAutoVacuumParameters(wxString &setStr, wxString &resetStr,
                                       const wxString &parameter, const wxString &val)
{
	if (val == wxT("-1"))
	{
		if (resetStr.IsEmpty())
			resetStr = wxT(" RESET (");
		else
			resetStr += wxT(",");
		resetStr += wxT("\n  ") + parameter;
	}
	else
	{
		if (setStr.IsEmpty())
			setStr = wxT(" SET (");
		else
			setStr += wxT(",");
		setStr += wxT("\n  ") + parameter + wxT(" = ") + val;
	}
}

wxString dlgView::AppendNum(bool &changed, wxTextCtrl *ctl, wxString val)
{
	wxString str = ctl->GetValue();
	if (str.IsEmpty() || str.StartsWith(wxT("-")))
		str = wxT("-1");

	changed |= (str != val);
	return str;
}

void dlgView::OnChangeVacuum(wxCommandEvent &ev)
{
	if (connection->BackendMinimumVersion(9, 3))
	{
		bool vacEn = chkCustomVac->GetValue() && chkVacEnabled->GetValue();
		chkCustomVac->Enable(chkMaterializedView->GetValue());
		chkVacEnabled->Enable(chkCustomVac->GetValue());
		cboTablespace->Enable(chkMaterializedView->GetValue());
		txtFillFactor->Enable(chkMaterializedView->GetValue());
		chkMatViewWithData->Enable(chkMaterializedView->GetValue());

		txtBaseVac->Enable(vacEn);
		txtBaseAn->Enable(vacEn);
		txtFactorVac->Enable(vacEn);
		txtFactorAn->Enable(vacEn);
		txtVacDelay->Enable(vacEn);
		txtVacLimit->Enable(vacEn);
		txtFreezeMinAge->Enable(vacEn);
		txtFreezeMaxAge->Enable(vacEn);

		stBaseVacCurr->SetLabel(tableVacBaseThr == wxT("-1") ? settingVacBaseThr : tableVacBaseThr);
		stBaseAnCurr->SetLabel(tableAnlBaseThr == wxT("-1") ? settingAnlBaseThr : tableAnlBaseThr);
		stFactorVacCurr->SetLabel(tableVacFactor == wxT("-1") ? settingVacFactor : tableVacFactor);
		stFactorAnCurr->SetLabel(tableAnlFactor == wxT("-1") ? settingAnlFactor : tableAnlFactor);
		stVacDelayCurr->SetLabel(tableCostDelay == wxT("-1") ? settingCostDelay : tableCostDelay);
		stVacLimitCurr->SetLabel(tableCostLimit == wxT("-1") ? settingCostLimit : tableCostLimit);

		stFreezeMinAgeCurr->SetLabel(tableFreezeMinAge == wxT("-1") ? settingFreezeMinAge : tableFreezeMinAge);
		stFreezeMaxAgeCurr->SetLabel(tableFreezeMaxAge == wxT("-1") ? settingFreezeMaxAge : tableFreezeMaxAge);

		txtFreezeTableAge->Enable(vacEn);
		stFreezeTableAgeCurr->SetLabel(tableFreezeTableAge == wxT("-1") ? settingFreezeTableAge : tableFreezeTableAge);
		/* Toast Table Vacuum Settings */
		bool toastVacEn = chkCustomToastVac->GetValue() && chkToastVacEnabled->GetValue();
		chkCustomToastVac->Enable(chkMaterializedView->GetValue());
		chkToastVacEnabled->Enable(chkCustomToastVac->GetValue());

		txtBaseToastVac->Enable(toastVacEn);
		txtFactorToastVac->Enable(toastVacEn);
		txtToastVacDelay->Enable(toastVacEn);
		txtToastVacLimit->Enable(toastVacEn);
		txtToastFreezeMinAge->Enable(toastVacEn);
		txtToastFreezeMaxAge->Enable(toastVacEn);
		txtToastFreezeTableAge->Enable(toastVacEn);

		stBaseToastVacCurr->SetLabel(toastTableVacBaseThr == wxT("-1") ? settingVacBaseThr : toastTableVacBaseThr);
		stFactorToastVacCurr->SetLabel(toastTableVacFactor == wxT("-1") ? settingVacFactor : toastTableVacFactor);
		stToastVacDelayCurr->SetLabel(toastTableCostDelay == wxT("-1") ? settingCostDelay : toastTableCostDelay);
		stToastVacLimitCurr->SetLabel(toastTableCostLimit == wxT("-1") ? settingCostLimit : toastTableCostLimit);
		stToastFreezeMinAgeCurr->SetLabel(toastTableFreezeMinAge == wxT("-1") ? settingFreezeMinAge : toastTableFreezeMinAge);
		stToastFreezeMaxAgeCurr->SetLabel(toastTableFreezeMaxAge == wxT("-1") ? settingFreezeMaxAge : toastTableFreezeMaxAge);
		txtToastFreezeTableAge->Enable(toastVacEn);
		stToastFreezeTableAgeCurr->SetLabel(toastTableFreezeTableAge == wxT("-1") ? settingFreezeTableAge : toastTableFreezeTableAge);
	}
	OnChange(ev);
}

void dlgView::DisableMaterializedView()
{
	chkMaterializedView->Disable();
	cboTablespace->Disable();
	txtFillFactor->Disable();
	chkMatViewWithData->Disable();

	chkCustomVac->Disable();
	chkVacEnabled->Disable();
	txtBaseVac->Disable();
	txtBaseAn->Disable();
	txtFactorVac->Disable();
	txtFactorAn->Disable();
	txtVacDelay->Disable();
	txtVacLimit->Disable();
	txtFreezeMinAge->Disable();
	txtFreezeMaxAge->Disable();
	txtFreezeTableAge->Disable();

	chkToastVacEnabled->Disable();
	chkCustomToastVac->Disable();
	txtBaseToastVac->Disable();
	txtFactorToastVac->Disable();
	txtToastVacDelay->Disable();
	txtToastVacLimit->Disable();
	txtToastFreezeMinAge->Disable();
	txtToastFreezeMaxAge->Disable();
	txtToastFreezeTableAge->Disable();
}

void dlgView::DisableStorageParameters()
{
	cboTablespace->Disable();
	txtFillFactor->Disable();
	chkMatViewWithData->Disable();

	chkCustomVac->Disable();
	chkVacEnabled->Disable();
	txtBaseVac->Disable();
	txtBaseAn->Disable();
	txtFactorVac->Disable();
	txtFactorAn->Disable();
	txtVacDelay->Disable();
	txtVacLimit->Disable();
	txtFreezeMinAge->Disable();
	txtFreezeMaxAge->Disable();
	txtFreezeTableAge->Disable();

	chkToastVacEnabled->Disable();
	chkCustomToastVac->Disable();
	txtBaseToastVac->Disable();
	txtFactorToastVac->Disable();
	txtToastVacDelay->Disable();
	txtToastVacLimit->Disable();
	txtToastFreezeMinAge->Disable();
	txtToastFreezeMaxAge->Disable();
	txtToastFreezeTableAge->Disable();
}

int dlgView::GetIndexCheckOption(const wxString &str) const
{
	if (str.Cmp(wxT("local")) == 0)
		return 1;
	if (str.Cmp(wxT("cascaded")) == 0)
		return 2;

	return 0;
}
