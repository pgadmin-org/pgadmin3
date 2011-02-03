//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmEditGrid.cpp - Edit Grid Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>

// App headers
#include "pgAdmin3.h"
#include "utils/pgDefs.h"
#include "frm/frmMain.h"
#include "frm/menu.h"
#include "db/pgQueryThread.h"

#include <wx/generic/gridctrl.h>
#include <wx/clipbrd.h>

#include "frm/frmEditGrid.h"
#include "ctl/ctlMenuToolbar.h"
#include "dlg/dlgEditGridOptions.h"
#include "frm/frmHint.h"
#include "schema/pgCatalogObject.h"
#include "schema/pgTable.h"
#include "schema/pgView.h"
#include "schema/gpExtTable.h"

// wxAUI
#include <wx/aui/aui.h>

// Icons
#include "images/viewdata.xpm"
#include "images/storedata.xpm"
#include "images/readdata.xpm"
#include "images/delete.xpm"
#include "images/edit_undo.xpm"
#include "images/sortfilter.xpm"
#include "images/help.xpm"
#include "images/clip_copy.xpm"
#include "images/clip_paste.xpm"

#define CTRLID_LIMITCOMBO       4226


BEGIN_EVENT_TABLE(frmEditGrid, pgFrame)
	EVT_ERASE_BACKGROUND(       frmEditGrid::OnEraseBackground)
	EVT_SIZE(                   frmEditGrid::OnSize)
	EVT_MENU(MNU_REFRESH,       frmEditGrid::OnRefresh)
	EVT_MENU(MNU_DELETE,        frmEditGrid::OnDelete)
	EVT_MENU(MNU_SAVE,          frmEditGrid::OnSave)
	EVT_MENU(MNU_INCLUDEFILTER, frmEditGrid::OnIncludeFilter)
	EVT_MENU(MNU_EXCLUDEFILTER, frmEditGrid::OnExcludeFilter)
	EVT_MENU(MNU_REMOVEFILTERS, frmEditGrid::OnRemoveFilters)
	EVT_MENU(MNU_ASCSORT,       frmEditGrid::OnAscSort)
	EVT_MENU(MNU_DESCSORT,      frmEditGrid::OnDescSort)
	EVT_MENU(MNU_REMOVESORT,    frmEditGrid::OnRemoveSort)
	EVT_MENU(MNU_UNDO,          frmEditGrid::OnUndo)
	EVT_MENU(MNU_OPTIONS,       frmEditGrid::OnOptions)
	EVT_MENU(MNU_HELP,          frmEditGrid::OnHelp)
	EVT_MENU(MNU_CONTENTS,      frmEditGrid::OnContents)
	EVT_MENU(MNU_COPY,          frmEditGrid::OnCopy)
	EVT_MENU(MNU_PASTE,         frmEditGrid::OnPaste)
	EVT_MENU(MNU_LIMITBAR,      frmEditGrid::OnToggleLimitBar)
	EVT_MENU(MNU_TOOLBAR,       frmEditGrid::OnToggleToolBar)
	EVT_MENU(MNU_SCRATCHPAD,    frmEditGrid::OnToggleScratchPad)
	EVT_MENU(MNU_DEFAULTVIEW,   frmEditGrid::OnDefaultView)
	EVT_MENU(MNU_CLOSE,         frmEditGrid::OnClose)
	EVT_CLOSE(                  frmEditGrid::OnCloseWindow)
	EVT_KEY_DOWN(               frmEditGrid::OnKey)
	EVT_GRID_RANGE_SELECT(      frmEditGrid::OnGridSelectCells)
	EVT_GRID_SELECT_CELL(       frmEditGrid::OnCellChange)
	EVT_GRID_EDITOR_SHOWN(      frmEditGrid::OnEditorShown)
	EVT_GRID_EDITOR_HIDDEN(     frmEditGrid::OnEditorHidden)
	EVT_GRID_CELL_RIGHT_CLICK(  frmEditGrid::OnCellRightClick)
	EVT_GRID_LABEL_RIGHT_CLICK( frmEditGrid::OnLabelRightClick)
	EVT_AUI_PANE_BUTTON(        frmEditGrid::OnAuiUpdate)
END_EVENT_TABLE()


frmEditGrid::frmEditGrid(frmMain *form, const wxString &_title, pgConn *_conn, pgSchemaObject *obj)
	: pgFrame(NULL, _title)
{
	closing = false;

	SetIcon(wxIcon(viewdata_xpm));
	wxWindowBase::SetFont(settings->GetSystemFont());
	dlgName = wxT("frmEditGrid");
	RestorePosition(-1, -1, 600, 500, 300, 350);
	connection = _conn;
	mainForm = form;
	thread = 0;
	relkind = 0;
	limit = 0;
	relid = (Oid)obj->GetOid();
	editorCell = new sqlCell();

	// notify wxAUI which frame to use
	manager.SetManagedWindow(this);
	manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG);

	SetMinSize(wxSize(300, 350));

	CreateStatusBar();
	SetStatusBarPane(-1);

	sqlGrid = new ctlSQLEditGrid(this, CTL_EDITGRID, wxDefaultPosition, wxDefaultSize);
	sqlGrid->SetTable(0);
#ifdef __WXMSW__
	sqlGrid->SetDefaultRowSize(sqlGrid->GetDefaultRowSize() + 2, true);
#endif

	// Set up toolbar
	toolBar = new ctlMenuToolbar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	toolBar->SetToolBitmapSize(wxSize(16, 16));

	toolBar->AddTool(MNU_SAVE, _("Save"), wxBitmap(storedata_xpm), _("Save the changed row."), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_REFRESH, _("Refresh"), wxBitmap(readdata_xpm), _("Refresh."), wxITEM_NORMAL);
	toolBar->AddTool(MNU_UNDO, _("Undo"), wxBitmap(edit_undo_xpm), _("Undo change of data."), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_COPY, _("Copy"), wxBitmap(clip_copy_xpm), _("Copy selected lines to clipboard."), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_PASTE, _("Paste"), wxBitmap(clip_paste_xpm), _("Paste data from the clipboard."), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_DELETE, _("Delete"), wxBitmap(delete_xpm), _("Delete selected rows."), wxITEM_NORMAL);
	toolBar->AddSeparator();

	toolBar->AddTool(MNU_OPTIONS, _("Options"), wxBitmap(sortfilter_xpm), _("Sort/filter options."), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_HELP, _("Edit grid help"), wxBitmap(help_xpm), _("Display help on this window."));

	toolBar->Realize();
	toolBar->EnableTool(MNU_SAVE, false);
	toolBar->EnableTool(MNU_UNDO, false);
	toolBar->EnableTool(MNU_DELETE, false);

	// Setup the limit bar
#ifndef __WXMAC__
	cbLimit = new wxComboBox(this, CTRLID_LIMITCOMBO, wxEmptyString, wxPoint(0, 0), wxSize(GetCharWidth() * 12, -1), NULL, wxCB_DROPDOWN);
#else
	cbLimit = new wxComboBox(this, CTRLID_LIMITCOMBO, wxEmptyString, wxPoint(0, 0), wxSize(GetCharWidth() * 24, -1), NULL, wxCB_DROPDOWN);
#endif
	cbLimit->Append(_("No limit"));
	cbLimit->Append(_("1000 rows"));
	cbLimit->Append(_("500 rows"));
	cbLimit->Append(_("100 rows"));
	cbLimit->SetValue(_("No limit"));

	// Finally, the scratchpad
	scratchPad = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxHSCROLL);

	// Menus

	// File menu
	fileMenu = new wxMenu();
	fileMenu->Append(MNU_SAVE, _("&Save"), _("Save the changed row."));
	fileMenu->AppendSeparator();
	fileMenu->Append(MNU_CLOSE, _("&Close"), _("Close this window."));
	fileMenu->Enable(MNU_SAVE, false);

	// Edit menu
	editMenu = new wxMenu();
	editMenu->Append(MNU_UNDO, _("&Undo\tCtrl-Z"), _("Undo change of data."));
	editMenu->AppendSeparator();
	editMenu->Append(MNU_COPY, _("&Copy\tCtrl-C"), _("Copy selected cells to clipboard."));
	editMenu->Append(MNU_PASTE, _("&Paste\tCtrl-V"), _("Paste data from the clipboard."));
	editMenu->Append(MNU_DELETE, _("&Delete"), _("Delete selected rows."));
	editMenu->Enable(MNU_UNDO, false);
	editMenu->Enable(MNU_DELETE, false);


	// View menu
	viewMenu = new wxMenu();
	viewMenu->Append(MNU_REFRESH, _("&Refresh\tF5"), _("Refresh."));
	viewMenu->AppendSeparator();
	viewMenu->Append(MNU_LIMITBAR, _("&Limit bar\tCtrl-Alt-L"), _("Show or hide the row limit options bar."), wxITEM_CHECK);
	viewMenu->Append(MNU_SCRATCHPAD, _("S&cratch pad\tCtrl-Alt-S"), _("Show or hide the scratch pad."), wxITEM_CHECK);
	viewMenu->Append(MNU_TOOLBAR, _("&Tool bar\tCtrl-Alt-T"), _("Show or hide the tool bar."), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MNU_DEFAULTVIEW, _("&Default view\tCtrl-Alt-V"),     _("Restore the default view."));


	// Tools menu
	toolsMenu = new wxMenu();
	toolsMenu->Append(MNU_OPTIONS, _("&Sort / Filter ..."), _("Sort / Filter options."));
	toolsMenu->AppendSeparator();
	toolsMenu->Append(MNU_INCLUDEFILTER, _("Filter By &Selection"), _("Display only those rows that have this value in this column."));
	toolsMenu->Append(MNU_EXCLUDEFILTER, _("Filter E&xcluding Selection"), _("Display only those rows that do not have this value in this column."));
	toolsMenu->Append(MNU_REMOVEFILTERS, _("&Remove Filter"), _("Remove all filters on this table"));
	toolsMenu->AppendSeparator();
	toolsMenu->Append(MNU_ASCSORT, _("Sort &Ascending"), _("Append an ASCENDING sort condition based on this column"));
	toolsMenu->Append(MNU_DESCSORT, _("Sort &Descending"), _("Append a DESCENDING sort condition based on this column"));
	toolsMenu->Append(MNU_REMOVESORT, _("&Remove Sort"), _("Remove all sort conditions"));

	// Help menu
	helpMenu = new wxMenu();
	helpMenu->Append(MNU_CONTENTS, _("&Help contents"), _("Open the helpfile."));
	helpMenu->Append(MNU_HELP, _("&Edit grid help"), _("Display help on this window."));

	menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(editMenu, _("&Edit"));
	menuBar->Append(viewMenu, _("&View"));
	menuBar->Append(toolsMenu, _("&Tools"));
	menuBar->Append(helpMenu, _("&Help"));
	SetMenuBar(menuBar);

	// Accelerators
	wxAcceleratorEntry entries[7];

	entries[0].Set(wxACCEL_CTRL,                (int)'S',      MNU_SAVE);
	entries[1].Set(wxACCEL_NORMAL,              WXK_F5,        MNU_REFRESH);
	entries[2].Set(wxACCEL_CTRL,                (int)'Z',      MNU_UNDO);
	entries[3].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);
	entries[4].Set(wxACCEL_CTRL,                (int)'C',      MNU_COPY);
	entries[5].Set(wxACCEL_CTRL,                (int)'V',      MNU_PASTE);
	entries[6].Set(wxACCEL_NORMAL,              WXK_DELETE,    MNU_DELETE);

	wxAcceleratorTable accel(7, entries);
	SetAcceleratorTable(accel);
	sqlGrid->SetAcceleratorTable(accel);

	// Kickstart wxAUI
	manager.AddPane(toolBar, wxAuiPaneInfo().Name(wxT("toolBar")).Caption(_("Tool bar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));
	manager.AddPane(cbLimit, wxAuiPaneInfo().Name(wxT("limitBar")).Caption(_("Limit bar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));
	manager.AddPane(sqlGrid, wxAuiPaneInfo().Name(wxT("sqlGrid")).Caption(_("Data grid")).Center().CaptionVisible(false).CloseButton(false).MinSize(wxSize(200, 100)).BestSize(wxSize(300, 200)));
	manager.AddPane(scratchPad, wxAuiPaneInfo().Name(wxT("scratchPad")).Caption(_("Scratch pad")).Bottom().MinSize(wxSize(200, 100)).BestSize(wxSize(300, 150)));

	// Now load the layout
	wxString perspective;
	settings->Read(wxT("frmEditGrid/Perspective-") + wxString(FRMEDITGRID_PERSPECTIVE_VER), &perspective, FRMEDITGRID_DEFAULT_PERSPECTIVE);
	manager.LoadPerspective(perspective, true);

	// and reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));
	manager.GetPane(wxT("limitBar")).Caption(_("Limit bar"));
	manager.GetPane(wxT("sqlGrid")).Caption(_("Data grid"));
	manager.GetPane(wxT("scratchPad")).Caption(_("Scratch pad"));

	// Sync the View menu options
	viewMenu->Check(MNU_LIMITBAR, manager.GetPane(wxT("limitBar")).IsShown());
	viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());
	viewMenu->Check(MNU_SCRATCHPAD, manager.GetPane(wxT("scratchPad")).IsShown());

	// tell the manager to "commit" all the changes just made
	manager.Update();

	if (obj->GetMetaType() == PGM_TABLE || obj->GetMetaType() == GP_PARTITION)
	{
		pgTable *table = (pgTable *)obj;

		relkind = 'r';
		hasOids = table->GetHasOids();
		tableName = table->GetSchema()->GetQuotedFullIdentifier() + wxT(".") + table->GetQuotedIdentifier();
		primaryKeyColNumbers = table->GetPrimaryKeyColNumbers();
		orderBy = table->GetQuotedPrimaryKey();
		if (orderBy.IsEmpty() && hasOids)
			orderBy = wxT("oid");
		if (!orderBy.IsEmpty())
			orderBy += wxT(" ASC");
	}
	else if (obj->GetMetaType() == PGM_VIEW)
	{
		pgView *view = (pgView *)obj;

		relkind = 'v';
		hasOids = false;
		tableName = view->GetSchema()->GetQuotedFullIdentifier() + wxT(".") + view->GetQuotedIdentifier();
	}
	else if (obj->GetMetaType() == GP_EXTTABLE)
	{
		gpExtTable *exttable = (gpExtTable *)obj;

		relkind = 'x';
		hasOids = false;
		tableName = exttable->GetSchema()->GetQuotedFullIdentifier() + wxT(".") + exttable->GetQuotedIdentifier();
	}
	else if (obj->GetMetaType() == PGM_CATALOGOBJECT)
	{
		pgCatalogObject *catobj = (pgCatalogObject *)obj;

		relkind = 'v';
		hasOids = false;
		tableName = catobj->GetSchema()->GetQuotedFullIdentifier() + wxT(".") + catobj->GetQuotedIdentifier();
	}
}

void frmEditGrid::OnEraseBackground(wxEraseEvent &event)
{
	event.Skip();
}

void frmEditGrid::OnSize(wxSizeEvent &event)
{
	event.Skip();
}

void frmEditGrid::OnToggleLimitBar(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_LIMITBAR))
		manager.GetPane(wxT("limitBar")).Show(true);
	else
		manager.GetPane(wxT("limitBar")).Show(false);
	manager.Update();
}

void frmEditGrid::OnToggleToolBar(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_TOOLBAR))
		manager.GetPane(wxT("toolBar")).Show(true);
	else
		manager.GetPane(wxT("toolBar")).Show(false);
	manager.Update();
}

