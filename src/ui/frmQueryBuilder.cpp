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
#include <wx/wx.h>
#include <wx/laywin.h>
#include <wx/settings.h>
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/imaglist.h>
#include <wx/grid.h>
#include <wx/stc/stc.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmQueryBuilder.h"
#include "dlgAddTableView.h"
#include "frmChildTableViewFrame.h"
#include "pgConn.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"

// Icons
#include "images/aggregate.xpm"
#include "images/arguments.xpm"
#include "images/baddatabase.xpm"
#include "images/check.xpm"
#include "images/closeddatabase.xpm"
#include "images/column.xpm"
#include "images/connect.xpm"
#include "images/create.xpm"
#include "images/database.xpm"
#include "images/domain.xpm"
#include "images/drop.xpm"
#include "images/function.xpm"
#include "images/group.xpm"
#include "images/hiproperty.xpm"
#include "images/index.xpm"
#include "images/indexcolumn.xpm"
#include "images/language.xpm"
#include "images/namespace.xpm"
#include "images/operator.xpm"
#include "images/pgAdmin3.xpm"
#include "images/properties.xpm"
#include "images/property.xpm"
#include "images/public.xpm"
#include "images/record.xpm"
#include "images/refresh.xpm"
#include "images/relationship.xpm"
#include "images/rule.xpm"
#include "images/sequence.xpm"
#include "images/server.xpm"
#include "images/serverbad.xpm"
#include "images/sql.xpm"
#include "images/statistics.xpm"
#include "images/stop.xpm"
#include "images/table.xpm"
#include "images/trigger.xpm"
#include "images/type.xpm"
#include "images/user.xpm"
#include "images/vacuum.xpm"
#include "images/view.xpm"
#include "images/viewdata.xpm"

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(frmQueryBuilder, wxMDIParentFrame)

	EVT_SIZE(OnSize)
	EVT_MENU(frmQueryBuilder::MNU_EXIT,         OnExit)
	EVT_MENU(frmQueryBuilder::MNU_ADDTABLEVIEW, OnAddTableView)
#ifdef __WXMSW__
    EVT_CONTEXT_MENU(                           OnContextMenu)
#else
    EVT_RIGHT_UP(                               OnRightUp)
#endif
	EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK,      OnNotebookPageChanged)
    EVT_SASH_DRAGGED_RANGE(ID_SASH_WINDOW_BOTTOM, ID_SASH_WINDOW_BOTTOM, OnSashDrag)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
