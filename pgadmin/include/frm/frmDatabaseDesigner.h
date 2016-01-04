//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmDatabaseDesigner.h - The database designer form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRM_DATABASEDESIGNER_H
#define __FRM_DATABASEDESIGNER_H

// Designer headers
#include "hotdraw/figures/hdAbstractFigure.h"
#include "hotdraw/figures/hdPolyLineFigure.h"
#include "hotdraw/figures/hdLineConnection.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "ctl/ctlSQLBox.h"
#include <wx/bmpcbox.h>
#include "dlg/dlgSelectConnection.h"

enum
{
	CTL_DDNOTEBOOK = 1001,
	CTL_DDCONNECTION,
	CTL_IMPSCHEMA
};

class frmDatabaseDesigner : public pgFrame
{
public:
	frmDatabaseDesigner(frmMain *form, const wxString &_title, pgConn *conn);
	~frmDatabaseDesigner();
	void Go();
	void setModelChanged(bool value);
private:
	int deletedTab;
	bool changed, previousChanged;
	wxBitmapComboBox *cbConnection;
	wxMenu *diagramMenu, *preferencesMenu, *viewMenu;
	wxString lastFile;
	frmMain *mainForm;
	pgConn *connection;

	// These status flags are required to work round some wierdness on wxGTK,
	// particularly on Solaris.
	bool closing, loading;

	ddDatabaseDesign *design;
	wxPanel *browserPanel, *connectionPanel;
	ddModelBrowser *modelBrowser;
	ctlAuiNotebook *diagrams;
	ctlSQLBox *sqltext;
	void setExtendedTitle();
	void OnClose(wxCloseEvent &event);
	void OnAddDiagram(wxCommandEvent &event);
	void OnAddDiagram2(wxAuiNotebookEvent &event);
	void OnDeleteDiagram(wxCommandEvent &event);
	void OnRenameDiagram(wxCommandEvent &event);
	void OnClickDiagramTab(wxAuiNotebookEvent &event);
	void OnDeleteDiagramTab(wxAuiNotebookEvent &event);
	void OnDeletedDiagramTab(wxAuiNotebookEvent &event);
	void OnAddTable(wxCommandEvent &event);
	void OnDeleteTable(wxCommandEvent &event);
	void OnAddColumn(wxCommandEvent &event);
	void OnNewModel(wxCommandEvent &event);
	void OnModelGeneration(wxCommandEvent &event);
	void OnModelSaveAs(wxCommandEvent &event);
	void OnDiagramGeneration(wxCommandEvent &event);
	void OnModelSave(wxCommandEvent &event);
	void OnModelLoad(wxCommandEvent &event);
	void OnToggleModelBrowser(wxCommandEvent &event);
	void OnToggleSQLWindow(wxCommandEvent &event);
	void OnChangeConnection(wxCommandEvent &event);
	void OnImportSchema(wxCommandEvent &WXUNUSED(event));
	wxBitmap CreateBitmap(const wxColour &colour);
	wxColour GetServerColour(pgConn *connection);
	void UpdateToolbar();
	wxAuiManager manager;
	DECLARE_EVENT_TABLE()
};

///////////////////////////////////////////////////////

class databaseDesignerBaseFactory : public actionFactory
{
protected:
	databaseDesignerBaseFactory(menuFactoryList *list) : actionFactory(list) {}
	wxWindow *StartDialogDesigner(frmMain *form, pgObject *obj, const wxString &sql);
public:
	bool CheckEnable(pgObject *obj);
};

class databaseDesignerFactory : public databaseDesignerBaseFactory
{
public:
	databaseDesignerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif // __FRM_DATABASEDESIGNER_H