void frmEditGrid::OnToggleScratchPad(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_SCRATCHPAD))
		manager.GetPane(wxT("scratchPad")).Show(true);
	else
		manager.GetPane(wxT("scratchPad")).Show(false);
	manager.Update();
}

void frmEditGrid::OnAuiUpdate(wxAuiManagerEvent &event)
{
	if(event.pane->name == wxT("limitBar"))
	{
		viewMenu->Check(MNU_LIMITBAR, false);
	}
	else if(event.pane->name == wxT("toolBar"))
	{
		viewMenu->Check(MNU_TOOLBAR, false);
	}
	else if(event.pane->name == wxT("scratchPad"))
	{
		viewMenu->Check(MNU_SCRATCHPAD, false);
	}
	event.Skip();
}

void frmEditGrid::OnDefaultView(wxCommandEvent &event)
{
	manager.LoadPerspective(FRMEDITGRID_DEFAULT_PERSPECTIVE, true);

	// Reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));
	manager.GetPane(wxT("limitBar")).Caption(_("Limit bar"));
	manager.GetPane(wxT("sqlGrid")).Caption(_("Data grid"));
	manager.GetPane(wxT("scratchPad")).Caption(_("Scratch pad"));

	// tell the manager to "commit" all the changes just made
	manager.Update();

	// Sync the View menu options
	viewMenu->Check(MNU_LIMITBAR, manager.GetPane(wxT("limitBar")).IsShown());
	viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());
	viewMenu->Check(MNU_SCRATCHPAD, manager.GetPane(wxT("scratchPad")).IsShown());
}

void frmEditGrid::SetSortCols(const wxString &cols)
{
	if (orderBy != cols)
	{
		orderBy = cols;
	}
}

void frmEditGrid::SetFilter(const wxString &filter)
{
	if (rowFilter != filter)
	{
		rowFilter = filter;
	}
}

void frmEditGrid::SetLimit(const int rowlimit)
{
	if (rowlimit != limit)
	{
		limit = rowlimit;

		if (limit <= 0)
			cbLimit->SetValue(_("No limit"));
		else
			cbLimit->SetValue(wxString::Format(wxPLURAL("%i row", "%i rows", limit), limit));
	}
}

void frmEditGrid::OnLabelRightClick(wxGridEvent &event)
{
	wxMenu *xmenu = new wxMenu();
	wxArrayInt rows = sqlGrid->GetSelectedRows();
	xmenu->Append(MNU_COPY, _("&Copy"), _("Copy selected cells to clipboard."));
	xmenu->Append(MNU_PASTE, _("&Paste"), _("Paste data from the clipboard."));
	xmenu->Append(MNU_DELETE, _("&Delete"), _("Delete selected rows."));

	if ((rows.GetCount()) && (!sqlGrid->IsCurrentCellReadOnly()))
	{
		xmenu->Enable(MNU_COPY, true);
		xmenu->Enable(MNU_DELETE, true);
		xmenu->Enable(MNU_PASTE, true);
	}
	else
	{
		xmenu->Enable(MNU_COPY, false);
		xmenu->Enable(MNU_DELETE, false);
		xmenu->Enable(MNU_PASTE, false);
	}
	sqlGrid->PopupMenu(xmenu);
}


void frmEditGrid::OnCellRightClick(wxGridEvent &event)
{
	wxMenu *xmenu = new wxMenu();

	// If we cannot refresh, assume there is a data thread running. We cannot
	// check thread->IsRunning() as it can crash if the thread is in some
	// states :-(
	if (!toolBar->GetToolEnabled(MNU_REFRESH))
		return;

	sqlGrid->SetGridCursor(event.GetRow(), event.GetCol());

	xmenu->Append(MNU_INCLUDEFILTER, _("Filter By &Selection"), _("Display only those rows that have this value in this column."));
	xmenu->Append(MNU_EXCLUDEFILTER, _("Filter E&xcluding Selection"), _("Display only those rows that do not have this value in this column."));
	xmenu->Append(MNU_REMOVEFILTERS, _("&Remove Filter"), _("Remove all filters on this table"));
	xmenu->InsertSeparator(3);
	xmenu->Append(MNU_ASCSORT, _("Sort &Ascending"), _("Append an ASCENDING sort condition based on this column"));
	xmenu->Append(MNU_DESCSORT, _("Sort &Descending"), _("Append a DESCENDING sort condition based on this column"));
	xmenu->Append(MNU_REMOVESORT, _("&Remove Sort"), _("Remove all sort conditions"));

	xmenu->Enable(MNU_INCLUDEFILTER, true);
	xmenu->Enable(MNU_EXCLUDEFILTER, true);
	xmenu->Enable(MNU_REMOVEFILTERS, true);
	xmenu->Enable(MNU_ASCSORT, true);
	xmenu->Enable(MNU_DESCSORT, true);
	xmenu->Enable(MNU_REMOVESORT, true);

	sqlGrid->PopupMenu(xmenu);
}


void frmEditGrid::OnCellChange(wxGridEvent &event)
{
	sqlTable *table = sqlGrid->GetTable();
	bool doSkip = true;

	if (table)
	{
		if (table->LastRow() >= 0)
		{
			if (table->LastRow() != event.GetRow())
			{
				doSkip = DoSave();
			}
		}
		else if (sqlGrid->GetGridCursorRow() != event.GetRow())
		{
			toolBar->EnableTool(MNU_SAVE, false);
			toolBar->EnableTool(MNU_UNDO, false);
			fileMenu->Enable(MNU_SAVE, false);
			editMenu->Enable(MNU_UNDO, false);
		}
	}

	if (doSkip)
		event.Skip();
}


void frmEditGrid::OnIncludeFilter(wxCommandEvent &event)
{
	int curcol = sqlGrid->GetGridCursorCol();
	int currow = sqlGrid->GetGridCursorRow();

	if (curcol == -1 || currow == -1)
		return;

	sqlTable *table = sqlGrid->GetTable();
	wxString column_label = qtIdent(table->GetColLabelValueUnformatted(curcol));
	wxString new_filter_string;

	size_t old_filter_string_length = GetFilter().Trim().Len();

	if (old_filter_string_length > 0)
	{
		new_filter_string = GetFilter().Trim() + wxT(" \n    AND ");
	}

	if (table->IsColText(curcol))
	{

		if (sqlGrid->GetCellValue(currow, curcol).IsNull())
		{
			new_filter_string += column_label + wxT(" IS NULL ");
		}
		else
		{

			if (sqlGrid->GetCellValue(currow, curcol) == wxT("\'\'"))
			{
				new_filter_string += column_label + wxT(" = ''");
			}
			else
			{
				new_filter_string += column_label + wxT(" = ") + connection->qtDbString(sqlGrid->GetCellValue(currow, curcol)) + wxT(" ");
			}
		}
	}
	else
	{

		if (sqlGrid->GetCellValue(currow, curcol).IsNull())
		{
			new_filter_string += column_label + wxT(" IS NULL ");
		}
		else
		{
			new_filter_string += column_label + wxT(" = ") + sqlGrid->GetCellValue(currow, curcol);
		}
	}

	SetFilter(new_filter_string);

	Go();
}


void frmEditGrid::OnExcludeFilter(wxCommandEvent &event)
{
	int curcol = sqlGrid->GetGridCursorCol();
	int currow = sqlGrid->GetGridCursorRow();

	if (curcol == -1 || currow == -1)
		return;

	sqlTable *table = sqlGrid->GetTable();
	wxString column_label = qtIdent(table->GetColLabelValueUnformatted(curcol));
	wxString new_filter_string;

	size_t old_filter_string_length = GetFilter().Trim().Len();

	if (old_filter_string_length > 0)
	{
		new_filter_string = GetFilter().Trim() + wxT(" \n    AND ");
	}

	if (table->IsColText(curcol))
	{
		if (sqlGrid->GetCellValue(currow, curcol).IsNull())
		{
			new_filter_string += column_label + wxT(" IS NOT NULL ");
		}
		else
		{

			if (sqlGrid->GetCellValue(currow, curcol) == wxT("\'\'"))
			{
				new_filter_string += column_label + wxString::Format(wxT(" IS DISTINCT FROM '' ")) ;
			}
			else
			{
				new_filter_string += column_label + wxT(" IS DISTINCT FROM ") + connection->qtDbString(sqlGrid->GetCellValue(currow, curcol)) + wxT(" ");
			}
		}
	}
	else
	{

		if (sqlGrid->GetCellValue(currow, curcol).IsNull())
		{
			new_filter_string += column_label + wxT(" IS NOT NULL ") ;
		}
		else
		{
			new_filter_string += column_label + wxT(" IS DISTINCT FROM ") + sqlGrid->GetCellValue(currow, curcol);
		}
	}

	SetFilter(new_filter_string);

	Go();
}


void frmEditGrid::OnRemoveFilters(wxCommandEvent &event)
{
	SetFilter(wxT(""));

	Go();
}


