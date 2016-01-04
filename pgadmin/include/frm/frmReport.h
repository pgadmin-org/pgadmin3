//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmReport.h - The report file dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMREPORT_H
#define FRMREPORT_H

#include "dlg/dlgClasses.h"
#include "ctl/ctlListView.h"
#include "ctl/ctlSQLResult.h"

// Class declarations
class frmReport : public pgDialog
{
public:
	frmReport(wxWindow *p);
	~frmReport();

	void SetReportTitle(const wxString &t);

	void XmlAddHeaderValue(const wxString &name, const wxString &value);
	int XmlCreateSection(const wxString &name);
	void XmlSetSectionTableHeader(const int section, const int columns, const wxChar *name, ...);
	void XmlAddSectionTableRow(const int section, const int number, const int columns, const wxChar *value, ...);
	void XmlAddSectionTableFromListView(const int section, ctlListView *list);
	void XmlAddSectionTableFromGrid(const int section, ctlSQLResult *grid);
	void XmlSetSectionTableInfo(const int section, const wxString &info)
	{
		sectionTableInfo[section - 1] = info;
	};
	void XmlSetSectionSql(int section, const wxString &sql);
	void XmlAddSectionValue(const int section, const wxString &name, const wxString &value);

private:
	void OnChange(wxCommandEvent &ev);
	void OnHelp(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnBrowseFile(wxCommandEvent &ev);
	void OnBrowseStylesheet(wxCommandEvent &ev);

	wxString GetSectionTableColumns(const int section);
	wxString GetSectionTableRows(const int section);
	wxString GetSectionTable(const int section);
	wxString GetSection(const int section);
	wxString GetXmlReport(const wxString &stylesheet);
	wxString XslProcessReport(const wxString &xml, const wxString &xsl);

	wxString GetCssLink(const wxString &file);
	wxString GetEmbeddedCss(const wxString &css);
	const wxString GetDefaultCss();
	wxString GetDefaultXsl(const wxString &css);

	wxWindow *parent;
	wxString header;
	wxArrayString sectionName, sectionData, sectionTableHeader, sectionTableRows, sectionTableInfo, sectionSql;

	DECLARE_EVENT_TABLE()
};

///////////////////////////////////////////////////////
// Report Factory base class
///////////////////////////////////////////////////////
class reportBaseFactory : public actionFactory
{
protected:
	reportBaseFactory(menuFactoryList *list) : actionFactory(list) {}
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	frmMain *GetFrmMain()
	{
		return parent;
	};
	virtual void GenerateReport(frmReport *report, pgObject *object) {};

	frmMain *parent;
public:
	bool CheckEnable(pgObject *obj)
	{
		return false;
	};
};


///////////////////////////////////////////////////////
// Object properties report
///////////////////////////////////////////////////////
class reportObjectPropertiesFactory : public reportBaseFactory
{
public:
	reportObjectPropertiesFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	bool CheckEnable(pgObject *obj);
	void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object DDL report
///////////////////////////////////////////////////////
class reportObjectDdlFactory : public reportBaseFactory
{
public:
	reportObjectDdlFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	bool CheckEnable(pgObject *obj);
	void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object Data dictionary report
///////////////////////////////////////////////////////
class reportObjectDataDictionaryFactory : public reportBaseFactory
{
public:
	reportObjectDataDictionaryFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	bool CheckEnable(pgObject *obj);
	void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object statistics report
///////////////////////////////////////////////////////
class reportObjectStatisticsFactory : public reportBaseFactory
{
public:
	reportObjectStatisticsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	bool CheckEnable(pgObject *obj);
	void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object dependencies report
///////////////////////////////////////////////////////
class reportObjectDependenciesFactory : public reportBaseFactory
{
public:
	reportObjectDependenciesFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	bool CheckEnable(pgObject *obj);
	void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object Dependents report
///////////////////////////////////////////////////////
class reportObjectDependentsFactory : public reportBaseFactory
{
public:
	reportObjectDependentsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	bool CheckEnable(pgObject *obj);
	void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object list report
///////////////////////////////////////////////////////
class reportObjectListFactory : public reportBaseFactory
{
public:
	reportObjectListFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	bool CheckEnable(pgObject *obj);
	void GenerateReport(frmReport *report, pgObject *object);
};

#endif
