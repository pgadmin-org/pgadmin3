//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmDatabaseDesigner.cpp - The database designer form
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/aui/aui.h>
#include <wx/splitter.h>
#include <wx/filename.h>

// App headers
#include "frm/frmMain.h"
#include "frm/frmDatabaseDesigner.h"
#include "ctl/ctlMenuToolbar.h"
#include "schema/pgObject.h"
#include "schema/pgDatabase.h"
#include "ctl/ctlSQLBox.h"

// Designer headers
#include "dd/dditems/figures/ddColumnKindIcon.h"
#include "dd/wxhotdraw/figures/wxhdPolyLineFigure.h"
#include "dd/wxhotdraw/figures/wxhdSimpleTextFigure.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/wxhotdraw/figures/wxhdRectangleFigure.h"
#include "dd/wxhotdraw/figures/wxhdBitmapFigure.h"
#include "dd/wxhotdraw/tools/wxhdConnectionCreationTool.h"
#include "dd/ddmodel/ddModelBrowser.h"

#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/ddmodel/ddDrawingView.h"

#include "dd/dditems/figures/ddTextTableItemFigure.h"
#include "dd/dditems/figures/ddColumnFigure.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/utilities/ddTableNameDialog.h"

#include "dd/dditems/figures/xml/ddXmlStorage.h"

// Icons
#include "images/ddmodel-32.pngc"
#include "images/file_new.pngc"
#include "images/table.pngc"
#include "images/ddRemoveTable2.pngc"
#include "images/continue.pngc"
#include "images/ddnewdiagram.pngc"
#include "images/ddgendiagram.pngc"
#include "images/help.pngc"
#include "images/file_save.pngc"
#include "images/file_open.pngc"

BEGIN_EVENT_TABLE(frmDatabaseDesigner, pgFrame)
	EVT_MENU(MNU_NEW,             			  	frmDatabaseDesigner::OnNewModel)
	EVT_MENU(MNU_ADDTABLE,        			  	frmDatabaseDesigner::OnAddTable)
	EVT_MENU(MNU_DELETETABLE,      			  	frmDatabaseDesigner::OnDeleteTable)
	EVT_MENU(MNU_ADDCOLUMN,         			frmDatabaseDesigner::OnAddColumn)
	EVT_MENU(MNU_GENERATEDIAGRAM,   			frmDatabaseDesigner::OnDiagramGeneration)
	EVT_MENU(MNU_GENERATEMODEL,     			frmDatabaseDesigner::OnModelGeneration)
	EVT_MENU(MNU_SAVEMODEL,						frmDatabaseDesigner::OnModelSave)
	EVT_MENU(MNU_SAVEMODELAS,       			frmDatabaseDesigner::OnModelSaveAs)
	EVT_MENU(MNU_LOADMODEL,						frmDatabaseDesigner::OnModelLoad)
	EVT_MENU(MNU_NEWDIAGRAM,					frmDatabaseDesigner::OnAddDiagram)
	EVT_MENU(MNU_DELDIAGRAM,					frmDatabaseDesigner::OnDeleteDiagram)
	EVT_MENU(MNU_RENDIAGRAM,					frmDatabaseDesigner::OnRenameDiagram)
	EVT_MENU(MNU_CHGFONT,						frmDatabaseDesigner::OnChangeDefaultFont)
	EVT_AUINOTEBOOK_PAGE_CLOSE(CTL_DDNOTEBOOK,  frmDatabaseDesigner::OnDeleteDiagramTab)
	EVT_AUINOTEBOOK_PAGE_CLOSED(CTL_DDNOTEBOOK, frmDatabaseDesigner::OnDeletedDiagramTab)
	EVT_AUINOTEBOOK_BUTTON(CTL_DDNOTEBOOK,      frmDatabaseDesigner::OnClickDiagramTab)
	EVT_AUINOTEBOOK_BG_DCLICK(CTL_DDNOTEBOOK,      frmDatabaseDesigner::OnAddDiagram2)
	EVT_CLOSE(                      			frmDatabaseDesigner::OnClose)
END_EVENT_TABLE()