void frmEditGrid::OnAscSort(wxCommandEvent &ev)
{
	int curcol = sqlGrid->GetGridCursorCol();

	if (curcol == -1)
		return;

	sqlTable *table = sqlGrid->GetTable();
	wxString column_label = qtIdent(table->GetColLabelValueUnformatted(curcol));
	wxString old_sort_string = GetSortCols().Trim();
	wxString new_sort_string;

	if (old_sort_string.Find(column_label) == wxNOT_FOUND)
	{
		if (old_sort_string.Len() > 0)
			new_sort_string = old_sort_string + wxT(" , ");

		new_sort_string += column_label + wxT(" ASC ");
	}
	else
	{
		if (old_sort_string.Find(column_label + wxT(" ASC")) == wxNOT_FOUND)
		{
			// Previous occurrence was for DESCENDING sort
			new_sort_string = old_sort_string;
			new_sort_string.Replace(column_label + wxT(" DESC"), column_label + wxT(" ASC"));
		}
		else
		{
			// Previous occurrence was for ASCENDING sort. Nothing to do
			new_sort_string = old_sort_string;
		}
	}

	SetSortCols(new_sort_string);

	Go();
}


void frmEditGrid::OnDescSort(wxCommandEvent &ev)
{
	int curcol = sqlGrid->GetGridCursorCol();

	if (curcol == -1)
		return;

	sqlTable *table = sqlGrid->GetTable();
	wxString column_label = qtIdent(table->GetColLabelValueUnformatted(curcol));
	wxString old_sort_string = GetSortCols().Trim();
	wxString new_sort_string;

	if (old_sort_string.Find(column_label) == wxNOT_FOUND)
	{
		if (old_sort_string.Len() > 0)
			new_sort_string = old_sort_string + wxT(" , ");

		new_sort_string += column_label + wxT(" DESC ");
	}
	else
	{
		if (old_sort_string.Find(column_label + wxT(" DESC")) == wxNOT_FOUND)
		{
			// Previous occurrence was for ASCENDING sort
			new_sort_string = old_sort_string;
			new_sort_string.Replace(column_label + wxT(" ASC"), column_label + wxT(" DESC"));
		}
		else
		{
			// Previous occurrence was for DESCENDING sort. Nothing to do
			new_sort_string = old_sort_string;
		}
	}

	SetSortCols(new_sort_string);

	Go();
}


void frmEditGrid::OnRemoveSort(wxCommandEvent &ev)
{
	SetSortCols(wxT(""));

	Go();
}


void frmEditGrid::OnCopy(wxCommandEvent &ev)
{
	wxWindow *wnd = FindFocus();
	if (wnd == scratchPad)
	{
		scratchPad->Copy();
	}
	else
	{
		if (editorCell->IsSet())
		{
			if (wxTheClipboard->Open())
			{
				if (sqlGrid->GetTable()->IsColText(sqlGrid->GetGridCursorCol()))
				{
					wxStyledTextCtrl *text = (wxStyledTextCtrl *)sqlGrid->GetCellEditor(sqlGrid->GetGridCursorRow(), sqlGrid->GetGridCursorCol())->GetControl();
					if (text)
						wxTheClipboard->SetData(new wxTextDataObject(text->GetSelectedText()));
				}
				else
				{
					wxTextCtrl *text = (wxTextCtrl *)sqlGrid->GetCellEditor(sqlGrid->GetGridCursorRow(), sqlGrid->GetGridCursorCol())->GetControl();
					if (text)
						wxTheClipboard->SetData(new wxTextDataObject(text->GetStringSelection()));
				}
				wxTheClipboard->Close();
			}
		}
		else if(sqlGrid->GetNumberRows() > 0)
		{
			int copied;
			copied = sqlGrid->Copy();
			SetStatusText(wxString::Format(
			                  wxPLURAL("Data from %d row copied to clipboard.", "Data from %d rows copied to clipboard.", copied),
			                  copied));
		}
	}
}


void frmEditGrid::OnPaste(wxCommandEvent &ev)
{
	wxWindow *wnd = FindFocus();
	if (wnd == scratchPad)
	{
		scratchPad->Paste();
	}
	else if (editorCell->IsSet())
	{
		if (wxTheClipboard->Open())
		{
			if (wxTheClipboard->IsSupported(wxDF_TEXT))
			{
				wxTextDataObject data;
				wxTheClipboard->GetData(data);
				wxControl *ed = sqlGrid->GetCellEditor(editorCell->GetRow(), editorCell->GetCol())->GetControl();
				if (ed->IsKindOf(CLASSINFO(wxStyledTextCtrl)))
				{
					wxStyledTextCtrl *txtEd = (wxStyledTextCtrl *)ed;
					txtEd->ReplaceSelection(data.GetText());
				}
				else if (ed->IsKindOf(CLASSINFO(wxCheckBox)))
				{
					wxCheckBox *boolEd = (wxCheckBox *)ed;

					if (data.GetText().Lower() == wxT("true"))
						boolEd->Set3StateValue(wxCHK_CHECKED);
					else if (data.GetText().Lower() == wxT("false"))
						boolEd->Set3StateValue(wxCHK_UNCHECKED);
					else
						boolEd->Set3StateValue(wxCHK_UNDETERMINED);
				}
			}
			wxTheClipboard->Close();
		}
	}
	else if(sqlGrid->GetNumberRows() > 0)
	{
		if (toolBar->GetToolEnabled(MNU_SAVE))
		{
			wxMessageDialog msg(this, _("There is unsaved data in a row.\nDo you want to store to the database?"), _("Unsaved data"),
			                    wxYES_NO | wxICON_QUESTION | wxCANCEL);
			switch (msg.ShowModal())
			{
				case wxID_YES:
					if (!DoSave())
						return;
					break;

				case wxID_CANCEL:
					return;
					break;

				case wxID_NO:
					CancelChange();
					break;
			}
		}

		if (sqlGrid->GetTable()->Paste())
		{
			toolBar->EnableTool(MNU_SAVE, true);
			toolBar->EnableTool(MNU_UNDO, true);
			fileMenu->Enable(MNU_SAVE, true);
			editMenu->Enable(MNU_UNDO, true);
		}
	}
}

void frmEditGrid::OnHelp(wxCommandEvent &ev)
{
	DisplayHelp(wxT("editgrid"), HELP_PGADMIN);
}

void frmEditGrid::OnContents(wxCommandEvent &ev)
{
	DisplayHelp(wxT("index"), HELP_PGADMIN);
}

void frmEditGrid::OnKey(wxKeyEvent &event)
{
	int curcol = sqlGrid->GetGridCursorCol();
	int currow = sqlGrid->GetGridCursorRow();

	if (curcol == -1 || currow == -1)
		return;

	int keycode = event.GetKeyCode();
	wxCommandEvent ev;

	switch (keycode)
	{
		case WXK_DELETE:
		{
			if (editorCell->IsSet() || !toolBar->GetToolEnabled(MNU_DELETE))
			{
				if (!sqlGrid->IsCurrentCellReadOnly())
				{
					sqlGrid->EnableCellEditControl();
					sqlGrid->ShowCellEditControl();

					wxGridCellEditor *edit = sqlGrid->GetCellEditor(currow, curcol);
					if (edit)
					{
						wxControl *ctl = edit->GetControl();
						if (ctl)
						{
							wxStyledTextCtrl *txt = wxDynamicCast(ctl, wxStyledTextCtrl);
							if (txt)
								txt->SetText(wxEmptyString);
						}
						edit->DecRef();
					}
				}
			}
			else
			{
				OnDelete(ev);
			}
			return;
		}
		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			// check for shift etc.
			if (event.ControlDown() || event.ShiftDown())
			{
				// Inject a RETURN into the control
				wxGridCellEditor *edit = sqlGrid->GetCellEditor(currow, curcol);
				if (edit)
				{
					wxControl *ctl = edit->GetControl();
					if (ctl)
					{
						wxStyledTextCtrl *txt = wxDynamicCast(ctl, wxStyledTextCtrl);
						if (txt)
							txt->ReplaceSelection(END_OF_LINE);
					}
					edit->DecRef();
				}
				return;
			}
			else
			{
				if( keycode != WXK_NUMPAD_ENTER )
				{
					// if we are at the end of the row
					if (curcol == sqlGrid->GetNumberCols() - 1)
					{
						// we first get to the first column of the next row
						curcol = 0;
						currow++;

						// * if the displayed object is a table,
						//   we first need to make sure that the new selected
						//   cell is read/write, otherwise we need to select
						//   the next one
						// * if the displayed object is not a table (for
						//   example, a view), all cells are readonly, so
						//   we skip that part
						if (relkind == 'r')
						{
							// locate first editable column
							while (curcol < sqlGrid->GetNumberCols() && sqlGrid->IsReadOnly(currow, curcol))
								curcol++;
							// next line is completely read-only
							if (curcol == sqlGrid->GetNumberCols())
								return;
						}
					}
					else
						curcol++;
				}
				else // ( keycode==WXK_NUMPAD_ENTER )
				{
					currow++;
				}

				OnSave(ev);
				sqlGrid->SetGridCursor(currow, curcol);

				return;
			}

		case WXK_TAB:
			if (event.ControlDown())
			{
				wxStyledTextCtrl *text = (wxStyledTextCtrl *)sqlGrid->GetCellEditor(sqlGrid->GetGridCursorRow(), sqlGrid->GetGridCursorCol())->GetControl();
				if (text)
					text->SetText(wxT("\t"));
				return;
			}

			break;

		case WXK_ESCAPE:
			CancelChange();
			break;

		default:
			if (sqlGrid->IsEditable() && keycode >= WXK_SPACE && keycode < WXK_START)
			{
				if (sqlGrid->IsCurrentCellReadOnly())
					return;

				toolBar->EnableTool(MNU_SAVE, true);
				toolBar->EnableTool(MNU_UNDO, true);
				fileMenu->Enable(MNU_SAVE, true);
				editMenu->Enable(MNU_UNDO, true);
			}
			break;
	}
	event.Skip();
}

void frmEditGrid::OnClose(wxCommandEvent &event)
{
	this->Close();
}

void frmEditGrid::OnCloseWindow(wxCloseEvent &event)
{
	// We need to call OnCellChange to check if some cells for the row have been changed
	wxGridEvent evt;
	OnCellChange(evt);

	// If MNU_SAVE item is still enabled, we need to ask about the unsaved data
	if (toolBar->GetToolEnabled(MNU_SAVE))
	{
		int flag = wxYES_NO | wxICON_QUESTION;
		if (event.CanVeto())
			flag |= wxCANCEL;

		wxMessageDialog msg(this, _("There is unsaved data in a row.\nDo you want to store to the database?"), _("Unsaved data"),
		                    flag);
		switch (msg.ShowModal())
		{
			case wxID_YES:
			{
				if (!DoSave())
				{
					event.Veto();
					return;
				}
				break;
			}
			case wxID_CANCEL:
				event.Veto();
				return;
		}
	}
	Abort();
	Destroy();
}


void frmEditGrid::OnUndo(wxCommandEvent &event)
{
	sqlGrid->DisableCellEditControl();
	sqlGrid->GetTable()->UndoLine(sqlGrid->GetGridCursorRow());
	sqlGrid->ForceRefresh();

	toolBar->EnableTool(MNU_SAVE, false);
	toolBar->EnableTool(MNU_UNDO, false);
	fileMenu->Enable(MNU_SAVE, false);
	editMenu->Enable(MNU_UNDO, false);
}


void frmEditGrid::OnRefresh(wxCommandEvent &event)
{
	if (!toolBar->GetToolEnabled(MNU_REFRESH))
		return;

	if (toolBar->GetToolEnabled(MNU_SAVE))
	{
		wxMessageDialog msg(this, _("There is unsaved data in a row.\nDo you want to store to the database?"), _("Unsaved data"),
		                    wxYES_NO | wxICON_QUESTION | wxCANCEL);
		switch (msg.ShowModal())
		{
			case wxID_YES:
			{
				if (!DoSave())
					return;
				break;
			}
			case wxID_CANCEL:
				return;
		}
	}

	sqlGrid->DisableCellEditControl();
	Go();
}


void frmEditGrid::OnSave(wxCommandEvent &event)
{
	if (sqlGrid->GetBatchCount() == 0)
		DoSave();
}

bool frmEditGrid::DoSave()
{
	sqlGrid->HideCellEditControl();
	sqlGrid->SaveEditControlValue();
	sqlGrid->DisableCellEditControl();

	if (!sqlGrid->GetTable()->StoreLine())
		return false;

	toolBar->EnableTool(MNU_SAVE, false);
	toolBar->EnableTool(MNU_UNDO, false);
	fileMenu->Enable(MNU_SAVE, false);
	editMenu->Enable(MNU_UNDO, false);

	return true;
}

