//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmQueryBuilder.cpp - The query builder main form
//
// NOTE: Much of the code in this file is the same as in frmQuery.cpp
//       If any changes are made, please check there as well.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include "wx/regex.h"
#include "math.h"

// App headers
#include "frmQueryBuilder.h"
#include "frmQuery.h"
#include "dlgAddTableView.h"
#include "ctlSQLResult.h"
#include "menu.h"

// Icons
#include "images/sql.xpm"

// Bitmaps
#include "images/file_save.xpm"
#include "images/query_execute.xpm"
#include "images/query_explain.xpm"
#include "images/query_cancel.xpm"
#include "images/table.xpm"

extern sysSettings *settings;


// Event Table

BEGIN_EVENT_TABLE(frmQueryBuilder, wxMDIParentFrame)

    EVT_MENU(MNU_SAVE,         frmQueryBuilder::OnSave)
    EVT_MENU(MNU_SAVEAS,       frmQueryBuilder::OnSaveAs)
    EVT_MENU(MNU_EXPORT,       frmQueryBuilder::OnExport)
    EVT_MENU(MNU_EXECUTE,      frmQueryBuilder::OnExecute)
    EVT_MENU(MNU_EXPLAIN,      frmQueryBuilder::OnExplain)
    EVT_MENU(MNU_CANCEL,       frmQueryBuilder::OnCancel)
    EVT_MENU(MNU_EXIT,         frmQueryBuilder::OnExit)
    EVT_MENU(MNU_ADDTABLEVIEW, frmQueryBuilder::OnAddTableView)
    EVT_MENU(MNU_CLEARHISTORY, frmQueryBuilder::OnClearHistory)
    EVT_MENU(MNU_SAVEHISTORY,  frmQueryBuilder::OnSaveHistory)

    EVT_SIZE(frmQueryBuilder::OnSize)
    EVT_CLOSE(frmQueryBuilder::OnClose)

#ifdef __WXMSW__
    EVT_CONTEXT_MENU(frmQueryBuilder::OnContextMenu)
#else
    EVT_RIGHT_UP(frmQueryBuilder::OnRightUp)
#endif

    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, frmQueryBuilder::OnNotebookPageChanged)
    EVT_GRID_CELL_LEFT_CLICK(frmQueryBuilder::OnCellSelect) 
    EVT_GRID_CELL_CHANGE(frmQueryBuilder::OnCellChange)
    EVT_COMBOBOX(-1, frmQueryBuilder::OnCellChoice) 

    EVT_SASH_DRAGGED_RANGE(ID_SASH_WINDOW_BOTTOM, ID_SASH_WINDOW_BOTTOM, frmQueryBuilder::OnSashDrag)
    EVT_STC_MODIFIED(CTL_SQLPANEL, frmQueryBuilder::OnChange)

END_EVENT_TABLE()


// Event Table

BEGIN_EVENT_TABLE(myClientWindow, wxMDIClientWindow)
    EVT_PAINT(myClientWindow::OnPaint)
END_EVENT_TABLE()