frmQueryBuilder::frmQueryBuilder(wxWindow* parent, pgDatabase *database)
{
    extern sysSettings *settings;

	this->database = database;

	m_server = database->GetServer();
	wxString title = "Query Builder: " + database->GetName() +
		" (" + m_server->GetName() + ")";

	this->Create(parent, -1, title,
		settings->GetFrmQueryBuilderPos(), 
		settings->GetFrmQueryBuilderSize());

	sashwindow = NULL;

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

    // Build menus
    menuBar = new wxMenuBar();

    // File Menu
    fileMenu = new wxMenu();
    fileMenu->Append(MNU_EXIT, wxT("E&xit"), wxT("Close this Window."));
    menuBar->Append(fileMenu, wxT("&File"));

    // Tools Menu
    toolsMenu = new wxMenu();
    toolsMenu->Append(MNU_ADDTABLEVIEW, wxT("&Add Table/View..."), 
		wxT("Add a table or view to the datagram."));
    menuBar->Append(toolsMenu, wxT("&Tools"));
/*
    // View Menu
    viewMenu = new wxMenu();
    viewMenu->Append(MNU_SYSTEMOBJECTS, wxT("&System objects"), wxT("Show or hide system objects."), wxITEM_CHECK);
    viewMenu->AppendSeparator();
    viewMenu->Append(MNU_REFRESH, wxT("&Refresh"), 
		wxT("Refresh the selected object."));
    menuBar->Append(viewMenu, wxT("&View"));

    // Help Menu
    helpMenu = new wxMenu();
    helpMenu->Append(MNU_CONTENTS, wxT("&Help..."), wxT("Open the helpfile."));
    helpMenu->Append(MNU_TIPOFTHEDAY, wxT("&Tip of the day..."), wxT("Show a tip of the day."));
    helpMenu->AppendSeparator();
    helpMenu->Append(MNU_ABOUT, wxT("&About..."), wxT("Show about dialog."));
    menuBar->Append(helpMenu, wxT("&Help"));
*/

    // Datagram Context Menu
    datagramContextMenu = new wxMenu();
    datagramContextMenu->Append(MNU_ADDTABLEVIEW, wxT("&Add Table/View..."), 
		wxT("Add a table or view to the datagram."));

    // Add the Menubar
    SetMenuBar(menuBar);

    // Status bar
    CreateStatusBar(1);
    SetStatusText(wxT(""), 0);

    // Toolbar bar
    // CreateToolBar();

    // Return objects
    statusBar = GetStatusBar();
    //toolBar = GetToolBar();

    // Set up toolbar
/*    wxBitmap barBitmaps[10];
    toolBar->SetToolBitmapSize(wxSize(32, 32));
    barBitmaps[0] = wxBitmap(connect_xpm);
    barBitmaps[1] = wxBitmap(refresh_xpm);
    barBitmaps[2] = wxBitmap(create_xpm);
    barBitmaps[3] = wxBitmap(drop_xpm);
    barBitmaps[4] = wxBitmap(properties_xpm);
    barBitmaps[5] = wxBitmap(sql_xpm);
    barBitmaps[6] = wxBitmap(viewdata_xpm);
    barBitmaps[7] = wxBitmap(vacuum_xpm);
    barBitmaps[8] = wxBitmap(record_xpm);
    barBitmaps[9] = wxBitmap(stop_xpm);

    toolBar->AddTool(BTN_ADDSERVER, wxT("Add Server"), barBitmaps[0], wxT("Add a connection to a server."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_REFRESH, wxT("Refresh"), barBitmaps[1], wxT("Refresh the data below the selected object."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(BTN_CREATE, wxT("Create"), barBitmaps[2], wxT("Create a new object of the same type as the selected object."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_DROP, wxT("Drop"), barBitmaps[3], wxT("Drop the currently selected object."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_PROPERTIES, wxT("Properties"), barBitmaps[4], wxT("Display/edit the properties of the selected object."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(BTN_SQL, wxT("SQL"), barBitmaps[5], wxT("Execute arbitrary SQL queries."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_VIEWDATA, wxT("View Data"), barBitmaps[6], wxT("View the data in the selected object."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_VACUUM, wxT("Vacuum"), barBitmaps[7], wxT("Vacuum the current database or table."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(BTN_RECORD, wxT("Record"), barBitmaps[8], wxT("Record a query log."), wxITEM_NORMAL);
    toolBar->AddTool(BTN_STOP, wxT("Stop"), barBitmaps[9], wxT("Stop recording the query log."), wxITEM_NORMAL);

    // Display the bar and configure buttons. 
    toolBar->Realize();
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
    toolBar->EnableTool(BTN_STOP, FALSE);
*/
    
	// Set up the sash window
	sashwindow = new wxSashLayoutWindow(this, ID_SASH_WINDOW_BOTTOM,
		wxDefaultPosition, wxSize(200, 200),
		wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

	sashwindow->SetDefaultSize(wxSize(1000, 200));
	sashwindow->SetOrientation(wxLAYOUT_HORIZONTAL);
	sashwindow->SetAlignment(wxLAYOUT_BOTTOM);
	sashwindow->SetBackgroundColour(wxColour(255, 0, 0));
	sashwindow->SetSashVisible(wxSASH_TOP, TRUE);

	// Setup the add table/view dialog
	addtableview = new dlgAddTableView(this, database);

    // Setup the notebook
    notebook = new wxNotebook(sashwindow, ID_NOTEBOOK, 
		wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN );

	// Setup the design tab
    design = new wxGrid(notebook, 0, 0, 1, 1, wxHSCROLL | wxVSCROLL );
    design->CreateGrid(0, 12);

	// We don't want our cells overflowing (default = TRUE)
	design->SetDefaultCellOverflow(FALSE);

	int tmpcol = 0;
	design->SetColLabelValue(tmpcol++, "Output");		// show / don't show
	design->SetColLabelValue(tmpcol++, "Table");		// table
	design->SetColLabelValue(tmpcol++, "Column");		// column
	design->SetColLabelValue(tmpcol++, "Expression");	// expression 
	design->SetColLabelValue(tmpcol++, "Alias");		// alias
	design->SetColLabelValue(tmpcol++, "Aggregate");	// aggregate
	design->SetColLabelValue(tmpcol++, "Condition");	// inner where/having clause
	design->SetColLabelValue(tmpcol++, "Or");			// outer where/having clause
	design->SetColLabelValue(tmpcol++, "Or");			// outer where/having clause
	design->SetColLabelValue(tmpcol++, "Or");			// outer where/having clause
	design->SetColLabelValue(tmpcol++, "Or");			// outer where/having clause
	design->SetColLabelValue(tmpcol++, "Or");			// outer where/having clause

	// Update the design
    design->UpdateDimensions();

	// Setup the sql tab
    sql = new wxTextCtrl(notebook, CTL_SQLPANEL);

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

/*
    //Setup a Browser imagelist
    wxImageList *browserImages = new wxImageList(16, 16);
    browser->SetImageList(browserImages);

    //Stuff the Image List
    browserImages->Add(wxIcon(server_xpm));
    browserImages->Add(wxIcon(serverbad_xpm));
    browserImages->Add(wxIcon(database_xpm));
    browserImages->Add(wxIcon(language_xpm));
    browserImages->Add(wxIcon(namespace_xpm));
    browserImages->Add(wxIcon(aggregate_xpm));
    browserImages->Add(wxIcon(function_xpm));
    browserImages->Add(wxIcon(operator_xpm));
    browserImages->Add(wxIcon(sequence_xpm));
    browserImages->Add(wxIcon(table_xpm));
    browserImages->Add(wxIcon(type_xpm));
    browserImages->Add(wxIcon(view_xpm));
    browserImages->Add(wxIcon(user_xpm));
    browserImages->Add(wxIcon(group_xpm));
    browserImages->Add(wxIcon(baddatabase_xpm));
    browserImages->Add(wxIcon(closeddatabase_xpm));

    // Add the root node
    pgObject *serversObj = new pgObject(PG_SERVERS, wxString("Servers"));
    servers = browser->AddRoot(wxT("Servers"), 0, -1, serversObj);
    pgObject *addServerObj = new pgObject(PG_ADD_SERVER, wxString("Add Server"));
    browser->AppendItem(servers, wxT("Add Server..."), 0, -1, addServerObj);
    browser->Expand(servers);

    // Setup the property imagelist
    wxImageList *propertiesImages = new wxImageList(16, 16);
    properties->SetImageList(propertiesImages, wxIMAGE_LIST_SMALL);
    propertiesImages->Add(wxIcon(property_xpm));

    // Add the property view columns
    properties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    properties->InsertItem(0, wxT("No properties are available for the current selection"), 0);

    // Setup a statistics view imagelist
    wxImageList *statisticsImages = new wxImageList(16, 16);
    statistics->SetImageList(statisticsImages, wxIMAGE_LIST_SMALL);
    statisticsImages->Add(wxIcon(statistics_xpm));

    // Add the statistics view columns & set the colour
    statistics->InsertColumn(0, wxT("Statistics"), wxLIST_FORMAT_LEFT, 500);
    statistics->InsertItem(0, wxT("No statistics are available for the current selection"), 0);
    wxColour background;
    background = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    statistics->SetBackgroundColour(background);

    // Load servers
    RetrieveServers();
	*/

	//DrawTablesAndViews();

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::DrawTablesAndViews()
{
	wxMiniFrame *test;

	test = new wxMiniFrame(this, -1, "Table 1" );
	test->Show();

	wxMessageBox("This is not yet implemented.");

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
frmQueryBuilder::~frmQueryBuilder()
{
    extern sysSettings *settings;

	settings->SetFrmQueryBuilderPos(GetPosition());
	settings->SetFrmQueryBuilderSize(GetSize());

	delete datagramContextMenu;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnExit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void frmQueryBuilder::OnAddTableView()
{
	this->addtableview->InitLists();
	addtableview->Show();  
}

void frmQueryBuilder::OnSize(wxSizeEvent& event)
{
	wxLayoutAlgorithm layout;
	layout.LayoutMDIFrame(this);
}

void frmQueryBuilder::OnRightClick(wxPoint& point)
{
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
            sashwindow->SetDefaultSize(wxSize(1000, 
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
	int count = this->m_aliases.GetCount();

	if ( newname == "" )
		newname = firstname;

	wxString alias = newname;

	for (int si = 0; si < count; si++ )
	{
		wxString oldname = this->m_aliases.Item(si);
		if ( oldname.Lower() == newname.Lower() )
		{
			alias = firstname + wxString::Format( "_%d", postfix++ );
			wxString tmpalias = this->GetTableViewAlias(firstname, 
				alias, postfix);
			if ( alias.Lower() != tmpalias.Lower() )
				alias = tmpalias;
		}
	}

	return alias;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::AddColumn(frmChildTableViewFrame *frame, int item)
{
	// Get the number of rows
	int rows = design->GetNumberRows();
	
	// Add a new row
	if (!design->AppendRows(1))
	{
		wxMessageBox("Can't add another column.");
		return;
	}

	// Output column
   const wxString outputchoices[] =
    {
		_T("Yes"),
		_T("No"),
    };
    design->SetCellEditor(rows, 0, 
		new wxGridCellChoiceEditor(2, outputchoices));
    design->SetCellValue(rows, 0, outputchoices[0]);
    design->SetCellOverflow(rows, 0, FALSE);

	// Table Column
	UpdateGridTables(frame);

	// Column Column
	UpdateGridColumns(frame,item);

	// Expression Column
	UpdateGridExpressions();

	// Aggregate Column
    const wxString aggregatechoices[] =
    {
		_T("Where"),
		_T("Group by"),
        _T("Avg"),
        _T("Count"),
        _T("Max"),
        _T("Min"),
        _T("StdDev"),
        _T("Sum"),
        _T("Variance")
    };
    design->SetCellEditor(rows, 5, 
		new wxGridCellChoiceEditor(WXSIZEOF(aggregatechoices), 
		aggregatechoices));
    design->SetCellValue(rows, 5, aggregatechoices[0]);
    design->SetCellOverflow(rows, 5, FALSE);

    design->UpdateDimensions();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::UpdateGridTables(frmChildTableViewFrame *frame)
{
	// Get the number of rows
	int rows = design->GetNumberRows();

	// We need the name of the table that was selected
	wxString tablename = frame->GetTitle();

	// Table Choices
	int tblcount = this->m_aliases.GetCount();

	// Do this to sort the table choices
	wxSortedArrayString tablechoices;
	for (int si = 0; si < tblcount; si++ )
	{
		tablechoices.Add(this->m_aliases[si]);
	}

	// We need to know the index of the table that was chosen
	int chosentable;

	// Do this because SetCellEditor wont take accept wxSortedArrayString
	wxString *tablechoices2 = new wxString[tblcount];
	for (si = 0; si < tblcount; si++ )
	{
		tablechoices2[si] = tablechoices[si];
		if ( tablechoices[si] == tablename )
			chosentable = si;
	}

	// Update the table editors
	for (si = 0; si < rows; si++ )
	{
		// Set the cell editor to a dropdown
		wxGridCellChoiceEditor *tmpeditor = 
			new wxGridCellChoiceEditor(tblcount, tablechoices2);

		// We MUST hide the edit control to replace it
		design->HideCellEditControl();
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
											int item)
{
	// Get the number of rows
	int rows = design->GetNumberRows();

	// We need the name of the column that was selected
	wxString columnname = frame->m_columnlist->GetString(item);

	// Column Choices
	int colcount = frame->m_columnlist->GetCount();

	// Do this to sort the column choices
	wxSortedArrayString columnchoices;
	for (int si = 0; si < colcount; si++ )
	{
		columnchoices.Add(frame->m_columnlist->GetString(si));
	}

	// We need to know the index of the column that was chosen
	int chosencolumn = 0;

	// Do this because SetCellEditor wont take accept wxSortedArrayString
	wxString *columnchoices2 = new wxString[colcount];
	for (si = 0; si < colcount; si++ )
	{
		columnchoices2[si] = columnchoices[si];
		if ( columnchoices[si] == columnname )
			chosencolumn = si;
	}

	// Update the column editors
	for (si = 0; si < rows; si++ )
	{
		// Set the cell editor to a dropdown
		wxGridCellChoiceEditor *tmpeditor = 
			new wxGridCellChoiceEditor(colcount, columnchoices2);

		// We MUST hide the edit control to replace it
		design->HideCellEditControl();
		design->SetCellEditor(si, 2, tmpeditor);

		// Get the current value
		wxString tmpname = this->design->GetCellValue( si, 2 );

		// Unless there is already a value, use the chosen value
		if ( tmpname.length() > 0 )
		{
			for (int sj = 0; sj < colcount; sj++ )
			{
				if ( columnchoices2[sj] == tmpname )
					design->SetCellValue(si, 2, columnchoices2[sj]);
			}
		}		
		else
		{
			design->SetCellValue(si, 2, columnchoices2[chosencolumn]);
		}

		// Show the control again		
		design->ShowCellEditControl();

	}

	// Cleanup
	delete[] columnchoices2;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::UpdateGridExpressions()
{
	// Get the number of rows
	int rows = design->GetNumberRows();

	// Update the expressions
	for ( int si = 0; si < rows; si++ )
	{
		// We need the current expression
		wxString currentexpression = design->GetCellValue(si, 3);

		// Don't update it if it already has a value
		if ( currentexpression.length() < 1 )
		{
			// We need the table name
			wxString tablename = design->GetCellValue(si, 1);

			// We need the column name
			wxString columnname = design->GetCellValue(si, 2);

			// The new expression
			wxString expression = tablename + "." + columnname;

			design->SetCellValue(si, 3, expression);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::OnNotebookPageChanged(wxNotebookEvent& event)
{
	int page = event.GetSelection();

	switch (page)
	{
	case 1:
		BuildQuery();
		break;
	case 2:
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
	// The query strings
	wxString querystr = "SELECT";
	wxString tablestr = "";
	wxString columnstr = "";

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
			tablestr += tablealiases[si] + tables[si];
		else
			tablestr += ", " + tablealiases[si] + tables[si];
	}

	// Iterate through the rows to build the column list
	for ( si = 0; si < rows; si++ )
	{
		// Grab data
		wxString output = design->GetCellValue(si, 0);
		wxString table = design->GetCellValue(si, 1);
		wxString column = design->GetCellValue(si, 2);
		wxString expression = design->GetCellValue(si, 3);
		wxString alias = design->GetCellValue(si, 4);
		wxString aggregate = design->GetCellValue(si, 5);
		wxString condition = design->GetCellValue(si, 6);

		wxString *orcondition = new wxString[5];

		orcondition[0] = design->GetCellValue(si, 7);
		orcondition[1] = design->GetCellValue(si, 8);
		orcondition[2] = design->GetCellValue(si, 9);
		orcondition[3] = design->GetCellValue(si, 10);
		orcondition[4] = design->GetCellValue(si, 11);

		delete[] orcondition;

		if ( si==0 ) 
			columnstr += expression;
		else
			columnstr += ", " + expression;
	}

	querystr += " " + columnstr + " FROM " + tablestr;

	this->sql->SetValue(querystr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQueryBuilder::RunQuery()
{
	BuildQuery();

	wxString query = this->sql->GetValue();
	wxString querycount = "SELECT count(*) AS ct FROM (" + query + ") a";

	pgSet *queryset = database->ExecuteSet(wxT(querycount));
	if (queryset->Eof())
		return;

	int count = atoi(queryset->GetVal(wxT("ct")));

	if (count > 100)
	{
		wxString tmpstr = wxString::Format("This query will return %d results."
			"\n\nLoad all results?", count); 

		wxMessageDialog *messagebox = 
			new wxMessageDialog(this, tmpstr, "Query", wxYES_NO );

		if (messagebox->ShowModal() == wxID_NO)
			return;
	}

	pgSet *querydata = database->ExecuteSet(wxT(query));
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

	// Show the grid again
	data->Show();
}