void frmEditGrid::CancelChange()
{
	sqlGrid->HideCellEditControl();
	sqlGrid->SaveEditControlValue();
	sqlGrid->DisableCellEditControl();

	toolBar->EnableTool(MNU_SAVE, false);
	toolBar->EnableTool(MNU_UNDO, false);
	fileMenu->Enable(MNU_SAVE, false);
	editMenu->Enable(MNU_UNDO, false);

	sqlGrid->GetTable()->UndoLine(sqlGrid->GetGridCursorRow());
	sqlGrid->ForceRefresh();
}


void frmEditGrid::OnOptions(wxCommandEvent &event)
{
	if (toolBar->GetToolEnabled(MNU_SAVE))
	{
		wxMessageDialog msg(this, _("There is unsaved data in a row.\nDo you want to store to the database?"), _("Unsaved data"),
		                    wxYES_NO | wxICON_QUESTION | wxCANCEL);
		switch (msg.ShowModal())
		{
			case wxID_YES:
			{
				if (!DoSave())
					return;
				break;
			}
			case wxID_CANCEL:
				return;
			case wxID_NO:
				CancelChange();
		}
	}

	dlgEditGridOptions *winOptions = new dlgEditGridOptions(this, connection, tableName, sqlGrid);
	if (winOptions->ShowModal())
		Go();
}


template < class T >
int ArrayCmp(T *a, T *b)
{
	if (*a == *b)
		return 0;

	if (*a > *b)
		return 1;
	else
		return -1;
}

void frmEditGrid::OnDelete(wxCommandEvent &event)
{
	// Don't bugger about with keypresses to the scratch pad.
	if (FindFocus() == scratchPad)
	{
		event.Skip();
		return;
	}

	if (editorCell->IsSet())
	{
		if (sqlGrid->GetTable()->IsColBoolean(sqlGrid->GetGridCursorCol()))
			return;

		if (sqlGrid->GetTable()->IsColText(sqlGrid->GetGridCursorCol()))
		{
			wxStyledTextCtrl *text = (wxStyledTextCtrl *)sqlGrid->GetCellEditor(sqlGrid->GetGridCursorRow(), sqlGrid->GetGridCursorCol())->GetControl();
			if (text && text->GetCurrentPos() <= text->GetTextLength())
			{
				if (text->GetSelectionStart() == text->GetSelectionEnd())
					text->SetSelection(text->GetSelectionStart(), text->GetSelectionStart() + 1);
				text->Clear();
			}
		}
		else
		{
			wxTextCtrl *text = (wxTextCtrl *)sqlGrid->GetCellEditor(sqlGrid->GetGridCursorRow(), sqlGrid->GetGridCursorCol())->GetControl();
			if (text)
			{
				long x, y;
				text->GetSelection(&x, &y);
				if (x != y)
					text->Remove(x, x + y + 1);
				else
					text->Remove(x, x + 1);
			}
		}
		return;
	}

	// If the delete button is disabled, don't try to delete anything
	if (!toolBar->GetToolEnabled(MNU_DELETE))
		return;

	wxArrayInt delrows = sqlGrid->GetSelectedRows();
	int i = delrows.GetCount();

	if (i == 0)
		return;

	wxString prompt;
	prompt.Printf(wxPLURAL("Are you sure you wish to delete the selected row?", "Are you sure you wish to delete the %d selected rows?", i), i);

	wxMessageDialog msg(this, prompt, _("Delete rows?"), wxYES_NO | wxICON_QUESTION);
	if (msg.ShowModal() != wxID_YES)
		return;

	sqlGrid->BeginBatch();

	// Sort the grid so we always delete last->first, otherwise we
	// could end up deleting anything because the array returned by
	// GetSelectedRows is in the order that rows were selected by
	// the user.
	delrows.Sort(ArrayCmp);

	// don't care a lot about optimizing here; doing it line by line
	// just as sqlTable::DeleteRows does
	bool show_continue_message = true;
	while (i--)
	{
		if (!sqlGrid->DeleteRows(delrows.Item(i), 1) &&
		        i > 0 &&
		        show_continue_message)
		{
			wxMessageDialog msg(this, wxString::Format(wxPLURAL(
			                        "There was an error deleting the previous record.\nAre you sure you wish to delete the remaining %d row?",
			                        "There was an error deleting the previous record.\nAre you sure you wish to delete the remaining %d rows?",
			                        i), i), _("Delete more records ?"), wxYES_NO | wxICON_QUESTION);
			if (msg.ShowModal() != wxID_YES)
				break;
			else
				show_continue_message = false;
		}
	}


	sqlGrid->EndBatch();

	SetStatusText(wxString::Format(wxPLURAL("%d row.", "%d rows.", sqlGrid->GetTable()->GetNumberStoredRows()), sqlGrid->GetTable()->GetNumberStoredRows()), 0);
}


void frmEditGrid::OnEditorShown(wxGridEvent &event)
{
	toolBar->EnableTool(MNU_SAVE, true);
	toolBar->EnableTool(MNU_UNDO, true);
	fileMenu->Enable(MNU_SAVE, true);
	editMenu->Enable(MNU_UNDO, true);
	editorCell->SetCell(event.GetRow(), event.GetCol());

	event.Skip();
}


void frmEditGrid::OnEditorHidden(wxGridEvent &event)
{
	editorCell->ClearCell();
}


void frmEditGrid::OnGridSelectCells(wxGridRangeSelectEvent &event)
{
	if (sqlGrid->GetEditable())
	{
		wxArrayInt rows = sqlGrid->GetSelectedRows();

		bool enable = rows.GetCount() > 0;
		if (enable)
		{
			wxCommandEvent nullEvent;
			OnSave(event);

			// check if a readonly line is selected
			int row, col;
			size_t i;

			for (i = 0 ; i < rows.GetCount() ; i++)
			{
				row = rows.Item(i);
				bool lineEnabled = false;

				if (row == sqlGrid->GetNumberRows() - 1)
				{
					// the (*) line may not be deleted/copied
					enable = false;
					break;
				}
				for (col = 0 ; col < sqlGrid->GetNumberCols() ; col++)
				{
					if (!sqlGrid->IsReadOnly(row, col))
					{
						lineEnabled = true;
						break;
					}
				}

				if (!lineEnabled)
				{
					enable = false;
					break;
				}
			}
		}
		toolBar->EnableTool(MNU_DELETE, enable);
		editMenu->Enable(MNU_DELETE, enable);
	}
	event.Skip();
}


void frmEditGrid::ShowForm(bool filter)
{
	bool abort = false;

	if (relkind == 'r' || relkind == 'v' || relkind == 'x')
	{
		if (filter)
		{
			dlgEditGridOptions *winOptions = new dlgEditGridOptions(this, connection, tableName, sqlGrid);
			abort = !(winOptions->ShowModal());
		}
		if (abort)
		{
			// Hack to ensure there's a table for ~wxGrid() to delete
			sqlGrid->CreateGrid(0, 0);
			sqlGrid->SetTable(0);
			Close();
			Destroy();
		}
		else
		{
			Show(true);
			Go();
		}
	}
	else
	{
		wxLogError(__("No Table or view."));
		// Hack to ensure there's a table for ~wxGrid() to delete
		sqlGrid->CreateGrid(0, 0);
		Close();
		Destroy();
	}
}

void frmEditGrid::Go()
{
	int col;
	long templong;

	if (cbLimit->GetValue() != wxT("") &&
	        cbLimit->GetValue() != _("No limit") &&
	        !cbLimit->GetValue().BeforeFirst(' ').ToLong(&templong))
	{
		wxLogError(_("The row limit must be an integer number or 'No limit'"));
		return;
	}

	if (cbLimit->GetValue() == _("No limit"))
		SetLimit(0);
	else
	{
		cbLimit->GetValue().BeforeFirst(' ').ToLong(&templong);
		SetLimit(templong);
	}

	// Check we have access
	if (connection->ExecuteScalar(wxT("SELECT count(*) FROM ") + tableName + wxT(" WHERE false")) == wxT(""))
		return;

	SetStatusText(_("Refreshing data, please wait."), 0);

	toolBar->EnableTool(MNU_REFRESH, false);
	viewMenu->Enable(MNU_REFRESH, false);
	toolBar->EnableTool(MNU_OPTIONS, false);
	toolsMenu->Enable(MNU_OPTIONS, false);
	toolsMenu->Enable(MNU_INCLUDEFILTER, false);
	toolsMenu->Enable(MNU_EXCLUDEFILTER, false);
	toolsMenu->Enable(MNU_REMOVEFILTERS, false);
	toolsMenu->Enable(MNU_ASCSORT, false);
	toolsMenu->Enable(MNU_DESCSORT, false);
	toolsMenu->Enable(MNU_REMOVESORT, false);

	// Stash the column sizes so we can reset them
	wxArrayInt colWidths;
	for (col = 0 ; col < sqlGrid->GetNumberCols() ; col++)
	{
		colWidths.Add(sqlGrid->GetColumnWidth(col));
	}

	wxString qry = wxT("SELECT ");
	if (hasOids)
		qry += wxT("oid, ");
	qry += wxT("* FROM ") + tableName;
	if (!rowFilter.IsEmpty())
	{
		qry += wxT(" WHERE ") + rowFilter;
	}
	if (!orderBy.IsEmpty())
	{
		qry += wxT("\n ORDER BY ") + orderBy;
	}
	if (limit > 0)
		qry += wxT(" LIMIT ") + wxString::Format(wxT("%i"), limit);

	thread = new pgQueryThread(connection, qry);
	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
		Abort();
		toolBar->EnableTool(MNU_REFRESH, true);
		viewMenu->Enable(MNU_REFRESH, true);
		toolBar->EnableTool(MNU_OPTIONS, true);
		toolsMenu->Enable(MNU_OPTIONS, true);
		toolsMenu->Enable(MNU_INCLUDEFILTER, true);
		toolsMenu->Enable(MNU_EXCLUDEFILTER, true);
		toolsMenu->Enable(MNU_REMOVEFILTERS, true);
		toolsMenu->Enable(MNU_ASCSORT, true);
		toolsMenu->Enable(MNU_DESCSORT, true);
		toolsMenu->Enable(MNU_REMOVESORT, true);

		return;
	}

	thread->Run();

	while (thread && thread->IsRunning())
	{
		wxTheApp->Yield(true);
		wxMilliSleep(10);
	}

	// Brute force check to ensure the user didn't get bored and close the window
	if (closing)
		return;

	if (!thread)
	{
		toolBar->EnableTool(MNU_REFRESH, true);
		viewMenu->Enable(MNU_REFRESH, true);
		toolBar->EnableTool(MNU_OPTIONS, true);
		toolsMenu->Enable(MNU_OPTIONS, true);
		toolsMenu->Enable(MNU_INCLUDEFILTER, true);
		toolsMenu->Enable(MNU_EXCLUDEFILTER, true);
		toolsMenu->Enable(MNU_REMOVEFILTERS, true);
		toolsMenu->Enable(MNU_ASCSORT, true);
		toolsMenu->Enable(MNU_DESCSORT, true);
		toolsMenu->Enable(MNU_REMOVESORT, true);
		return;
	}

	if (!thread->DataValid())
	{
		Abort();
		toolBar->EnableTool(MNU_REFRESH, true);
		viewMenu->Enable(MNU_REFRESH, true);
		toolBar->EnableTool(MNU_OPTIONS, true);
		toolsMenu->Enable(MNU_OPTIONS, true);
		toolsMenu->Enable(MNU_INCLUDEFILTER, true);
		toolsMenu->Enable(MNU_EXCLUDEFILTER, true);
		toolsMenu->Enable(MNU_REMOVEFILTERS, true);
		toolsMenu->Enable(MNU_ASCSORT, true);
		toolsMenu->Enable(MNU_DESCSORT, true);
		toolsMenu->Enable(MNU_REMOVESORT, true);
		return;
	}

	SetStatusText(wxString::Format(wxPLURAL("%d row.", "%d rows.", thread->DataSet()->NumRows()), thread->DataSet()->NumRows()), 0);

	sqlGrid->BeginBatch();

	// to force the grid to create scrollbars, we make sure the size  so small that scrollbars are needed
	// later, we will resize the grid's parent to force the correct size (now including scrollbars, even if
	// they are suppressed initially. Win32 won't need this.
	sqlGrid->SetSize(10, 10);

	sqlGrid->SetTable(new sqlTable(connection, thread, tableName, relid, hasOids, primaryKeyColNumbers, relkind), true);

	// Reset the column widths
	for (col = 0 ; col < sqlGrid->GetNumberCols() ; col++)
	{
		if ((col + 1) <= (int)colWidths.Count())
			sqlGrid->SetColumnWidth(col, colWidths[col]);
	}

	sqlGrid->EndBatch();

	toolBar->EnableTool(MNU_REFRESH, true);
	viewMenu->Enable(MNU_REFRESH, true);
	toolBar->EnableTool(MNU_OPTIONS, true);
	toolsMenu->Enable(MNU_OPTIONS, true);
	toolsMenu->Enable(MNU_INCLUDEFILTER, true);
	toolsMenu->Enable(MNU_EXCLUDEFILTER, true);
	toolsMenu->Enable(MNU_REMOVEFILTERS, true);
	toolsMenu->Enable(MNU_ASCSORT, true);
	toolsMenu->Enable(MNU_DESCSORT, true);
	toolsMenu->Enable(MNU_REMOVESORT, true);

	manager.Update();

	if (!hasOids && primaryKeyColNumbers.IsEmpty() && relkind == 'r')
		frmHint::ShowHint(this, HINT_READONLY_NOPK, tableName);

	// Set the thread variable to zero so we don't try to
	// abort it if the user cancels now.
	thread = 0;
}


