//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmQueryBuilder.cpp - The query builder main form
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include "wx/regex.h"
#include "math.h"

// App headers
#include "frmQueryBuilder.h"

// Icons
#include "images/sql.xpm"

// Bitmaps
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/query_execute.xpm"
#include "images/query_explain.xpm"
#include "images/query_cancel.xpm"

extern sysSettings *settings;

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(frmQueryBuilder, wxMDIParentFrame)

    EVT_MENU(frmQueryBuilder::BTN_OPEN, OnOpen)
    EVT_MENU(frmQueryBuilder::BTN_SAVE, OnSave)
    EVT_MENU(frmQueryBuilder::BTN_EXECUTE, OnExecute)
    EVT_MENU(frmQueryBuilder::BTN_EXPLAIN, OnExplain)
    EVT_MENU(frmQueryBuilder::BTN_CANCEL, OnCancel)

    EVT_MENU(frmQueryBuilder::MNU_OPEN, OnOpen)
    EVT_MENU(frmQueryBuilder::MNU_SAVE, OnSave)
    EVT_MENU(frmQueryBuilder::MNU_SAVEAS, OnSaveAs)
    EVT_MENU(frmQueryBuilder::MNU_EXECUTE, OnExecute)
    EVT_MENU(frmQueryBuilder::MNU_EXPLAIN, OnExplain)
    EVT_MENU(frmQueryBuilder::MNU_CANCEL, OnCancel)
	EVT_MENU(frmQueryBuilder::MNU_EXIT, OnExit)
	EVT_MENU(frmQueryBuilder::MNU_ADDTABLEVIEW, OnAddTableView)

	EVT_SIZE(OnSize)
    EVT_CLOSE(OnClose)

#ifdef __WXMSW__
    EVT_CONTEXT_MENU(OnContextMenu)
#else
    EVT_RIGHT_UP(OnRightUp)
