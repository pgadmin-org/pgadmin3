//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgDirectDbg.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dlgDirectDbg
//
//	This class implements 'direct-debugging'. In direct-debugging, the user
//  provides a function signature, procedure signature, or OID on the command
//  line (this identifies the debug target).  We query the server for the
//  names, types, and in/out modes for each target parameter and then prompt
//	the user to enter a value for each of the IN (and IN/OUT) parameters.
//
//  When the user fills in the parameter values and clicks OK, we set a
//  breakpoint at the target and then execute a SELECT statement or an
//  EXEC statement that invokes the target (with the parameter values
//  provided by the user).
//
//  A dlgDirectDbg object is typically a child of the frmDebugger object
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DLGDIRECTDBG_H
#define DLGDIRECTDBG_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/strconv.h>

#include "dlg/dlgClasses.h"

class dbgArgInfo;
class frmDebugger;
class dbgController;

class ctlGridCellBoolEditor : public wxGridCellBoolEditor
{
public:
	ctlGridCellBoolEditor(dbgArgInfo *_arg = NULL);
	void BeginEdit (int _row, int _col, wxGrid *_grid);
	wxGridCellEditor *Clone() const;

	dbgArgInfo *GetArg()
	{
		return m_arg;
	}

private:
	dbgArgInfo *m_arg;
};

class dlgDirectDbg;

class dbgArgValueEvaluator : public wxThread
{
public:
	dbgArgValueEvaluator(pgConn *, dlgDirectDbg *);

	void *Entry();
	void  CancelEval();
	static void NoticeHandler(void *, const char *);

private:
	pgConn       *m_conn;
	dlgDirectDbg *m_dlg;

	bool          m_cancelled;
};


class dlgDirectDbg : public pgDialog
{
	DECLARE_CLASS(dlgDirectDbg)

public:
	dlgDirectDbg(frmDebugger *_parent, dbgController *_controller,
	             pgConn *_conn);

	enum
	{
		COL_NAME = 0, // Column 0 contains the variable name
		COL_TYPE,     // Type of column
		COL_NULL,     // Value Set to NULL (yes/no)
		COL_EXPR,     // Value is an expression (yes/no)
		COL_VALUE,    // Value (constant ,or an expression)
		COL_USE_DEF,  // Use the default value
		COL_DEF_VAL   // Default value for the column
	};

private:

	void PopulateParamGrid();

	void OnOk(wxCommandEvent &event);
	void OnCancel(wxCommandEvent &event);
	void OnClickGridLabel(wxGridEvent &event);

	void ResultArgsUpdated(wxCommandEvent &);
	void ResultArgsUpdateError(wxCommandEvent &);

	void SaveSettings();
	void LoadSettings();
	// Function to retrive last cell value if the provieded parameter value is an invalid.
	void LoadLastCellSetting(int row_number, int array_row_number,
	                         int index_number, bool isArray);

	wxGrid *GetParamsGrid();
	bool DebugPkgConstructor();

	dbgController        *m_controller;
	pgConn               *m_conn;
	dbgArgValueEvaluator *m_thread;

	friend class dbgArgValueEvaluator;

	DECLARE_EVENT_TABLE()
};

#endif
