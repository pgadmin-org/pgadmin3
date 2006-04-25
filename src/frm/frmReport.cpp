//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmReport.cpp - The report file dialogue
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"
#include <wx/file.h>
#include "frmReport.h"
#include "sysSettings.h"
#include "misc.h"

#define txtTitle        CTRL_TEXT("txtTitle")
#define txtNotes        CTRL_TEXT("txtNotes")
#define txtOPFilename   CTRL_TEXT("txtOPFilename")
#define txtCSSFilename  CTRL_TEXT("txtCSSFilename")
#define btnOK           CTRL_BUTTON("wxID_OK")
#define btnOPFilename   CTRL_BUTTON("btnOPFilename")
#define btnCSSFilename  CTRL_BUTTON("btnCSSFilename")
#define rbBuiltin       CTRL_RADIOBUTTON("rbBuiltin")
#define rbEmbed         CTRL_RADIOBUTTON("rbEmbed")
#define rbLink          CTRL_RADIOBUTTON("rbLink")

BEGIN_EVENT_TABLE(frmReport, pgDialog)
    EVT_RADIOBUTTON(XRCID("rbBuiltin"),     frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbEmbed"),       frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbLink"),        frmReport::OnChange)
    EVT_TEXT(XRCID("txtOPFilename"),        frmReport::OnChange)
    EVT_TEXT(XRCID("txtCSSFilename"),       frmReport::OnChange)
    EVT_BUTTON(XRCID("btnOPFilename"),      frmReport::OnBrowseOPFile)
    EVT_BUTTON(XRCID("btnCSSFilename"),     frmReport::OnBrowseCSSFile)
    EVT_BUTTON(wxID_HELP,                   frmReport::OnHelp)
    EVT_BUTTON(wxID_OK,                     frmReport::OnOK)
    EVT_BUTTON(wxID_CANCEL,                 frmReport::OnCancel)
END_EVENT_TABLE()