#endif

	EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, OnNotebookPageChanged)
	EVT_GRID_CELL_LEFT_CLICK(OnCellSelect) 
	EVT_GRID_CELL_CHANGE(OnCellChange)
	EVT_COMBOBOX(-1, OnCellChoice) 

    EVT_SASH_DRAGGED_RANGE(ID_SASH_WINDOW_BOTTOM, 
		ID_SASH_WINDOW_BOTTOM, OnSashDrag)

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(myClientWindow, wxMDIClientWindow)

	EVT_PAINT(OnPaint)

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
frmQueryBuilder::frmQueryBuilder(frmMain* form, pgDatabase *database)
{
	// Initialize Data
    m_mainForm = form;
	m_sashwindow = NULL;
	m_database = database;
	m_server = m_database->GetServer();

	wxString title = wxT("Query Builder: ") + 
		m_database->GetName() +
		wxT(" (") + 
		m_server->GetName() + 
		wxT(")");

	// Create
	this->Create(form, -1, title,
		settings->GetFrmQueryBuilderPos(), 
		settings->GetFrmQueryBuilderSize());

    // Icon
    SetIcon(wxIcon(sql_xpm));

	// Log
	wxLogInfo(wxT("Creating SQL Query box"));

    // Build menus
    menuBar = new wxMenuBar();

    fileMenu = new wxMenu();
    fileMenu->Append(MNU_OPEN, wxT("&Open..."), wxT("Open a query file"));
    fileMenu->Append(MNU_SAVE, wxT("&Save"), wxT("Save current file"));
    fileMenu->Append(MNU_SAVEAS, wxT("Save &as..."), wxT("Save file under new name"));
    fileMenu->Append(MNU_EXIT, wxT("E&xit"), wxT("Close this Window."));
    menuBar->Append(fileMenu, wxT("&File"));

	// Query Menu
    queryMenu = new wxMenu();
    queryMenu->Append(MNU_EXECUTE, wxT("&Execute"), wxT("Execute query"));
    queryMenu->Append(MNU_EXPLAIN, wxT("E&xplain"), wxT("Explain query"));
    queryMenu->Append(MNU_CANCEL, wxT("&Cancel"), wxT("Cancel query"));
    menuBar->Append(queryMenu, wxT("&Query"));

    // Tools Menu
    toolsMenu = new wxMenu();
    toolsMenu->Append(MNU_ADDTABLEVIEW, wxT("&Add Table/View..."), 
		wxT("Add a table or view to the datagram."));
    menuBar->Append(toolsMenu, wxT("&Tools"));

    // View Menu
    viewMenu = new wxMenu();
    viewMenu->Append(MNU_QUERYBUILDER, wxT("&Query Builder"), 
		wxT("Refresh the selected object."));
    viewMenu->Append(MNU_QUERYANALYZER, wxT("&Query Analyzer"), 
		wxT("Refresh the selected object."));
    //viewMenu->Check(MNU_QUERYBUILDER, TRUE);

    menuBar->Append(viewMenu, wxT("&View"));
    
	// Set the Menu Bar
	SetMenuBar(menuBar);

	// Accelerators
    wxAcceleratorEntry entries[6];
    entries[0].Set(wxACCEL_ALT,     (int)'E',      MNU_EXECUTE);
    entries[1].Set(wxACCEL_ALT,     (int)'X',      MNU_EXPLAIN);
    entries[2].Set(wxACCEL_CTRL,    (int)'O',      MNU_OPEN);
    entries[3].Set(wxACCEL_CTRL,    (int)'S',      MNU_SAVE);
    entries[4].Set(wxACCEL_NORMAL,  WXK_F5,        MNU_EXECUTE);
    entries[5].Set(wxACCEL_ALT,     WXK_PAUSE,     MNU_CANCEL);

    wxAcceleratorTable accel(6, entries);
    SetAcceleratorTable(accel);

    fileMenu->Enable(MNU_SAVE, false);
    queryMenu->Enable(MNU_CANCEL, false);

	// Status Bar
    int iWidths[4] = {0, -1, 110, 110};
    CreateStatusBar(4);
    SetStatusWidths(4, iWidths);
    SetStatusText(wxT("ready"), STATUSPOS_MSGS);
    statusBar = GetStatusBar();

	// Tool Bar
    CreateToolBar();
    toolBar = GetToolBar();

    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(BTN_OPEN, wxT("Open"), wxBitmap(file_open_xpm), wxT("Open file"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_SAVE, wxT("Save"), wxBitmap(file_save_xpm), wxT("Save file"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_EXECUTE, wxT("Execute"), wxBitmap(query_execute_xpm), wxT("Execute query"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_EXPLAIN, wxT("Explain"), wxBitmap(query_explain_xpm), wxT("Explain query"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_CANCEL, wxT("Cancel"), wxBitmap(query_cancel_xpm), wxT("Cancel query"), wxITEM_NORMAL);

    toolBar->Realize();
    setTools(false);
    toolBar->EnableTool(BTN_SAVE, false);

    // Datagram Context Menu
    datagramContextMenu = new wxMenu();
    datagramContextMenu->Append(MNU_ADDTABLEVIEW, wxT("&Add Table/View..."), 
		wxT("Add a table or view to the datagram."));
  
	// Set up the sash window
	m_sashwindow = new wxSashLayoutWindow(this, ID_SASH_WINDOW_BOTTOM,
		wxDefaultPosition, wxSize(200, 200),
		wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

	m_sashwindow->SetDefaultSize(wxSize(1000, 200));
	m_sashwindow->SetOrientation(wxLAYOUT_HORIZONTAL);
	m_sashwindow->SetAlignment(wxLAYOUT_BOTTOM);
	m_sashwindow->SetBackgroundColour(wxColour(255, 0, 0));
	m_sashwindow->SetSashVisible(wxSASH_TOP, TRUE);

	// Setup the add table/view dialog
	addtableview = new dlgAddTableView(this, m_database);

    // Setup the notebook
    notebook = new wxNotebook(m_sashwindow, ID_NOTEBOOK, 
		wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN | wxNB_BOTTOM );

	// Setup the design tab
    design = new wxGrid(notebook, 0, 0, 1, 1, wxHSCROLL | wxVSCROLL );
    design->CreateGrid(0, 12);

	// We don't want our cells overflowing (default = TRUE)
	design->SetDefaultCellOverflow(FALSE);

	// show / don't show
	design->SetColLabelValue(DESIGN_OUTPUT, wxT("Output"));
	// table
	design->SetColLabelValue(DESIGN_TABLE, wxT("Table"));
	// column
	design->SetColLabelValue(DESIGN_COLUMN, wxT("Column"));
	// expression 
	design->SetColLabelValue(DESIGN_EXPRESSION, wxT("Expression"));
	// alias
	design->SetColLabelValue(DESIGN_ALIAS, wxT("Alias"));
	// aggregate
	design->SetColLabelValue(DESIGN_AGGREGATE, wxT("Aggregate"));
	// inner where/having clause
	design->SetColLabelValue(DESIGN_CONDITION, wxT("Condition"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR1, wxT("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR2, wxT("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR3, wxT("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR4, wxT("Or"));
	// outer where/having clause
	design->SetColLabelValue(DESIGN_OR5, wxT("Or"));

	// Update the design
    design->UpdateDimensions();

	// Setup the sql tab
    sql = new ctlSQLBox(notebook, CTL_SQLPANEL, wxDefaultPosition, 
		wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_RICH2);

    // Setup the data tab
    data = new wxGrid(notebook, 0, 0, 400, 400);
    data->CreateGrid(64, 64);

	// We don't want our cells overflowing (default = TRUE)
	data->SetDefaultCellOverflow(FALSE);
	
	// Update the design
	data->UpdateDimensions();
    
	notebook->AddPage(design, wxT("Design"));
	//notebook->AddPage(design, wxT("Union"));
	//notebook->AddPage(design, wxT("Global"));
    notebook->AddPage(sql, wxT("SQL"));
	notebook->AddPage(data, wxT("Data"));

	//DrawTablesAndViews();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::setTools(const bool running)
{
    toolBar->EnableTool(BTN_EXECUTE, !running);
    toolBar->EnableTool(BTN_EXPLAIN, !running);
    toolBar->EnableTool(BTN_CANCEL, running);
    queryMenu->Enable(MNU_EXECUTE, !running);
    queryMenu->Enable(MNU_EXPLAIN, !running);
    queryMenu->Enable(MNU_CANCEL, running);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
wxMDIClientWindow* frmQueryBuilder::OnCreateClient()
{
	myClientWindow *tmpwin = new myClientWindow();
	return tmpwin;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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
	wxFont font(10, wxSWISS, wxNORMAL, wxBOLD);
    dc.SetFont(font);

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
		int offsety = tleftsize.y - tleftcsize.y;

		int leftyoffset = tmpleft->m_columnlist->
			GetCharHeight() * (tmpjoin->leftcolumn) + offsety;

		int rightyoffset = tmpleft->m_columnlist->
			GetCharHeight() * (tmpjoin->rightcolumn) + offsety;

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
		dc.DrawRotatedText(tmpleft->m_columnlist->
			GetString(tmpjoin->leftcolumn), start, angle);

		int tw, th;
		dc.GetTextExtent(tmpright->m_columnlist->
			GetString(tmpjoin->rightcolumn), &tw, &th);

		float xPlot = (tw * -1.0 * cos(atan(slope)));
        float yPlot = (tw * -1.0 * sin(atan(slope)));

		wxString tmp = wxString::Format( "%0.2f, %0.2f", xPlot, yPlot );
		tmpparent->SetStatusText(tmp, frmQueryBuilder::STATUSPOS_MSGS);

		dc.DrawRotatedText(tmpright->m_columnlist->
			GetString(tmpjoin->rightcolumn), xPlot + finish.x, 
			yPlot + finish.y, angle);

	}

	dc.SetPen(wxNullPen);
	dc.SetFont(wxNullFont);

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnExit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnAddTableView()
{
	this->addtableview->InitLists();
	addtableview->Show();  
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnSize(wxSizeEvent& event)
{
	wxLayoutAlgorithm layout;
	layout.LayoutMDIFrame(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnRightClick(wxPoint& point)
{
    // This handler will display a popup menu for the item
	PopupMenu(datagramContextMenu, point);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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
			alias = firstname + wxString::Format( "_%d", postfix++ );

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::AddColumn(frmChildTableViewFrame *frame, int item)
{
	// Get the number of rows (table columns)
	int rows = design->GetNumberRows();
	
	// Add a new row (table column)
	if (!design->AppendRows(1))
	{
		wxLogError(wxT("Can't add another column to the query."));
		return;
	}

	// Output column
   const wxString outputchoices[] =
    {
		wxT("Yes"),
		wxT("No"),
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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
	for (int si = 0; si < tblcount; si++ )
	{
		tablechoices.Add(this->m_aliases[si]);
	}

	// We need to know the index of the table that was chosen
	int chosentable;

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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
		columnname = frame->m_columnlist->GetString(item);

		// Column Choices
		colcount = frame->m_columnlist->GetCount();
	}

	// Sort the column choices
	wxSortedArrayString columnchoices;
	for (int si = 0; si < colcount; si++ )
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
		if (tmptable.Lower() == tablename.Lower())
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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
			RunQuery();
			break;

		default:
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::BuildQuery()
{
	// Make the query blank
	sql->SetText(wxT(""));

	// The query strings
	wxString querystr = wxT("SELECT");
	wxString tablestr = wxT("");
	wxString columnstr = wxT("");
	wxString conditionstr = wxT("");

	// Get the number of rows
	int rows = design->GetNumberRows();

	wxString *tables = new wxString[rows];
	wxString *tablealiases = new wxString[rows];
	wxString tmptable1, tmptable2;
	int tblcount = 0;

	// Grab all the tables and cull the duplicates
	for ( int si = 0; si < rows; si++ )
	{
		tmptable1 = design->GetCellValue(si, 1);
	
		int found = 0;
		for ( int sj = 0; sj < tblcount; sj++ )
		{
			tmptable2 = tables[sj];

			if ( tmptable1 == tmptable2 )
			{
				found = 1;
				break;
			}
		}

		if ( found == 0 )
		{
			int aliascount = this->m_aliases.GetCount();

			// Locate the table alias
			for ( int sj = 0; sj < aliascount; sj++ )
			{
				if ( tmptable1 == m_aliases[sj] &&
					m_aliases[sj] != m_names[sj] )
				{
					tablealiases[tblcount] = m_names[sj] + " ";
					break;
				}
			}

			tables[tblcount++] = tmptable1;
		}
	}

	// Iterate through the rows to build the table list
	for ( si = 0; si < tblcount; si++ )
	{
		if ( si==0 ) 
			tablestr += wxT("\n\t") + tablealiases[si] + tables[si];
		else
			tablestr += wxT(", \n\t") + tablealiases[si] + tables[si];
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
			wxMessageBox("Error: No table or no column");
			return;
		}

		if (alias.length())
			expression += wxT(" AS ") + alias;

		bool conderr;
		condition = RebuildCondition(condition, conderr);
		if (conderr)
		{
			wxLogError(condition + wxT("in the condition for row #") + 
				wxString::Format(wxT("%d"),si + 1) + wxT("."));
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

	if (conditionstr.length())
		conditionstr = wxT("\nWHERE \n(\n") + conditionstr + wxT("\n)");

	querystr += columnstr + 
		wxT("\nFROM") + 
		tablestr +
		conditionstr;

	this->sql->SetText(querystr);

	// Cleanup
	delete[] tables;
	delete[] tablealiases;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::RunQuery()
{
	// If there is no query, then there is nothing to do
	wxString query = this->sql->GetText();
	if (!query.length())
		return;

	// We need to know how many rows the query is going to return
	wxString querycount = "SELECT count(*) AS ct FROM (" + query + ") a";

	// We should probably use ExecuteScalar here, but pgDatabase doesn't
	// have ExecuteScalar yet 
	pgSet *queryset = m_database->ExecuteSet(wxT(querycount));
	if (queryset->Eof())
		return;

	// PGSet returns strings, so convert the count to an integer
	int count = atoi(queryset->GetVal(wxT("ct")));

	// We're done with the data
	delete queryset;

	if (count > 100)
	{
		wxString tmpstr = wxString::Format("This query will return %d results."
			"\n\nLoad all results?", count); 

		wxMessageDialog *messagebox = 
			new wxMessageDialog(this, tmpstr, "Query", wxYES_NO );

		if (messagebox->ShowModal() == wxID_NO)
			return;
	}

	pgSet *querydata = m_database->ExecuteSet(wxT(query));
	if (querydata->Eof())
		return;

	// Get the number of rows
	int rows = data->GetNumberRows();
	int cols = data->GetNumberCols();

	// Turn off grid updating to prevent redraw
	data->Hide();

	if ( rows > 0 )
		data->DeleteRows(0, rows);
	if ( cols > 0 )
	data->DeleteCols(0, cols);

	// If it's a bad query, these need to be 0
	int rowct = 0;
	int colct = 0;

	// Get row and column counts from the query
	rowct = querydata->NumRows();
	colct = querydata->NumCols();

	// Get the column names from the query and set them in the grid
	for (int si = 0; si < colct; si++ )
	{
		wxString tmpcolname = querydata->ColName(si);
		data->SetColLabelValue(si, tmpcolname);
	}

    data->UpdateDimensions();
	
	// Only append if we need to
	if (rowct > 0 )
		data->AppendRows(rowct);
	if (colct > 0 )
		data->AppendCols(colct);

	// Iterate through all the rows
	for ( si = 0; si < rowct; si++ )
	{
		// Iterate through all the columns
		for ( int sj = 0; sj < colct; sj++ )
		{
			// Set the value for the cell to our query data
			data->SetCellValue(si, sj, querydata->GetVal(sj));

			// Data view should be read only until the update is written
			data->SetReadOnly(si, sj);
		}

		// Move to the next row
		querydata->MoveNext();
	}

	// We're done with the data
	delete querydata;

	// Show the grid again
	data->Show();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnCellSelect(wxGridEvent& event)
{
	design->SelectBlock(event.GetRow(), event.GetCol(), 
		event.GetRow(), event.GetCol());
	design->SetGridCursor(event.GetRow(), event.GetCol());
	design->EnableCellEditControl();
	design->ShowCellEditControl();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::VerifyExpression(int celly)
{
	wxString cellv = design->GetCellValue(celly, DESIGN_EXPRESSION);
	wxString tmpcolumn = design->GetCellValue(celly, DESIGN_COLUMN);
	wxString tmptable = design->GetCellValue(celly, DESIGN_TABLE);

	if (cellv.length() == 0 && tmpcolumn == wxT(" "))
	{
		wxMessageDialog dlg(this, wxT("You cannot leave both the "
			"column and expression blank.\n\n"
			"Click 'OK' to insert a default function "
			"into the expression."), 
			wxT("Expression Error"), 
			wxICON_ERROR | wxOK);

		dlg.ShowModal();
		
		design->SetCellValue(celly, DESIGN_EXPRESSION, "now()");
	}
	else if (cellv.length() == 0 && tmptable != wxT(" "))
		design->SetCellValue(celly, DESIGN_COLUMN, wxT("*"));
	else
		design->SetCellValue(celly, DESIGN_COLUMN, wxT(" "));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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
				wxMessageBox(wxT("'") + tmpstr + wxT("' is not a valid "
				"identifier. It must start with A-Z or a-z or _ and can " 
				"contain A-Z and a-z and 0-9 and _."));
			break;
		default:
			// do nothing
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
wxString frmQueryBuilder::RebuildCondition(wxString condition, bool &errout)
{
	// Assume no errors
	errout = FALSE;

	wxString tmpstr = condition;

	wxString binarycomp = "<|>|<=|>=|=|<>|!=|";
	wxString binarypat = "NOT LIKE"; //|LIKE|ILIKE|SIMILAR TO|!~*|~*|!~|~"
	wxString errmsg = wxT("");

	wxString txt = tmpstr;

	wxString marker = wxT("\x080");

	int singlequotedcount = 0;
	wxString singlequotedmatches[255];
	wxRegEx re;
	
	// Replace the "special" character
	re.Compile(marker);
	re.Replace(&txt,"");

	// Check for ''
	re.Compile("''");
	if (re.Matches(txt))
	{
		re.Replace(&tmpstr, marker + "_0");
		txt = tmpstr;
	}

	// Check for 'x'
	re.Compile("'[^']'");
	while (re.Matches(txt))
	{
		size_t start, len;
		re.GetMatch(&start, &len);
		singlequotedmatches[singlequotedcount++] = txt.Mid(start, len);
		txt = txt.Mid(start + len);
	}

	re.Replace(&tmpstr, marker + "_1");
	txt = tmpstr;

	// Check for unbalanced '
	re.Compile("'");
	if (re.Matches(txt))
	{
		errmsg = "Check for unbalanced single quote marks ";
	}

	// Collapse multiple whitespace to a single space
	re.Compile("[[:space:]]{1,}");
	re.Replace(&tmpstr, " ");

	// Trim leading/trailing spaces
	tmpstr.Trim(TRUE);
	tmpstr.Trim(FALSE);

	// Replace string 'x' literals
	for (int si = 0; si < singlequotedcount; si++)
	{
		tmpstr.Replace(marker + "_1", singlequotedmatches[si], FALSE);
	}

	// Replace null string '' literals
	tmpstr.Replace(marker + "_0", "''", FALSE);

	if (errmsg.length())
	{
		tmpstr = errmsg;
		errout = TRUE;
	}

	return tmpstr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnClose(wxCloseEvent& event)
{
    if (m_changed && settings->GetAskSaveConfirmation())
    {
        wxString fn;
        if (!m_lastPath.IsNull())
            fn = wxT(" in file ") + m_lastPath;
        wxMessageDialog msg(this, wxT("The text") + fn + wxT(" has m_changed.\nDo you want to save changes?"), wxT("pgAdmin III Query"), 
                    wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION|
                    (event.CanVeto() ? wxCANCEL : 0));

        switch (msg.ShowModal())
        {
            case wxID_YES:
                if (m_lastPath.IsNull())
                {
                    OnSaveAs(wxCommandEvent());
                    if (m_changed && event.CanVeto())
                        event.Veto();
                }
                else
                    OnSave(wxCommandEvent());
                break;
            case wxID_CANCEL:
                event.Veto();
                return;
        }
    }
    Destroy();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnChange(wxNotifyEvent& event)
{
    if (!m_changed)
    {
        m_changed=true;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnOpen(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Open query file"), m_lastDir, wxT(""), 
        wxT("Query files (*.sql)|*.sql|All files (*.*)|*.*"), wxOPEN|wxHIDE_READONLY);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_lastFilename = dlg.GetFilename();
        m_lastDir = dlg.GetDirectory();
        m_lastPath = dlg.GetPath();

        FILE *f=fopen(m_lastPath.c_str(), "rt");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            int len=ftell(f);
            fseek(f, 0, SEEK_SET);
            wxString buf("", len+1);
            fread((char*)buf.c_str(), len, 1, f);
            fclose(f);
            ((char*)buf.c_str())[len]=0;
			
            sql->SetText(buf);
            m_changed = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnSave(wxCommandEvent& event)
{
    FILE *f=fopen(m_lastPath.c_str(), "w+t");
    if (f)
    {
        wxString buf=sql->GetText();
        fwrite(buf.c_str(), buf.Length(), 1, f);
        fclose(f);
        m_changed=false;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog *dlg=new wxFileDialog(this, wxT("Save query file as"), m_lastDir, m_lastFilename, 
        wxT("Query files (*.sql)|*.sql|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() == wxID_OK)
    {
        m_lastFilename=dlg->GetFilename();
        m_lastDir = dlg->GetDirectory();
        m_lastPath = dlg->GetPath();

        OnSave(event);
    }
    delete dlg;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnCancel(wxCommandEvent& event)
{
    toolBar->EnableTool(BTN_CANCEL, FALSE);
    queryMenu->Enable(MNU_CANCEL, FALSE);
    SetStatusText(wxT("Cancelling."), STATUSPOS_MSGS);

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnExplain(wxCommandEvent& event)
{
//
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnExecute(wxCommandEvent& event)
{
//
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::DeleteChild(wxString talias)
{
	// Get the count of children
	int count = m_children.GetCount();

	// Iterate through the children
	for (int si = 0; si < count; si++)
	{
		void *tmpchild = m_children[si];

		// If the child is not null
		if (tmpchild!=NULL)
		{
			wxString tmpalias = m_aliases[si];

			// If the aliases match 
			if (tmpalias==talias)
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
} 