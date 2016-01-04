//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgDirectDbg.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>

// App headers
#include "ctl/ctlAuiNotebook.h"
#include "db/pgConn.h"
#include "db/pgQueryThread.h"
#include "db/pgQueryResultEvent.h"
#include "schema/pgObject.h"
#include "debugger/dbgConst.h"
#include "debugger/dbgBreakPoint.h"
#include "debugger/dbgController.h"
#include "debugger/dbgModel.h"
#include "debugger/ctlStackWindow.h"
#include "debugger/ctlMessageWindow.h"
#include "debugger/ctlTabWindow.h"
#include "debugger/frmDebugger.h"
#include "debugger/dlgDirectDbg.h"

#include "images/debugger.pngc"

#define lblMessage                  CTRL_STATIC("lblMessage")
#define grdParams                   CTRL("grdParams", wxGrid)
#define chkPkgInit                  CTRL_CHECKBOX("chkPkgInit")
#define btnDebug                    CTRL_BUTTON("wxID_OK")

IMPLEMENT_CLASS(dlgDirectDbg, pgDialog)

BEGIN_EVENT_TABLE(dlgDirectDbg, pgDialog)
	EVT_BUTTON(wxID_OK,                               dlgDirectDbg::OnOk)
	EVT_BUTTON(wxID_CANCEL,                           dlgDirectDbg::OnCancel)
	EVT_GRID_CMD_LABEL_LEFT_CLICK(XRCID("grdParams"), dlgDirectDbg::OnClickGridLabel)
	EVT_MENU(RESULT_ID_ARGS_UPDATED,                  dlgDirectDbg::ResultArgsUpdated)
	EVT_MENU(RESULT_ID_ARGS_UPDATE_ERROR,             dlgDirectDbg::ResultArgsUpdateError)
END_EVENT_TABLE()


////////////////////////////////////////////////////////////////////////////////
// dlgDirectDbg constructor
//
//    This class implements 'direct-debugging'. In direct-debugging, the user
//  provides a function signature, procedure signature, or OID on the command
//  line (this identifies the debug target).  We query the server for the
//  names, types, and in/out modes for each target parameter and then prompt
//    the user to enter a value for each of the IN (and IN/OUT) parameters.
//
//  When the user fills in the parameter values and clicks OK, we set a
//  breakpoint at the target and then execute a SELECT statement or an
//  EXEC statement that invokes the target (with the parameter values
//  provided by the user).

dlgDirectDbg::dlgDirectDbg(frmDebugger *_parent, dbgController *_controller,
                           pgConn *_conn)
	: m_controller(_controller), m_thread(NULL), m_conn(_conn)
{
	int width, height, totalWidth = 0;

	SetFont(settings->GetSystemFont());
	LoadResource(_parent, wxT("dlgDirectDbg"));

	// Icon
	SetIcon(*debugger_png_ico);
	RestorePosition();

	grdParams->SetRowLabelSize(wxGRID_AUTOSIZE);
	grdParams->SetColLabelSize(20);
	grdParams->AutoSizeColumns(true);

	chkPkgInit->SetValue(false);

	PopulateParamGrid();

	LoadSettings();

	for(int i = 0; i < grdParams->GetNumberCols(); i++)
		grdParams->AutoSizeColumn(i, true);

	if (grdParams->GetNumberCols() > 1)
	{
		// Extend grid to it's parent width
		grdParams->GetClientSize(&width, &height);
		for (int i = 0; i < grdParams->GetNumberCols(); i++)
		{
			totalWidth += grdParams->GetColumnWidth(i);
		}
		// Total client width - total six column widths - the first (an empty) column
		// width
		grdParams->SetColumnWidth(COL_DEF_VAL, width - totalWidth - 100);
	}
}


