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
    void AddReportDetailCode(const wxString &c);
    void AddReportDetailHeader1(const wxString &h) { detail += wxT("<h1>\n") + HtmlEntities(h) + wxT("\n</h1>\n"); };
    void AddReportDetailHeader2(const wxString &h) { detail += wxT("<h2>\n") + HtmlEntities(h) + wxT("\n</h2>\n"); };
    void AddReportDetailHeader3(const wxString &h) { detail += wxT("<h3>\n") + HtmlEntities(h) + wxT("\n</h3>\n"); };
    void AddReportDetailHeader4(const wxString &h) { detail += wxT("<h4>\n") + HtmlEntities(h) + wxT("\n</h4>\n"); };
    void AddReportDetailHeader5(const wxString &h) { detail += wxT("<h5>\n") + HtmlEntities(h) + wxT("\n</h5>\n"); };
    void AddReportDetailHeader6(const wxString &h) { detail += wxT("<h6>\n") + HtmlEntities(h) + wxT("\n</h6>\n"); };


private:
    void OnChange(wxCommandEvent &ev);
    void OnHelp(wxCommandEvent& ev);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnBrowseOPFile(wxCommandEvent &ev);
    void OnBrowseCSSFile(wxCommandEvent &ev);

    wxWindow *parent;
    wxString title, header, detail;
    int row;

    DECLARE_EVENT_TABLE()
};

#endif