frmDatabaseDesigner::frmDatabaseDesigner(frmMain *form, const wxString &_title, pgConn *conn)
	: pgFrame(NULL, _title)
{
	mainForm = form;
	SetTitle(wxT("Database Designer"));
	SetIcon(wxIcon(*ddmodel_32_png_ico));

	RestorePosition(100, 100, 600, 500, 450, 300);
	SetMinSize(wxSize(450, 300));

	// connection
	connection = conn;

	// notify wxAUI which frame to use
	manager.SetManagedWindow(this);
	manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG);

	wxWindowBase::SetFont(settings->GetSystemFont());

	// Set File menu
	fileMenu = new wxMenu();
	fileMenu->Append(MNU_NEW, _("&New database design\tCtrl-N"), _("Create a new database design"));
	fileMenu->AppendSeparator();
	fileMenu->Append(MNU_LOADMODEL, _("&Open Model..."), _("Open an existing database design from a file"));
	fileMenu->Append(MNU_SAVEMODEL, _("&Save Model"), _("Save changes at database design"));
	fileMenu->Append(MNU_SAVEMODELAS, _("&Save Model As..."), _("Save database design at new file"));
	fileMenu->AppendSeparator();
	fileMenu->Append(MNU_EXIT, _("E&xit\tCtrl-W"), _("Exit database designer window"));

	// Set Diagram menu
	diagramMenu = new wxMenu();
	diagramMenu->Append(MNU_NEWDIAGRAM, _("&New model diagram"), _("Create a new diagram"));
	diagramMenu->Append(MNU_DELDIAGRAM, _("&Delete selected model diagram..."), _("Delete selected diagram"));
	diagramMenu->Append(MNU_RENDIAGRAM, _("&Rename selected model diagram..."), _("Rename selected diagram"));

	// Set Preferences menu
	preferencesMenu = new wxMenu();
	preferencesMenu->Append(MNU_CHGFONT, _("&Change default font for figures..."), _("Change default font for figures"));

	// Set Help menu
	helpMenu = new wxMenu();
	helpMenu->Append(MNU_CONTENTS, _("&Help"), _("Open the helpfile."));
	helpMenu->Append(MNU_HELP, _("&SQL Help\tF1"), _("Display help on SQL commands."));

	// Set menu bar
	menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(diagramMenu, _("&Diagram"));
	menuBar->Append(preferencesMenu, _("&Preferences"));
	menuBar->Append(helpMenu, _("&Help"));
	SetMenuBar(menuBar);

	// Set status bar
	int iWidths[6] = {0, -1, 40, 150, 80, 80};
	CreateStatusBar(6);
	SetStatusBarPane(-1);
	SetStatusWidths(6, iWidths);

	// Set toolbar
	toolBar = new ctlMenuToolbar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	toolBar->SetToolBitmapSize(wxSize(16, 16));
	toolBar->AddTool(MNU_NEW, _("New Model"), *file_new_png_bmp, _("Create new model"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_NEWDIAGRAM, _("New Diagram"), *ddnewdiagram_png_bmp, _("Add new diagram"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_LOADMODEL, _("Open Model"), *file_open_png_bmp, _("Open existing model"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_SAVEMODEL, _("Save Model"), *file_save_png_bmp, _("Save current model"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_ADDTABLE, _("Add Table"), *table_png_bmp, _("Add empty table to the current model"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_DELETETABLE, _("Delete Table"), wxBitmap(*ddRemoveTable2_png_img), _("Delete selected table"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_ADDCOLUMN, _("Add Column"), *table_png_bmp, _("Add new column to the selected table"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_GENERATEMODEL, _("Generate Model"), *continue_png_bmp, _("Generate SQL for the current model"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_GENERATEDIAGRAM, _("Generate Diagram"), *ddgendiagram_png_bmp, _("Generate SQL for the current diagram"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_HELP, _("Help"), *help_png_bmp, _("Display help"), wxITEM_NORMAL);
	toolBar->Realize();

	// Create notebook for diagrams
	diagrams = new ctlAuiNotebook(this, CTL_DDNOTEBOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_CLOSE_ON_ALL_TABS);

	// Now, the scratchpad
	sqltext = new ctlSQLBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_RICH2);

	//Now, the Objects Browser
	wxSizer *browserSizer = new wxBoxSizer(wxALL);
	browserPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	// Add the database designer
	design = new ddDatabaseDesign(diagrams, this);

	// Create database model browser
	modelBrowser = new ddModelBrowser(browserPanel, DD_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER, design);
	design->registerBrowser(modelBrowser);

	// Set Sizers
	browserSizer->Add(modelBrowser, 1, wxEXPAND);
	browserPanel->SetSizer(browserSizer);
	browserSizer->SetSizeHints(browserPanel);

	// Add view to notebook
	diagrams->AddPage(design->createDiagram(diagrams, _("unnamed"), false)->getView(), _("unnamed"));

	// Add the panes
	manager.AddPane(diagrams,
	                wxAuiPaneInfo().Center().
	                Name(wxT("sqlQuery")).Caption(_("Database Designer")).
	                CaptionVisible(true).CloseButton(false).MaximizeButton(true).
	                Dockable(true).Movable(true));
	manager.AddPane(browserPanel,
	                wxAuiPaneInfo().Left().
	                Name(wxT("ModelBrowser")).Caption(_("Model Browser")).
	                CaptionVisible(true).CloseButton(true).
	                MinSize(wxSize(140, 100)).BestSize(wxSize(200, 200)));
	manager.AddPane(sqltext,
	                wxAuiPaneInfo().Bottom().
	                Name(wxT("sqlText")).Caption(_("SQL query")).
	                CaptionVisible(true).CloseButton(true).MaximizeButton(true).
	                MinSize(wxSize(200, 100)).BestSize(wxSize(350, 150)));
	manager.AddPane(toolBar,
	                wxAuiPaneInfo().Top().
	                Name(wxT("toolBar")).Caption(_("Tool bar")).
	                ToolbarPane().
	                LeftDockable(false).RightDockable(false));

	// Update the AUI manager
	manager.Update();

	//Update browser info
	modelBrowser->SetSize(browserPanel->GetSize());

	previousChanged = true;
	setModelChanged(false);
	SetStatusText(wxString(wxT("Ready")), 1);
}


frmDatabaseDesigner::~frmDatabaseDesigner()
{
	// Save form's position
	SavePosition();

	// Uninitialize wxAUIManager
	manager.UnInit();

	if (mainForm)
		mainForm->RemoveFrame(this);

	if(modelBrowser)
		delete modelBrowser;

	if(browserPanel)
		delete browserPanel;

	if(diagrams)
		delete diagrams;

	if(design)
		delete design;

	if (connection)
	{
		if (connection->IsAlive())
			delete connection;
	}
}


void frmDatabaseDesigner::Go()
{
	Show(true);
}

void frmDatabaseDesigner::setModelChanged(bool value)
{
	previousChanged = changed;
	changed = value;

	// optimization to avoid unneeded operations
	// false changed for false don't trigger it, but false changed to true trigger it.
	if (previousChanged != changed)
	{
		wxMenuItem *itemMenu = fileMenu->FindItem(MNU_SAVEMODEL);

		if (value)
		{
			itemMenu->Enable(true);
			toolBar->EnableTool(MNU_SAVEMODEL, true);
			setExtendedTitle();
		}
		else
		{
			itemMenu->Enable(false);
			toolBar->EnableTool(MNU_SAVEMODEL, false);
			setExtendedTitle();
		}
	}
}


void frmDatabaseDesigner::OnClose(wxCloseEvent &event)
{
	// Ask what to do with old model
	if (changed)
	{
		int answer = wxMessageBox(_("Save model changes?"), _("Confirm"), wxYES_NO | wxCANCEL);

		if (answer == wxYES)
		{
			if (lastFile.IsEmpty())
			{
				wxFileDialog openFileDialog( this, _("Save model"), _(""), _(""), _("*.pgd"),
				                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

				if (openFileDialog.ShowModal() == wxID_OK)
				{
					wxString path;
					path.append(openFileDialog.GetDirectory());
					path.append(wxFileName::GetPathSeparator());
					path.append(openFileDialog.GetFilename());
					if(!path.Lower().Matches(_("*.pgd")))
						path.append(_(".pgd"));
					lastFile = path;
				}
			}

			if (!lastFile.IsEmpty())
			{
				design->writeXmlModel(lastFile);
				setModelChanged(false);
				setExtendedTitle();
			}
		}
		else if (answer == wxCANCEL)
		{
			event.Veto();
			return;
		}
	}

	Hide();
	Destroy();
}

void frmDatabaseDesigner::setExtendedTitle()
{
	wxString title = wxT("Database Design");
	wxString chgStr;
	if (changed)
		chgStr = wxT(" *");

	if (lastFile.IsEmpty())
		SetTitle(title + chgStr);
	else
		SetTitle(title + wxT(" - [") + lastFile + wxT("]") + chgStr);
}

void frmDatabaseDesigner::OnAddTable(wxCommandEvent &event)
{
	if (diagrams->GetPageCount() > 0)
	{
		wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());

		ddTableNameDialog *newTableDialog = new ddTableNameDialog(
		    this,
		    design->getNewTableName(),
		    wxEmptyString,
		    NULL
		);

		bool done = false, existsTable;
		int answer;

		do
		{
			answer = newTableDialog->ShowModal();
			existsTable = design->getTable(newTableDialog->GetValue1()) != NULL;

			if (answer == wxID_OK && !newTableDialog->GetValue1().IsEmpty() && !existsTable)
			{
				ddTableFigure *newTable = new ddTableFigure(newTableDialog->GetValue1(),
				        view->getIdx(),
				        rand() % 90 + 200,
				        rand() % 90 + 140,
				        newTableDialog->GetValue2()
				                                           );
				design->addTableToView(view->getIdx(), newTable);
				design->refreshDraw(view->getIdx());
				setModelChanged(true);
				setExtendedTitle();
				done = true;
			}
			else if(existsTable && answer == wxID_OK)
			{
				wxMessageBox(_("You have to change the table name because there is already a table with that name in this model."), _("Table already existing"), wxICON_EXCLAMATION);
			}
		}
		while (answer != wxID_CANCEL && !done);

		delete newTableDialog;
	}
}

void frmDatabaseDesigner::OnDeleteTable(wxCommandEvent &event)
{
	if (diagrams->GetPageCount() > 0)
	{
		wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());
		view->getDrawing()->deleteSelectedFigures();
		setModelChanged(true);
		setExtendedTitle();
	}
}

void frmDatabaseDesigner::OnAddColumn(wxCommandEvent &event)
{
	if (diagrams->GetPageCount() > 0)
	{
		wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());
		ddTableFigure *table = design->getSelectedTable(view->getIdx());
		wxTextEntryDialog nameDialog (this, wxT("New column name"), wxT("Add a column"), wxT("NewColumn"));
		int answer;
		wxString tmpString;

		if (table)
		{
			bool again;
			do
			{
				again = false;
				answer = nameDialog.ShowModal();
				if (answer == wxID_OK)
				{
					tmpString = nameDialog.GetValue();
					if(table->getColByName(tmpString) == NULL)
					{
						table->addColumn(view->getIdx(), new ddColumnFigure(tmpString, table));
						setModelChanged(true);
						setExtendedTitle();
					}
					else
					{
						wxString msg(wxT("Error trying to add new column '"));
						msg.Append(tmpString);
						msg.Append(wxT("' column name already in use"));
						wxMessageDialog info( this, msg ,
						                      wxT("Column name already in use"),
						                      wxNO_DEFAULT | wxOK | wxICON_EXCLAMATION);
						again = true;
						info.ShowModal();
					}

				}
			}
			while(again);
		}
		view->Refresh();
	}
	else
	{
		wxMessageBox(_("Warning about adding a column to a table without a diagram"), _("Please create a model diagram first"), wxICON_EXCLAMATION);
	}
}


void frmDatabaseDesigner::OnNewModel(wxCommandEvent &event)
{
	wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());

	if (changed)
	{
		// Ask what to do with old model
		int answer = wxMessageBox(_("Save model changes?"), _("Confirm"), wxYES_NO | wxCANCEL);

		if (answer == wxYES)
		{
			if (!lastFile.IsEmpty())
				OnModelSave(event);
			else
				OnModelSaveAs(event);
		}
		else if (answer == wxCANCEL)
			return;
	}

	// Clean treeview, notebook, and sql pane
	design->emptyModel();
	while (diagrams->GetPageCount() > 0)
	{
		diagrams->RemovePage(0);
		design->deleteDiagram(0);
	}
	sqltext->SetText(wxEmptyString);

	// Add new diagram
	diagrams->AddPage(design->createDiagram(diagrams, _("unnamed"), false)->getView(), _("unnamed"));

	// Misc
	lastFile = wxEmptyString;
	setModelChanged(false);
	setExtendedTitle();
	UpdateToolbar();
}

void frmDatabaseDesigner::OnDiagramGeneration(wxCommandEvent &event)
{
	if (diagrams->GetPageCount() > 0)
	{
		wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());
		wxString errors;
		if(!design->validateModel(errors))
		{
			wxMessageDialog dialog(this, errors , wxT("Errors detected in the database model"), wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
			dialog.ShowModal();
		}
		else
		{
			sqltext->SetText(design->generateDiagram(view->getIdx()));
		}
	}
}

void frmDatabaseDesigner::OnModelGeneration(wxCommandEvent &event)
{
	wxString errors;
	if(!design->validateModel(errors))
	{
		wxMessageDialog dialog(this, errors , wxT("Errors detected in the database model"), wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
		dialog.ShowModal();
	}
	else
	{
		sqltext->SetText(design->generateModel());
	}
}

void frmDatabaseDesigner::OnModelSaveAs(wxCommandEvent &event)
{
	wxFileDialog openFileDialog( this, _("Save model"), _(""), _(""), _("*.pgd"),
	                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	if (openFileDialog.ShowModal() == wxID_OK)
	{
		wxString path;
		path.append( openFileDialog.GetDirectory() );
		path.append( wxFileName::GetPathSeparator() );
		path.append( openFileDialog.GetFilename() );
		if(!path.Lower().Matches(_("*.pgd")))
			path.append(_(".pgd"));
		lastFile = path;
		setModelChanged(false);
		setExtendedTitle();
		design->writeXmlModel(path);
	}
}

void frmDatabaseDesigner::OnModelSave(wxCommandEvent &event)
{
	if (lastFile != wxEmptyString)
	{
		design->writeXmlModel(lastFile);
		setModelChanged(false);
		setExtendedTitle();
	}
	else
	{
		OnModelSaveAs(event);
	}
}

void frmDatabaseDesigner::OnModelLoad(wxCommandEvent &event)
{
	if(changed)
	{
		// Ask what to do with old model
		int answer = wxMessageBox(_("Save model changes?"), _("Confirm"), wxYES_NO | wxCANCEL);
		if (answer == wxYES)
		{
			if (lastFile != wxEmptyString)
				OnModelSave(event);
			else
				OnModelSaveAs(event);
		}

		if(answer == wxCANCEL)
			return;
	}

	//Open Model
	wxFileDialog openFileDialog(this, _("Open model"), _(""), _(""), _("*.pgd"),
	                            wxOPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);

	if (openFileDialog.ShowModal() == wxID_OK)
	{
		wxString path;
		path.append(openFileDialog.GetDirectory());
		path.append(wxFileName::GetPathSeparator());
		path.append(openFileDialog.GetFilename());
		if(!path.Lower().Matches(_("*.pgd")))
			path.append(_(".pgd"));
		lastFile = path;

		// Clean treeview, notebook, and sql pane
		design->emptyModel();
		while(diagrams->GetPageCount() > 0)
		{
			diagrams->RemovePage(0);
			design->deleteDiagram(0);
		}
		sqltext->SetText(wxEmptyString);

		// Read model from xml file
		design->readXmlModel(path, diagrams);
		setModelChanged(false);
		setExtendedTitle();
	}
}

void frmDatabaseDesigner::OnAddDiagram(wxCommandEvent &event)
{
	wxString newName = wxGetTextFromUser(_("New Diagram Name"), _("Diagram Name"), _("unnamed"), this);

	if (!newName.IsEmpty())
	{
		diagrams->AddPage(design->createDiagram(diagrams, newName, false)->getView(), newName);
		setModelChanged(true);
	}

	UpdateToolbar();
}

void frmDatabaseDesigner::OnAddDiagram2(wxAuiNotebookEvent &event)
{
	wxCommandEvent evt;
	OnAddDiagram(evt);
}

void frmDatabaseDesigner::OnDeleteDiagram(wxCommandEvent &event)
{
	wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());
	int diagramIndex = view->getIdx();
	diagrams->RemovePage(diagrams->GetSelection());
	design->deleteDiagram(diagramIndex);
	setModelChanged(true);

	UpdateToolbar();
}

void frmDatabaseDesigner::OnRenameDiagram(wxCommandEvent &event)
{
	wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());
	int diagramIndex = view->getIdx();
	wxString name = wxGetTextFromUser(_("Rename diagram ") + diagrams->GetPageText(diagramIndex) + _(" to:"), _("Rename diagram..."), diagrams->GetPageText(diagramIndex), this);
	if(!name.IsEmpty() && !name.IsSameAs(diagrams->GetPageText(diagramIndex), false))
	{
		view->getDrawing()->setName(name);
		diagrams->SetPageText(diagramIndex, name);
		setModelChanged(true);
	}
}

void frmDatabaseDesigner::OnDeleteDiagramTab(wxAuiNotebookEvent &event)
{
	deletedTab = event.GetSelection();
	wxAuiNotebook *ctrl = (wxAuiNotebook *)event.GetEventObject();
	wxhdDrawingView *view = (wxhdDrawingView *) ctrl->GetPage(event.GetSelection());

	int res = wxMessageBox(wxT("Are you sure you want to delete diagram \"") + view->getDrawing()->getName() + _("\" from model?"),
	                       wxT("Delete diagram?"),
	                       wxYES_NO | wxNO_DEFAULT,
	                       this);
	if (res != wxYES)
	{
		event.Veto();
	}
	else
	{
		setModelChanged(true);
	}

	UpdateToolbar();
}

void frmDatabaseDesigner::OnDeletedDiagramTab(wxAuiNotebookEvent &event)
{
	//don't delete view when deleting diagram because it was deleted before by EVT_AUINOTEBOOK_PAGE_CLOSE event
	//option possible will be wxAuiPaneInfo().DestroyOnClose(false) but should be tried in a future
	design->deleteDiagram(deletedTab, false);
	UpdateToolbar();
}

void frmDatabaseDesigner::OnClickDiagramTab(wxAuiNotebookEvent &event)
{
//This event is not working by unknown reason right now.
	wxLogError(wxT("WTF?"));
}

void frmDatabaseDesigner::OnChangeDefaultFont(wxCommandEvent &event)
{
	design->getEditor()->changeDefaultFiguresFont();
}

void frmDatabaseDesigner::UpdateToolbar()
{
	toolBar->EnableTool(MNU_ADDTABLE, diagrams->GetPageCount() > 0);
	toolBar->EnableTool(MNU_DELETETABLE, diagrams->GetPageCount() > 0);

	if (diagrams->GetPageCount() > 0)
	{
		wxhdDrawingView *view = (wxhdDrawingView *) diagrams->GetPage(diagrams->GetSelection());
		ddTableFigure *table = design->getSelectedTable(view->getIdx());
		toolBar->EnableTool(MNU_ADDCOLUMN, table);
	}

	toolBar->EnableTool(MNU_GENERATEMODEL, diagrams->GetPageCount() > 0);
	toolBar->EnableTool(MNU_GENERATEDIAGRAM, diagrams->GetPageCount() > 0);
}

///////////////////////////////////////////////////////


bool databaseDesignerBaseFactory::CheckEnable(pgObject *obj)
{
	return true ;
}


wxWindow *databaseDesignerBaseFactory::StartDialogDesigner(frmMain *form, pgObject *obj, const wxString &sql)
{
	frmDatabaseDesigner *fd = new frmDatabaseDesigner(form, wxEmptyString, NULL);
	fd->Go();
	return fd;
}


databaseDesignerFactory::databaseDesignerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : databaseDesignerBaseFactory(list)
{
	mnu->Append(id, _("&Database Designer\tCtrl-F"), _("Create database designs"));
	toolbar->AddTool(id, _("Database Designer\tCtrl-F"), wxBitmap(*ddmodel_32_png_img), _("Create database designs."), wxITEM_NORMAL);
}


wxWindow *databaseDesignerFactory::StartDialog(frmMain *form, pgObject *obj)
{
	wxString qry;
	if (settings->GetStickySql())
		qry = obj->GetSql(form->GetBrowser());
	return StartDialogDesigner(form, obj, qry);
}