////////////////////////////////////////////////////////////////////////////////
// PopulateParamGrid()
//
//    This function reads parameter descriptions from m_targetInfo and adds a
//    new row to the grid control for each IN (or IN/OUT, VARAIADIC) parameter.
//    Each row displays the parameter name, the data type, and an entry box
//    where the user can type in a value for that parameter
//
void dlgDirectDbg::PopulateParamGrid()
{
	pgDbgArgs *args = m_controller->GetTargetInfo()->GetArgs();

	// If the target is defined within package, offer the user
	// a chance to debug the initializer (there may or may not
	// be an initializer, we don't really know at this point)
	if(m_controller->GetTargetInfo()->GetPkgOid() == 0)
		chkPkgInit->Disable();
	else
		chkPkgInit->Enable();

	if (!args)
	{
		grdParams->CreateGrid(0, 1);
		grdParams->SetColLabelValue(COL_NAME, _("No arguments required"));
		grdParams->SetColSize(0, 350);

		return;
	}

	// Add seven columns to the grid control:
	// - Name, Type, NULL?, EXPR?, Value, Default?, And Default Value
	grdParams->CreateGrid(0, 7);

	grdParams->SetColLabelValue(COL_NAME, _("Name"));
	grdParams->SetColLabelValue(COL_TYPE, _("Type"));
	grdParams->SetColLabelValue(COL_NULL, _("Null?"));
	grdParams->SetColLabelValue(COL_EXPR, _("Expression?"));
	grdParams->SetColLabelValue(COL_VALUE, _("Value"));
	grdParams->SetColLabelValue(COL_USE_DEF, _("Use default?"));
	grdParams->SetColLabelValue(COL_DEF_VAL, _("Default Value"));

	size_t idx = 0;

	for(size_t pIdx = 0; pIdx < args->Count(); pIdx++)
	{
		dbgArgInfo *arg = (*args)[pIdx];

		// If this is an IN parameter (or an IN/OUT parameter), add
		// a new row to the grid
		if(arg->GetMode() != pgParam::PG_PARAM_OUT)
		{
			grdParams->AppendRows(1);
			grdParams->SetCellValue(idx, COL_NAME,  arg->GetName());

			// Make it obvious which are variadics
			if (arg->GetMode() != pgParam::PG_PARAM_VARIADIC)
			{
				grdParams->SetRowLabelValue(idx, wxEmptyString);
				grdParams->SetCellValue(idx, COL_TYPE, arg->GetTypeName());

				if (!arg->IsArray())
				{
					// Is the value an expression?
					grdParams->SetCellEditor(idx, COL_EXPR, new ctlGridCellBoolEditor());
					grdParams->SetCellRenderer(idx, COL_EXPR, new wxGridCellBoolRenderer());
					grdParams->SetCellValue(idx, COL_EXPR, wxT(""));
				}
				else
				{
					grdParams->SetCellBackgroundColour(idx, COL_EXPR, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(idx, COL_VALUE, wxColour(235, 235, 235, 90));
				}

				// Use the default value?
				grdParams->SetCellRenderer(idx, COL_USE_DEF, new wxGridCellBoolRenderer());
				grdParams->SetCellEditor(idx, COL_USE_DEF, new ctlGridCellBoolEditor());
			}
			else
			{
				grdParams->SetCellValue(idx, COL_TYPE, wxT("VARIADIC ") + arg->GetTypeName());
				grdParams->SetRowLabelValue(idx, wxEmptyString);

				grdParams->AppendRows(1);
				grdParams->SetRowLabelValue(idx + 1, wxT("+"));
				grdParams->SetCellValue(idx + 1, COL_NAME, _("Click '+/-' to add/remove value to variadic"));

				grdParams->SetCellBackgroundColour(idx + 1, COL_EXPR,    wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx + 1, COL_VALUE,   wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx + 1, COL_USE_DEF, wxColour(235, 235, 235, 90));
			}

			// Set value to NULL?
			grdParams->SetCellEditor(idx, COL_NULL, new ctlGridCellBoolEditor(arg));
			grdParams->SetCellRenderer(idx, COL_NULL, new wxGridCellBoolRenderer());
			grdParams->SetCellValue(idx, COL_NULL, wxT(""));

			if (arg->HasDefault())
			{
				// Whenever the default value is available, use that by default
				grdParams->SetCellValue(idx, COL_USE_DEF, wxT("1"));
				grdParams->SetCellValue(idx, COL_DEF_VAL, arg->Default());

				// When "Use Defalut?" is enabled, disable VALUE, NULL? and
				// EXPR? columns
				grdParams->SetReadOnly(idx, COL_USE_DEF, false);
				grdParams->SetReadOnly(idx, COL_VALUE, false);
				grdParams->SetReadOnly(idx, COL_EXPR, false);
			}
			else
			{
				grdParams->SetCellValue(idx, COL_USE_DEF, wxT(""));
				grdParams->SetCellValue(idx, COL_DEF_VAL, _("<No default value>"));

				// When default value is not available, we should ask the user
				// to enter them
				grdParams->SetReadOnly(idx, COL_USE_DEF, true);
				grdParams->SetReadOnly(idx, COL_VALUE, arg->IsArray());
				grdParams->SetReadOnly(idx, COL_EXPR, arg->IsArray());

				grdParams->SetCellBackgroundColour(idx, COL_USE_DEF, wxColour(235, 235, 235, 90));
				if (arg->IsArray())
				{
					grdParams->SetCellBackgroundColour(idx, COL_VALUE, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(idx, COL_EXPR, wxColour(235, 235, 235, 90));
				}
			}

			grdParams->SetReadOnly(idx, COL_NULL, false);
			grdParams->SetReadOnly(idx, COL_NAME, true);
			grdParams->SetReadOnly(idx, COL_TYPE, true);
			grdParams->SetReadOnly(idx, COL_DEF_VAL, true);
			grdParams->SetCellBackgroundColour(idx, COL_NAME,    wxColour(235, 235, 235, 90));
			grdParams->SetCellBackgroundColour(idx, COL_TYPE,    wxColour(235, 235, 235, 90));
			grdParams->SetCellBackgroundColour(idx, COL_DEF_VAL, wxColour(235, 235, 235, 90));

			idx++;

			if (arg->GetMode() != pgParam::PG_PARAM_VARIADIC &&
			        arg->IsArray())
			{
				grdParams->AppendRows(1);

				grdParams->SetRowLabelValue(idx, wxT("+"));
				grdParams->SetCellValue(idx, COL_NAME, _("Click '+/-' to add/remove value to the array"));

				grdParams->SetReadOnly(idx, COL_NAME,    true);
				grdParams->SetReadOnly(idx, COL_TYPE,    true);
				grdParams->SetReadOnly(idx, COL_NULL,    true);
				grdParams->SetReadOnly(idx, COL_EXPR,    true);
				grdParams->SetReadOnly(idx, COL_VALUE,   true);
				grdParams->SetReadOnly(idx, COL_USE_DEF, true);
				grdParams->SetReadOnly(idx, COL_DEF_VAL, true);

				grdParams->SetCellBackgroundColour(idx, COL_NAME,    wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx, COL_TYPE,    wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx, COL_NULL,    wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx, COL_EXPR,    wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx, COL_VALUE,   wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx, COL_USE_DEF, wxColour(235, 235, 235, 90));
				grdParams->SetCellBackgroundColour(idx, COL_DEF_VAL, wxColour(235, 235, 235, 90));

				idx++;
			}
		}
	}

	// Move the cursor to the first value (so that the user
	// can just start typing)

	grdParams->SetGridCursor(0, COL_VALUE);
	grdParams->SetFocus();
}

void dlgDirectDbg::LoadLastCellSetting(int row_number, int parameter_index,
                                       int index_array_bound, bool isArray)
{
	wxString lastValue, valKey;
	dbgTargetInfo *target = m_controller->GetTargetInfo();
	long num;

	settings->Read(wxT("Debugger/Proc/OID"), &num, -1L);
	if(num != target->GetOid())
	{
		return;
	}
	else
	{
		if(!isArray)
		{
			// Non array elements cell index is 0
			valKey = wxString::Format(wxT("Debugger/Proc/%d/0/"), parameter_index);
			settings->Read(valKey + wxT("VAL"), &lastValue, wxEmptyString);
			grdParams->SetCellValue(row_number, COL_VALUE, lastValue);
		}
		else
		{
			long arrCnt;

			settings->Read(
			    wxString::Format(
			        wxT("Debugger/Proc/%d/"), parameter_index)
			    + wxT("ArrCnt"), &arrCnt, 0L);

			// Refresh an entire array element
			for( int i = 0; i <= index_array_bound && i <= arrCnt - 1; i++ )
			{
				valKey = wxString::Format(wxT("Debugger/Proc/%d/%d/"), parameter_index, i);
				settings->Read(valKey + wxT("VAL"), &lastValue, wxEmptyString);
				grdParams->SetCellValue(row_number - index_array_bound + i, COL_VALUE,
				                        lastValue);
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
// LoadSettings()
//
//    Loads default values from our .ini file. We save the OID of the most
//    recent direct-debugging target when close a session. If we're direct-
//    debugging the same target this time around, we load the argument values
//    from the .ini file.
void dlgDirectDbg::LoadSettings()
{
	long     num, arrCnt;
	wxString tmp, key;
	bool     initPkgCon = false;

	settings->Read(wxT("Debugger/Proc/OID"), &num, -1L);

	dbgTargetInfo *target = m_controller->GetTargetInfo();
	pgDbgArgs *args = m_controller->GetTargetInfo()->GetArgs();

	if (num != target->GetOid())
	{
		chkPkgInit->SetValue(target->GetPkgInitOid() != 0L);
		chkPkgInit->Enable(target->GetPkgInitOid() != 0L);

		return;
	}
	else
	{
		settings->Read(wxT("Debugger/Proc/initialize_package_constructor"), &initPkgCon, true);

		chkPkgInit->SetValue(&initPkgCon && target->GetPkgInitOid() != 0L);
		chkPkgInit->Enable(target->GetPkgInitOid() != 0L);
	}

	settings->Read(wxT("Debugger/Proc/args"), &num, 0L);

	if (num <= 0L)
		return;

	if (!args)
		return;

	for (int cnt = 0, row = 0; cnt < num && cnt < (int)args->Count();)
	{
		ctlGridCellBoolEditor *editor =
		    dynamic_cast<ctlGridCellBoolEditor *>(
		        grdParams->GetCellEditor(row, COL_NULL));
		dbgArgInfo *arg
		    = editor != NULL ? editor->GetArg() : NULL;

		if (arg == NULL)
		{
			row++;

			continue;
		}

		key = wxString::Format(wxT("Debugger/Proc/%d/"), cnt);

		// Use NULL?
		settings->Read(key + wxT("NULL"), &tmp, wxEmptyString);
		grdParams->SetCellValue(row, COL_NULL, tmp);

		// Use Default (if available)
		settings->Read(key + wxT("USE_DEF"), &tmp, wxEmptyString);
		grdParams->SetCellValue(row, COL_USE_DEF, tmp);

		// Is Array/VARIADIC?
		settings->Read(key + wxT("ArrCnt"), &arrCnt, 0L);

		if (arrCnt > 0L)
		{
			// An individual variable can be an expression or can be the 'NULL' value
			wxString valKey, strVal, strExpr, strNull;

			for (int idx = 0; idx < arrCnt; idx++, row++)
			{
				valKey = wxString::Format(wxT("Debugger/Proc/%d/%d/"), cnt, idx);

				// Use 'NULL'?
				settings->Read(valKey + wxT("NULL"), &strNull, wxEmptyString);

				// Use EXPR?
				settings->Read(valKey + wxT("EXPR"), &strExpr, wxEmptyString);

				// Value
				settings->Read(valKey + wxT("VAL"), &strVal, wxEmptyString);

				if (arg->IsArray())
				{
					int arrRow = row + 1;

					if (idx == 0)
					{
						grdParams->SetReadOnly(row, COL_EXPR, true);
						grdParams->SetReadOnly(row, COL_VALUE, true);

						grdParams->SetCellBackgroundColour(row, COL_EXPR,  wxColour(235, 235, 235, 90));
						grdParams->SetCellBackgroundColour(row, COL_VALUE, wxColour(235, 235, 235, 90));

						grdParams->SetRowLabelValue(
						    row, wxString::Format(wxT("%d"), cnt + 1));
					}
					grdParams->InsertRows(arrRow, 1, false);
					grdParams->SetRowLabelValue(arrRow, wxT("-"));

					grdParams->SetCellValue(arrRow, COL_TYPE, arg->GetBaseType());

					// Is the value an expression?
					grdParams->SetCellEditor(arrRow, COL_EXPR, new ctlGridCellBoolEditor());
					grdParams->SetCellRenderer(arrRow, COL_EXPR,
					                           new wxGridCellBoolRenderer());
					grdParams->SetCellValue(arrRow, COL_EXPR, strExpr);

					// Set value to NULL?
					grdParams->SetCellEditor(arrRow, COL_NULL,
					                         new ctlGridCellBoolEditor(arg));
					grdParams->SetCellRenderer(arrRow, COL_NULL,
					                           new wxGridCellBoolRenderer());
					grdParams->SetCellValue(arrRow, COL_NULL, strNull);

					// Set value
					grdParams->SetCellValue(arrRow, COL_VALUE, strVal);

					grdParams->SetReadOnly(arrRow, COL_NULL, false);
					grdParams->SetReadOnly(arrRow, COL_EXPR, false);
					grdParams->SetReadOnly(arrRow, COL_VALUE, false);

					grdParams->SetCellBackgroundColour(arrRow, COL_NAME,    wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow, COL_TYPE,    wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow, COL_USE_DEF, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow, COL_DEF_VAL, wxColour(235, 235, 235, 90));

					grdParams->SetReadOnly(arrRow + 1, COL_NAME,    true);
					grdParams->SetReadOnly(arrRow + 1, COL_TYPE,    true);
					grdParams->SetReadOnly(arrRow + 1, COL_NULL,    true);
					grdParams->SetReadOnly(arrRow + 1, COL_EXPR,    true);
					grdParams->SetReadOnly(arrRow + 1, COL_VALUE,   true);
					grdParams->SetReadOnly(arrRow + 1, COL_USE_DEF, true);
					grdParams->SetReadOnly(arrRow + 1, COL_DEF_VAL, true);

					grdParams->SetCellBackgroundColour(arrRow + 1, COL_NAME, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow + 1, COL_TYPE, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow + 1, COL_NULL, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow + 1, COL_EXPR, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow + 1, COL_VALUE, wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow + 1, COL_USE_DEF,
					                                   wxColour(235, 235, 235, 90));
					grdParams->SetCellBackgroundColour(arrRow + 1, COL_DEF_VAL,
					                                   wxColour(235, 235, 235, 90));

					grdParams->SetRowLabelValue(arrRow + 1, wxT("+"));
				}
				else
				{
					grdParams->SetCellValue(row, COL_VALUE, strVal);
					grdParams->SetCellValue(row, COL_EXPR, strExpr);
					grdParams->SetCellValue(row, COL_NULL, strNull);

					grdParams->SetReadOnly(row, COL_NULL, false);
					grdParams->SetReadOnly(row, COL_EXPR, false);
					grdParams->SetReadOnly(row, COL_VALUE, false);

					grdParams->SetRowLabelValue(
					    row, wxString::Format(wxT("%d"), cnt + 1));
				}
			}
			if (arg->IsArray())
				row++;
		}
		cnt++;
	}
}


void dlgDirectDbg::OnOk(wxCommandEvent &_ev)
{
	if (m_thread)
		return;

	grdParams->Enable(false);
	btnDebug->Enable(false);

	m_thread = new dbgArgValueEvaluator(m_conn, this);

	if (m_thread->Create() != wxTHREAD_NO_ERROR)
	{
		delete m_thread;
		m_thread = NULL;

		wxLogError(_("Failed to create a debugging thread."));
		EndModal(wxID_CANCEL);

		return;
	}

	m_thread->Run();
}


void dlgDirectDbg::OnCancel(wxCommandEvent &_ev)
{
	if (m_thread)
	{
		if (m_thread->IsRunning())
		{
			m_thread->CancelEval();
			m_thread->Wait();
		}

		delete m_thread;
		m_thread = NULL;
	}
	_ev.Skip();
}

void dlgDirectDbg::OnClickGridLabel(wxGridEvent &_ev)
{
	if (_ev.AltDown() || _ev.ControlDown() || _ev.ShiftDown())
	{
		_ev.Skip();

		return;
	}

	int row = _ev.GetRow();
	int col = _ev.GetCol();

	if (row < 0 || col > 0)
	{
		_ev.Skip();

		return;
	}

	wxString strLabel = grdParams->GetRowLabelValue(row);

	if (strLabel == wxT("+"))
	{
		wxASSERT(row != 0);

		ctlGridCellBoolEditor *editor =
		    dynamic_cast<ctlGridCellBoolEditor *>(
		        grdParams->GetCellEditor(row - 1, COL_NULL));
		dbgArgInfo *arg
		    = editor != NULL ? editor->GetArg() : NULL;

		grdParams->InsertRows(row, 1, false);
		grdParams->SetRowLabelValue(row, wxT("-"));

		grdParams->SetCellValue(row, COL_TYPE, arg->GetBaseType());
		grdParams->SetCellBackgroundColour(row, COL_TYPE, wxColour(235, 235, 235, 90));
		grdParams->SetCellBackgroundColour(row, COL_NAME, wxColour(235, 235, 235, 90));
		grdParams->SetCellBackgroundColour(row, COL_USE_DEF, wxColour(235, 235, 235, 90));
		grdParams->SetCellBackgroundColour(row, COL_DEF_VAL, wxColour(235, 235, 235, 90));

		// Is the value an expression?
		grdParams->SetCellEditor(row, COL_EXPR, new ctlGridCellBoolEditor());
		grdParams->SetCellRenderer(row, COL_EXPR, new wxGridCellBoolRenderer());
		grdParams->SetCellValue(row, COL_EXPR, wxT(""));

		// Set value to NULL?
		grdParams->SetCellEditor(row, COL_NULL, new ctlGridCellBoolEditor(arg));
		grdParams->SetCellRenderer(row, COL_NULL, new wxGridCellBoolRenderer());
		grdParams->SetCellValue(row, COL_NULL, wxT("1"));

		row++;
		grdParams->SetRowLabelValue(row, wxT("+"));
	}
	else if (strLabel == wxT("-"))
	{
		dbgArgInfo *arg = NULL;
		grdParams->DeleteRows(row, 1, false);
	}
	else
		return;

	// Update the row labels
	ctlGridCellBoolEditor *editor = NULL;
	dbgArgInfo *arg = NULL,
	            *prev = NULL;
	wxString strName;

	int totalRows = grdParams->GetNumberRows(),
	    idx = 0;
	row = 0;

	while (row < totalRows)
	{
		editor =
		    dynamic_cast<ctlGridCellBoolEditor *>(
		        grdParams->GetCellEditor(row, COL_NULL));
		arg	= editor != NULL ? editor->GetArg() : NULL;
		strName = grdParams->GetCellValue(row, COL_NAME);

		if (strName.IsEmpty() && arg)
			grdParams->SetRowLabelValue(row, wxT("-"));
		else if (!strName.IsEmpty() && !arg)
			grdParams->SetRowLabelValue(row, wxT("+"));
		else
		{
			idx++;
			grdParams->SetRowLabelValue(row, wxString::Format(wxT("%d"), idx));
		}

		row++;
	}
}

////////////////////////////////////////////////////////////////////////////////
// SaveSettings()
//
//    Save default values to our .ini file. We save the OID of the most
//    recent direct-debugging target when close a session. We also save the
//    value of each argument - if you debug the same target again next time,
//    loadSettings() will initialize the parameter-values window with the
//    same parameter values that you entered in this session.
//
void dlgDirectDbg::SaveSettings()
{
	wxString    strName, strExpr, strVal, strNull, strDef, strKey, strValExpr;
	dbgArgInfo *prev = NULL,
	            *arg = NULL;
	int         row = 0,
	            idx = -1,
	            arrCnt = 1;

	// Save the current function/procedure/trigger OID
	settings->WriteLong(wxT("Debugger/Proc/OID"),
	                    m_controller->GetTargetInfo()->GetOid());

	// Save - whether we need to debug the package constructor
	settings->WriteBool(wxT("Debugger/Proc/initialize_package_constructor"),
	                    (chkPkgInit->IsEnabled() && chkPkgInit->GetValue()));

	for (; row < grdParams->GetNumberRows(); row++)
	{
		ctlGridCellBoolEditor *editor =
		    dynamic_cast<ctlGridCellBoolEditor *>(
		        grdParams->GetCellEditor(row, COL_NULL));
		arg	= editor != NULL ? editor->GetArg() : NULL;

		// This should be the information for add/remove values for an array
		if (arg == NULL)
		{
			continue;
		}
		else if (prev != arg)
		{
			idx++;
			prev = arg;

			if (arg->IsArray())
			{
				arrCnt = 0;
			}
			else
			{
				arrCnt = 1;
			}
		}
		else
		{
			arrCnt++;
		}

		settings->WriteInt(wxString::Format(wxT("Debugger/Proc/%d/ArrCnt"), idx), arrCnt);

		if ((arrCnt == 0 && arg->IsArray()) ||
		        (arrCnt == 1 && !arg->IsArray()))
		{
			// Use Default (if available)
			settings->Write(wxString::Format(wxT("Debugger/Proc/%d/USE_DEF"), idx),
			                grdParams->GetCellValue(row, COL_USE_DEF));

			// Use NULL?
			settings->Write(wxString::Format(wxT("Debugger/Proc/%d/NULL"), idx),
			                grdParams->GetCellValue(row, COL_NULL));

			if (arrCnt == 0 && arg->IsArray())
				continue;
		}

		strKey = wxString::Format(wxT("Debugger/Proc/%d/%d/"), idx, arrCnt - 1);

		// Use NULL?
		settings->Write(strKey + wxT("NULL"),
		                grdParams->GetCellValue(row, COL_NULL));

		// Is value EXPR?
		settings->Write(strKey + wxT("EXPR"),
		                grdParams->GetCellValue(row, COL_EXPR));

		// Value
		settings->Write(strKey + wxT("VAL"),
		                grdParams->GetCellValue(row, COL_VALUE));
	}

	// Save the number of arguments
	settings->WriteLong(wxT("Debugger/Proc/args"), (long)(idx + 1));
}


ctlGridCellBoolEditor::ctlGridCellBoolEditor(dbgArgInfo *_arg)
	: m_arg(_arg)
{}


void ctlGridCellBoolEditor::BeginEdit(int _row, int _col, wxGrid *_grid)
{
	wxGridCellBoolEditor::BeginEdit(_row, _col, _grid);

	wxFocusEvent event (wxEVT_KILL_FOCUS);
	if (m_control)
	{
		m_control->GetEventHandler()->AddPendingEvent(event);
	}
}


wxGrid *dlgDirectDbg::GetParamsGrid()
{
	return grdParams;
}

bool dlgDirectDbg::DebugPkgConstructor()
{
	return chkPkgInit->IsEnabled() && chkPkgInit->GetValue();
}


wxGridCellEditor *ctlGridCellBoolEditor::Clone() const
{
	return new ctlGridCellBoolEditor(m_arg);
}


void dlgDirectDbg::ResultArgsUpdated(wxCommandEvent &_ev)
{
	SaveSettings();
	SavePosition();

	if (m_thread)
	{
		if (m_thread->IsRunning())
		{
			m_thread->CancelEval();
			m_thread->Wait();
		}

		delete m_thread;
		m_thread = NULL;
	}

	if (IsModal())
		EndModal(wxID_OK);
	else
		Destroy();
}


void dlgDirectDbg::ResultArgsUpdateError(wxCommandEvent &_ev)
{
	if (_ev.GetInt() == pgQueryResultEvent::PGQ_CONN_LOST)
	{
		if(wxMessageBox(
		            _("Connection to the database server lost!\nDo you want to try to reconnect to the server?"),
		            _("Connection Lost"), wxICON_ERROR | wxICON_QUESTION | wxYES_NO) == wxID_YES)
		{
			if (m_thread)
			{
				if (m_thread->IsRunning())
				{
					m_thread->CancelEval();
					m_thread->Wait();
				}

				delete m_thread;
				m_thread = NULL;
			}
			m_conn->Reconnect();

			return;
		}
		EndModal(wxID_CANCEL);

		return;
	}

	if (m_thread)
	{
		if (m_thread->IsRunning())
		{
			m_thread->CancelEval();
			m_thread->Wait();
		}

		delete m_thread;
		m_thread = NULL;
	}

	wxLogError(_ev.GetString());

	grdParams->Enable(true);
	btnDebug->Enable(true);
}


void dbgArgValueEvaluator::NoticeHandler(void *, const char *)
{
	// Ignore the notices
}


void *dbgArgValueEvaluator::Entry()
{
	bool argNull, nullVal, useDef;
	wxString strVal, strValExpr;
	dbgArgInfo *prev = NULL,
	            *arg = NULL;

	wxGrid *grd = m_dlg->GetParamsGrid();

	m_dlg->m_controller->GetTargetInfo()->DebugPackageConstructor()
	    = m_dlg->DebugPkgConstructor();

	m_conn->RegisterNoticeProcessor(dbgArgValueEvaluator::NoticeHandler, NULL);

	for (int row = 0, idx = 0, arrCnt = 1, arr_idx_bound = 0;
	        row < grd->GetNumberRows() && !m_cancelled; row++)
	{
		ctlGridCellBoolEditor *editor =
		    dynamic_cast<ctlGridCellBoolEditor *>(
		        grd->GetCellEditor(row, dlgDirectDbg::COL_NULL));
		arg	= editor != NULL ? editor->GetArg() : NULL;

		// This should be the information for add/remove values for an array
		if (arg == NULL)
		{
			// prev was an array, can we fetch value for the same
			if (prev && !prev->Null())
			{
				wxString res =
				    m_conn->ExecuteScalar(
				        wxT("SELECT ARRAY[") + strValExpr + wxT("]::") + prev->GetTypeName(),
				        false);

				if (m_cancelled)
				{
					m_conn->RegisterNoticeProcessor(NULL, NULL);

					return (void *)NULL;
				}
				if (m_conn->GetStatus() == PGCONN_BAD)
				{
					wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, RESULT_ID_ARGS_UPDATE_ERROR);

					ev.SetInt(pgQueryResultEvent::PGQ_CONN_LOST);

					m_dlg->GetEventHandler()->AddPendingEvent(ev);

					m_conn->RegisterNoticeProcessor(NULL, NULL);

					return (void *)NULL;
				}
				if (m_conn->GetLastResultStatus() == PGRES_TUPLES_OK)
				{
					prev->Value() = res;
				}
				else
				{
					wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, RESULT_ID_ARGS_UPDATE_ERROR);
					wxString strName = prev->GetName();
					wxString strMsg;

					if (strName.IsEmpty())
					{
						ev.SetString(
						    wxString::Format(
						        _("The specified value for argument #%d is not valid.\nPlease re-enter the value for it."),
						        idx));
					}
					else
					{
						ev.SetString(
						    wxString::Format(
						        _("Specified value for argument '%s' is not valid.\nPlease re-enter the value for it."),
						        strName.c_str()));
					}
					ev.SetInt(pgQueryResultEvent::PGQ_RESULT_ERROR);
					m_dlg->LoadLastCellSetting(row - 1, idx - 1, arr_idx_bound - 1, true);
					m_dlg->GetEventHandler()->AddPendingEvent(ev);

					m_conn->RegisterNoticeProcessor(NULL, NULL);

					return (void *)NULL;

				}
				prev = NULL;
				arr_idx_bound = 0;
			}

			continue;
		}
		else if (prev != arg)
		{
			idx++;
			argNull = false;
			nullVal = false;
			useDef  = false;

			strValExpr = wxEmptyString;
		}

		if (prev != arg)
		{
			prev = arg;

			// Use Default (if available)
			arg->UseDefault() = wxGridCellBoolEditor::IsTrueValue(
			                        grd->GetCellValue(row, dlgDirectDbg::COL_USE_DEF));
			// NULL?
			arg->Null() = wxGridCellBoolEditor::IsTrueValue(
			                  grd->GetCellValue(row, dlgDirectDbg::COL_NULL));

			if (arg->UseDefault())
				strValExpr = arg->Default();

			if (arg->IsArray())
				continue;
		}

		// Use NULL?
		//
		//   Keep this just before 'Use Default' column as - we may have non-empty
		//   value in the 'VAL' column

		if (!arg->UseDefault() || !arg->Null())
		{
			if (!strValExpr.IsEmpty())
			{
				strValExpr += wxT(", ");
			}

			if (wxGridCellBoolEditor::IsTrueValue(
			            grd->GetCellValue(row, dlgDirectDbg::COL_NULL)))
			{
				strValExpr += wxT("NULL");
			}
			else if (wxGridCellBoolEditor::IsTrueValue(
			             grd->GetCellValue(row, dlgDirectDbg::COL_EXPR)))
			{
				strValExpr += wxT("(") + grd->GetCellValue(row, dlgDirectDbg::COL_VALUE) + wxT(")");
			}
			else
			{
				strValExpr += m_conn->qtDbString(
				                  grd->GetCellValue(row, dlgDirectDbg::COL_VALUE));
			}
			strValExpr.Append(wxT("::"))
			.Append(arg->IsArray() ?
			        arg->GetBaseType() : arg->GetTypeName());

			if(arg->IsArray())
			{
				arr_idx_bound ++;
			}
		}

		if (!arg->IsArray() && !arg->Null())
		{
			pgSet *set =
			    m_conn->ExecuteSet(wxT("SELECT ") + strValExpr, false);

			if (m_cancelled)
			{
				return (void *)NULL;
			}
			if (m_conn->GetStatus() == PGCONN_BAD)
			{
				wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, RESULT_ID_ARGS_UPDATE_ERROR);

				ev.SetString(_("Connection to the database server lost!"));
				ev.SetInt(pgQueryResultEvent::PGQ_CONN_LOST);

				m_dlg->GetEventHandler()->AddPendingEvent(ev);

				m_conn->RegisterNoticeProcessor(NULL, NULL);

				if (set)
					delete set;

				return (void *)NULL;
			}
			if (m_conn->GetLastResultStatus() == PGRES_TUPLES_OK &&
			        set && set->NumRows() > 0L)
			{
				if (set->IsNull(0))
				{
					arg->Null() = true;
				}
				else
				{
					arg->Null() = false;
					arg->Value() = set->GetVal(0);
				}
			}
			else
			{
				wxString strName = arg->GetName();

				if (strName.IsEmpty())
					strName = wxString::Format(_("Param#%d"), idx);

				wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, RESULT_ID_ARGS_UPDATE_ERROR);

				ev.SetString(
				    wxString::Format(_("Please re-enter the value for argument '%s'."),
				                     strName.c_str()));
				ev.SetInt(pgQueryResultEvent::PGQ_RESULT_ERROR);

				m_dlg->GetEventHandler()->AddPendingEvent(ev);
				m_dlg->LoadSettings();
				// For scalar variable, always index of array bound is 0.
				m_dlg->LoadLastCellSetting(row, idx - 1, 0, false);
				m_conn->RegisterNoticeProcessor(NULL, NULL);

				return (void *)NULL;
			}
		}
	}

	wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, RESULT_ID_ARGS_UPDATED);
	m_dlg->GetEventHandler()->AddPendingEvent(ev);

	m_conn->RegisterNoticeProcessor(NULL, NULL);

	return (void *)NULL;
}


void dbgArgValueEvaluator::CancelEval()
{
	m_cancelled = true;

	if (m_conn->GetTxStatus() == PGCONN_TXSTATUS_ACTIVE)
		m_conn->CancelExecution();
}

dbgArgValueEvaluator::dbgArgValueEvaluator(pgConn *_conn, dlgDirectDbg *_dlg)
	: wxThread(wxTHREAD_JOINABLE), m_conn(_conn), m_dlg(_dlg), m_cancelled(false)
{}