frmQueryBuilder::frmQueryBuilder(frmMain* form, pgDatabase *database):
 wxMDIParentFrame(form, -1, 
   wxT("Query Builder: ") + database->GetName()
    + wxT(" (") + database->GetServer()->GetName() + wxT(")"),
   settings->GetFrmQueryBuilderPos(), 
   settings->GetFrmQueryBuilderSize(), 
   wxFRAME_NO_WINDOW_MENU | wxSYSTEM_MENU | wxMAXIMIZE_BOX | 
   wxMINIMIZE_BOX | wxRESIZE_BORDER)
{
	// Log
	wxLogInfo(wxT("Creating Query builder"));

	// Initialize Data
    m_mainForm = form;
	m_sashwindow = NULL;
	m_database = database;
	m_server = m_database->GetServer();
	changed = FALSE;

    // Icon
    SetIcon(wxIcon(sql_xpm));

    // Build menus
    menuBar = new wxMenuBar();

    fileMenu = new wxMenu();
    fileMenu->Append(MNU_SAVE, _("&Save\tCtrl-S"),      _("Save current file"));
    fileMenu->Append(MNU_SAVEAS, _("Save &as..."),      _("Save file under new name"));
    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXPORT, _("&Export"),  _("Export data to file"));
    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"), _("Exit query window"));

    menuBar->Append(fileMenu, _("&File"));

	// Query Menu
    queryMenu = new wxMenu();
    queryMenu->Append(MNU_EXECUTE, _("&Execute\tF5"), _("Execute query"));
    queryMenu->Append(MNU_EXPLAIN, _("E&xplain\tF7"), _("Explain query"));

    wxMenu *eo=new wxMenu();
    eo->Append(MNU_VERBOSE, _("Verbose"), _("Explain verbose query"), wxITEM_CHECK);
    eo->Append(MNU_ANALYZE, _("Analyze"), _("Explain analyse query"), wxITEM_CHECK);
    queryMenu->Append(MNU_EXPLAINOPTIONS, _("Explain &options"), eo, _("Options modifying Explain output"));
    queryMenu->AppendSeparator();
    queryMenu->Append(MNU_SAVEHISTORY, _("Save history"), _("Save history of executed commands."));
    queryMenu->Append(MNU_CLEARHISTORY, _("Clear history"), _("Clear history window."));
    queryMenu->AppendSeparator();
    queryMenu->Append(MNU_CANCEL, _("&Cancel\tAlt-Break"), _("Cancel query"));
    menuBar->Append(queryMenu, _("&Query"));

    // Tools Menu
    toolsMenu = new wxMenu();
    toolsMenu->Append(MNU_ADDTABLEVIEW, _("&Add Table/View..."), 
		_("Add a table or view to the datagram."));
    menuBar->Append(toolsMenu, _("&Tools"));
    
	// Set the Menu Bar
	SetMenuBar(menuBar);

	// Accelerators
    wxAcceleratorEntry entries[6];
    entries[0].Set(wxACCEL_ALT,     (int)'E',      MNU_EXECUTE);
    entries[1].Set(wxACCEL_ALT,     (int)'X',      MNU_EXPLAIN);
    entries[2].Set(wxACCEL_CTRL,    (int)'S',      MNU_SAVE);
    entries[3].Set(wxACCEL_NORMAL,  WXK_F5,        MNU_EXECUTE);
    entries[4].Set(wxACCEL_ALT,     WXK_PAUSE,     MNU_CANCEL);
    entries[5].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

    wxAcceleratorTable accel(6, entries);
    SetAcceleratorTable(accel);

    queryMenu->Enable(MNU_CANCEL, false);
    queryMenu->Enable(MNU_SAVEHISTORY, false);
    queryMenu->Enable(MNU_CLEARHISTORY, false);

	// Status Bar
    int iWidths[4] = {0, -1, 110, 110};
    CreateStatusBar(4);
    SetStatusWidths(4, iWidths);
    SetStatusText(_("ready"), STATUSPOS_MSGS);
    statusBar = GetStatusBar();

	// Tool Bar
    CreateToolBar();
    toolBar = GetToolBar();

    toolBar->SetToolBitmapSize(wxSize(16, 16));
    toolBar->AddTool(MNU_SAVE, _("Save"), wxBitmap(file_save_xpm), _("Save current file"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_EXECUTE, _("Execute"), wxBitmap(query_execute_xpm), _("Execute query"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_EXPLAIN, _("Explain"), wxBitmap(query_explain_xpm), _("Explain query"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_CANCEL, _("Cancel"), wxBitmap(query_cancel_xpm), _("Cancel query"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_ADDTABLEVIEW, _("Add Table/View..."), wxBitmap(table_xpm), _("Add a table or view to the datagram."), wxITEM_NORMAL);

    toolBar->Realize();
    setTools(false);
    toolBar->EnableTool(MNU_CANCEL, false);

    // Datagram Context Menu
    datagramContextMenu = new wxMenu();
    datagramContextMenu->Append(MNU_ADDTABLEVIEW, _("&Add Table/View..."), 
		_("Add a table or view to the datagram."));
  
	// Set up the sash window
	m_sashwindow = new wxSashLayoutWindow(this, ID_SASH_WINDOW_BOTTOM,
		wxDefaultPosition, wxSize(200, 200),
		wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

	m_sashwindow->SetDefaultSize(wxSize(1000, 200));
	m_sashwindow->SetOrientation(wxLAYOUT_HORIZONTAL);
	m_sashwindow->SetAlignment(wxLAYOUT_BOTTOM);
	m_sashwindow->SetBackgroundColour(wxColour(255, 0, 0));
	m_sashwindow->SetSashVisible(wxSASH_TOP, TRUE);

    // Setup the notebook
    notebook = new wxNotebook(m_sashwindow, ID_NOTEBOOK, 
		wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN | wxNB_BOTTOM );

	// Setup the design tab
    design = new wxGrid(notebook, 0, 0, 1, 1, wxHSCROLL | wxVSCROLL );
    design->CreateGrid(0, 12);

	// We don't want our cells overflowing (default = TRUE)
	design->SetDefaultCellOverflow(FALSE);

	// show / don't show
	design->SetColLabelValue(DESIGN_OUTPUT, _("Output"));
	// table
	design->SetColLabelValue(DESIGN_TABLE, _("Table"));
	// column
	design->SetColLabelValue(DESIGN_COLUMN, _("Column"));
	// expression 
	design->SetColLabelValue(DESIGN_EXPRESSION, _("Expression"));
	// alias
	design->SetColLabelValue(DESIGN_ALIAS, _("Alias"));
	// aggregate
	design->SetColLabelValue(DESIGN_AGGREGATE, _("Aggregate"));
	// inner where/having clause
	design->SetColLabelValue(DESIGN_CONDITION, _("Condition"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR1, _("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR2, _("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR3, _("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR4, _("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR5, _("Or"));

	// Update the design
    design->UpdateDimensions();

	// Setup the sql tab
    sql = new ctlSQLBox(notebook, CTL_SQLPANEL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_RICH2);

    // Setup the data tab
    data = new ctlSQLResult(notebook, database->connection(), CTL_SQLRESULT, wxDefaultPosition, wxDefaultSize);

    // And the messages
    msgResult = new wxTextCtrl(notebook, CTL_MSGRESULT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_DONTWRAP);
    msgHistory = new wxTextCtrl(notebook, CTL_MSGHISTORY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_DONTWRAP);

	notebook->AddPage(design, _("Design"));
	//notebook->AddPage(design, _("Union"));
	//notebook->AddPage(design, _("Global"));
    notebook->AddPage(sql, _("SQL"));
	notebook->AddPage(data, _("Data"));
    notebook->AddPage(msgResult, _("Messages"));
    notebook->AddPage(msgHistory, _("History"));

	// Set the drop target
	design->SetDropTarget(new DnDDesign(this));

}

void frmQueryBuilder::OnSaveHistory(wxCommandEvent& event)
{
    wxFileDialog *dlg=new wxFileDialog(this, _("Save history"), lastDir, wxEmptyString, 
        _("Log files (*.log)|*.log|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() == wxID_OK)
    {
        FileWrite(dlg->GetPath(), msgHistory->GetValue(), false);
    }
    delete dlg;

}

void frmQueryBuilder::OnClearHistory(wxCommandEvent& event)
{
    queryMenu->Enable(MNU_SAVEHISTORY, false);
    queryMenu->Enable(MNU_CLEARHISTORY, false);
    msgHistory->Clear();
}

frmQueryBuilder::~frmQueryBuilder()
{
    wxLogInfo(wxT("Destroying SQL Query box"));

	wxPoint tmppoint = GetPosition();
	if (tmppoint.x < 0) 
		tmppoint.x = 0;
	if (tmppoint.y < 0)
		tmppoint.y = 0;

	wxSize tmpsize = GetSize();
	if (tmpsize.x < 10) 
		tmpsize.x = 10;
	if (tmpsize.y < 10)
		tmpsize.y = 10;

	// Save Settings
	settings->SetFrmQueryBuilderPos(tmppoint);
	settings->SetFrmQueryBuilderSize(tmpsize);

	// Cleanup
	delete datagramContextMenu;

	// Remove from main frame
    m_mainForm->RemoveFrame(this);
}

void frmQueryBuilder::setTools(const bool running)
{
    toolBar->EnableTool(MNU_EXECUTE, !running);
    toolBar->EnableTool(MNU_EXPLAIN, !running);
    toolBar->EnableTool(MNU_CANCEL, running);
    queryMenu->Enable(MNU_EXECUTE, !running);
    queryMenu->Enable(MNU_EXPLAIN, !running);
    queryMenu->Enable(MNU_CANCEL, running);
}

void frmQueryBuilder::OnExport(wxCommandEvent &ev)
{
    data->Export();
}



wxMDIClientWindow* frmQueryBuilder::OnCreateClient()
{
	m_clientWindow = new myClientWindow();
	return m_clientWindow;
}




void myClientWindow::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	// Get the parent Query Builder
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int w,h;
	GetClientSize(&w,&h);

	dc.DestroyClippingRegion();
	dc.SetClippingRegion(0, 0, w, h);

	dc.SetPen(*wxBLACK_PEN);

	int count = tmpparent->m_joins.GetCount();

	for (int si = 0; si < count; si++ )
	{
		JoinStruct *tmpjoin = NULL;
		tmpjoin = (JoinStruct *)tmpparent->m_joins[si];

		frmChildTableViewFrame *tmpleft = 
			tmpparent->GetFrameFromAlias(tmpjoin->left);
		frmChildTableViewFrame *tmpright = 
			tmpparent->GetFrameFromAlias(tmpjoin->right);

		wxSize tleftsize = tmpleft->GetSize();
		wxSize trightsize = tmpright->GetSize();
		wxSize tleftcsize = tmpleft->GetClientSize();
		
		int offsety = tleftsize.y - tleftcsize.y + 16;

		int tw, th, des, lead;
		dc.GetTextExtent(tmpright->m_columnlist->
			GetString(tmpjoin->rightcolumn), &tw, &th, &des, &lead);

		int cheight = th - des + lead + 1;

		int leftyoffset = cheight * (tmpjoin->leftcolumn) +
			(cheight / 2) + offsety;

		int rightyoffset = cheight * (tmpjoin->rightcolumn) + 
			(cheight / 2) + offsety;

		if (leftyoffset > tleftsize.y)
			leftyoffset = tleftsize.y;

		if (rightyoffset > trightsize.y)
			rightyoffset = trightsize.y;

		wxPoint leftpos = tmpleft->GetPosition();
		wxSize leftsize = tmpleft->GetSize();
		wxPoint rightpos = tmpright->GetPosition();
		wxSize rightsize = tmpright->GetSize();

		wxPoint start;
		wxPoint finish;
		double slope;
		double angle;

		if (leftpos.x < rightpos.x)
		{
			start.x = leftpos.x + leftsize.x;
			finish.x = rightpos.x;
			start.y = leftpos.y + leftyoffset;
			finish.y = rightpos.y + rightyoffset;
		}
		else
		{
			start.x = rightpos.x + rightsize.x;
			finish.x = leftpos.x;
			start.y = rightpos.y + rightyoffset;
			finish.y = leftpos.y + leftyoffset;	
		}
		
		slope = ((double)start.y-(double)finish.y) / 
			((double)start.x-(double)finish.x);
		angle = atan(slope) * 
			(double)-180.0 / (double)3.141592653;

		if (start.x >= finish.x)
			angle += 180;

		dc.DrawLine(start, finish);
	
		// Win32: Hit Testing Lines and Curves
	}

	dc.SetPen(wxNullPen);
}



void frmQueryBuilder::OnExit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}



void frmQueryBuilder::OnAddTableView(wxCommandEvent& event)
{
	// Setup the add table/view dialog
	dlgAddTableView addtableview(this, m_database);

	// Initialize the data
	addtableview.InitLists();

	// Show the dialog
	int result = addtableview.ShowModal();  

	// Process the result
	if (result == wxID_OK)
	{

		// Grab the selected items
		wxArrayInt tableselections, viewselections;
		addtableview.m_tablelist->GetSelections( tableselections );
		addtableview.m_viewlist->GetSelections( viewselections );

		// Find out how many tables there are to add
		int tblcount = tableselections.GetCount();
		int viewcount = viewselections.GetCount();

		int si;

		// Add the tables to the MDI Client Window
		for ( si = 0; si < tblcount; si++ )
		{
			// Grab the item number and the name of the table
			int itemno = tableselections.Item( si );
			wxString tmpname = addtableview.m_tablelist->GetString( itemno );

			// Check to see if that table already exists, and if it does
			// then we need to get the correct alias for it
			wxString tmpalias = GetTableViewAlias(tmpname);

			// Create the child frames
			frmChildTableViewFrame *tmpframe = 
				new frmChildTableViewFrame(this, tmpname, 
				tmpalias, m_database);

			m_children.Add(tmpframe);
			m_names.Add(tmpname);
			m_aliases.Add(tmpalias);
			UpdateGridTables(NULL);
		}

		// Add the views to the MDI Client Window
		for ( si = 0; si < viewcount; si++ )
		{
			// Grab the item number and the name of the view
			int itemno = viewselections.Item( si );
			wxString tmpname = addtableview.m_viewlist->GetString( itemno );

			// Check to see if that view already exists, and if it does
			// then we need to get the correct alias for it
			wxString tmpalias = GetTableViewAlias(tmpname);

			// Create the child frames
			frmChildTableViewFrame *tmpframe = 
				new frmChildTableViewFrame(this, tmpname, 
				tmpalias, m_database);

			m_children.Add(tmpframe);
			m_names.Add(tmpname);
			m_aliases.Add(tmpalias);
			UpdateGridTables(NULL);
		}
	}
}



void frmQueryBuilder::OnSize(wxSizeEvent& event)
{
    if (this->GetClientWindow() != NULL) {
	wxLayoutAlgorithm layout;
	layout.LayoutMDIFrame(this);
    }
}



void frmQueryBuilder::ExecRightClick(wxPoint& point)
{
	// Get mouse point data
	wxPoint origin = GetClientAreaOrigin();

	// Because this Tree is inside a vertical splitter, we
	// must compensate for the size of the other elements
	point.x += origin.x;
	point.y += origin.y;	

    // This handler will display a popup menu for the item
	PopupMenu(datagramContextMenu, point);
}



void frmQueryBuilder::OnSashDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId())
    {
        case ID_SASH_WINDOW_BOTTOM:
        {
            m_sashwindow->SetDefaultSize(wxSize(1000, 
				event.GetDragRect().height));
            break;
        }
    }

    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);

    // Leaves bits of itself behind sometimes
    GetClientWindow()->Refresh();
}



wxString frmQueryBuilder::GetTableViewAlias(wxString firstname, 
												wxString newname, 
												int postfix)
{
	// This is a recursive function, so if newname doesn't have a value
	// then assign it to the same value as the actual name of the table
	// which should only happen on the first iteration 
	if (!newname.length())
		newname = firstname;

	// The table alias -- assume that it is the table name
	wxString alias = newname;

	// How many table aliases do we have to search through?
	int count = this->m_aliases.GetCount();

	// Iterate through all the table aliases	
	for (int si = 0; si < count; si++ )
	{
		// Get the actual table alias
		wxString oldname = this->m_aliases.Item(si);

		// If the table alias is equivalent to the new alias
		if (oldname == newname)
		{
			// Postfix a numeral onto the actual table name (_1, _2, etc)
			alias = firstname + wxString::Format(wxT("_%d"), postfix++ );

			// Recurse to find out if the alias already exists
			wxString tmpalias = this->GetTableViewAlias(firstname, 
				alias, postfix);

			// If the table alias does not already exist, use it
			if ( alias.Lower() != tmpalias.Lower() )
				alias = tmpalias;
		}
	}

	// Return the table alias
	return alias;
}



void frmQueryBuilder::AddColumn(frmChildTableViewFrame *frame, int item)
{
	// Get the number of rows (table columns)
	int rows = design->GetNumberRows();
	
	// Add a new row (table column)
	if (!design->AppendRows(1))
	{
		wxLogError(__("Can't add another column to the query."));
		return;
	}

	// Output column
   const wxString outputchoices[] =
    {
		_("Yes"),
		_("No"),
    };
    design->SetCellEditor(rows, DESIGN_OUTPUT, 
		new wxGridCellChoiceEditor(2, outputchoices));
    design->SetCellValue(rows, DESIGN_OUTPUT, outputchoices[0]);
    design->SetCellOverflow(rows, DESIGN_OUTPUT, FALSE);

	// Table Column
	UpdateGridTables(frame);

	// Column Column
	UpdateGridColumns(frame,item);

	// Aggregate Column
    const wxString aggregatechoices[] =
    {
		wxT("Where"),
		wxT("Group by"),
        wxT("Avg"),
        wxT("Count"),
        wxT("Max"),
        wxT("Min"),
        wxT("StdDev"),
        wxT("Sum"),
        wxT("Variance")
    };

    design->SetCellEditor(rows, DESIGN_AGGREGATE, 
		new wxGridCellChoiceEditor(WXSIZEOF(aggregatechoices), 
		aggregatechoices));
    design->SetCellValue(rows, DESIGN_AGGREGATE, aggregatechoices[0]);
    design->SetCellOverflow(rows, DESIGN_AGGREGATE, FALSE);

	// Refresh the design grid
    design->UpdateDimensions();
}



void frmQueryBuilder::UpdateGridTables(frmChildTableViewFrame *frame)
{
	// Get the number of rows (table columns)
	int rows = design->GetNumberRows();

	// The table name - initially, a blank space (since wxGrid doesn't 
	// like to store "" values in it's wxGridCellChoiceEditor control)
	wxString tablename = wxT(" ");

	// We need the name of the table that was selected, and we get that
	// from the title of the frame in the datagram
	if (frame!=NULL)
		tablename = frame->GetTitle();

	// How many tables are we working with?
	int tblcount = this->m_aliases.GetCount();

	// Sort the table choices
	wxSortedArrayString tablechoices;
	int si;
	for (si = 0; si < tblcount; si++ )
	{
		tablechoices.Add(this->m_aliases[si]);
	}

	// We need to know the index of the table that was chosen
	int chosentable = 0;

	// Copy the SortedArrayString into a true array because
	// SetCellEditor won't accept wxSortedArrayString and add a 
	// blank choice to the top of the list
	wxString *tablechoices2 = new wxString[tblcount+1];
	tablechoices2[0] = wxT(" ");
	for (si = 0; si < tblcount; si++ )
	{
		tablechoices2[si+1] = tablechoices[si];
		if ( tablechoices[si] == tablename )
			chosentable = si+1;
	}

	// Update the table cell choice editors
	for (si = 0; si < rows; si++ )
	{
		// We MUST hide the edit control to replace it
		design->HideCellEditControl();

		// Set the cell editor to a dropdown (combobox, actually)
		wxGridCellChoiceEditor *tmpeditor = NULL;
		tmpeditor = new wxGridCellChoiceEditor(tblcount+1, tablechoices2);

		// Replace the old one with the new one
		design->SetCellEditor(si, 1, tmpeditor);

		// Get the current value
		wxString tmpname = this->design->GetCellValue( si, 1 );

		// Unless there is already a value, use the chosen value
		if ( tmpname.length() > 0 )
		{
			for (int sj = 0; sj < tblcount; sj++ )
			{
				if ( tablechoices2[sj] == tmpname )
					design->SetCellValue(si, 1, tablechoices2[sj]);
			}
		}		
		else
		{
			design->SetCellValue(si, 1, tablechoices2[chosentable]);
		}	

		design->ShowCellEditControl();
	}

	// Cleanup
	delete[] tablechoices2;
}



void frmQueryBuilder::UpdateGridColumns(frmChildTableViewFrame *frame, 
											int item, bool _FORCE,
											int _FORCEROW)
{
	// Get the number of rows (table columns)
	int rows = design->GetNumberRows();

	// We will assume blank choices
	wxString columnname = wxT(" ");
	wxString tablename = wxT(" ");

	// How many columns are there to choose from?
	int colcount = 0;

	// As long as the frame is not null, we can get information from it
	if (frame != NULL)
	{
		// We need the name of the table
		tablename = frame->GetTitle();
		
		// We need the name of the column that was selected
		if (item != -1)
            columnname = frame->m_columnlist->GetString(item);
        else
            columnname = wxT("*");

		// Column Choices
		colcount = frame->m_columnlist->GetCount();
	}

	// Sort the column choices
	wxSortedArrayString columnchoices;
	int si;
	for (si = 0; si < colcount; si++ )
	{
		columnchoices.Add(frame->m_columnlist->GetString(si));
	}

	// We need to know the index of the column that was chosen
	int chosencolumn = 0;

	// Copy the SortedArrayString into a true array because
	// SetCellEditor won't accept wxSortedArrayString and add a 
	// blank choice to the top of the list
	wxString *columnchoices2 = new wxString[colcount+1];
	columnchoices2[0] = wxT(" ");
	for (si = 0; si < colcount; si++ )
	{
		columnchoices2[si+1] = columnchoices[si];
		if (columnchoices[si] == columnname)
			chosencolumn = si+1;
	}

	// Update the column editors
	for (si = 0; si < rows; si++)
	{
		// Get the name of the row's (table column's) table
		wxString tmptable = design->GetCellValue(si, DESIGN_TABLE);

		// Only do this if we're on the same table
		// Or if we're forcing the row to update
		if (tmptable == tablename)
		{
			// We MUST hide the edit control to replace it
			design->HideCellEditControl();

			// Set the cell editor to a dropdown
			wxGridCellChoiceEditor *tmpeditor = NULL;
			tmpeditor = new wxGridCellChoiceEditor(colcount, columnchoices2);

			// Replace the old one with the new one
			design->SetCellEditor(si, 2, tmpeditor);

			// Get the current value
			wxString tmpname = this->design->GetCellValue(si, DESIGN_COLUMN);

			// Unless there is already a value, use the chosen value
			if (_FORCE && _FORCEROW == si)
			{
				if (colcount)
					design->SetCellValue(si, DESIGN_COLUMN, columnchoices2[1]);
				else
					design->SetCellValue(si, DESIGN_COLUMN, columnchoices2[0]);
			}
			else if (tmpname.length() > 0)
			{
				for (int sj = 0; sj < colcount; sj++ )
				{
					if ( columnchoices2[sj] == tmpname )
						design->SetCellValue(si, DESIGN_COLUMN, columnchoices2[sj]);
				}
			}		
			else
			{
				design->SetCellValue(si, DESIGN_COLUMN, columnchoices2[chosencolumn]);
			}

			// Show the control again		
			design->ShowCellEditControl();
		}
	}

	// Cleanup
	delete[] columnchoices2;
}



void frmQueryBuilder::OnNotebookPageChanged(wxNotebookEvent& event)
{
	// What page did we change to?
	int page = event.GetSelection();

	// Make sure to save whatever data the user was working on
	// design->SaveEditControlValue() doesn't function properly
	int cellx = design->GetGridCursorCol();
	int celly = design->GetGridCursorRow();
	design->SetGridCursor(0,0);
	design->SetGridCursor(celly, cellx);

	// Perform an action depending on the page we're on
	switch (page)
	{
		case 1:
			// We need a valid query
			BuildQuery();
			break;

		case 2:
			// We need a valid query, and we need to run it
			BuildQuery();
			break;

		default:
			break;
	}
}



int frmQueryBuilder::FindLeftmostTable()
{
	int tablecount = m_aliases.GetCount();
	int joincount = m_joins.GetCount();
	int si, sj;

	wxArrayInt count;

	for ( si = 0; si < tablecount; si++ )
		count[si] = 0;

	for ( si = 0; si < joincount; si++ )
	{
		JoinStruct *tmpjoin = NULL;
		tmpjoin = (JoinStruct *)m_joins[si];
		wxString tmpleft = tmpjoin->left;
		wxString tmpright = tmpjoin->right;

		for (sj = 0; sj < tablecount; sj++)
		{
			if (tmpleft == m_names[sj])
				count[sj]++;
			if (tmpright == m_names[sj])
				count[sj] = -1;
		}
	}

	int leftmost = -1;
	int leftmostcount = 0;
	for ( si = 0; si < tablecount; si++ )
	{
		if (count[si] > leftmostcount)
		{
			leftmostcount = count[si];
			leftmost = si;
		}
	}

	return leftmost;
}



bool frmQueryBuilder::IsTableLeftOnly(wxString tablename)
{
	int joincount = m_joins.GetCount();
	int si;

	int count = 0;

	for ( si = 0; si < joincount; si++ )
	{
		JoinStruct *tmpjoin = NULL;
		tmpjoin = (JoinStruct *)m_joins[si];
		wxString tmpleft = tmpjoin->left;
		wxString tmpright = tmpjoin->right;

		if (tmpleft == tablename)
			count++;
		if (tmpright == tablename)
		{
			count = -1;
			break;
		}
	}

	if (count == -1)
		return FALSE;

	return TRUE;
}


// RECURSIVE!

wxString frmQueryBuilder::BuildTableJoin(int table, int indent = 0)
{
	wxString tablename = m_names[table];
	wxString tablealias = m_aliases[table];

	wxString joinclause = tablename;
	if (tablename!=tablealias)
		joinclause += wxT(" ") + tablealias;

	int joincount = m_joins.GetCount();
	int tablecount = m_aliases.GetCount();

	int si;

	wxString indentstr = wxT("");
	for ( si = 0; si < indent; si++ )
	{
		indentstr += wxT("\t");
	}

	wxString nested = wxT("");

	for ( si = 0; si < joincount; si++ )
	{
		JoinStruct *tmpjoin = NULL;
		tmpjoin = (JoinStruct *)m_joins[si];

		if (tmpjoin->left == tablealias)
		{
			int sj;

			for (sj = 0; sj < tablecount; sj++)
			{
				wxString tmptable1 = m_aliases[sj];

				if (tmptable1 == tmpjoin->right && sj != table)
				{
					nested = BuildTableJoin(sj, ++indent);
					break;
				}
			}

			joinclause += wxT(" ") + tmpjoin->jointype +
				wxT(" ") + nested +
				wxT(" ") + wxT("ON\n\t") + 
				indentstr +	wxT("(");

			for (sj = 0; sj < tmpjoin->conditionct; sj++)
			{
				if (sj == 0) 
					joinclause += wxT("\n\t\t") + indentstr + 
						tmpjoin->conditions[sj];
				else
					joinclause += tmpjoin->joinop + wxT("\n\t\t") + 
						indentstr + tmpjoin->conditions[sj];
			}
			
			joinclause += wxT("\n\t") + indentstr + wxT(")");
		}
	}

	if (!nested.IsEmpty())
		joinclause = wxT("\n") + indentstr + wxT("(\n\t") + indentstr
			+ joinclause + wxT("\n") + indentstr + wxT(")");

	return joinclause;
}



void frmQueryBuilder::BuildQuery()
{
	// Make the query blank
	sql->SetText(wxT(""));

	// The query strings
	wxString querystr = wxT("SELECT");
	wxString tablestr = wxT("");
	wxString columnstr = wxT("");
	wxString joinstr = wxT("");
	wxString conditionstr = wxT("");

	// Get the number of rows
	int rows = design->GetNumberRows();
	int joincount = m_joins.GetCount();

	wxArrayString tables;

	wxString tmptable1, tmptable2;
	int tblcount = 0;
	int si;

	wxArrayString righttables;

	// Make sure there are no double right joins
	for ( si = 0; si < joincount; si++ )
	{
		JoinStruct *tmpjoin = NULL;
		tmpjoin = (JoinStruct *)m_joins[si];

		wxString tmptable1 = tmpjoin->right;

		int sj;
		int rightcount = righttables.GetCount();
		int found = 0;
		for ( sj = 0; sj < rightcount; sj++ )
		{
			wxString tmptable2 = righttables[sj];

			if (tmptable1==tmptable2)
			{
				found = 1;
				break;
			}
		}

		if (found)
		{
			wxLogError(__("Double right-handed joins are not allowed.\n")
				__("You must redraw your joins so that \"%s\"\ndoes not appear on the righthand side \nmore than once. \n\n")
				__("Try switching it to the lefthand side in one\nor more relationships."), tmptable1.c_str());
            sql->SetText(wxT(""));
			return;
		}
		else
			righttables.Add(tmptable1);
	}

	int tablecount = m_aliases.GetCount();

	// Grab all the tables and cull the duplicates
	for ( si = 0; si < tablecount; si++ )
	{
		tmptable1 = m_aliases[si];

		if (IsTableLeftOnly(tmptable1))
		{
			tables.Add(tmptable1);
			tblcount++;
		}
	}

	// Iterate through the rows to build the table list
	for ( si = 0; si < tblcount; si++ )
	{
		int tablecount = m_aliases.GetCount();

		wxString joinclause;
		int sj;
		for (sj = 0; sj < tablecount; sj++)
		{
			wxString tmptable1 = m_aliases[sj];
			if (tmptable1 == tables[si])		
				joinclause = this->BuildTableJoin(sj);
		}

		if (si == 0) 
			tablestr += joinclause;
		else
			tablestr += wxT(", ") + joinclause;
	}

	// Iterate through the rows to build the column list
	for ( si = 0; si < rows; si++ )
	{
		// Grab data
		wxString output = design->GetCellValue(si, DESIGN_OUTPUT);
		wxString table = design->GetCellValue(si, DESIGN_TABLE);
		wxString column = design->GetCellValue(si, DESIGN_COLUMN);
		wxString expression = design->GetCellValue(si, DESIGN_EXPRESSION);
		wxString alias = design->GetCellValue(si, DESIGN_ALIAS);
		wxString aggregate = design->GetCellValue(si, DESIGN_AGGREGATE);
		wxString condition = design->GetCellValue(si, DESIGN_CONDITION);

		wxString *orcondition = new wxString[5];

		orcondition[0] = design->GetCellValue(si, DESIGN_OR1);
		orcondition[1] = design->GetCellValue(si, DESIGN_OR2);
		orcondition[2] = design->GetCellValue(si, DESIGN_OR3);
		orcondition[3] = design->GetCellValue(si, DESIGN_OR4);
		orcondition[4] = design->GetCellValue(si, DESIGN_OR5);

		delete[] orcondition;

		if (!expression.length() && table.length() && column.length())
			expression = table + wxT(".") + column;

		if (!expression.length() && !(table.length() || column.length()))
		{
			wxLogError(__("Error: No table or no column"));
            sql->SetText(wxT(""));
            notebook->SetSelection(0);
			return;
		}

		if (alias.length())
			expression += wxT(" AS ") + alias;

		bool conderr;
		condition = RebuildCondition(condition, si+1, conderr);
		if (conderr)
		{
			wxLogError(condition);
            sql->SetText(wxT(""));
			return;
		}

		if (!condition.IsEmpty())
		{
			if (si == 0)
			{
				conditionstr += wxT("\t(") + condition + wxT(")");
			}
			else
				conditionstr += wxT(" AND \n\t(") + condition + wxT(")");
		}

		if (si == 0) 
			columnstr += wxT("\n\t") + expression;
		else
			columnstr += wxT(", \n\t") + expression;
	}

	if (columnstr.IsEmpty())
	{
		wxLogError(__("You must add at least on column."));
        sql->SetText(wxT(""));
        notebook->SetSelection(0);
		return;
	}

	if (conditionstr.length())
		conditionstr = wxT("\nWHERE \n(\n") + conditionstr + wxT("\n)");

	querystr += columnstr + 
		wxT("\nFROM ") + 
		tablestr +
		joinstr +
		conditionstr;

	this->sql->SetText(querystr);

}



void frmQueryBuilder::RunQuery(const wxString &query, int resultToRetrieve, bool singleResult, const int queryOffset)
{
    long rowsReadTotal=0;
    setTools(true);

    queryMenu->Enable(MNU_SAVEHISTORY, true);
    queryMenu->Enable(MNU_CLEARHISTORY, true);

    bool wasChanged = changed;
    sql->MarkerDeleteAll(0);
    if (!wasChanged)
    {
        changed=false;
        setExtendedTitle();
    }

    aborted=false;
    
    if (data->Execute(query, resultToRetrieve) >= 0)
    {
        SetStatusText(wxT(""), STATUSPOS_SECS);
        SetStatusText(_("Query is running."), STATUSPOS_MSGS);
        SetStatusText(wxT(""), STATUSPOS_ROWS);
        msgResult->Clear();

        msgHistory->AppendText(_("-- Executing query:\n"));
        msgHistory->AppendText(query);
        msgHistory->AppendText(wxT("\n"));
        Update();
        wxYield();

        wxString str;
        wxLongLong elapsedQuery;
        wxLongLong startTimeQuery=wxGetLocalTimeMillis();
        while (data->RunStatus() == CTLSQL_RUNNING)
        {
            elapsedQuery=wxGetLocalTimeMillis() - startTimeQuery;
            SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);
            wxYield();
            wxUsleep(10);
            str=data->GetMessagesAndClear();
            if (!str.IsEmpty())
            {
                msgResult->AppendText(str);
                msgHistory->AppendText(str);
            }
            wxYield();
        }

        str=data->GetMessagesAndClear();
        msgResult->AppendText(str);
        msgHistory->AppendText(str);

        elapsedQuery=wxGetLocalTimeMillis() - startTimeQuery;
        SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);

        if (data->RunStatus() != PGRES_TUPLES_OK)
        {
            notebook->SetSelection(3);
            if (data->RunStatus() == PGRES_COMMAND_OK)
            {
                showMessage(wxString::Format(_("Query returned successfully with no result in %s ms."),
                    elapsedQuery.ToString().c_str()), _("OK."));
            }
            else
            {
                wxString errMsg = data->GetErrorMessage();
                showMessage(errMsg);

                wxString atChar=wxT(" at character ");
                int chp=errMsg.Find(atChar);

                if (chp > 0)
                {
                    int selStart=sql->GetSelectionStart(), selEnd=sql->GetSelectionEnd();
                    if (selStart == selEnd)
                        selStart=0;

                    long errPos=0;
                    errMsg.Mid(chp+atChar.Length()).ToLong(&errPos);
                    errPos += queryOffset;  // do not count EXPLAIN or similar
                    int line=0, maxLine = sql->GetLineCount();
                    while (line < maxLine && sql->GetLineEndPosition(line) < errPos + selStart+1)
                        line++;
                    if (line < maxLine)
                    {
                        wasChanged=changed;
                        sql->MarkerAdd(line, 0);
                        if (!wasChanged)
                        {
                            changed=false;
                            setExtendedTitle();
                        }
                        sql->EnsureVisible(line);
                    }
                }
            }
        }
        else
        {
            notebook->SetSelection(2);
            long rowsTotal=data->NumRows();

            if (singleResult)
            {
                rowsReadTotal=data->RetrieveOne();
                showMessage(wxString::Format(_("%d rows retrieved."), rowsReadTotal), _("OK."));
            }
            else
            {
                SetStatusText(wxString::Format(_("Retrieving data: %d rows."), rowsTotal), STATUSPOS_MSGS);
                wxYield();

                long maxRows=settings->GetMaxRows();

                if (!maxRows)
                    maxRows = rowsTotal;
                if (rowsTotal > maxRows)
                {
                    wxMessageDialog msg(this, wxString::Format(
                            _("The maximum of %ld rows is exceeded (total %ld)."), maxRows, rowsTotal) +
                            _("\nRetrieve all rows anyway?"), _("Limit exceeded"), 
                                wxYES_NO|wxCANCEL|wxNO_DEFAULT|wxICON_EXCLAMATION);
                    switch (msg.ShowModal())
                    {
                        case wxID_YES:
                            maxRows = rowsTotal;
                            break;
                        case wxID_CANCEL:
                            maxRows = 0;
                            break;
                    }
                }
                wxLongLong startTimeRetrieve=wxGetLocalTimeMillis();
                wxLongLong elapsed;
                elapsedRetrieve=0;
		        bool resultFreezed=false;
                
                while (!aborted && rowsReadTotal < maxRows)
                {
                    // Rows will be retrieved in chunks, the first being smaller to have an early screen update
                    // later, screen update is disabled to speed up retrieval
                    long chunk;
                    if (!rowsReadTotal) chunk=20;
                    else                chunk=100;

                    if (chunk > maxRows-rowsReadTotal)
                        chunk = maxRows-rowsReadTotal;

                    long rowsRead=data->Retrieve(chunk);
                    if (!rowsRead)
                        break;

                    elapsed = wxGetLocalTimeMillis() - startTimeRetrieve;

                    if (!rowsReadTotal)
		            {
                        wxYield();
			            if (rowsRead < maxRows)
				    {
					resultFreezed=true;
					data->Freeze();
				    }
		            }
                    rowsReadTotal += rowsRead;

                    if (elapsed > elapsedRetrieve +100)
                    {
                        elapsedRetrieve=elapsed;
                        SetStatusText(elapsedQuery.ToString() + wxT("+") + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);
                        wxYield();
                    }
                }
		if (resultFreezed)
		    data->Thaw();

                elapsedRetrieve=wxGetLocalTimeMillis() - startTimeRetrieve;
                SetStatusText(elapsedQuery.ToString() + wxT("+") + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);

                str= _("Total query runtime: ") + elapsedQuery.ToString() + wxT(" ms.\n") +
                     _("Data retrieval runtime: ") + elapsedRetrieve.ToString() + wxT(" ms.\n");
                msgResult->AppendText(str);
                msgHistory->AppendText(str);

                if (rowsReadTotal == data->NumRows())
                    showMessage(wxString::Format(_("%ld rows retrieved."), rowsReadTotal), _("OK."));
                else
                {
                    wxString nr;
                    nr.Printf(_("%ld  rows not retrieved."), rowsTotal - rowsReadTotal);
                    showMessage(wxString::Format(_("Total %ld rows.\n"), rowsTotal) + nr, nr);
                }
                msgHistory->AppendText(wxT("\n"));

            }
            if (rowsTotal == rowsReadTotal)
                SetStatusText(wxString::Format(_("%d rows."), rowsTotal), STATUSPOS_ROWS);
            else
                SetStatusText(wxString::Format(_("%ld of %ld rows"), rowsReadTotal, rowsTotal), STATUSPOS_ROWS);
        }
    }

    if (rowsReadTotal)
    {
        fileMenu->Enable(MNU_EXPORT, data->CanExport());
    }
    setTools(false);

}



void frmQueryBuilder::OnCellSelect(wxGridEvent& event)
{
	design->SelectBlock(event.GetRow(), event.GetCol(), event.GetRow(), event.GetCol());
	design->SetGridCursor(event.GetRow(), event.GetCol());
	design->EnableCellEditControl();
	design->ShowCellEditControl();
}



void frmQueryBuilder::VerifyExpression(int celly)
{
	wxString cellv = design->GetCellValue(celly, DESIGN_EXPRESSION);
	wxString tmpcolumn = design->GetCellValue(celly, DESIGN_COLUMN);
	wxString tmptable = design->GetCellValue(celly, DESIGN_TABLE);

	if (cellv.length() == 0 && tmpcolumn == wxT(" "))
	{
		wxMessageDialog dlg(this, 
            _("You cannot leave both the column and expression blank.\n\nClick 'OK' to insert a default function into the expression."),
			_("Expression Error"), 
			wxICON_ERROR | wxOK);

		dlg.ShowModal();
		
		design->SetCellValue(celly, DESIGN_EXPRESSION, wxT("now()"));
	}
	else if (cellv.length() == 0 && tmptable != wxT(" "))
		design->SetCellValue(celly, DESIGN_COLUMN, wxT("*"));
	else
		design->SetCellValue(celly, DESIGN_COLUMN, wxT(" "));
}



void frmQueryBuilder::OnCellChoice(wxCommandEvent& event)
{
	int celly = design->GetGridCursorRow();
	int cellx = design->GetGridCursorCol();
	wxString tmpstr = event.GetString();

	design->SaveEditControlValue();

	switch (cellx) 
	{
		case DESIGN_OUTPUT:
			break;
		case DESIGN_TABLE:
			if (tmpstr != wxT(" "))
			{
				design->SetCellValue(celly, DESIGN_COLUMN, wxT(" "));
				design->SetCellValue(celly, DESIGN_EXPRESSION, wxT(""));
			}
			else
			{
				design->SetCellValue(celly, DESIGN_COLUMN, wxT("*"));
			}

			this->UpdateGridColumns(GetFrameFromAlias(tmpstr),
				0, TRUE, celly);
			VerifyExpression(celly);
			design->SetCellValue(celly, DESIGN_TABLE, tmpstr);
			break;
		case DESIGN_COLUMN:
			if (tmpstr != wxT(" "))
			{
				design->SetCellValue(celly, DESIGN_EXPRESSION, wxT(""));
			}
			VerifyExpression(celly);
			//design->SetCellValue(celly, DESIGN_COLUMN, tmpstr);
			break;
		case DESIGN_AGGREGATE:
		default:
			// do nothing
			break;
	}
}



void frmQueryBuilder::OnCellChange(wxGridEvent& event)
{
	int celly = design->GetGridCursorRow();
	int cellx = design->GetGridCursorCol();
	wxString tmpstr = design->GetCellValue(celly, cellx);

	switch (cellx) 
	{
		case DESIGN_EXPRESSION:
			VerifyExpression(celly);
			break;
		case DESIGN_ALIAS:
			if (!IsValidIdentifier(tmpstr))
            {
                wxString msg;
                msg.Printf(_("'%s' is not a valid identifier. It must start with A-Z or a-z or _ and can contain A-Z and a-z and 0-9 and _."), tmpstr.c_str());

                wxMessageBox(msg);
            }
			break;
		default:
			// do nothing
			break;
	}
}



frmChildTableViewFrame *frmQueryBuilder::GetFrameFromAlias(wxString alias)
{
	int count = m_children.GetCount();
	wxString tmptitle;
	void *tmpframe;

	for (int si = 0; si < count; si++)
	{
		tmpframe = (void *)m_children[si];
		tmptitle = m_aliases[si];

		if (tmptitle.Lower() == alias.Lower())
			return (frmChildTableViewFrame *)tmpframe;
	}

	return NULL;
}



wxString frmQueryBuilder::RebuildCondition(wxString condition, int row, bool &errout)
{
	// Assume no errors
	errout = FALSE;

	wxString tmpstr = condition;

	wxString binarycomp = wxT("<|>|<=|>=|=|<>|!=|");
	wxString binarypat = wxT("NOT LIKE"); //|LIKE|ILIKE|SIMILAR TO|!~*|~*|!~|~"
	wxString errmsg;

	wxString txt = tmpstr;

	wxString marker = wxT("\x080");

	int singlequotedcount = 0;
	wxString singlequotedmatches[255];
	wxRegEx re;
	
	// Replace the "special" character
	re.Compile(marker);
	re.Replace(&txt, wxT(""));

	// Check for ''
	re.Compile(wxT("''"));
	if (re.Matches(txt))
	{
		re.Replace(&tmpstr, marker + wxT("_0"));
		txt = tmpstr;
	}

	// Check for 'x'
	re.Compile(wxT("'[^']'"));
	while (re.Matches(txt))
	{
		size_t start, len;
		re.GetMatch(&start, &len);
		singlequotedmatches[singlequotedcount++] = txt.Mid(start, len);
		txt = txt.Mid(start + len);
	}

	re.Replace(&tmpstr, marker + wxT("_1"));
	txt = tmpstr;

	// Check for unbalanced '
	re.Compile(wxT("'"));
	if (re.Matches(txt))
	{
		errmsg.Printf(_("Check for unbalanced single quote marks in the condition for row #%d."), row);
	}

	// Collapse multiple whitespace to a single space
	re.Compile(wxT("[[:space:]]{1,}"));
	re.Replace(&tmpstr, wxT(" "));

	// Trim leading/trailing spaces
    tmpstr.Strip(wxString::both);

	// Replace string 'x' literals
	for (int si = 0; si < singlequotedcount; si++)
	{
		tmpstr.Replace(marker + wxT("_1"), singlequotedmatches[si], FALSE);
	}

	// Replace null string '' literals
	tmpstr.Replace(marker + wxT("_0"), wxT("''"), FALSE);

	if (errmsg.length())
	{
		tmpstr = errmsg;
		errout = TRUE;
    }

	return tmpstr;
}



void frmQueryBuilder::OnClose(wxCloseEvent& event)
{
    if (changed && settings->GetAskSaveConfirmation())
    {
        wxString fn;
        if (!lastPath.IsNull())
            fn = wxT(" in file ") + lastPath;
        wxMessageDialog msg(this, wxString::Format(_("The text %s has changed.\nDo you want to save changes?"), fn.c_str()), _("pgAdmin III Query"), 
                    wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION|
                    (event.CanVeto() ? wxCANCEL : 0));

    	wxCommandEvent noEvent;
        switch (msg.ShowModal())
        {
            case wxID_YES:
                if (lastPath.IsNull())
                {
                    OnSaveAs(noEvent);
                    if (changed && event.CanVeto())
                        event.Veto();
                }
                else
                    OnSave(noEvent);
                break;
            case wxID_CANCEL:
                event.Veto();
                return;
        }
    }
    Destroy();
}



void frmQueryBuilder::OnChange(wxNotifyEvent& event)
{
    if (!changed)
    {
        changed=true;
        setExtendedTitle();
    }
}

void frmQueryBuilder::setExtendedTitle()
{
    wxString chgStr;
    if (changed)
        chgStr = wxT(" *");

    if (lastPath.IsNull())
        SetTitle(title+chgStr);
    else
    {
        SetTitle(title + wxT(" - [") + lastPath + wxT("]") + chgStr);
    }
}


void frmQueryBuilder::OnSave(wxCommandEvent& event)
{
    if (lastPath.IsNull())
    {
        OnSaveAs(event);
        return;
    }

    if (FileWrite(lastPath, sql->GetText(), lastFileFormat ? 1 : 0))
    {
        changed=false;
        setExtendedTitle();
    }
}

void frmQueryBuilder::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog *dlg=new wxFileDialog(this, _("Save query file as"), lastDir, lastFilename, 
        _("Query files (*.sql)|*.sql|UTF-8 query files (*.usql)|*.usql|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() == wxID_OK)
    {
        lastFilename=dlg->GetFilename();
        lastDir = dlg->GetDirectory();
        lastPath = dlg->GetPath();
        switch (dlg->GetFilterIndex())
        {
            case 0: 
                lastFileFormat = false;
                break;
            case 1:
                lastFileFormat = true;
                break;
            default:
                lastFileFormat = settings->GetUnicodeFile();
                break;
        }

        OnSave(event);
    }
    delete dlg;
}



void frmQueryBuilder::OnCancel(wxCommandEvent& event)
{
    SetStatusText(wxT("Canceled query."), STATUSPOS_MSGS);
    aborted = true;
}



void frmQueryBuilder::OnExplain(wxCommandEvent& event)
{
    BuildQuery();
    wxString query = sql->GetText();

    if (query.IsNull())
        return;

    wxString qry;
    int resultToRetrieve=1;
    bool verbose=queryMenu->IsChecked(MNU_VERBOSE), analyze=queryMenu->IsChecked(MNU_ANALYZE);

    if (analyze)
    {
        qry += wxT("\nBEGIN;\n");
        resultToRetrieve++;
    }
    qry += wxT("EXPLAIN ");
    if (analyze)
        qry += wxT("ANALYZE ");
    if (verbose)
        qry += wxT("VERBOSE ");
    
    int offset=qry.Length();
    qry += query;

    if (analyze)
        qry += wxT(";\nROLLBACK;");

    RunQuery(qry, resultToRetrieve, true, offset);
    sql->SetFocus();
}



void frmQueryBuilder::OnExecute(wxCommandEvent& event)
{
    BuildQuery();

    wxString query = sql->GetText();

    if (query.IsNull())
        return;

    RunQuery(query);
}



void frmQueryBuilder::DeleteChild(wxString talias)
{
	int si;

	// Get the count of children
	int count = m_children.GetCount();

	wxArrayPtrVoid new_children;
	wxArrayString new_names;
	wxArrayString new_aliases;

	// Iterate through the children
	for (si = 0; si < count; si++)
	{
		void *tmpchild = m_children[si];

		// If the child is not null
		if (tmpchild != NULL)
		{
			wxString tmpalias = m_aliases[si];

			// If the aliases match 
			if (tmpalias == talias)
			{
				// Make the child null and clear its strings
				m_children[si] = NULL;
				m_names[si].Clear();
				m_aliases[si].Clear();

				// How many rows do we have in the design grid?
				int rowcount = design->GetNumberRows();

				// Iterate through all the rows
				for (int sj = rowcount - 1; sj >= 0; sj--) 
				{
					wxString tmptable = design->GetCellValue(sj, DESIGN_TABLE);
					
					// Delete row that matches the child we're deleting
					if (tmptable==talias) 
						design->DeleteRows(sj);
				}


			}
		} 
	} 

	for (si = 0; si < count; si++)
	{
		void *tmpchild = m_children[si];

		if (tmpchild != NULL)
		{
			new_children.Add(tmpchild);
			new_names.Add(m_names[si]);
			new_aliases.Add(m_aliases[si]);
		}
	}

	// Replace the lists with the new, packed lists
	m_children = new_children;
	m_names = new_names;
	m_aliases = new_aliases;

	wxArrayPtrVoid new_joins;

	// How many joins are there?
	int joincount = m_joins.GetCount();

	// Delete all the joins associated with this 
	for (si = 0; si < joincount; si++)
	{
		JoinStruct *tmpjoin = NULL;
		tmpjoin = (JoinStruct *)m_joins[si];

		if (tmpjoin->left != talias && tmpjoin->right != talias)
		{
			new_joins.Add(tmpjoin);
		}
	}

	// Replace the join list with the new, packed list
	m_joins = new_joins;

} 




bool DnDDesign::OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
	frmQueryBuilder *tmpparent = 
		(frmQueryBuilder*)m_frame;

	// Extract the left table name/column name
	wxStringTokenizer tmptok(text, wxT("."));
	wxString lefttable = tmptok.GetNextToken();
    lefttable += wxT(".") + tmptok.GetNextToken();
	wxString column = tmptok.GetNextToken();

	frmChildTableViewFrame *tmpframe =
		tmpparent->GetFrameFromAlias(lefttable);

	int item = tmpframe->m_columnlist->FindItem(-1, column);

	tmpparent->AddColumn(tmpframe, item);

	return TRUE;
}

void frmQueryBuilder::showMessage(const wxString& msg, const wxString &msgShort)
{
    msgResult->AppendText(msg + wxT("\n"));
    msgHistory->AppendText(msg + wxT("\n"));
    wxString str;
    if (msgShort.IsNull())
        str=msg;
    else
        str=msgShort;
    str.Replace(wxT("\n"), wxT(" "));
    SetStatusText(str, STATUSPOS_MSGS);
}

