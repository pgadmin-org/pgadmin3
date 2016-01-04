//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddGenerationWizard.cpp - Wizard to allow generation of tables.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/regex.h>

// App headers
#include "schema/pgSchema.h"
#include "dd/ddmodel/ddGenerationWizard.h"
#include "dd/ddmodel/ddDBReverseEngineering.h"
#include "gqb/gqbGridRestTable.h"
#include "images/gqbOrderAddAll.pngc"
#include "images/gqbOrderRemoveAll.pngc"
#include "images/gqbOrderRemove.pngc"
#include "images/gqbOrderAdd.pngc"


#include "images/continue.pngc"

const wxColour LIGHT_YELLOW(0xff, 0xff, 0x80);

BEGIN_EVENT_TABLE(ddGenerationWizard, wxWizard)
	EVT_WIZARD_FINISHED(wxID_ANY, ddGenerationWizard::OnFinishPressed)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, ddGenerationWizard::OnWizardPageChanging)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SelGenTablesPage, wxWizardPage)
	EVT_BUTTON(DDREMOVE, SelGenTablesPage::OnButtonRemove)
	EVT_BUTTON(DDREMOVEALL, SelGenTablesPage::OnButtonRemoveAll)
	EVT_BUTTON(DDADD, SelGenTablesPage::OnButtonAdd)
	EVT_BUTTON(DDADDALL, SelGenTablesPage::OnButtonAddAll)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, SelGenTablesPage::OnWizardPageChanging)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SelGenSchemaPage, wxWizardPage)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, SelGenSchemaPage::OnWizardPageChanging)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ReportGridPage, wxWizardPage)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, ReportGridPage::OnWizardPageChanging)
END_EVENT_TABLE()

ddGenerationWizard::ddGenerationWizard(wxFrame *frame, ddDatabaseDesign *design, pgConn *connection, bool useSizer)
	: wxWizard(frame, wxID_ANY, wxT("Generate DDL from model"),
	           wxBitmap(*continue_png_bmp), wxDefaultPosition,
	           wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	conn = connection;
	figuresDesign = design;

	// a wizard page may be either an object of predefined class
	initialPage = new wxWizardPageSimple(this);

	frontText = new wxStaticText(initialPage, wxID_ANY,
	                             wxT("Build DDL from model tables.\n")
	                             wxT("\n")
	                             wxT("The next pages will allow the built of DDL of the current database designer model.")
	                             wxT("\n\n")
	                             wxT("Restrictions apply when using experimental function ALTER TABLE instead of DROP/CREATE:\n\n")
	                             wxT("1. Database connection is required to check differences with existing tables.\n\n")
	                             wxT("2. Only changes done in the design will be applied to the db, not otherwise.\n\n")
	                             wxT("3. Rename of columns or tables is not (yet) supported.\n\n")
	                             wxT("4. Rename of constraints generate a drop and create, except for primary key constraints.\n\n")
	                             wxT("5. All constraints should have a defined name.\n\n")
	                             , wxPoint(5, 5)
	                            );

	page2 = new SelGenTablesPage(this, initialPage);
	initialPage->SetNext(page2);
	page3 = new SelGenSchemaPage(this, page2);
	page2->SetNext(page3);
	page4 = new ReportGridPage(this, page3);
	page3->SetNext(page4);

	if (useSizer)
	{
		// allow the wizard to size itself around the pages
		GetPageAreaSizer()->Add(initialPage);
	}
}

ddGenerationWizard::~ddGenerationWizard()
{
}

void ddGenerationWizard::OnFinishPressed(wxWizardEvent &event)
{
	wxString strChoicesSimple[1] = {_("Create table")};
	wxString strChoicesAlter[4] = {_("Alter table"), _("Drop, then create"), _("Create table [conflict]"), wxT("No action")};
	wxArrayString tables;
	wxArrayInt options;
	if(page4->getGrid()->GetRows() > 0)
	{
		int i;
		for(i = page4->getGrid()->GetRows() - 1; i >= 0; i--)
		{
			tables.Add(page4->getGrid()->GetCellValue(i, 0));
			wxString value = page4->getGrid()->GetCellValue(i, 1);
			if(value.IsSameAs(strChoicesAlter[0]))
			{
				options.Add(DDGENALTER);
			}
			else if(value.IsSameAs(strChoicesAlter[1]))
			{
				options.Add(DDGENDROPCRE);
			}
			else if(value.IsSameAs(strChoicesAlter[2]) || value.IsSameAs(strChoicesSimple[0]))
			{
				options.Add(DDGENCREATE);
			}
			else if(value.IsSameAs(strChoicesAlter[3]))
			{
				options.Add(DDGENNOTHING);
			}
		}
	}
	DDL = getDesign()->generateList(tables, options, conn, schemaName);
}