frmEditGrid::~frmEditGrid()
{
	closing = true;

	mainForm->RemoveFrame(this);

	settings->Write(wxT("frmEditGrid/Perspective-") + wxString(FRMEDITGRID_PERSPECTIVE_VER), manager.SavePerspective());
	manager.UnInit();

	if (connection)
		delete connection;
}


void frmEditGrid::Abort()
{
	if (sqlGrid->GetTable())
	{
		sqlGrid->HideCellEditControl();
		sqlGrid->SetTable(0);
	}

	if (thread)
	{
		SetStatusText(_("aborting."), 0);
		if (thread->IsRunning())
			thread->Delete();
		delete thread;
		thread = 0;
	}
}


ctlSQLEditGrid::ctlSQLEditGrid(wxFrame *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
	: ctlSQLGrid(parent, id, pos, size)
{
}

bool ctlSQLEditGrid::CheckRowPresent(int row)
{
	return GetTable()->CheckInCache(row);
}

void ctlSQLEditGrid::ResizeEditor(int row, int col)
{

	if (GetTable()->needsResizing(col))
	{
		wxGridCellAttr *attr = GetCellAttr(row, col);
		wxGridCellRenderer *renderer = attr->GetRenderer(this, row, col);
		if ( renderer )
		{
			wxClientDC dc(GetGridWindow());
			wxSize size = renderer->GetBestSize(*this, *attr, dc, row, col);
			renderer->DecRef();

			int w = wxMax(size.GetWidth(), 15) + 20;
			int h = wxMax(size.GetHeight(), 15) + 20;


			wxGridCellEditor *editor = attr->GetEditor(this, row, col);
			if (editor)
			{
				wxRect cellRect = CellToRect(m_currentCellCoords);
				wxRect rect = cellRect;
				rect.SetWidth(w);
				rect.SetHeight(h);

				// we might have scrolled
				CalcUnscrolledPosition(0, 0, &w, &h);
				rect.SetLeft(rect.GetLeft() - w);
				rect.SetTop(rect.GetTop() - h);

				// Clip rect to client size
				GetClientSize(&w, &h);
				rect.SetRight(wxMin(rect.GetRight(), w));
				rect.SetBottom(wxMin(rect.GetBottom(), h));

				// but not smaller than original cell
				rect.SetWidth(wxMax(cellRect.GetWidth(), rect.GetWidth()));
				rect.SetHeight(wxMax(cellRect.GetHeight(), rect.GetHeight()));

				editor->SetSize(rect);
				editor->DecRef();
			}
		}

		attr->DecRef();
	}
}

wxArrayInt ctlSQLEditGrid::GetSelectedRows() const
{
	wxArrayInt rows, rows2;

	wxGridCellCoordsArray tl = GetSelectionBlockTopLeft(), br = GetSelectionBlockBottomRight();

	int maxCol = ((ctlSQLEditGrid *)this)->GetNumberCols() - 1;
	size_t i;
	for (i = 0 ; i < tl.GetCount() ; i++)
	{
		wxGridCellCoords c1 = tl.Item(i), c2 = br.Item(i);
		if (c1.GetCol() != 0 || c2.GetCol() != maxCol)
			return rows2;

		int j;
		for (j = c1.GetRow() ; j <= c2.GetRow() ; j++)
			rows.Add(j);
	}

	rows2 = wxGrid::GetSelectedRows();

	rows.Sort(ArrayCmp);
	rows2.Sort(ArrayCmp);

	size_t i2 = 0, cellRowMax = rows.GetCount();

	for (i = 0 ; i < rows2.GetCount() ; i++)
	{
		int row = rows2.Item(i);
		while (i2 < cellRowMax && rows.Item(i2) < row)
			i2++;
		if (i2 == cellRowMax || row != rows.Item(i2))
			rows.Add(row);
	}

	return rows;
}


class sqlGridTextEditor : public wxGridCellTextEditor
{
public:
	virtual wxGridCellEditor *Clone() const
	{
		return new sqlGridTextEditor();
	}
	void Create(wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler);
	void BeginEdit(int row, int col, wxGrid *grid);
	bool EndEdit(int row, int col, wxGrid *grid);
	wxString GetValue() const;
	virtual void Reset()
	{
		DoReset(m_startValue);
	}
	void StartingKey(wxKeyEvent &event);

protected:
	void DoBeginEdit(const wxString &startValue);
	wxStyledTextCtrl *Text() const
	{
		return (wxStyledTextCtrl *)m_control;
	}
	void DoReset(const wxString &startValue);

	wxString m_startValue;
};




void sqlGridTextEditor::Create(wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler)
{
	int flags = wxSTC_WRAP_NONE;

	m_control = new wxStyledTextCtrl(parent, id,
	                                 wxDefaultPosition, wxDefaultSize, flags
	                                );

	wxGridCellEditor::Create(parent, id, evtHandler);
}


void sqlGridTextEditor::BeginEdit(int row, int col, wxGrid *grid)
{
	m_startValue = grid->GetTable()->GetValue(row, col);
	DoBeginEdit(m_startValue);
	((ctlSQLEditGrid *)grid)->ResizeEditor(row, col);
}

void sqlGridTextEditor::DoBeginEdit(const wxString &startValue)
{
	Text()->SetMarginWidth(1, 0);
	Text()->SetText(startValue);
	Text()->SetCurrentPos(Text()->GetTextLength());
	Text()->SetUseHorizontalScrollBar(false);
	Text()->SetUseVerticalScrollBar(false);
	Text()->SetSelection(0, -1);
	Text()->SetFocus();
}

bool sqlGridTextEditor::EndEdit(int row, int col, wxGrid *grid)
{
	bool changed = false;
	wxString value = Text()->GetText();

	if (value != m_startValue)
		changed = true;

	if (changed)
		grid->GetTable()->SetValue(row, col, value);

	return changed;
}

wxString sqlGridTextEditor::GetValue() const
{
	return Text()->GetText();
}

void sqlGridTextEditor::DoReset(const wxString &startValue)
{
	Text()->SetText(startValue);
	Text()->SetSelection(-1, -1);
}

void sqlGridTextEditor::StartingKey(wxKeyEvent &event)
{
	wxChar ch;

#if wxUSE_UNICODE
	ch = event.GetUnicodeKey();
	if (ch <= 127)
		ch = (wxChar)event.GetKeyCode();
#else
	ch = (wxChar)event.GetKeyCode();
#endif

	if (ch != (wxChar)WXK_BACK)
	{
		Text()->SetText(ch);
		Text()->GotoPos(Text()->GetLength());
	}
}

class sqlGridNumericEditor : public wxGridCellTextEditor
{
public:
	sqlGridNumericEditor(int len = -1, int prec = -1)
	{
		numlen = len;
		numprec = prec;
	}
	virtual wxGridCellEditor *Clone() const
	{
		return new sqlGridNumericEditor(numlen, numprec);
	}
	virtual void Create(wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler);

	virtual bool IsAcceptedKey(wxKeyEvent &event);
	virtual void BeginEdit(int row, int col, wxGrid *grid);
	virtual bool EndEdit(int row, int col, wxGrid *grid);

	virtual void Reset()
	{
		DoReset(m_startValue);
	}
	virtual void StartingKey(wxKeyEvent &event);
	virtual void SetParameters(const wxString &params);

protected:
	int numlen, numprec;
	wxString m_startValue;

};



void sqlGridNumericEditor::StartingKey(wxKeyEvent &event)
{
	int keycode = event.GetKeyCode();
	bool allowed = false;

	switch (keycode)
	{
		case WXK_DECIMAL:
		case WXK_NUMPAD_DECIMAL:
		case '.':
			if (numprec)
				allowed = true;
			break;
		case '+':
		case WXK_ADD:
		case WXK_NUMPAD_ADD:
		case '-':
		case WXK_SUBTRACT:
		case WXK_NUMPAD_SUBTRACT:

		case WXK_NUMPAD0:
		case WXK_NUMPAD1:
		case WXK_NUMPAD2:
		case WXK_NUMPAD3:
		case WXK_NUMPAD4:
		case WXK_NUMPAD5:
		case WXK_NUMPAD6:
		case WXK_NUMPAD7:
		case WXK_NUMPAD8:
		case WXK_NUMPAD9:
			allowed = true;
			break;
		default:
			if (wxIsdigit(keycode))
				allowed = true;
			break;

	}
	if (allowed)
		wxGridCellTextEditor::StartingKey(event);
	else
		event.Skip();
}



bool sqlGridNumericEditor::IsAcceptedKey(wxKeyEvent &event)
{
	if ( wxGridCellEditor::IsAcceptedKey(event) )
	{
		int keycode = event.GetKeyCode();
		switch ( keycode )
		{
			case WXK_DECIMAL:
			case WXK_NUMPAD_DECIMAL:
				return (numprec != 0);

			case '+':
			case WXK_ADD:
			case WXK_NUMPAD_ADD:
			case '-':
			case WXK_SUBTRACT:
			case WXK_NUMPAD_SUBTRACT:

			case WXK_NUMPAD0:
			case WXK_NUMPAD1:
			case WXK_NUMPAD2:
			case WXK_NUMPAD3:
			case WXK_NUMPAD4:
			case WXK_NUMPAD5:
			case WXK_NUMPAD6:
			case WXK_NUMPAD7:
			case WXK_NUMPAD8:
			case WXK_NUMPAD9:
				return true;
			default:
				return wxIsdigit(keycode) != 0;
		}
	}

	return false;
}



void sqlGridNumericEditor::BeginEdit(int row, int col, wxGrid *grid)
{
	m_startValue = grid->GetTable()->GetValue(row, col);


	wxString value = m_startValue;
	// localize value here

	DoBeginEdit(value);
}



bool sqlGridNumericEditor::EndEdit(int row, int col, wxGrid *grid)
{
	wxASSERT_MSG(m_control,
	             wxT("The sqlGridNumericEditor must be Created first!"));

	bool changed = false;
	wxString value = Text()->GetValue();

	// de-localize value here

	if (value != m_startValue)
		changed = true;

	if (changed)
		grid->GetTable()->SetValue(row, col, value);

	m_startValue = wxEmptyString;
	Text()->SetValue(m_startValue);

	return changed;
}


void sqlGridNumericEditor::SetParameters(const wxString &params)
{
	if ( !params )
	{
		// reset to default
		numlen = -1;
		numprec = -1;
	}
	else
	{
		long tmp;
		if ( params.BeforeFirst(wxT(',')).ToLong(&tmp) )
		{
			numlen = (int)tmp;

			if ( params.AfterFirst(wxT(',')).ToLong(&tmp) )
			{
				numprec = (int)tmp;

				// skip the error message below
				return;
			}
		}
	}
}


void sqlGridNumericEditor::Create(wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler)
{
	m_control = new wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize);

	wxGridCellEditor::Create(parent, id, evtHandler);
	Text()->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
}


//////////////////////////////////////////////////////////////////////
// Bool editor
//////////////////////////////////////////////////////////////////////

class sqlGridBoolEditor : public wxGridCellEditor
{
public:
	sqlGridBoolEditor() { }

	virtual void Create(wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler);

	virtual void SetSize(const wxRect &rect);
	virtual void Show(bool show, wxGridCellAttr *attr = (wxGridCellAttr *)NULL);