frmReport::frmReport(wxWindow *p)
{
    parent = p;
    title = wxT("");
    header = wxT("");
    detail = wxT("");

    wxLogInfo(wxT("Creating the report dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(p, wxT("frmReport"));

    // Icon
    appearanceFactory->SetIcons(this);
    CenterOnParent();
    btnOK->Disable();

    wxString val;

    // Stylesheet
    settings->Read(wxT("Reports/StylesheetMode"), &val, wxT("b"));
    if (val == wxT("e"))
    {
        rbBuiltin->SetValue(false);
        rbEmbed->SetValue(true);
        rbLink->SetValue(false);
        txtCSSFilename->Enable();
        btnCSSFilename->Enable();
    }
    else if (val == wxT("l"))
    {
        rbBuiltin->SetValue(false);
        rbEmbed->SetValue(false);
        rbLink->SetValue(true);
        txtCSSFilename->Enable();
        btnCSSFilename->Enable();
    }
    else
    {
        rbBuiltin->SetValue(true);
        rbEmbed->SetValue(false);
        rbLink->SetValue(false);
        txtCSSFilename->Disable();
        btnCSSFilename->Disable();
    }

    // Default values
    settings->Read(wxT("Reports/LastNotes"), &val, wxT(""));
    txtNotes->SetValue(val);

    settings->Read(wxT("Reports/LastFile"), &val, wxEmptyString);
    txtOPFilename->SetValue(val);

    settings->Read(wxT("Reports/LastStylesheet"), &val, wxT("reports/pgadmin.css"));
    txtCSSFilename->SetValue(val);

    wxCommandEvent ev;
    OnChange(ev);
}


frmReport::~frmReport()
{
    wxLogInfo(wxT("Destroying the report dialogue"));
}


void frmReport::OnHelp(wxCommandEvent &ev)
{
    DisplayHelp(this, wxT("reports"));
}


void frmReport::OnChange(wxCommandEvent &ev)
{
    bool enable = true;

    if (txtOPFilename->GetValue().IsEmpty())
        enable = false;

    if (rbBuiltin->GetValue())
    {
        txtCSSFilename->Disable();
        btnCSSFilename->Disable();
    }
    else
    {
        txtCSSFilename->Enable();
        btnCSSFilename->Enable();
    }

    if (rbEmbed->GetValue())
    {
        if (!wxFile::Exists(txtCSSFilename->GetValue()))
          enable = false;
    }

    btnOK->Enable(enable);
}


void frmReport::OnOK(wxCommandEvent &ev)
{
    // Check if the file exsits, and if so, whether to overwrite it
    if (wxFileExists(txtOPFilename->GetValue()))
    {
        wxString msg;
        msg.Printf(_("The file: \n\n%s\n\nalready exists. Do you want to overwrite it?"), txtOPFilename->GetValue().c_str());

        if (wxMessageBox(msg, _("Overwrite file?"), wxYES_NO | wxICON_QUESTION) == wxNO)
        {
            txtOPFilename->SetFocus();
            return;
        }
    }

    // Generate the HTML
    wxString report;

    report = wxT("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    
    report += wxT("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    report += wxT("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
    report += wxT("<head>\n");

	report += wxT("<title>") + title + wxT("</title>\n");
    report += wxT("<meta http-equiv=\"Content-Type\" content=\"utf-8\" />\n");

    // add the stylesheet
    if (rbBuiltin->GetValue())
    {
        report += wxT("<style type=\"text/css\">\n");
        report += wxT("body {  font-family: verdana, helvetica, sans-serif; margin: 0px; padding: 0; }\n");
        report += wxT("h1 { font-weight: bold; font-size: 150%; border-bottom-style: solid; border-bottom-width: 2px; margin-top: 0px; padding-bottom: 0.5ex; color: #eeeeee; }\n");
        report += wxT("h2 { font-weight: bold; font-size: 120%; padding-bottom: 0.5ex; color: #eeeeee; }\n");
        report += wxT("h3 { font-weight: bold; font-size: 115%; padding-bottom: 0.5ex; color: #eeeeee; }\n");
        report += wxT("h4 { font-size: 130%; padding-bottom: 0.5ex; color: #009ace; }\n");
        report += wxT("h5 { font-size: 115%; padding-bottom: 0.5ex; color: #009ace; }\n");
        report += wxT("h6 { font-size: 105%; padding-bottom: 0.5ex; color: #009ace; }\n");
        report += wxT("code { display: block; background-color: #eeeeee; font-family: monospace; font-size: 120% }\n");
        report += wxT("th { text-align: left; background-color: #009ace; color: #eeeeee; }\n");
        report += wxT("#ReportHeader { padding: 10px; background-color: #009ace; color: #eeeeee; border-bottom-style: solid; border-bottom-width: 2px; border-color: #999999; }\n");
        report += wxT("#ReportHeader th { width: 25%; white-space: nowrap; vertical-align: top; }\n");
        report += wxT("#ReportHeader td { vertical-align: top; color: #eeeeee; }\n");
        report += wxT("#ReportNotes { padding: 10px; background-color: #eeeeee; font-size: 80%; border-bottom-style: solid; border-bottom-width: 2px; border-color: #999999; }\n");
        report += wxT("#ReportDetails { padding: 5px; margin-left: 10px; margin-right: 10px; font-size: 80%; }\n");
        report += wxT(".ReportDetailsOddDataRow { background-color: #dddddd; }\n");
        report += wxT(".ReportDetailsEvenDataRow { background-color: #eeeeee; }\n");
        report += wxT(".ReportPropertyTableHeaderCell { background-color: #dddddd; color: #009ace; width: 25%; vertical-align: top }\n");
        report += wxT(".ReportPropertyTableValueCell { background-color: #eeeeee; vertical-align: top }\n");
        report += wxT("#ReportFooter { font-weight: bold; font-size: 80%; text-align: right; background-color: #009ace; color: #eeeeee; margin-top: 10px; padding: 2px; border-bottom-style: solid; border-bottom-width: 2px; border-top-style: solid; border-top-width: 2px; border-color: #999999; }\n");
        report += wxT("#ReportFooter a { color: #ffffff; text-decoration: none; }\n");
        report += wxT("</style>\n");
    }
    if (rbEmbed->GetValue())
    {
        report += wxT("<style type=\"text/css\">\n");
        report += FileRead(txtCSSFilename->GetValue());
        report += wxT("</style>\n");
    }
    else
    {
        if (!txtCSSFilename->GetValue().IsEmpty())
            report += wxT("<link rel=\"stylesheet\" href=\"") + txtCSSFilename->GetValue() + wxT("\" type=\"text/css\" />\n");
    }
    
    report += wxT("</head>\n");
    report += wxT("<body>\n");
    report += wxT("<div id=\"ReportHeader\">\n");
    report += wxT("<h1>") + title + wxT("</h1>\n");
    report += wxT("<table>\n");
    report += header;
    report += wxT("</table>\n");
    report += wxT("</div>\n\n");

    if (!txtNotes->GetValue().IsEmpty())
    {
        wxString notes = HtmlEntities(txtNotes->GetValue());
        notes.Replace(wxT("\r\n"), wxT("<br />"));
        notes.Replace(wxT("\n"), wxT("<br />"));
        notes.Replace(wxT("\r"), wxT("<br />"));

        report += wxT("<div id=\"ReportNotes\">\n");
        report += wxT("<b>Notes: </b><br /><br />\n");
        report += notes;
        report += wxT("\n</div>\n");
    }

    report += wxT("<div id=\"ReportDetails\">\n");
    report += detail;
    report += wxT("\n</div>\n");

    report += wxT("<div id=\"ReportFooter\">\n");
    wxString credit;
    credit.Printf(_("Report generated by %s"), wxT("<a href=\"http://www.pgadmin.org/\">pgAdmin</a>"));
    report += credit;
    report += wxT("\n</div>\n");

    report += wxT("</body>\n");
    report += wxT("</html>\n");

    // Save it to disk
    wxFile file(txtOPFilename->GetValue(), wxFile::write);
    if (!file.IsOpened())
    {
        wxLogError(_("Failed to open file %s."), txtOPFilename->GetValue().c_str());
        return;
    }
    file.Write(report, wxConvUTF8);
    file.Close();

    // Save the settings for next time round
    settings->Write(wxT("Reports/LastNotes"), txtNotes->GetValue());

    if (rbBuiltin->GetValue())
        settings->Write(wxT("Reports/StylesheetMode"), wxT("b"));
    else if (rbEmbed->GetValue())
        settings->Write(wxT("Reports/StylesheetMode"), wxT("e"));
    else
        settings->Write(wxT("Reports/StylesheetMode"), wxT("l"));

    settings->Write(wxT("Reports/LastStylesheet"), txtCSSFilename->GetValue());
    settings->Write(wxT("Reports/LastFile"), txtOPFilename->GetValue());

    // Now go away
    if (IsModal())
        EndModal(wxID_OK);
    else
        Destroy();
}

void frmReport::OnCancel(wxCommandEvent &ev)
{
    if (IsModal())
        EndModal(wxID_CANCEL);
    else
        Destroy();
}

void frmReport::OnBrowseCSSFile(wxCommandEvent &ev)
{
    wxString def = txtCSSFilename->GetValue();

    if (!wxFile::Exists(def))
        def.Empty();
   
    wxFileDialog cssfile(this, _("Select stylesheet filename"), wxGetHomeDir(), def, _("CSS files (*.css)|*.css"), wxOPEN);

    if (cssfile.ShowModal() == wxID_OK)
    {
        txtCSSFilename->SetValue(cssfile.GetPath());
        OnChange(ev);
    }
}

void frmReport::OnBrowseOPFile(wxCommandEvent &ev)
{
    wxFileDialog opfile(this, _("Select output filename"), wxGetHomeDir(), txtOPFilename->GetValue(),
        _("HTML files (*.html)|*.html"), wxSAVE && wxOVERWRITE_PROMPT);

    if (opfile.ShowModal() == wxID_OK)
    {
        txtOPFilename->SetValue(opfile.GetPath());
        OnChange(ev);
    }
}

void frmReport::SetReportTitle(const wxString &t) 
{ 
    title = HtmlEntities(t); 
    txtTitle->SetValue(t); 
}


void frmReport::AddReportHeaderValue(const wxString &name, const wxString &value)
{
    header += wxT("<tr><th>") + HtmlEntities(name) + wxT(": </th><td>") + HtmlEntities(value) + wxT("</td></tr>\n");
}

void frmReport::AddReportDataTableHeaderRow(const int cols, const wxChar *name,...)
{
    va_list ap;
    const wxChar *p = name;

    row = 0;

    detail += wxT("<tr>");
    va_start(ap, name);

    for (int x = 0; x < cols; x++)
    {
        detail += wxT("<th>");
        detail += HtmlEntities(p);
        detail += wxT("</th>");
        p = va_arg(ap, wxChar*);
    }

    va_end(ap);
    detail += wxT("</tr>\n");
}

void frmReport::AddReportDataTableDataRow(const int cols, const wxChar *name,...)
{
    va_list ap;
    const wxChar *p = name;

    row++;

    detail += wxT("<tr>");

    va_start(ap, name);

    for (int x = 0; x < cols; x++)
    {
        if (row % 2 == 1)
            detail += wxT("<td class=\"ReportDetailsOddDataRow\">");
        else
            detail += wxT("<td class=\"ReportDetailsEvenDataRow\">");
        detail += HtmlEntities(p);
        detail += wxT("</td>");
        p = va_arg(ap, wxChar*);
    }

    va_end(ap);
    detail += wxT("</tr>\n");
}

void frmReport::AddReportPropertyTableRow(const wxString &name, const wxString &value)
{
    detail += wxT("<tr><th class=\"ReportPropertyTableHeaderCell\">");
    detail += HtmlEntities(name);
    detail += wxT("</th><td class=\"ReportPropertyTableValueCell\">");
    detail += HtmlEntities(value) + wxT("</td></tr>\n");
}

void frmReport::AddReportDetailCode(const wxString &c)
{ 
    wxString sql = HtmlEntities(c);
    sql.Replace(wxT(" "), wxT("&nbsp;"));
    sql.Replace(wxT("\n"), wxT("<br />"));
    detail += wxT("<code>\n");
    detail += sql;
    detail += wxT("\n</code>\n"); 
}