void ddGenerationWizard::OnWizardPageChanging(wxWizardEvent &event)
{
	if(event.GetDirection())
	{
		page2->RefreshTablesList();
	}
}

// -----  SelGenTablesPage Implementation

SelGenTablesPage::SelGenTablesPage(wxWizard *parent, wxWizardPage *prev)
	: wxWizardPage(parent)
{
	wparent = (ddGenerationWizard *) parent;
	m_prev = prev;
	m_next = NULL;

	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(2, 3, 0, 0);
	this->SetSizer(mainSizer);

	leftText = new wxStaticText(this, wxID_STATIC, _("Table(s) from selected schema"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	mainSizer->Add(leftText);
	centerText = new wxStaticText(this, wxID_STATIC, _(" "), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	mainSizer->Add(centerText);

	rightText = new wxStaticText(this, wxID_STATIC, _("Tables(s) to be generated"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	mainSizer->Add(rightText, wxALIGN_LEFT);

	// Left listbox with all tables
	m_allTables = new wxListBox( this, DDALLTABS, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED | wxLB_ALWAYS_SB | wxLB_SORT);
	mainSizer->AddGrowableRow(1);
	mainSizer->AddGrowableCol(0);
	mainSizer->Add(m_allTables , 1, wxEXPAND);

	addBitmap = *gqbOrderAdd_png_bmp;
	addAllBitmap = *gqbOrderAddAll_png_bmp;
	removeBitmap = *gqbOrderRemove_png_bmp;
	removeAllBitmap = *gqbOrderRemoveAll_png_bmp;

	buttonAdd = new wxBitmapButton( this, DDADD,  addBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add Column") );
	buttonAdd->SetToolTip(_("Add the selected table(s)"));
	buttonAddAll = new wxBitmapButton( this, DDADDALL,  addAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add All Columns") );
	buttonAddAll->SetToolTip(_("Add all tables"));
	buttonRemove = new wxBitmapButton( this, DDREMOVE,  removeBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove Column") );
	buttonRemove->SetToolTip(_("Remove the selected table(s)"));
	buttonRemoveAll = new wxBitmapButton( this, DDREMOVEALL,  removeAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove All Columns") );
	buttonRemoveAll->SetToolTip(_("Remove all tables"));

	wxBoxSizer *buttonsSizer = new wxBoxSizer( wxVERTICAL );

	buttonsSizer->Add(
	    this->buttonAdd,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	buttonsSizer->Add(
	    this->buttonAddAll,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	buttonsSizer->Add(
	    this->buttonRemove,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	buttonsSizer->Add(
	    this->buttonRemoveAll,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 3

	mainSizer->Add(
	    buttonsSizer,
	    0,                                        // make vertically unstretchable
	    wxALIGN_CENTER );                         // no border and centre horizontally

	//right listbox with selected tables from schema to be imported.
	m_selTables = new wxListBox( this, DDSELTABS, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED | wxLB_ALWAYS_SB | wxLB_SORT);
	mainSizer->AddGrowableCol(2);
	mainSizer->Add(m_selTables , 1, wxEXPAND);
	mainSizer->Fit(this);
}

SelGenTablesPage::~SelGenTablesPage()
{
	if(rightText)
		delete rightText;
	if(centerText)
		delete centerText;
	if(leftText)
		delete leftText;
	if(m_allTables)
		delete m_allTables;
	if(m_selTables)
		delete m_selTables;
	if(buttonAdd)
		delete buttonAdd;
	if(buttonAddAll)
		delete buttonAddAll;
	if(buttonRemove)
		delete buttonRemove;
	if(buttonRemoveAll)
		delete buttonRemoveAll;
}

void SelGenTablesPage::OnButtonAdd(wxCommandEvent &)
{
	wxArrayInt positions;
	if(m_allTables->GetSelections(positions) > 0)
	{
		int i;
		int size = positions.Count();
		for(i = 0; i < size; i++)
		{
			m_selTables->Append(m_allTables->GetString(positions[i]));
			m_allTables->Deselect(positions[i]);
		}

		for(i = (size - 1); i >= 0 ; i--)
		{
			m_allTables->Delete(positions[i]);
		}

		if(m_allTables->GetCount() > 0)
			m_allTables->Select(0);
	}
}

void SelGenTablesPage::OnButtonAddAll(wxCommandEvent &)
{
	int itemsCount = m_allTables->GetCount();
	if( itemsCount > 0)
	{
		do
		{
			m_allTables->Deselect(0);
			m_selTables->Append(m_allTables->GetString(0));
			m_allTables->Delete(0);
			itemsCount--;
		}
		while(itemsCount > 0);
	}
}

void SelGenTablesPage::OnButtonRemove(wxCommandEvent &)
{
	wxArrayInt positions;
	if(m_selTables->GetSelections(positions) > 0)
	{
		int i;
		int size = positions.Count();  // Warning about conversion should be ignored
		for(i = 0; i < size; i++)
		{
			m_allTables->Append(m_selTables->GetString(positions[i]));
			m_selTables->Deselect(positions[i]);
		}

		for(i = (size - 1); i >= 0 ; i--)
		{
			m_selTables->Delete(positions[i]);
		}

		if(m_selTables->GetCount() > 0)
			m_selTables->Select(0);
	}
}

void SelGenTablesPage::OnButtonRemoveAll(wxCommandEvent &)
{
	int itemsCount = m_selTables->GetCount();
	if( itemsCount > 0)
	{
		do
		{
			m_selTables->Deselect(0);
			m_allTables->Append(m_selTables->GetString(0));
			m_selTables->Delete(0);
			itemsCount--;
		}
		while(itemsCount > 0);
	}
}


void SelGenTablesPage::moveToSelectList(wxString tableName)
{
	int position = m_allTables->FindString(tableName);
	if(position != wxNOT_FOUND)
	{
		m_selTables->Append(m_allTables->GetString(position));
		m_allTables->Delete(position);
	}
}


void SelGenTablesPage::RefreshTablesList()
{
	wxArrayString tablesList;
	m_allTables->Clear();
	m_selTables->Clear();

	hdIteratorBase *iterator = wparent->getDesign()->getEditor()->modelFiguresEnumerator();
	hdIFigure *tmpFigure;
	ddTableFigure *table;

	while(iterator->HasNext())
	{
		tmpFigure = (hdIFigure *)iterator->Next();
		if(tmpFigure->getKindId() == DDTABLEFIGURE)
		{
			table = (ddTableFigure *)tmpFigure;
			tablesList.Add(table->getTableName());
		}
	}
	delete iterator;
	m_allTables->Set(tablesList, (void **)NULL);

	int max = wparent->preSelTables.Count();
	if(max > 0)
	{
		int i;
		for(i = 0; i < max; i++)
		{
			moveToSelectList(wparent->preSelTables[i]);
		}
	}

}


void SelGenTablesPage::OnWizardPageChanging(wxWizardEvent &event)
{
	if(event.GetDirection() && m_selTables->GetCount() <= 0)
	{
		wxMessageBox(_("Please select at least one table to move to next step."), _("No tables selected"), wxICON_WARNING | wxOK, this);
		event.Veto();
	}
}

// -----  SelGenSchemaPage Implementation

SelGenSchemaPage::SelGenSchemaPage(wxWizard *parent, wxWizardPage *prev)
	: wxWizardPage(parent)
{
	wparent = (ddGenerationWizard *) parent;
	m_prev = prev;
	m_next = NULL;

	// A top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL );
	this->SetSizer(topSizer);

	//Add a message
	message = new wxStaticText(this, wxID_STATIC, _("Please select a schema to use as check target.\n\nThis affects the choice between CREATE/ALTER table(s) matching for the DDL generated depending on whether or not the table exists in that schema."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	topSizer->Add(message);
	topSizer->AddSpacer(10);

	//Get Schemas info
	if(wparent && wparent->getConnection())
		refreshSchemas(wparent->getConnection());
	else
		schemasNames.Add(_("No schema"));

	//Add a listbox with schemas
	m_allSchemas = new wxListBox(this, DDALLSCHEMAS, wxDefaultPosition, wxDefaultSize, schemasNames, wxLB_SORT | wxLB_ALWAYS_SB | wxLB_SINGLE);
	topSizer->Add(m_allSchemas, 1, wxEXPAND);
}

SelGenSchemaPage::~SelGenSchemaPage()
{
	if(m_allSchemas)
		delete m_allSchemas;
	if(message)
		delete message;
}

void SelGenSchemaPage::OnWizardPageChanging(wxWizardEvent &event)
{
	if(event.GetDirection() && m_allSchemas->GetSelection() == wxNOT_FOUND)
	{
		wxMessageBox(_("Please select an item to move to next step."), _("No choice made"), wxICON_WARNING | wxOK, this);
		event.Veto();
	}
	else if(event.GetDirection())
	{
		if(m_allSchemas->GetSelection() > 0)
		{
			wparent->OIDSelectedSchema = schemasHM[schemasNames[m_allSchemas->GetSelection()]];
			wparent->schemaName = schemasNames[m_allSchemas->GetSelection()];
			wxArrayString tables = ddImportDBUtils::getTablesNames(wparent->getConnection(), wparent->schemaName);
			wparent->getDesign()->unMarkSchemaOnAll();
			wparent->getDesign()->markSchemaOn(tables);
		}
		else
		{
			wparent->OIDSelectedSchema = -1;
			wparent->schemaName = _("");
		}
		wparent->page4->populateGrid();
	}
}

void SelGenSchemaPage::refreshSchemas(pgConn *connection)
{

	schemasHM.clear();
	schemasNames.Clear();
	schemasNames.Add(wxT("Not schema selected"));

	// Search schemas and insert them
	wxString restr =  wxT(" WHERE ")
	                  wxT("NOT ")
	                  wxT("((nspname = 'pg_catalog' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pg_class' AND relnamespace = nsp.oid LIMIT 1)) OR\n")
	                  wxT("(nspname = 'pgagent' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pga_job' AND relnamespace = nsp.oid LIMIT 1)) OR\n")
	                  wxT("(nspname = 'information_schema' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'tables' AND relnamespace = nsp.oid LIMIT 1)) OR\n")
	                  wxT("(nspname LIKE '_%' AND EXISTS (SELECT 1 FROM pg_proc WHERE proname='slonyversion' AND pronamespace = nsp.oid LIMIT 1)) OR\n")
	                  wxT("(nspname = 'dbo' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'systables' AND relnamespace = nsp.oid LIMIT 1)) OR\n")
	                  wxT("(nspname = 'sys' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'all_tables' AND relnamespace = nsp.oid LIMIT 1)))\n");

	if (connection->EdbMinimumVersion(8, 2))
	{
		restr += wxT("  AND nsp.nspparent = 0\n");
		// Do not show dbms_job_procedure in schemas
		if (!settings->GetShowSystemObjects())
			restr += wxT("AND NOT (nspname = 'dbms_job_procedure' AND EXISTS(SELECT 1 FROM pg_proc WHERE pronamespace = nsp.oid and proname = 'run_job' LIMIT 1))\n");
	}

	wxString sql;

	if (connection->BackendMinimumVersion(8, 1))
	{
		sql = wxT("SELECT CASE WHEN nspname LIKE E'pg\\\\_temp\\\\_%' THEN 1\n")
		      wxT("            WHEN (nspname LIKE E'pg\\\\_%') THEN 0\n");
	}
	else
	{
		sql = wxT("SELECT CASE WHEN nspname LIKE 'pg\\\\_temp\\\\_%' THEN 1\n")
		      wxT("            WHEN (nspname LIKE 'pg\\\\_%') THEN 0\n");
	}
	sql += wxT("            ELSE 3 END AS nsptyp, nspname, nsp.oid\n")
	       wxT("  FROM pg_namespace nsp\n")
	       + restr +
	       wxT(" ORDER BY 1, nspname");

	pgSet *schemas = connection->ExecuteSet(sql);
	wxTreeItemId parent;

	if (schemas)
	{
		while (!schemas->Eof())
		{
			wxString name = schemas->GetVal(wxT("nspname"));
			long nsptyp = schemas->GetLong(wxT("nsptyp"));

			wxStringTokenizer tokens(settings->GetSystemSchemas(), wxT(","));
			while (tokens.HasMoreTokens())
			{
				wxRegEx regex(tokens.GetNextToken());
				if (regex.Matches(name))
				{
					nsptyp = SCHEMATYP_USERSYS;
					break;
				}
			}

			if (nsptyp <= SCHEMATYP_USERSYS && !settings->GetShowSystemObjects())
			{
				schemas->MoveNext();
				continue;
			}
			schemasNames.Add(name);
			schemasHM[name] = schemas->GetOid(wxT("oid"));
			schemas->MoveNext();
		}

		delete schemas;
	}
}

// -----  SelGenSchemaPage Implementation

ReportGridPage::ReportGridPage(wxWizard *parent, wxWizardPage *prev)
	: wxWizardPage(parent)
{
	wparent = (ddGenerationWizard *) parent;
	m_prev = prev;
	m_next = NULL;

	// A top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL );
	this->SetSizer(topSizer);

	// Add a message
	message = new wxStaticText(this, wxID_STATIC, _("Please check the choice for each table:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	topSizer->Add(message);
	topSizer->AddSpacer(10);

	reportGrid = new wxDDGrid(this, DDTABSGRID);
	reportGrid->SetRowLabelSize(25);
	reportGrid->CreateGrid(wparent->page2->countSelTables(), 2);
	populateGrid();
	topSizer->Add(reportGrid, 1, wxEXPAND);

	// Add Event
	this->Connect(DDTABSGRID, wxEVT_GRID_CELL_LEFT_CLICK, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &ReportGridPage::OnCellLeftClick);
}

ReportGridPage::~ReportGridPage()
{

}

void ReportGridPage::populateGrid()
{
	reportGrid->ClearGrid();
	if(reportGrid->GetNumberRows() > 0)
		reportGrid->DeleteRows(0, reportGrid->GetNumberRows());
	int row , max = wparent->page2->countSelTables();
	reportGrid->InsertRows(0, max);
	reportGrid->SetColLabelValue(0, _("Tables"));
	reportGrid->SetColLabelValue(1, _("Select an action"));
	wxString strChoicesSimple[1] = {_("Create table")};
	wxString strChoicesAlter[4] = {_("Alter table"), _("Drop, then create"), _("Create table [conflict]"), wxT("No action")};

	//Two Steps process first for tables that belongs to a schema (need alter table)
	//Later for normal tables (just create table)
	int useRow = 0;
	for(row = 0; row < max; row++)
	{
		ddTableFigure *table = wparent->getDesign()->getTable(wparent->page2->getSelTableName(row));
		if(table != NULL && table->getBelongsToSchema())
		{
			wxString labelString = wxString::Format(wxT("%i"), (row + 1));
			reportGrid->SetRowLabelValue(row, labelString);
			reportGrid->SetReadOnly( row, 0, true);
			reportGrid->SetCellValue( row, 0, wparent->page2->getSelTableName(row));
			reportGrid->SetCellRenderer(row, 1, new wxGridCellComboBoxRenderer);
			reportGrid->SetCellEditor(row, 1, new dxGridCellSizedChoiceEditor(WXSIZEOF(strChoicesAlter), strChoicesAlter));
			reportGrid->SetCellValue( row, 1, _("Alter table"));
			reportGrid->SetCellBackgroundColour(row, 0, LIGHT_YELLOW);
			reportGrid->SetCellBackgroundColour(row, 1, LIGHT_YELLOW);
			useRow++;
		}
		else if(table == NULL)
		{
			wxMessageBox(_("Metadata of table to be generated not found at database designer model"), _("Error at generation process"),  wxICON_ERROR | wxOK);
			return;
		}
	}

	// Normal tables (just create table)
	for(row = 0; row < max; row++)
	{
		ddTableFigure *table = wparent->getDesign()->getTable(wparent->page2->getSelTableName(row));
		if(table != NULL && !table->getBelongsToSchema())
		{
			reportGrid->SetCellValue( row, 0, wparent->page2->getSelTableName(row));
			reportGrid->SetCellRenderer(row, 1, new wxGridCellComboBoxRenderer);
			reportGrid->SetCellEditor(row, 1, new dxGridCellSizedChoiceEditor(WXSIZEOF(strChoicesSimple), strChoicesSimple));
			reportGrid->SetCellValue( row, 1, _("Create table"));
			useRow++;
		}
		else if(table == NULL)
		{
			wxMessageBox(_("Metadata of table to be generated not found at database designer model"), _("Error importing at generation process"),  wxICON_ERROR | wxOK);
			return;
		}

	}

	reportGrid->AutoSizeColumns();
	reportGrid->Fit();

}

void ReportGridPage::OnWizardPageChanging(wxWizardEvent &event)
{
	if(!event.GetDirection())
	{
		//Reset tables after a warning
		int answer = wxMessageBox(_("Going back to \"Select schema\" page will reinitialize all selections.\nDo you want to continue?"), _("Going back?"), wxYES_NO | wxCANCEL, this);
		if (answer != wxYES)
		{
			event.Veto();
		}
	}
}

void ReportGridPage::OnCellLeftClick(wxGridEvent &event)
{
	// Only show editor y case of column 1
	if(event.GetCol() == 1)
	{
		reportGrid->ComboBoxEvent(event);
	}

	event.Skip();
}


//
//  -- Special version of wxGrid to allow use of fast comboboxes and grid columns auto fit
//
wxDDGrid::wxDDGrid(wxWindow *parent, wxWindowID id):
	wxGrid(parent, id, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_BESTWRAP, wxT("")),
	m_selTemp(NULL), keepFit(1)
{
	// Initially all columns have s-factor=1
	for( unsigned i = 0; i < 10; ++i ) sf[i] = 1;

	Connect( wxEVT_SIZE, wxSizeEventHandler( wxDDGrid::OnSizeEvt) );
	// Adjust the default row height to be more compact
	wxFont font = GetLabelFont();
	int nWidth = 0;
	int nHeight = 18;
	GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
	SetColLabelSize(nHeight + 6);
	SetRowLabelSize(35);
#ifdef __WXGTK__
	SetDefaultRowSize(nHeight + 8, TRUE);
#else
	SetDefaultRowSize(nHeight + 4, TRUE);
#endif
}

void wxDDGrid::ComboBoxEvent(wxGridEvent &event)
{

	// This forces the cell to go into edit mode directly
	this->m_waitForSlowClick = TRUE;
	int row = event.GetRow();
	int col = event.GetCol();

	this->SetGridCursor(row, col);

	// Store the click co-ordinates in the editor if possible
	// if an editor has created a ClientData area, we presume it's
	// a wxPoint and we store the click co-ordinates
	wxGridCellEditor *pEditor  = this->GetCellEditor(event.GetRow(), event.GetCol());
	wxPoint *pClickPoint = (wxPoint *)pEditor->GetClientData();
	if (pClickPoint)
	{
		*pClickPoint = this->ClientToScreen(event.GetPosition());
#ifndef __WINDOWS__
		EnableCellEditControl(true);
#endif
	}

	// hack to prevent selection from being lost when click combobox
	if (this->IsInSelection(event.GetRow(), event.GetCol()))
	{
		this->m_selTemp = this->m_selection;
		this->m_selection = NULL;
	}
	pEditor->DecRef();
}

void wxDDGrid::RevertSel()
{
	if (m_selTemp)
	{
		wxASSERT(m_selection == NULL);
		m_selection = m_selTemp;
		m_selTemp = NULL;
	}
}

void wxDDGrid::OnSizeEvt( wxSizeEvent &ev )
{
	if( !StretchIt() ) ev.Skip();
}

int wxDDGrid::StretchIt()
{
	int new_width = GetClientSize().GetWidth() - GetRowLabelSize() - 10;
	int fixedWidth = 0, numStretches = 0, numStretched = 0;

	for( int i = 0; i < GetNumberCols(); ++i )
	{
		if( sf[i] == 0 ) fixedWidth += GetColSize(i);
		else if( sf[i] < 0 )
		{
			AutoSizeColumn(i, false);
			fixedWidth += GetColSize(i);
		}
		else
		{
			numStretches += sf[i];
			numStretched += 1;
		}
	}

	// Now either we have space for normal layout or resort to wxGrid default behaviour
	if( numStretched && ((fixedWidth + numStretched * 10) < new_width) )
	{
		int stretchSpace = (new_width - fixedWidth) / numStretches;
		//BeginBatch();
		int i, max = GetNumberCols();
		for(i = 0; i < max; ++i )
			if( sf[i] > 0 )
				SetColSize(i, stretchSpace * sf[i]);
		//EndBatch();
		return 1;
	}
	return 0;
}