	virtual bool IsAcceptedKey(wxKeyEvent &event);
	virtual void BeginEdit(int row, int col, wxGrid *grid);
	virtual bool EndEdit(int row, int col, wxGrid *grid);

	virtual void Reset();
	virtual void StartingClick();
	virtual void StartingKey(wxKeyEvent &event);

	virtual wxGridCellEditor *Clone() const
	{
		return new sqlGridBoolEditor;
	}

	virtual wxString GetValue() const;

protected:
	wxCheckBox *CBox() const
	{
		return (wxCheckBox *)m_control;
	}

private:
	wxCheckBoxState m_startValue;

	DECLARE_NO_COPY_CLASS(sqlGridBoolEditor)
};


void sqlGridBoolEditor::Create(wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler)
{
	m_control = new wxCheckBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);

	wxGridCellEditor::Create(parent, id, evtHandler);
}

void sqlGridBoolEditor::SetSize(const wxRect &r)
{
	bool resize = false;
	wxSize size = m_control->GetSize();
	wxCoord minSize = wxMin(r.width, r.height);

	// check if the checkbox is not too big/small for this cell
	wxSize sizeBest = m_control->GetBestSize();
	if ( !(size == sizeBest) )
	{
		// reset to default size if it had been made smaller
		size = sizeBest;

		resize = true;
	}

	if ( size.x >= minSize || size.y >= minSize )
	{
		// leave 1 pixel margin
		size.x = size.y = minSize - 2;

		resize = true;
	}

	if ( resize )
	{
		m_control->SetSize(size);
	}

	// position it in the centre of the rectangle (TODO: support alignment?)

#if defined(__WXGTK__)
	// the checkbox without label still has some space to the right in wxGTK,
	// so shift it to the right
	size.x -= 8;
#elif defined(__WXMSW__)
	// here too, but in other way
	size.x += 1;
	size.y -= 2;
#endif

	int hAlign = wxALIGN_CENTRE;
	int vAlign = wxALIGN_CENTRE;
	if (GetCellAttr())
		GetCellAttr()->GetAlignment(& hAlign, & vAlign);

	int x = 0, y = 0;
	if (hAlign == wxALIGN_LEFT)
	{
		x = r.x + 2;
#ifdef __WXMSW__
		x += 2;
#endif
		y = r.y + r.height / 2 - size.y / 2;
	}
	else if (hAlign == wxALIGN_RIGHT)
	{
		x = r.x + r.width - size.x - 2;
		y = r.y + r.height / 2 - size.y / 2;
	}
	else if (hAlign == wxALIGN_CENTRE)
	{
		x = r.x + r.width / 2 - size.x / 2;
		y = r.y + r.height / 2 - size.y / 2;
	}

	m_control->Move(x, y);
}

void sqlGridBoolEditor::Show(bool show, wxGridCellAttr *attr)
{
	m_control->Show(show);

	if ( show )
	{
		wxColour colBg = attr ? attr->GetBackgroundColour() : *wxLIGHT_GREY;
		CBox()->SetBackgroundColour(colBg);
	}
}

void sqlGridBoolEditor::BeginEdit(int row, int col, wxGrid *grid)
{
	wxASSERT_MSG(m_control, wxT("The sqlGridBoolEditor must be Created first!"));

	wxString value = grid->GetTable()->GetValue(row, col);
	if (value == wxT("TRUE"))
		m_startValue = wxCHK_CHECKED;
	else if (value == wxT("FALSE"))
		m_startValue = wxCHK_UNCHECKED;
	else
		m_startValue = wxCHK_UNDETERMINED;

	CBox()->Set3StateValue(m_startValue);
	CBox()->SetFocus();
}

bool sqlGridBoolEditor::EndEdit(int row, int col, wxGrid *grid)
{
	wxASSERT_MSG(m_control, wxT("The sqlGridBoolEditor must be Created first!"));

	bool changed = false;
	wxCheckBoxState value = CBox()->Get3StateValue();
	if ( value != m_startValue )
		changed = true;

	if ( changed )
	{
		switch (value)
		{
			case wxCHK_UNCHECKED:
				grid->GetTable()->SetValue(row, col, wxT("FALSE"));
				break;
			case wxCHK_CHECKED:
				grid->GetTable()->SetValue(row, col, wxT("TRUE"));
				break;
			case wxCHK_UNDETERMINED:
				grid->GetTable()->SetValue(row, col, wxEmptyString);
				break;
		}
	}

	return changed;
}

void sqlGridBoolEditor::Reset()
{
	wxASSERT_MSG(m_control, wxT("The wxGridCellEditor must be Created first!"));

	CBox()->Set3StateValue(m_startValue);
}

void sqlGridBoolEditor::StartingClick()
{
	// We used to cycle the value on click here but
	// that can lead to odd behaviour of the cell.
	// Without cycling here, the checkbox is displayed
	// but the user must toggle the box itself - she
	// cannot just keep clicking the cell.
}

bool sqlGridBoolEditor::IsAcceptedKey(wxKeyEvent &event)
{
	if ( wxGridCellEditor::IsAcceptedKey(event) )
	{
		int keycode = event.GetKeyCode();
		switch ( keycode )
		{
			case WXK_SPACE:
			case '+':
			case '-':
			case 'n':
			case 'N':
				return true;
		}
	}

	return false;
}

void sqlGridBoolEditor::StartingKey(wxKeyEvent &event)
{
	int keycode = event.GetKeyCode();
	wxCheckBoxState value = CBox()->Get3StateValue();

	switch ( keycode )
	{
		case WXK_SPACE:
			switch (value)
			{
				case wxCHK_UNCHECKED:
				case wxCHK_UNDETERMINED:
					CBox()->Set3StateValue(wxCHK_CHECKED);
					break;
				case wxCHK_CHECKED:
					CBox()->Set3StateValue(wxCHK_UNCHECKED);
					break;
			}
			break;

		case '+':
			CBox()->Set3StateValue(wxCHK_CHECKED);
			break;

		case '-':
			CBox()->Set3StateValue(wxCHK_UNCHECKED);
			break;

		case 'n':
		case 'N':
			CBox()->Set3StateValue(wxCHK_UNDETERMINED);
			break;
	}
}


// return the value as "1" for true and the empty string for false
wxString sqlGridBoolEditor::GetValue() const
{

	wxCheckBoxState value = CBox()->Get3StateValue();

	switch (value)
	{
		case wxCHK_UNCHECKED:
			return wxT("FALSE");
			break;
		case wxCHK_CHECKED:
			return wxT("TRUE");
			break;
		case wxCHK_UNDETERMINED:
			return wxEmptyString;
			break;
	}
	return wxEmptyString;
}

//////////////////////////////////////////////////////////////////////
// End Bool editor
//////////////////////////////////////////////////////////////////////


sqlTable::sqlTable(pgConn *conn, pgQueryThread *_thread, const wxString &tabName, const OID _relid, bool _hasOid, const wxString &_pkCols, char _relkind)
{
	connection = conn;
	primaryKeyColNumbers = _pkCols;
	relid = _relid;
	relkind = _relkind;
	tableName = tabName;
	hasOids = _hasOid;
	thread = _thread;

	rowsCached = 0;
	rowsAdded = 0;
	rowsStored = 0;
	rowsDeleted = 0;


	dataPool = 0;
	addPool = new cacheLinePool(500);       // arbitrary initial size
	lastRow = -1;
	int i;
	lineIndex = 0;

	nRows = thread->DataSet()->NumRows();
	nCols = thread->DataSet()->NumCols();

	columns = new sqlCellAttr[nCols];
	savedLine.cols = new wxString[nCols];

	// Get the "real" column list, including any dropped columns, as
	// key positions etc do not ignore these.
	pgSet *allColsSet = connection->ExecuteSet(
	                        wxT("SELECT attisdropped FROM pg_attribute")
	                        wxT(" WHERE attnum > 0 AND attrelid=") + NumToStr(relid) + wxT("::oid\n")
	                        wxT(" ORDER BY attnum"));

	int x = 1;
	if (allColsSet)
	{
		for (i = 0; i < allColsSet->NumRows(); i++)
		{
			if (allColsSet->GetVal(wxT("attisdropped")) == wxT("t"))
			{
				colMap.Add(0);
			}
			else
			{
				colMap.Add(x);
				x++;
			}
			allColsSet->MoveNext();
		}
	}

	pgSet *colSet = connection->ExecuteSet(
	                    wxT("SELECT n.nspname AS nspname, relname, format_type(t.oid,NULL) AS typname, format_type(t.oid, att.atttypmod) AS displaytypname, ")
	                    wxT("nt.nspname AS typnspname, attname, attnum, COALESCE(b.oid, t.oid) AS basetype, atthasdef, adsrc,\n")
	                    wxT("       CASE WHEN t.typbasetype::oid=0 THEN att.atttypmod else t.typtypmod END AS typmod,\n")
	                    wxT("       CASE WHEN t.typbasetype::oid=0 THEN att.attlen else t.typlen END AS typlen\n")
	                    wxT("  FROM pg_attribute att\n")
	                    wxT("  JOIN pg_type t ON t.oid=att.atttypid\n")
	                    wxT("  JOIN pg_namespace nt ON nt.oid=t.typnamespace\n")
	                    wxT("  JOIN pg_class c ON c.oid=attrelid\n")
	                    wxT("  JOIN pg_namespace n ON n.oid=relnamespace\n")
	                    wxT("  LEFT OUTER JOIN pg_type b ON b.oid=t.typbasetype\n")
	                    wxT("  LEFT OUTER JOIN pg_attrdef def ON adrelid=attrelid AND adnum=attnum\n")
	                    wxT(" WHERE attnum > 0 AND NOT attisdropped AND attrelid=") + NumToStr(relid) + wxT("::oid\n")
	                    wxT(" ORDER BY attnum"));



	bool canInsert = false;
	if (colSet)
	{
		if (hasOids)
		{
			columns[0].name = wxT("oid");
			columns[0].numeric = true;
			columns[0].attr->SetReadOnly(true);
			columns[0].type = PGTYPCLASS_NUMERIC;
		}

		for (i = (hasOids ? 1 : 0) ; i < nCols ; i++)
		{
			wxGridCellEditor *editor = 0;

			columns[i].name = colSet->GetVal(wxT("attname"));
			columns[i].typeName = colSet->GetVal(wxT("typname"));
			columns[i].displayTypeName = colSet->GetVal(wxT("displaytypname"));

			// Special case for character datatypes. We always cast them to text to avoid
			// truncation issues with casts like ::character(3)
			if (columns[i].typeName == wxT("character") || columns[i].typeName == wxT("character varying") || columns[i].typeName == wxT("\"char\""))
				columns[i].typeName = wxT("text");

			columns[i].type = (Oid)colSet->GetOid(wxT("basetype"));
			if ((columns[i].type == PGOID_TYPE_INT4 || columns[i].type == PGOID_TYPE_INT8)
			        && colSet->GetBool(wxT("atthasdef")))
			{
				wxString adsrc = colSet->GetVal(wxT("adsrc"));
				if (adsrc ==  wxT("nextval('") + colSet->GetVal(wxT("relname")) + wxT("_") + columns[i].name + wxT("_seq'::text)") ||
				        adsrc ==  wxT("nextval('") + colSet->GetVal(wxT("nspname")) + wxT(".") + colSet->GetVal(wxT("relname")) + wxT("_") + columns[i].name + wxT("_seq'::text)") ||
				        adsrc ==  wxT("nextval('") + colSet->GetVal(wxT("relname")) + wxT("_") + columns[i].name + wxT("_seq'::regclass)") ||
				        adsrc ==  wxT("nextval('") + colSet->GetVal(wxT("nspname")) + wxT(".") + colSet->GetVal(wxT("relname")) + wxT("_") + columns[i].name + wxT("_seq'::regclass)"))
				{
					if (columns[i].type == PGOID_TYPE_INT4)
						columns[i].type = (Oid)PGOID_TYPE_SERIAL;
					else
						columns[i].type = (Oid)PGOID_TYPE_SERIAL8;
				}
			}
			columns[i].typlen = colSet->GetLong(wxT("typlen"));
			columns[i].typmod = colSet->GetLong(wxT("typmod"));

			switch (columns[i].type)
			{
				case PGOID_TYPE_BOOL:
					columns[i].numeric = false;
					columns[i].attr->SetReadOnly(false);
					editor = new sqlGridBoolEditor();
					break;
				case PGOID_TYPE_INT8:
				case PGOID_TYPE_SERIAL8:
					SetNumberEditor(i, 20);
					break;
				case PGOID_TYPE_INT2:
					SetNumberEditor(i, 5);
					break;
				case PGOID_TYPE_INT4:
				case PGOID_TYPE_SERIAL:
					SetNumberEditor(i, 10);
					break;
				case PGOID_TYPE_OID:
				case PGOID_TYPE_TID:
				case PGOID_TYPE_XID:
				case PGOID_TYPE_CID:
					SetNumberEditor(i, 10);
					break;
				case PGOID_TYPE_FLOAT4:
				case PGOID_TYPE_FLOAT8:
					columns[i].numeric = true;
					columns[i].attr->SetReadOnly(false);
					editor = new sqlGridNumericEditor();
					break;
				case PGOID_TYPE_MONEY:
					columns[i].numeric = false;
					columns[i].attr->SetReadOnly(false);
					editor = new wxGridCellTextEditor();
					break;
				case PGOID_TYPE_NUMERIC:
				{
					columns[i].numeric = true;
					columns[i].attr->SetReadOnly(false);
					int len = columns[i].size();
					int prec = columns[i].precision();
					if (prec > 0)
						len -= (prec);
					editor = new sqlGridNumericEditor(len, prec);
					break;
				}
				case PGOID_TYPE_BYTEA:
					columns[i].numeric = false;
					columns[i].attr->SetReadOnly(true);
					break;
				case PGOID_TYPE_DATE:
				case PGOID_TYPE_TIME:
				case PGOID_TYPE_TIMETZ:
				case PGOID_TYPE_TIMESTAMP:
				case PGOID_TYPE_TIMESTAMPTZ:
				case PGOID_TYPE_INTERVAL:
					columns[i].numeric = false;
					columns[i].attr->SetReadOnly(false);
					editor = new sqlGridTextEditor();
					break;
				case PGOID_TYPE_CHAR:
				case PGOID_TYPE_NAME:
				case PGOID_TYPE_TEXT:
				default:
					columns[i].numeric = false;
					columns[i].attr->SetReadOnly(false);
					columns[i].needResize = true;
					editor = new sqlGridTextEditor();
					break;
			}
			if (editor)
				columns[i].attr->SetEditor(editor);

			if (relkind != 'r' || (!hasOids && primaryKeyColNumbers.IsNull()))
			{
				// for security reasons, we need oid or pk to enable updates. If none is available,
				// the table definition can be considered faulty.
				columns[i].attr->SetReadOnly(true);
			}
			if (!columns[i].attr->IsReadOnly())
				canInsert = true;

			wxStringTokenizer collist(primaryKeyColNumbers, wxT(","));
			long cn = 0;
			long attnum = colSet->GetLong(wxT("attnum"));

			while (cn < attnum && collist.HasMoreTokens())
			{
				cn = StrToLong(collist.GetNextToken());
				if (cn == attnum)
					columns[i].isPrimaryKey = true;
			}

			colSet->MoveNext();
		}
		delete colSet;
	}
	else
	{
		// um, we never should reach here because this would mean
		// that datatypes are unreadable.
		// *if* we reach here, namespace info is missing.
		for (i = 0 ; i < nCols ; i++)
		{
			columns[i].typeName = thread->DataSet()->ColType(i);
			columns[i].name = thread->DataSet()->ColName(i);
		}
	}

	if (nRows)
	{
		dataPool = new cacheLinePool(nRows);
		lineIndex = new int[nRows];
		for (i = 0 ; i < nRows ; i++)
			lineIndex[i] = i;
	}

	if (canInsert)
	{
		// an empty line waiting for inserts
		rowsAdded = 1;
	}
}


