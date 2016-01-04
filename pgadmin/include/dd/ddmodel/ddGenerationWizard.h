//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddBrowserDataContainer.h - Item to contain data for each treview child.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDGENERATIONWIZARD_H
#define DDGENERATIONWIZARD_H

#include "hotdraw/figures/hdIFigure.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/ddmodel/ddDBReverseEngineering.h"
#include <wx/grid.h>

class SelGenTablesPage;
class SelGenSchemaPage;
class ReportGridPage;

enum
{
	DDTABSGRID = 7100
};

class ddGenerationWizard : public wxWizard
{
public:
	ddGenerationWizard(wxFrame *frame, ddDatabaseDesign *design, pgConn *connection, bool useSizer = true);
	~ddGenerationWizard();

	// Wizard related functions
	wxWizardPage *GetFirstPage() const
	{
		return initialPage;
	}
	ddDatabaseDesign *getDesign()
	{
		return figuresDesign;
	};
	pgConn *getConnection()
	{
		return conn;
	};
	// Transfer data between pages
	OID OIDSelectedSchema;
	wxString schemaName;
	SelGenTablesPage *page2;
	SelGenSchemaPage *page3;
	ReportGridPage *page4;
	wxString DDL;
	wxArrayString preSelTables;

private:

	void OnWizardPageChanging(wxWizardEvent &event);

	// Page 4 - Report and Finish
	void OnFinishPressed(wxWizardEvent &event);

	pgConn *conn;
	wxWizardPageSimple *initialPage;
	wxStaticText *frontText;
	ddDatabaseDesign *figuresDesign;
	DECLARE_EVENT_TABLE()
};

class SelGenTablesPage : public wxWizardPage
{
public:
	SelGenTablesPage(wxWizard *parent, wxWizardPage *prev);
	~SelGenTablesPage();
	virtual wxWizardPage *GetPrev() const
	{
		return m_prev;
	};
	virtual wxWizardPage *GetNext() const
	{
		return m_next;
	};
	void SetPrev(wxWizardPage *prev)
	{
		m_prev = prev;
	}
	void SetNext(wxWizardPage *next)
	{
		m_next = next;
	}
	void OnButtonAdd(wxCommandEvent &);
	void OnButtonAddAll(wxCommandEvent &);
	void OnButtonRemove(wxCommandEvent &);
	void OnButtonRemoveAll(wxCommandEvent &);
	void RefreshTablesList();
	int countSelTables()
	{
		return m_selTables->GetCount();
	};
	wxString getSelTableName(int index)
	{
		return m_selTables->GetString(index);
	};
private:
	void moveToSelectList(wxString tableName);
	void OnWizardPageChanging(wxWizardEvent &event);
	wxStaticText *leftText, *rightText, *centerText;
	wxWizardPage *m_prev, *m_next;
	wxListBox *m_allTables, *m_selTables;
	ddGenerationWizard *wparent;
	wxArrayString tablesNames;
	wxBitmapButton *buttonAdd, *buttonAddAll, *buttonRemove, *buttonRemoveAll;
	wxBitmap addBitmap, addAllBitmap, removeBitmap, removeAllBitmap;
	DECLARE_EVENT_TABLE()
};

class SelGenSchemaPage : public wxWizardPage
{
public:
	SelGenSchemaPage(wxWizard *parent, wxWizardPage *prev);
	~SelGenSchemaPage();
	virtual wxWizardPage *GetPrev() const
	{
		return m_prev;
	};
	virtual wxWizardPage *GetNext() const
	{
		return m_next;
	};
	void SetPrev(wxWizardPage *prev)
	{
		m_prev = prev;
	}
	void SetNext(wxWizardPage *next)
	{
		m_next = next;
	}
private:
	void OnWizardPageChanging(wxWizardEvent &event);
	void refreshSchemas(pgConn *connection);
	wxStaticText *message;
	wxWizardPage *m_prev, *m_next;
	wxListBox *m_allSchemas;
	wxArrayString schemasNames;
	oidsHashMap schemasHM;
	ddGenerationWizard *wparent;
	DECLARE_EVENT_TABLE()
};

// Special version of wxGrid to allow use of fast comboboxes and grid columns auto fit
// some snippets from http://forums.wxwidgets.org/viewtopic.php?t=27568 under same wxwidgets license
// others from pgAdming gqb
class wxDDGrid: public wxGrid
{
public:
	wxDDGrid(wxWindow *parent, wxWindowID id);
	void ComboBoxEvent(wxGridEvent &event);
	void RevertSel();

	int sf[10];

	void OnSizeEvt( wxSizeEvent &ev );
	int StretchIt();
	int keepFit;

public:
	void SetColStretch ( unsigned i, int factor )
	{
		if( i < 10 ) sf[i] = factor;
	}
	int  GetColStretch ( unsigned i ) const
	{
		return (i < 10) ? sf[i] : 1;
	}
	void ReLayout()
	{
		StretchIt();
	}
	void SetFit( int fit_style )
	{
		keepFit = fit_style;
	}
private:
	wxGridSelection *m_selTemp;
};

class ReportGridPage : public wxWizardPage
{
public:
	ReportGridPage(wxWizard *parent, wxWizardPage *prev);
	~ReportGridPage();
	virtual wxWizardPage *GetPrev() const
	{
		return m_prev;
	};
	virtual wxWizardPage *GetNext() const
	{
		return m_next;
	};
	void SetPrev(wxWizardPage *prev)
	{
		m_prev = prev;
	}
	void SetNext(wxWizardPage *next)
	{
		m_next = next;
	}
	void populateGrid();
	wxDDGrid *getGrid()
	{
		return reportGrid;
	};
private:
	void OnWizardPageChanging(wxWizardEvent &event);
	void OnCellLeftClick(wxGridEvent &ev);

	wxStaticText *message;
	wxWizardPage *m_prev, *m_next;
	wxDDGrid *reportGrid;
	ddGenerationWizard *wparent;
	DECLARE_EVENT_TABLE()
};
#endif
