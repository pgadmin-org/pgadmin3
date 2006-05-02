//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmReport.h - The report file dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMREPORT_H
#define FRMREPORT_H

#include "dlgClasses.h"
#include "ctl/ctlListView.h"

// Class declarations
class frmReport : public pgDialog
{
public:
    frmReport(wxWindow *p);
    ~frmReport();

    void SetReportTitle(const wxString &t);
    void AddReportHeaderValue(const wxString &name, const wxString &value);
    void AddReportDetailParagraph(const wxString &p) { detail += wxT("<p>\n") + HtmlEntities(p) + wxT("\n</p>\n"); };
    void StartReportTable() { detail += wxT("<table>\n"); };
    void EndReportTable() { detail += wxT("</table>\n"); };
    void AddReportDataTableHeaderRow(const int cols, const wxChar *name,...);
    void AddReportDataTableDataRow(const int cols, const wxChar *value,...);
    void AddReportPropertyTableRow(const wxString &name, const wxString &value);
    void AddReportDataRawHtml(const wxString &row) { detail += row + wxT("\n"); };
    void AddReportSql(const wxString &s);
    void AddReportDetailHeader(const wxString &h) { detail += wxT("<h2>\n") + HtmlEntities(h) + wxT("\n</h2>\n"); };
    void AddReportTableFromListView(ctlListView *list);

private:
    void OnChange(wxCommandEvent &ev);
    void OnHelp(wxCommandEvent& ev);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnBrowseOPFile(wxCommandEvent &ev);
    void OnBrowseCSSFile(wxCommandEvent &ev);

    wxWindow *parent;
    wxString header, detail, sql;
    int row;

    DECLARE_EVENT_TABLE()
};

#endif

///////////////////////////////////////////////////////
// Report Factory base class
///////////////////////////////////////////////////////
class reportBaseFactory : public contextActionFactory
{
protected:
	reportBaseFactory(menuFactoryList *list) : contextActionFactory(list) {}
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    frmMain *GetFrmMain() { return parent; };
    virtual void GenerateReport(frmReport *report, pgObject *object) {};

    frmMain *parent;
public:
    bool CheckEnable(pgObject *obj) { return false; };
};


///////////////////////////////////////////////////////
// Object properties report
///////////////////////////////////////////////////////
class reportObjectPropertiesFactory : public reportBaseFactory
{
public:
    reportObjectPropertiesFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    bool CheckEnable(pgObject *obj);
    void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object statistics report
///////////////////////////////////////////////////////
class reportObjectStatisticsFactory : public reportBaseFactory
{
public:
    reportObjectStatisticsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    bool CheckEnable(pgObject *obj);
    void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object dependencies report
///////////////////////////////////////////////////////
class reportObjectDependenciesFactory : public reportBaseFactory
{
public:
    reportObjectDependenciesFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    bool CheckEnable(pgObject *obj);
    void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object dependees report
///////////////////////////////////////////////////////
class reportObjectDependeesFactory : public reportBaseFactory
{
public:
    reportObjectDependeesFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    bool CheckEnable(pgObject *obj);
    void GenerateReport(frmReport *report, pgObject *object);
};

///////////////////////////////////////////////////////
// Object list report
///////////////////////////////////////////////////////
class reportObjectListFactory : public reportBaseFactory
{
public:
    reportObjectListFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    bool CheckEnable(pgObject *obj);
    void GenerateReport(frmReport *report, pgObject *object);
};