sqlTable::~sqlTable()
{
	if (thread)
		delete thread;
	if (dataPool)
		delete dataPool;

	delete addPool;

	delete[] columns;

	if (lineIndex)
		delete[] lineIndex;
}


int sqlTable::GetNumberCols()
{
	return nCols;
}


int sqlTable::GetNumberRows()
{
	return nRows + rowsAdded - rowsDeleted;
}


int sqlTable::GetNumberStoredRows()
{
	return nRows + rowsStored - rowsDeleted;
}


bool ctlSQLEditGrid::IsColText(int col)
{
	return GetTable()->IsColText(col);
}

bool sqlTable::IsColText(int col)
{
	return !columns[col].numeric && !(columns[col].type == PGOID_TYPE_BOOL);
}

bool sqlTable::IsColBoolean(int col)
{
	return (columns[col].type == PGOID_TYPE_BOOL);
}

wxString sqlTable::GetColLabelValue(int col)
{
	wxString label = columns[col].name + wxT("\n");
	if (columns[col].isPrimaryKey)
		label += wxT("[PK] ");

	switch (columns[col].type)
	{
		case (Oid)PGOID_TYPE_SERIAL:
			label += wxT("serial");
			break;
		case (Oid)PGOID_TYPE_SERIAL8:
			label += wxT("bigserial");
			break;
		default:
			label += columns[col].displayTypeName;
			break;
	}
	return label;
}


wxString sqlTable::GetColLabelValueUnformatted(int col)
{
	return columns[col].name;
}


wxString sqlTable::GetRowLabelValue(int row)
{
	wxString label;
	if (row < nRows - rowsDeleted || GetLine(row)->stored)
		label.Printf(wxT("%d"), row + 1);
	else
		label = wxT("*");
	return label;
}



void sqlTable::SetNumberEditor(int col, int len)
{
	columns[col].numeric = true;
	columns[col].attr->SetReadOnly(false);
	columns[col].attr->SetEditor(new sqlGridNumericEditor(len, 0));
}


bool sqlTable::CheckInCache(int row)
{
	if (row > nRows - rowsDeleted + rowsAdded)
		return false;
	if (row >= nRows - rowsDeleted)
		return true;

	return dataPool->IsFilled(row);
}


cacheLine *sqlTable::GetLine(int row)
{
	cacheLine *line;
	if (row < nRows - rowsDeleted)
		line = dataPool->Get(lineIndex[row]);
	else
		line = addPool->Get(row - (nRows - rowsDeleted));

	return line;
}



wxString sqlTable::MakeKey(cacheLine *line)
{
	wxString whereClause;
	if (!primaryKeyColNumbers.IsEmpty())
	{
		wxStringTokenizer collist(primaryKeyColNumbers, wxT(","));
		long cn;
		int offset;

		if (hasOids)
			offset = 0;
		else
			offset = 1;

		while (collist.HasMoreTokens())
		{
			cn = StrToLong(collist.GetNextToken());

			// Translate the column location to the real location in the actual columns still present
			cn = colMap[cn - 1];

			wxString colval = line->cols[cn - offset];
			if (colval.IsEmpty())
				return wxEmptyString;

			if (colval == wxT("''") && columns[cn - offset].typeName == wxT("text"))
				colval = wxEmptyString;

			if (!whereClause.IsEmpty())
				whereClause += wxT(" AND ");
			whereClause += qtIdent(columns[cn - offset].name) + wxT(" = ") + connection->qtDbString(colval);

			if (columns[cn - offset].typeName != wxT(""))
			{
				whereClause += wxT("::");
				whereClause += columns[cn - offset].displayTypeName;
			}
		}
	}
	else if (hasOids)
		whereClause = wxT("oid = ") + line->cols[0];

	return whereClause;
}



void sqlTable::UndoLine(int row)
{
	if (lastRow >= 0 && row >= 0)
	{
		cacheLine *line = GetLine(row);
		if (line)
		{
			int i;
			for (i = 0 ; i < nCols ; i++)
				line->cols[i] = savedLine.cols[i];
			ctlMenuToolbar *tb = (ctlMenuToolbar *)((wxFrame *)GetView()->GetParent())->GetToolBar();
			if (tb)
			{
				tb->EnableTool(MNU_SAVE, false);
				tb->EnableTool(MNU_UNDO, false);
			}
			wxMenu *fm = ((frmEditGrid *)GetView()->GetParent())->GetFileMenu();
			if (fm)
				fm->Enable(MNU_SAVE, false);
			wxMenu *em = ((frmEditGrid *)GetView()->GetParent())->GetEditMenu();
			if (em)
				em->Enable(MNU_UNDO, false);
		}
	}
	lastRow = -1;
}


bool sqlTable::StoreLine()
{
	bool done = false;

	GetView()->BeginBatch();
	if (lastRow >= 0)
	{
		cacheLine *line = GetLine(lastRow);

		int i;
		wxString colList, valList;

		if (line->stored)
		{
			// UPDATE

			for (i = (hasOids ? 1 : 0) ; i < nCols ; i++)
			{
				if (savedLine.cols[i] != line->cols[i])
				{
					if (!valList.IsNull())
						valList += wxT(", ");
					valList += qtIdent(columns[i].name) + wxT("=") + columns[i].Quote(connection, line->cols[i]);
				}
			}

			if (valList.IsEmpty())
				done = true;
			else
			{
				wxString key = MakeKey(&savedLine);
				wxASSERT(!key.IsEmpty());
				done = connection->ExecuteVoid(wxT(
				                                   "UPDATE ") + tableName + wxT(
				                                   " SET ") + valList + wxT(
				                                   " WHERE ") + key);
			}
		}
		else
		{
			// INSERT

			for (i = 0 ; i < nCols ; i++)
			{
				if (!columns[i].attr->IsReadOnly() && !line->cols[i].IsEmpty())
				{
					if (!colList.IsNull())
					{
						valList += wxT(", ");
						colList += wxT(", ");
					}
					colList += qtIdent(columns[i].name);

					valList += columns[i].Quote(connection, line->cols[i]);
				}
			}

			if (!valList.IsEmpty())
			{
				pgSet *set = connection->ExecuteSet(
				                 wxT("INSERT INTO ") + tableName
				                 + wxT("(") + colList
				                 + wxT(") VALUES (") + valList
				                 + wxT(")"));
				if (set)
				{
					if (set->GetInsertedCount() > 0)
					{
						if (hasOids)
							line->cols[0] = NumToStr((long)set->GetInsertedOid());
						delete set;

						done = true;
						rowsStored++;
						((wxFrame *)GetView()->GetParent())->SetStatusText(wxString::Format(wxT("%d rows."), GetNumberStoredRows()));
						if (rowsAdded == rowsStored)
							GetView()->AppendRows();

						// Read back what we inserted to get default vals
						wxString key = MakeKey(line);

						if (key.IsEmpty())
						{
							// That's a problem: obviously, the key generated isn't present
							// because it's serial or default or otherwise generated in the backend
							// we don't get.
							// That's why the whole line is declared readonly.

							line->readOnly = true;
						}
						else
						{
							set = connection->ExecuteSet(
							          wxT("SELECT * FROM ") + tableName +
							          wxT(" WHERE ") + key);
							if (set)
							{
								for (i = (hasOids ? 1 : 0) ; i < nCols ; i++)
								{
									line->cols[i] = set->GetVal(columns[i].name);
								}
								delete set;
							}
						}
					}
				}
			}
		}
		if (done)
		{
			line->stored = true;
			lastRow = -1;
		}
		else
			GetView()->SelectRow(lastRow);
	}

	GetView()->EndBatch();

	return done;
}


void sqlTable::SetValue(int row, int col, const wxString &value)
{
	cacheLine *line = GetLine(row);

	if (!line)
	{
		// Bad problem, no line!
		return;
	}


	if (row != lastRow)
	{
		if (lastRow >= 0)
			StoreLine();

		if (!line->cols)
			line->cols = new wxString[nCols];

		// remember line contents for later reference in update ... where
		int i;
		for (i = 0 ; i < nCols ; i++)
			savedLine.cols[i] = line->cols[i];
		lastRow = row;
	}
	ctlMenuToolbar *tb = (ctlMenuToolbar *)((wxFrame *)GetView()->GetParent())->GetToolBar();
	if (tb)
	{
		tb->EnableTool(MNU_SAVE, true);
		tb->EnableTool(MNU_UNDO, true);
	}
	wxMenu *fm = ((frmEditGrid *)GetView()->GetParent())->GetFileMenu();
	if (fm)
		fm->Enable(MNU_SAVE, true);
	wxMenu *em = ((frmEditGrid *)GetView()->GetParent())->GetEditMenu();
	if (em)
		em->Enable(MNU_UNDO, true);
	line->cols[col] = value;
}



wxString sqlTable::GetValue(int row, int col)
{
	wxString val;
	cacheLine *line;
	if (row < nRows - rowsDeleted)
		line = dataPool->Get(lineIndex[row]);
	else
		line = addPool->Get(row - (nRows - rowsDeleted));

	if (!line)
	{
		// Bad problem, no line!
		return val;
	}

	if (!line->cols)
	{
		line->cols = new wxString[nCols];
		if (row < nRows - rowsDeleted)
		{
			if (!thread)
			{
				wxLogError(__("Unexpected empty cache line: dataSet already closed."));
				return val;
			}

			line->stored = true;
			if (lineIndex[row] != thread->DataSet()->CurrentPos() - 1)
				thread->DataSet()->Locate(lineIndex[row] + 1);

			int i;
			for (i = 0 ; i < nCols ; i++)
			{
				wxString val;
				if (thread->DataSet()->ColType(i) == wxT("bytea"))
					val = _("<binary data>");
				else
				{
					val = thread->DataSet()->GetVal(i);
					if (val.IsEmpty())
					{
						if (!thread->DataSet()->IsNull(i))
							val = wxT("''");
					}
					else if (val == wxT("''"))
						val = wxT("\\'\\'");
				}
				line->cols[i] = val;
			}
			rowsCached++;

			if (rowsCached == nRows)
			{
				delete thread;
				thread = 0;
			}
		}
	}
	if (columns[col].type == PGOID_TYPE_BOOL)
	{
		if (line->cols[col] != wxEmptyString)
			line->cols[col] = (StrToBool(line->cols[col]) ? wxT("TRUE") : wxT("FALSE"));
	}

	val = line->cols[col];
	return val;
}

bool sqlTable::AppendRows(size_t rows)
{
	rowsAdded += rows;
	GetLine(nRows + rowsAdded - rowsDeleted - 1);

	wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, rows);
	GetView()->ProcessTableMessage(msg);

	return true;
}


bool sqlTable::DeleteRows(size_t pos, size_t rows)
{
	size_t i = pos;
	size_t rowsDone = 0;

	while (i < pos + rows)
	{
		cacheLine *line = GetLine(pos);
		if (!line)
			break;

		// If line->cols is null, it probably means we need to force the cacheline to be populated.
		if (!line->cols)
		{
			GetValue(pos, 0);
			line = GetLine(pos);
		}

		if (line->stored)
		{
			wxString key = MakeKey(line);
			wxASSERT(!key.IsEmpty());
			bool done = connection->ExecuteVoid(wxT(
			                                        "DELETE FROM ") + tableName + wxT(" WHERE ") + key);
			if (!done)
				break;

			if ((int)pos < nRows - rowsDeleted)
			{
				rowsDeleted++;
				if ((int)pos < nRows - rowsDeleted)
					memmove(lineIndex + pos, lineIndex + pos + 1, sizeof(cacheLine *) * (nRows - rowsDeleted - pos));
			}
			else
			{
				rowsAdded--;
				if (GetLine(pos)->stored)
					rowsStored--;
				addPool->Delete(pos - (nRows - rowsDeleted));
			}
			rowsDone++;
		}
		else
		{
			// last empty line won't be deleted, just cleared
			int j;
			for (j = 0 ; j < nCols ; j++)
				line->cols[j] = wxT("");
		}
		i++;
	}

	if (rowsDone > 0 && GetView())
	{
		wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, rowsDone);
		GetView()->ProcessTableMessage(msg);
	}
	return (rowsDone != 0);
}


bool sqlTable::Paste()
{
	int row, col;
	int start, pos, len;
	wxArrayString data;
	wxString text, quoteChar, colSep;
	bool inQuotes, inData, skipSerial;

	if (!this)
		return false;

	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDF_TEXT))
		{
			wxTextDataObject textData;
			wxTheClipboard->GetData(textData);
			text = textData.GetText();
		}
		else
		{
			wxTheClipboard->Close();
			return false;
		}
		wxTheClipboard->Close();
	}
	else
	{
		return false;
	}

	start = pos = 0;
	len = text.Len();
	quoteChar = settings->GetCopyQuoteChar();
	colSep = settings->GetCopyColSeparator();
	inQuotes = inData = false;

	while (pos < len && !(text[pos] == '\n' && !inQuotes))
	{
		if (!inData)
		{
			if (text[pos] == quoteChar)
			{
				inQuotes = inData = true;
				pos++;
				start++;
				continue;
			}
			else
			{
				inQuotes = false;
			}
			inData = true;
		}

		if (inQuotes && text[pos] == quoteChar &&
		        text[pos + 1] == colSep)
		{
			data.Add(text.Mid(start, pos - start));
			start = (pos += 2);
			inData = false;
		}
		else if (!inQuotes && text[pos] == colSep)
		{
			data.Add(text.Mid(start, pos - start));
			start = ++pos;
			inData = false;
		}
		else
		{
			pos++;
		}
	}
	if (start < pos)
	{
		if (inQuotes && text[pos - 1] == quoteChar)
			data.Add(text.Mid(start, pos - start - 1));
		else
			data.Add(text.Mid(start, pos - start));
	}

	row = GetNumberRows() - 1;
	skipSerial = false;

	for (col = 0; col < nCols; col++)
	{
		if (columns[col].type == (unsigned int)PGOID_TYPE_SERIAL ||
		        columns[col].type == (unsigned int)PGOID_TYPE_SERIAL8)
		{
			wxMessageDialog msg(GetView()->GetParent(),
			                    _("This table contains serial columns. Do you want to use the values in the clipboard for these columns?"),
			                    _("Paste Data"), wxYES_NO | wxICON_QUESTION);
			if (msg.ShowModal() != wxID_YES)
			{
				skipSerial = true;
			}
			break;
		}
	}

	bool pasted = false;
	for (col = (hasOids ? 1 : 0); col < nCols && col < (int)data.GetCount(); col++)
	{
		if (!(skipSerial && (columns[col].type == (unsigned int)PGOID_TYPE_SERIAL ||
		                     columns[col].type == (unsigned int)PGOID_TYPE_SERIAL8)))
		{
			SetValue(row, col, data.Item(col));
			GetView()->SetGridCursor(row, col);
			GetView()->MakeCellVisible(row, col);
			pasted = true;
		}
	}
	GetView()->ForceRefresh();

	return pasted;
}




wxGridCellAttr *sqlTable::GetAttr(int row, int col, wxGridCellAttr::wxAttrKind  kind)
{
	cacheLine *line = GetLine(row);
	if (line && line->readOnly)
	{
		wxGridCellAttr *attr = new wxGridCellAttr(columns[col].attr);
		attr->SetReadOnly();
		return attr;
	}
	else
	{
		columns[col].attr->IncRef();
		return columns[col].attr;
	}
}



wxString sqlCellAttr::Quote(pgConn *conn, const wxString &value)
{
	wxString str;
	if (value.IsEmpty())
		str = wxT("NULL");
	else if (numeric)
		str = conn->qtDbString(value);
	else if (value == wxT("\\'\\'"))
		str = conn->qtDbString(wxT("''"));
	else if (value == wxT("''"))
		str = wxT("''");
	else if (type == PGOID_TYPE_BOOL)
		str = value;
	else if (type == PGOID_TYPE_BIT)
		// Don't cast this one
		return wxT("B'") + value + wxT("'");
	else
		str = conn->qtDbString(value);

	return str + wxT("::") + typeName;
}




int sqlCellAttr::size()
{
	if (typlen == -1 && typmod > 0)
	{
		return (typmod - 4) >> 16;
	}
	else
		return typlen;
}


int sqlCellAttr::precision()
{
	if (typlen == -1 && typmod > 0)
	{
		return (typmod - 4) & 0x7fff;
	}
	else
		return -1;
}


cacheLinePool::cacheLinePool(int initialLines)
{
	ptr = new cacheLine*[initialLines];
	if (ptr)
	{
		anzLines = initialLines;
		memset(ptr, 0, sizeof(cacheLine *)*anzLines);
	}
	else
	{
		anzLines = 0;
		wxLogError(__("Out of Memory for cacheLinePool"));
	}
}



cacheLinePool::~cacheLinePool()
{
	if (ptr)
	{
		while (anzLines--)
		{
			if (ptr[anzLines])
				delete ptr[anzLines];
		}
		delete[] ptr;
	}
}



void cacheLinePool::Delete(int lineNo)
{
	if (ptr && lineNo >= 0 && lineNo < anzLines)
	{
#if 1
		if (ptr[lineNo])
			delete ptr[lineNo];

		if (lineNo < anzLines - 1)
		{
			// beware: overlapping copy
			memmove(ptr + lineNo, ptr + lineNo + 1, sizeof(cacheLine *) * (anzLines - lineNo - 1));
		}
#else
		cacheLine *c;
		c = ptr[0];
		ptr[0] = ptr[1];
		ptr[1] = c;
#endif
		ptr[anzLines - 1] = 0;
	}
}


cacheLine *cacheLinePool::Get(int lineNo)
{
	if (lineNo < 0) return 0;

	if (lineNo >= anzLines)
	{
		cacheLine **old = ptr;
		int oldAnz = anzLines;
		anzLines = lineNo + 100;
		ptr = new cacheLine*[anzLines];
		if (!ptr)
		{
			anzLines = 0;
			wxLogError(__("Out of Memory for cacheLinePool"));
		}
		else
		{
			if (oldAnz)
			{
				memcpy(ptr, old, sizeof(cacheLine *)*oldAnz);
				delete[] old;
			}
			memset(ptr + oldAnz, 0, anzLines - oldAnz);
		}
	}

	if (lineNo < anzLines)
	{
		if (!ptr[lineNo])
			ptr[lineNo] = new cacheLine();
		return ptr[lineNo];
	}
	return 0;
}


bool cacheLinePool::IsFilled(int lineNo)
{
	return (lineNo < anzLines && ptr[lineNo]);
}


bool editGridFactoryBase::CheckEnable(pgObject *obj)
{
	if (obj)
	{
		pgaFactory *factory = obj->GetFactory();
		return factory == &tableFactory || factory == &viewFactory || factory == &extTableFactory || factory == &catalogObjectFactory;
	}
	return false;
}


wxWindow *editGridFactoryBase::ViewData(frmMain *form, pgObject *obj, bool filter)
{
	pgDatabase *db = ((pgSchemaObject *)obj)->GetDatabase();
	wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Edit Grid");

	pgServer *server = db->GetServer();
	pgConn *conn = db->CreateConn(applicationname);
	if (conn)
	{
		wxString txt = _("Edit Data - ")
		               + server->GetDescription()
		               + wxT(" (") + server->GetName()
		               + wxT(":") + NumToStr((long)server->GetPort())
		               + wxT(") - ") + db->GetName()
		               + wxT(" - ") + obj->GetFullIdentifier();

		frmEditGrid *eg = new frmEditGrid(form, txt, conn, (pgSchemaObject *)obj);
		eg->SetLimit(rowlimit);
		eg->ShowForm(filter);
		return eg;
	}
	return 0;
}


editGridFactory::editGridFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : editGridFactoryBase(list)
{
	mnu->Append(id, _("View &All Rows\tCtrl-D"), _("View the data in the selected object."));
	toolbar->AddTool(id, _("View All Rows\tCtrl-D"), wxBitmap(viewdata_xpm), _("View the data in the selected object."), wxITEM_NORMAL);
	context = false;
}


wxWindow *editGridFactory::StartDialog(frmMain *form, pgObject *obj)
{
	return ViewData(form, obj, false);
}


#include "images/viewfiltereddata.xpm"
editGridFilteredFactory::editGridFilteredFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : editGridFactoryBase(list)
{
	mnu->Append(id, _("View F&iltered Rows...\tCtrl-G"), _("Apply a filter and view the data in the selected object."));
	toolbar->AddTool(id, _("View Filtered Rows\tCtrl-G"), wxBitmap(viewfiltereddata_xpm), _("Apply a filter and view the data in the selected object."), wxITEM_NORMAL);
	context = false;
}


wxWindow *editGridFilteredFactory::StartDialog(frmMain *form, pgObject *obj)
{
	return ViewData(form, obj, true);
}

editGridLimitedFactory::editGridLimitedFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, int limit) : editGridFactoryBase(list)
{
	mnu->Append(id, wxString::Format(wxPLURAL("View Top %i Row", "View Top %i Rows", limit), limit), _("View a limited number of rows in the selected object."));
	rowlimit = limit;
	context = false;
}

wxWindow *editGridLimitedFactory::StartDialog(frmMain *form, pgObject *obj)
{
	return ViewData(form, obj, false);
}
