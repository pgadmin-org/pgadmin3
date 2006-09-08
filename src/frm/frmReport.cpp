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

#include "frmMain.h"
#include "frmReport.h"
#include "sysSettings.h"
#include "misc.h"
#include "ctl/ctlListView.h"
#include "pgObject.h"
#include "pgCollection.h"
#include "pgServer.h"
#include "pgDatabase.h"
#include "pgSchema.h"
#include "pgTable.h"
#include "pgColumn.h"
#include "pgConstraints.h"
#include "pgaJob.h"
#include "pgForeignKey.h"
#include "pgIndexConstraint.h"
#include "pgCheck.h"

// XML2/XSLT headers
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#define txtTitle          CTRL_TEXT("txtTitle")
#define txtNotes          CTRL_TEXT("txtNotes")
#define txtHtmlFile       CTRL_TEXT("txtHtmlFile")
#define txtXmlFile        CTRL_TEXT("txtXmlFile")
#define txtHtmlStylesheet CTRL_TEXT("txtHtmlStylesheet")
#define txtXmlStylesheet  CTRL_TEXT("txtXmlStylesheet")
#define btnOK             CTRL_BUTTON("wxID_OK")
#define btnFile           CTRL_BUTTON("btnFile")
#define btnStylesheet     CTRL_BUTTON("btnStylesheet")
#define rbHtml            CTRL_RADIOBUTTON("rbHtml")
#define rbXml             CTRL_RADIOBUTTON("rbXml")
#define rbHtmlBuiltin     CTRL_RADIOBUTTON("rbHtmlBuiltin")
#define rbHtmlEmbed       CTRL_RADIOBUTTON("rbHtmlEmbed")
#define rbHtmlLink        CTRL_RADIOBUTTON("rbHtmlLink")
#define rbXmlPlain        CTRL_RADIOBUTTON("rbXmlPlain")
#define rbXmlLink         CTRL_RADIOBUTTON("rbXmlLink")
#define rbXmlProcess      CTRL_RADIOBUTTON("rbXmlProcess")
#define chkSql            CTRL_CHECKBOX("chkSql")

BEGIN_EVENT_TABLE(frmReport, pgDialog)
    EVT_RADIOBUTTON(XRCID("rbHtml"),        frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbXml"),         frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbHtmlBuiltin"), frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbHtmlEmbed"),   frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbHtmlLink"),    frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbXmlPlain"),    frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbXmlLink"),     frmReport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbXmlProcess"),  frmReport::OnChange)
    EVT_TEXT(XRCID("txtHtmlFile"),          frmReport::OnChange)
    EVT_TEXT(XRCID("txtXmlFile"),           frmReport::OnChange)
    EVT_TEXT(XRCID("txtHtmlStylesheet"),    frmReport::OnChange)
    EVT_TEXT(XRCID("txtXmlStylesheet"),     frmReport::OnChange)
    EVT_BUTTON(XRCID("btnFile"),            frmReport::OnBrowseFile)
    EVT_BUTTON(XRCID("btnStylesheet"),      frmReport::OnBrowseStylesheet)
    EVT_BUTTON(wxID_HELP,                   frmReport::OnHelp)
    EVT_BUTTON(wxID_OK,                     frmReport::OnOK)
    EVT_BUTTON(wxID_CANCEL,                 frmReport::OnCancel)
END_EVENT_TABLE()

frmReport::frmReport(wxWindow *p)
{
    parent = p;
    header = wxT("");

    wxLogInfo(wxT("Creating the report dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(p, wxT("frmReport"));

    // Icon
    appearanceFactory->SetIcons(this);
    CenterOnParent();
    btnOK->Disable();

    wxString val;
    bool bVal;

    // Output format
    settings->Read(wxT("Reports/ReportFormat"), &val, wxT("h"));
    if (val == wxT("x"))
    {
        rbHtml->SetValue(false);
        rbXml->SetValue(true);
    }
    else
    {
        rbHtml->SetValue(true);
        rbXml->SetValue(false);
    }

    // HTML Stylesheet
    settings->Read(wxT("Reports/HtmlStylesheetMode"), &val, wxT("b"));
    if (val == wxT("e"))
    {
        rbHtmlBuiltin->SetValue(false);
        rbHtmlEmbed->SetValue(true);
        rbHtmlLink->SetValue(false);
    }
    else if (val == wxT("l"))
    {
        rbHtmlBuiltin->SetValue(false);
        rbHtmlEmbed->SetValue(false);
        rbHtmlLink->SetValue(true);
    }
    else
    {
        rbHtmlBuiltin->SetValue(true);
        rbHtmlEmbed->SetValue(false);
        rbHtmlLink->SetValue(false);
    }

    // XML Stylesheet
    settings->Read(wxT("Reports/XmlStylesheetMode"), &val, wxT("p"));
    if (val == wxT("l"))
    {
        rbXmlPlain->SetValue(false);
        rbXmlLink->SetValue(true);
        rbXmlProcess->SetValue(false);
    }
    else if (val == wxT("r"))
    {
        rbXmlPlain->SetValue(false);
        rbXmlLink->SetValue(false);
        rbXmlProcess->SetValue(true);
    }
    else
    {
        rbXmlPlain->SetValue(true);
        rbXmlLink->SetValue(false);
        rbXmlProcess->SetValue(false);
    }

    // Default values
    settings->Read(wxT("Reports/LastNotes"), &val, wxT(""));
    txtNotes->SetValue(val);

    settings->Read(wxT("Reports/LastHtmlStylesheet"), &val, wxEmptyString);
    txtHtmlStylesheet->SetValue(val);

    settings->Read(wxT("Reports/LastXmlStylesheet"), &val, wxEmptyString);
    txtXmlStylesheet->SetValue(val);

    settings->Read(wxT("Reports/LastHtmlFile"), &val, wxEmptyString);
    txtHtmlFile->SetValue(val);

    settings->Read(wxT("Reports/LastXmlFile"), &val, wxEmptyString);
    txtXmlFile->SetValue(val);

    settings->Read(wxT("Reports/IncludeSQL"), &bVal, true);
    chkSql->SetValue(bVal);

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

    if (rbHtml->GetValue())
    {
        // Show/hide the appropriate controls
        rbHtmlBuiltin->Show(true);
        rbHtmlEmbed->Show(true);
        rbHtmlLink->Show(true);
        txtHtmlStylesheet->Show(true);
        txtHtmlFile->Show(true);

        rbXmlPlain->Show(false);
        rbXmlLink->Show(false);
        rbXmlProcess->Show(false);
        txtXmlStylesheet->Show(false);
        txtXmlFile->Show(false);

        // Enable/disable as appropriate
        if (txtHtmlFile->GetValue().IsEmpty())
            enable = false;

        if (rbHtmlBuiltin->GetValue())
        {
            txtHtmlStylesheet->Disable();
            btnStylesheet->Disable();
        }
        else
        {
            txtHtmlStylesheet->Enable();
            btnStylesheet->Enable();
        }

        if (rbHtmlEmbed->GetValue())
        {
            if (!wxFile::Exists(txtHtmlStylesheet->GetValue()))
              enable = false;
        }
    }
    else
    {
        // Show/hide the appropriate controls
        rbHtmlBuiltin->Show(false);
        rbHtmlEmbed->Show(false);
        rbHtmlLink->Show(false);
        txtHtmlStylesheet->Show(false);
        txtHtmlFile->Show(false);

        rbXmlPlain->Show(true);
        rbXmlLink->Show(true);
        rbXmlProcess->Show(true);
        txtXmlStylesheet->Show(true);
        txtXmlFile->Show(true);

        // Enable/disable as appropriate
        if (txtXmlFile->GetValue().IsEmpty())
            enable = false;

        if (rbXmlPlain->GetValue())
        {
            txtXmlStylesheet->Disable();
            btnStylesheet->Disable();
        }
        else
        {
            txtXmlStylesheet->Enable();
            btnStylesheet->Enable();
        }

        if (rbXmlProcess->GetValue())
        {
            if (!wxFile::Exists(txtXmlStylesheet->GetValue()))
              enable = false;
        }
    }

    btnOK->Enable(enable);
}


void frmReport::OnOK(wxCommandEvent &ev)
{
    wxString filename;
    if (rbHtml->GetValue())
        filename = txtHtmlFile->GetValue();
    else
        filename = txtXmlFile->GetValue();

    // Check if the file exsits, and if so, whether to overwrite it
    if (wxFileExists(filename))
    {
        wxString msg;
        msg.Printf(_("The file: \n\n%s\n\nalready exists. Do you want to overwrite it?"), filename.c_str());

        if (wxMessageBox(msg, _("Overwrite file?"), wxYES_NO | wxICON_QUESTION) == wxNO)
        {
            if (rbHtml->GetValue())
                txtHtmlFile->SetFocus();
            else
                txtXmlFile->SetFocus();
            return;
        }
    }

    // Yield, to allow the messagebox to go
    wxTheApp->Yield(true);

    // Add the title, notes and SQL
    if (txtTitle->GetValue() != wxT(""))
        XmlAddHeaderValue(wxT("title"), txtTitle->GetValue());

    if (txtNotes->GetValue() != wxT(""))
        XmlAddHeaderValue(wxT("notes"), txtNotes->GetValue());

    // Generate the report data
    wxString report;
    
    if (rbHtml->GetValue())
    {
        if (rbHtmlBuiltin->GetValue())
        {
            wxString xml = GetXmlReport(wxEmptyString);            
            wxString xsl = GetDefaultXsl(GetEmbeddedCss(GetDefaultCss()));
            report = XslProcessReport(xml, xsl);
        }
        else if (rbHtmlEmbed->GetValue())
        {
            wxString xml = GetXmlReport(wxEmptyString);
            wxString css = FileRead(txtHtmlStylesheet->GetValue());
            if (css.IsEmpty())
            {
                wxLogError(_("No stylesheet data could be read from the file %s: Errcode=%d."), txtHtmlStylesheet->GetValue().c_str(), wxSysErrorCode());
                return;
            }
            wxString xsl = GetDefaultXsl(GetEmbeddedCss(css));
            report = XslProcessReport(xml, xsl);
        }
        else
        {
            wxString xml = GetXmlReport(wxEmptyString);
            wxString xsl = GetDefaultXsl(GetCssLink(txtHtmlStylesheet->GetValue()));
            report = XslProcessReport(xml, xsl);
        }
    }
    else
    {
        if (rbXmlPlain->GetValue())
        {
            report = GetXmlReport(wxEmptyString);
        }
        else if (rbXmlLink->GetValue())
        {
            report = GetXmlReport(txtXmlStylesheet->GetValue());
        }
        else
        {
            wxString xml = GetXmlReport(wxEmptyString);
            wxString xsl = FileRead(txtXmlStylesheet->GetValue());
            if (xsl.IsEmpty())
            {
                wxLogError(_("No stylesheet data could be read from the file %s: Errcode=%d."), txtXmlStylesheet->GetValue().c_str(), wxSysErrorCode());
                return;
            }
            report = XslProcessReport(xml, xsl);
        }
    }

    // If report is empty, an error must have occured
    if (report.IsEmpty())
        return;

    // Save it to disk
    wxFile file(filename, wxFile::write);
    if (!file.IsOpened())
    {
        wxLogError(_("Failed to open file %s."), filename.c_str());
        return;
    }
    file.Write(report, wxConvUTF8);
    file.Close();


    // Save the settings for next time round
    settings->Write(wxT("Reports/LastNotes"), txtNotes->GetValue());

    if (rbHtml->GetValue())
        settings->Write(wxT("Reports/ReportFormat"), wxT("h"));
    else
        settings->Write(wxT("Reports/ReportFormat"), wxT("x"));

    if (rbHtmlBuiltin->GetValue())
        settings->Write(wxT("Reports/HtmlStylesheetMode"), wxT("b"));
    else if (rbHtmlEmbed->GetValue())
        settings->Write(wxT("Reports/HtmlStylesheetMode"), wxT("e"));
    else
        settings->Write(wxT("Reports/HtmlStylesheetMode"), wxT("l"));

    if (rbXmlPlain->GetValue())
        settings->Write(wxT("Reports/XmlStylesheetMode"), wxT("p"));
    else if (rbXmlLink->GetValue())
        settings->Write(wxT("Reports/XmlStylesheetMode"), wxT("l"));
    else
        settings->Write(wxT("Reports/XmlStylesheetMode"), wxT("r"));

    settings->Write(wxT("Reports/LastHtmlStylesheet"), txtHtmlStylesheet->GetValue());
    settings->Write(wxT("Reports/LastXmlStylesheet"), txtXmlStylesheet->GetValue());


    settings->Write(wxT("Reports/LastHtmlFile"), txtHtmlFile->GetValue());
    settings->Write(wxT("Reports/LastXmlFile"), txtXmlFile->GetValue());

    settings->Write(wxT("Reports/IncludeSQL"), chkSql->GetValue());

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

void frmReport::OnBrowseStylesheet(wxCommandEvent &ev)
{
    if (rbHtml->GetValue())
    {
        wxString def = txtHtmlStylesheet->GetValue();

        if (!wxFile::Exists(def))
            def.Empty();
   
        wxFileDialog file(this, _("Select stylesheet filename"), wxGetHomeDir(), def, _("HTML Stylesheet files (*.css)|*.css|All files (*.*)|*.*"), wxOPEN);

        if (file.ShowModal() == wxID_OK)
        {
            txtHtmlStylesheet->SetValue(file.GetPath());
            OnChange(ev);
        }
    }
    else
    {
        wxString def = txtXmlStylesheet->GetValue();

        if (!wxFile::Exists(def))
            def.Empty();
   
        wxFileDialog file(this, _("Select stylesheet filename"), wxGetHomeDir(), def, _("XML Stylesheet files (*.xsl)|*.xsl|All files (*.*)|*.*"), wxOPEN);

        if (file.ShowModal() == wxID_OK)
        {
            txtXmlStylesheet->SetValue(file.GetPath());
            OnChange(ev);
        }
    }
}

void frmReport::OnBrowseFile(wxCommandEvent &ev)
{
    if (rbHtml->GetValue())
    {
        wxFileDialog file(this, _("Select output filename"), wxGetHomeDir(), txtHtmlFile->GetValue(),
            _("HTML files (*.html)|*.html|All files (*.*)|*.*"), wxSAVE && wxOVERWRITE_PROMPT);

        if (file.ShowModal() == wxID_OK)
        {
            txtHtmlFile->SetValue(file.GetPath());
            OnChange(ev);
        }
    }
    else
    {
        wxFileDialog file(this, _("Select output filename"), wxGetHomeDir(), txtXmlFile->GetValue(),
            _("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxSAVE && wxOVERWRITE_PROMPT);

        if (file.ShowModal() == wxID_OK)
        {
            txtXmlFile->SetValue(file.GetPath());
            OnChange(ev);
        }
    }
}

void frmReport::SetReportTitle(const wxString &t) 
{ 
    txtTitle->SetValue(t); 
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// START STYLESHEET FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

wxString frmReport::GetCssLink(const wxString &file)
{
    wxString data;

    data = wxT("    <link rel=\"stylesheet\" href=\"") + file + wxT("\" type=\"text/css\" />\n");

    return data;
}

wxString frmReport::GetEmbeddedCss(const wxString &css)
{
    wxString data;

    data  = wxT("    <style type=\"text/css\">\n");
    data += css;
    data += wxT("    </style>\n");

    return data;
}

const wxString frmReport::GetDefaultCss()
{
    wxString data;

    data = wxT("      body {  font-family: verdana, helvetica, sans-serif; margin: 0px; padding: 0; }\n")
           wxT("      h1 { font-weight: bold; font-size: 150%; border-bottom-style: solid; border-bottom-width: 2px; margin-top: 0px; padding-bottom: 0.5ex; color: #eeeeee; }\n")
           wxT("      h2 { font-size: 130%; padding-bottom: 0.5ex; color: #009ace; border-bottom-style: solid; border-bottom-width: 2px; }\n")
           wxT("      h3 { font-size: 110%; padding-bottom: 0.5ex; color: #000000; }\n")
           wxT("      th { text-align: left; background-color: #009ace; color: #eeeeee; }\n")
           wxT("      #ReportHeader { padding: 10px; background-color: #009ace; color: #eeeeee; border-bottom-style: solid; border-bottom-width: 2px; border-color: #999999; }\n")
           wxT("      #ReportHeader th { width: 25%; white-space: nowrap; vertical-align: top; }\n")
           wxT("      #ReportHeader td { vertical-align: top; color: #eeeeee; }\n")
           wxT("      #ReportNotes { padding: 10px; background-color: #eeeeee; font-size: 80%; border-bottom-style: solid; border-bottom-width: 2px; border-color: #999999; }\n")
           wxT("      .ReportSQL { margin-bottom: 10px; padding: 10px; display: block; background-color: #eeeeee; font-family: monospace; }\n")
           wxT("      #ReportDetails { margin-left: 10px; margin-right: 10px; margin-bottom: 10px; }\n")
           wxT("      #ReportDetails td, th { font-size: 80%; margin-left: 2px; margin-right: 2px; }\n")
           wxT("      #ReportDetails th { border-bottom-color: #777777; border-bottom-style: solid; border-bottom-width: 2px; }\n")
           wxT("      .ReportDetailsOddDataRow { background-color: #dddddd; }\n")
           wxT("      .ReportDetailsEvenDataRow { background-color: #eeeeee; }\n")
           wxT("      .ReportTableHeaderCell { background-color: #dddddd; color: #009ace; vertical-align: top; font-size: 80%; white-space: nowrap; }\n")
           wxT("      .ReportTableValueCell { vertical-align: top; font-size: 80%; white-space: nowrap; }\n")
           wxT("      .ReportTableInfo { font-size: 80%; font-style: italic; }\n")
           wxT("      #ReportFooter { font-weight: bold; font-size: 80%; text-align: right; background-color: #009ace; color: #eeeeee; margin-top: 10px; padding: 2px; border-bottom-style: solid; border-bottom-width: 2px; border-top-style: solid; border-top-width: 2px; border-color: #999999; }\n")
           wxT("      #ReportFooter a { color: #ffffff; text-decoration: none; }\n");

    return data;
}

wxString frmReport::GetDefaultXsl(const wxString &css)
{
    wxString data;

    data = wxT("<?xml version=\"1.0\"?>\n")
           wxT("<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n")
           wxT("<xsl:output method=\"xml\" doctype-system=\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\" doctype-public=\"-//W3C//DTD XHTML 1.0 Transitional//EN\" indent=\"yes\" encoding=\"utf-8\" />\n")
           wxT("<xsl:template match=\"/report\">\n")
           wxT("\n")
           wxT("<html>\n")
           wxT("  <head>\n")
           wxT("    <xsl:if test=\"header/title != ''\">\n")
           wxT("      <title><xsl:value-of select=\"header/title\" /></title>\n")
           wxT("    </xsl:if>\n")
           wxT("    <meta http-equiv=\"Content-Type\" content=\"utf-8\" />\n");
    data += css;
    data += wxT("  </head>\n")
            wxT("\n")
            wxT("  <body>\n")
            wxT("    <div id=\"ReportHeader\">\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/title != ''\">\n")
            wxT("      <h1><xsl:value-of select=\"header/title\" /></h1>\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/generated != ''\">\n")
            wxT("      <b>");
    data +=   _("Generated");
    data += wxT(": </b><xsl:value-of select=\"header/generated\" /><br />\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/server != ''\">\n")
            wxT("      <b>");
    data +=   _("Server");
    data += wxT(": </b><xsl:value-of select=\"header/server\" /><br />\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/database != ''\">\n")
            wxT("      <b>");
    data +=   _("Database");
    data += wxT(": </b><xsl:value-of select=\"header/database\" /><br />\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/schema != ''\">\n")
            wxT("      <b>");
    data +=   _("Schema");
    data += wxT(": </b><xsl:value-of select=\"header/schema\" /><br />\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/table != ''\">\n")
            wxT("      <b>");
    data +=   _("Table");
    data += wxT(": </b><xsl:value-of select=\"header/table\" /><br />\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/job != ''\">\n")
            wxT("      <b>");
    data +=   _("Job");
    data += wxT(": </b><xsl:value-of select=\"header/job\" /><br />\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    </div>\n")
            wxT("\n")
            wxT("    <xsl:if test=\"header/notes != ''\">\n")
            wxT("      <div id=\"ReportNotes\">\n")
            wxT("      <b>");
    data +=   _("Notes");
    data += wxT(": </b><br /><br />\n")
            wxT("      <xsl:call-template name=\"substitute\">\n")
            wxT("         <xsl:with-param name=\"string\" select=\"header/notes\" />\n")
            wxT("      </xsl:call-template>\n")
            wxT("      </div>\n")
            wxT("    </xsl:if>\n")
            wxT("\n")
            wxT("    <div id=\"ReportDetails\">\n")
            wxT("      <xsl:apply-templates select=\"section\" >\n")
			wxT("        <xsl:sort select=\"@number\" data-type=\"number\" order=\"ascending\" />\n")
            wxT("      </xsl:apply-templates>\n")
            wxT("    </div>\n")
            wxT("\n")
            wxT("    <div id=\"ReportFooter\">\n");
    data +=   _("Report generated by");
    data += wxT(" <a href=\"http://www.pgadmin.org/\">pgAdmin</a>\n")
            wxT("    </div>\n")
            wxT("\n")
            wxT("    <br />\n")
            wxT("  </body>\n")
            wxT("</html>\n")
            wxT("\n")
            wxT("</xsl:template>\n")
            wxT("\n")
            wxT("<xsl:template match=\"section\">\n")
            wxT("  <xsl:if test=\"../section[@id = current()/@id]/@name != ''\">\n")
            wxT("    <h2><xsl:value-of select=\"../section[@id = current()/@id]/@name\" /></h2>\n")
            wxT("  </xsl:if>\n")
            wxT("\n")
            wxT("  <xsl:if test=\"count(../section[@id = current()/@id]/table/columns/column) > 0\">\n")
            wxT("    <table>\n")
            wxT("      <tr>\n")
            wxT("        <xsl:apply-templates select=\"../section[@id = current()/@id]/table/columns/column\">\n")
			wxT("          <xsl:sort select=\"@number\" data-type=\"number\" order=\"ascending\" />\n")
            wxT("          <xsl:with-param name=\"count\" select=\"count(../section[@id = current()/@id]/table/columns/column)\" />\n")
            wxT("        </xsl:apply-templates>\n")
            wxT("      </tr>\n")
            wxT("      <xsl:apply-templates select=\"../section[@id = current()/@id]/table/rows/*\" mode=\"rows\">\n")
			wxT("          <xsl:sort select=\"@number\" data-type=\"number\" order=\"ascending\" />\n")
            wxT("        <xsl:with-param name=\"column-meta\" select=\"../section[@id = current()/@id]/table/columns/column\" />\n")
            wxT("      </xsl:apply-templates>\n")
            wxT("    </table>\n")
            wxT("    <br />\n")
            wxT("    <xsl:if test=\"../section[@id = current()/@id]/table/info != ''\">\n")
            wxT("      <p class=\"ReportTableInfo\"><xsl:value-of select=\"../section[@id = current()/@id]/table/info\" /></p>\n")
            wxT("    </xsl:if>\n")
            wxT("  </xsl:if>\n")
            wxT("\n")
            wxT("  <xsl:if test=\"../section[@id = current()/@id]/sql != ''\">\n")
            wxT("    <pre class=\"ReportSQL\">\n")
            wxT("      <xsl:call-template name=\"substitute\">\n")
            wxT("         <xsl:with-param name=\"string\" select=\"../section[@id = current()/@id]/sql\" />\n")
            wxT("      </xsl:call-template>\n")
            wxT("    </pre>\n")
            wxT("  </xsl:if>\n")
            wxT("</xsl:template>\n")
            wxT("\n")
            wxT("<xsl:template match=\"column\">\n")
            wxT("  <xsl:param name=\"count\" />\n")
            wxT("  <th class=\"ReportTableHeaderCell\">\n")
            wxT("  <xsl:attribute name=\"width\"><xsl:value-of select=\"100 div $count\" />%</xsl:attribute>\n")
            wxT("    <xsl:call-template name=\"substitute\">\n")
            wxT("       <xsl:with-param name=\"string\" select=\"@name\" />\n")
            wxT("    </xsl:call-template>\n")
            wxT("  </th>\n")
            wxT("</xsl:template>\n")
            wxT("\n")
            wxT("<xsl:template match=\"*\" mode=\"rows\">\n")
            wxT("  <xsl:param name=\"column-meta\" />\n")
            wxT("  <tr>\n")
            wxT("  <xsl:choose>\n")
            wxT("  <xsl:when test=\"position() mod 2 != 1\">\n")
            wxT("    <xsl:attribute name=\"class\">ReportDetailsOddDataRow</xsl:attribute>\n")
            wxT("  </xsl:when>\n")
            wxT("  <xsl:otherwise>\n")
            wxT("    <xsl:attribute name=\"class\">ReportDetailsEvenDataRow</xsl:attribute>\n")
            wxT("  </xsl:otherwise>\n")
            wxT("  </xsl:choose>\n")
            wxT("    <xsl:apply-templates select=\"$column-meta\" mode=\"cells\">\n")
            wxT("      <xsl:with-param name=\"row\" select=\".\" />\n")
            wxT("    </xsl:apply-templates>\n")
            wxT("  </tr>\n")
            wxT("</xsl:template>\n")
            wxT("\n")
            wxT("<xsl:template match=\"*\" mode=\"cells\">\n")
            wxT("  <xsl:param name=\"row\" />\n")
            wxT("    <td class=\"ReportTableValueCell\">\n")
            wxT("    <xsl:choose>\n")
            wxT("      <xsl:when test=\"$row/@*[name() = current()/@id]|$row/*[name() = current()/@id] != ''\">\n")
            wxT("        <xsl:call-template name=\"substitute\">\n")
            wxT("          <xsl:with-param name=\"string\" select=\"$row/@*[name() = current()/@id]|$row/*[name() = current()/@id]\" />\n")
            wxT("        </xsl:call-template>\n")
            wxT("      </xsl:when>\n")
            wxT("      <xsl:otherwise>\n")
            wxT("        <xsl:text> </xsl:text>\n")
            wxT("      </xsl:otherwise>\n")
            wxT("    </xsl:choose>\n")
            wxT("  </td>\n")
            wxT("</xsl:template>\n")
            wxT("\n")
            wxT("<xsl:template name=\"substitute\">\n")
            wxT("   <xsl:param name=\"string\" />\n")
            wxT("   <xsl:param name=\"from\" select=\"'&#xA;'\" />\n")
            wxT("   <xsl:param name=\"to\">\n")
            wxT("      <br />\n")
            wxT("   </xsl:param>\n")
            wxT("   <xsl:choose>\n")
            wxT("      <xsl:when test=\"contains($string, $from)\">\n")
            wxT("         <xsl:value-of select=\"substring-before($string, $from)\" />\n")
            wxT("         <xsl:copy-of select=\"$to\" />\n")
            wxT("         <xsl:call-template name=\"substitute\">\n")
            wxT("            <xsl:with-param name=\"string\" select=\"substring-after($string, $from)\" />\n")
            wxT("            <xsl:with-param name=\"from\" select=\"$from\" />\n")
            wxT("            <xsl:with-param name=\"to\" select=\"$to\" />\n")
            wxT("         </xsl:call-template>\n")
            wxT("      </xsl:when>\n")
            wxT("      <xsl:otherwise>\n")
            wxT("         <xsl:value-of select=\"$string\" />\n")
            wxT("      </xsl:otherwise>\n")
            wxT("   </xsl:choose>\n")
            wxT("</xsl:template>\n")
            wxT("\n")
            wxT("</xsl:stylesheet>\n");

    return data;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END STYLESHEET FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// START XML FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frmReport::XmlAddHeaderValue(const wxString &name, const wxString &value)
{
    header += wxT("    <") + HtmlEntities(name) + wxT(">") + HtmlEntities(value) + wxT("</") + HtmlEntities(name) + wxT(">\n");
}

int frmReport::XmlCreateSection(const wxString &name)
{
    int ind = sectionName.Add(HtmlEntities(name));
    sectionData.Add(wxT(""));
    sectionTableHeader.Add(wxT(""));
    sectionTableRows.Add(wxT(""));
    sectionTableInfo.Add(wxT(""));
    sectionSql.Add(wxT(""));
    return ind+1;
}

void frmReport::XmlSetSectionTableHeader(const int section, int columns, const wxChar *name,...)
{
    va_list ap;
    const wxChar *p = name;
    wxString data;

    va_start(ap, name);

    for (int x = 0; x < columns; x++)
    {
        data += wxT("        <column id=\"c");
        data += NumToStr((long)(x+1));
        data += wxT("\" number=\"");
		data += NumToStr((long)(x+1));
        data += wxT("\" name=\"");
        data += HtmlEntities(p);
		data += wxT("\"/>\n");
        p = va_arg(ap, wxChar*);
    }

    va_end(ap);

    sectionTableHeader[section-1] = data;
}

void frmReport::XmlAddSectionTableRow(const int section, int number, int columns, const wxChar *value,...)
{
    va_list ap;
    const wxChar *p = value;
    wxString data;

    va_start(ap, value);

    data = wxT("        <row id=\"r");
    data += NumToStr((long)number);
	data += wxT("\" number=\"");
    data += NumToStr((long)number);
	data += wxT("\"");
	
    for (int x = 0; x < columns; x++)
    {
        data += wxT(" c");
        data += NumToStr((long)(x+1));
        data += wxT("=\"");
        data += HtmlEntities(p);
        data += wxT("\"");
        p = va_arg(ap, wxChar*);
    }


    data += wxT(" />\n");

    va_end(ap);

    sectionTableRows[section-1] += data;
}

void frmReport::XmlAddSectionTableFromListView(const int section, ctlListView *list)
{
    // Get the column headers
    int cols = list->GetColumnCount();

    wxString data;
    wxListItem itm;

    // Build the columns
    for (int x = 0; x < cols; x++)
    {
        itm.SetMask(wxLIST_MASK_TEXT);
        list->GetColumn(x, itm);
        wxString label = itm.GetText();
        data += wxT("        <column id=\"c");
        data += NumToStr((long)(x+1));
        data += wxT("\" number=\"");
        data += NumToStr((long)(x+1));
        data += wxT("\" name=\"");
        data += HtmlEntities(label);
        data += wxT("\" />\n");
    }
    sectionTableHeader[section-1] = data;

    // Build the rows
    int rows = list->GetItemCount();

    for (int y = 0; y < rows; y++)
    {
        data = wxT("        <row id=\"r");
        data += NumToStr((long)(y+1));
	    data += wxT("\" number=\"");
        data += NumToStr((long)(y+1));
		data += wxT("\"");

        for (int x = 0; x < cols; x++)
        {
            data += wxT(" c");
            data += NumToStr((long)(x+1));
            data += wxT("=\"");
            data += HtmlEntities(list->GetText(y, x));
            data += wxT("\"");
        }
        data += wxT(" />\n");
        sectionTableRows[section-1] += data;
    }
}

void frmReport::XmlAddSectionTableFromGrid(const int section, ctlSQLResult *grid)
{
#if USE_LISTVIEW
    // Get the column headers
    int cols = grid->GetColumnCount();
    int shift = 0;

    wxString data;
    wxListItem itm;

    if (grid->GetRowCountSuppressed())
        shift = 1;

    // Build the columns (Skip the Row column)
    for (int x = 1; x < cols + shift; x++)
    {
        itm.SetMask(wxLIST_MASK_TEXT);
        grid->GetColumn(x - shift, itm);
        wxString label = itm.GetText();
        data += wxT("        <column id=\"c");
        data += NumToStr((long)(x+1));
        data += wxT("\" number=\"");
        data += NumToStr((long)(x+1));
        data += wxT("\" name=\"");
        data += HtmlEntities(label);
        data += wxT("\" />\n");
    }
    sectionTableHeader[section-1] = data;

    // Build the rows
    int rows = grid->GetItemCount();

    for (int y = 0; y < rows; y++)
    {
        data = wxT("        <row id=\"r");
        data += NumToStr((long)(y+1));
	    data += wxT("\" number=\"");
        data += NumToStr((long)(y+1));
		data += wxT("\"");

        for (int x = 1; x < cols + shift; x++)
        {
            data += wxT(" c");
            data += NumToStr((long)(x+1));
            data += wxT("=\"");
            itm.SetId(y);
            itm.SetColumn(x - shift);
            itm.SetMask(wxLIST_MASK_TEXT);
            grid->GetItem(itm);
            data += HtmlEntities(itm.GetText());
            data += wxT("\"");
        }
        data += wxT(" />\n");
        sectionTableRows[section-1] += data;
    }
#else
    // Get the column headers
    int cols = grid->GetNumberCols();
    int shift = 0;

    wxString data;
    wxListItem itm;

    if (grid->GetRowCountSuppressed())
        shift = 1;

    for (int x = 1; x <= cols; x++)
    {
        wxString label = grid->OnGetItemText(-1, x - shift);
        data += wxT("        <column id=\"c");
        data += NumToStr((long)(x));
        data += wxT("\" number=\"");
        data += NumToStr((long)(x));
        data += wxT("\" name=\"");
        data += HtmlEntities(label);
        data += wxT("\" />\n");
    }
    sectionTableHeader[section-1] = data;

    // Build the rows
    int rows = grid->NumRows();

    for (int y = 0; y < rows; y++)
    {
        data = wxT("        <row id=\"r");
        data += NumToStr((long)(y+1));
	    data += wxT("\" number=\"");
        data += NumToStr((long)(y+1));
		data += wxT("\"");

        for (int x = 1; x <= cols; x++)
        {
            data += wxT(" c");
            data += NumToStr((long)(x));
            data += wxT("=\"");
            data += HtmlEntities(grid->OnGetItemText(y, x - shift));
            data += wxT("\"");
        }
        data += wxT(" />\n");
        sectionTableRows[section-1] += data;
    }
#endif
}

void frmReport::XmlAddSectionValue(const int section, const wxString &name, const wxString &value)
{
    sectionData[section-1] += wxT("    <") + HtmlEntities(name) + wxT(">") + HtmlEntities(value) + wxT("</") + HtmlEntities(name) + wxT(">\n");
}

wxString frmReport::GetSectionTableColumns(const int section)
{
    wxString data;
    
    data  = wxT("      <columns>\n");
    data += sectionTableHeader[section-1];
    data += wxT("      </columns>\n");

    return data;
}

wxString frmReport::GetSectionTableRows(const int section)
{
    wxString data;
    
    data  = wxT("      <rows>\n");
    data += sectionTableRows[section-1];
    data += wxT("      </rows>\n");

    return data;
}

wxString frmReport::GetSectionTable(const int section)
{
    wxString data;
    
    data  = wxT("    <table>\n");
    data += GetSectionTableColumns(section);
    data += GetSectionTableRows(section);

    if (!sectionTableInfo[section-1].IsEmpty())
    {
        data += wxT("      <info>");
        data += sectionTableInfo[section-1];
        data += wxT("</info>\n");
    }

    data += wxT("    </table>\n");

    return data;
}

wxString frmReport::GetSection(const int section)
{
    wxString data;
    
    data  = wxT("  <section id=\"s");
    data += NumToStr((long)section);
    data += wxT("\" number=\"");
    data += NumToStr((long)section);
    data += wxT("\" name=\"");
    data += sectionName[section-1];
    data += wxT("\">\n");
    data += GetSectionTable(section);


    if (chkSql->GetValue() && !sectionSql[section-1].IsEmpty())
    {
        data += wxT("    <sql>");
        data += sectionSql[section-1];
        data += wxT("</sql>\n");
    }
    
    data += sectionData[section-1];
    data += wxT("  </section>\n");

    return data;
}

wxString frmReport::GetXmlReport(const wxString &stylesheet = wxT(""))
{
    wxString data;

    data  = wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    
    if (!stylesheet.IsEmpty())
    {
        data += wxT("<?xml-stylesheet type=\"text/xsl\" href=\"");
        data += stylesheet;
        data += wxT("\" ?>\n");
    }

    data += wxT("\n");
    data += wxT("<report>\n\n");

    data += wxT("  <header>\n");
    data += header;
    data += wxT("  </header>\n\n");

    for (unsigned int x=1; x <= sectionName.GetCount(); x++ )
    {
        data += GetSection(x);
        data += wxT("\n");
    }

    data += wxT("</report>\n");

    return data;
}

//
// libxml convenience macros
//
#define XML_FROM_WXSTRING(s) ((xmlChar *)(const char *)s.mb_str(wxConvUTF8))
#define WXSTRING_FROM_XML(s) wxString((char *)s, wxConvUTF8)

wxString frmReport::XslProcessReport(const wxString &xml, const wxString &xsl)
{
    xmlChar *output=0;
    xmlDocPtr ssDoc=0, xmlDoc=0, resDoc=0;
    xsltStylesheetPtr ssPtr=0;
    int length;

    wxBeginBusyCursor();

    // Apply the stylesheet
    xmlSubstituteEntitiesDefault (1); // Substitute entities
    xmlLoadExtDtdDefaultValue = 1; // Load external entities

    // Parse the stylesheet
    ssDoc = xmlParseDoc(XML_FROM_WXSTRING(xsl));
    if (!ssDoc)
    {
        wxEndBusyCursor();
        wxLogError(_("Failed to parse the XML stylesheet!"));
        goto cleanup;
    }

    ssPtr = xsltParseStylesheetDoc(ssDoc);
    if (!ssPtr)
    {
        wxEndBusyCursor();
        wxLogError(_("Failed to parse the XSL stylesheet!"));
        goto cleanup;
    }

    // Parse the data
    xmlDoc = xmlParseDoc(XML_FROM_WXSTRING(xml));
    if (!xmlDoc)
    {
        wxEndBusyCursor();
        wxLogError(_("Failed to parse the XML document!"));
        goto cleanup;
    }

    // Apply the stylesheet
    resDoc = xsltApplyStylesheet(ssPtr, xmlDoc, NULL);
    if (!resDoc)
    {
        wxEndBusyCursor();
        wxLogError(_("Failed to to apply the XSL stylesheet to the XML document!"));
        goto cleanup;
    }

    // Get the result
    xsltSaveResultToString (&output, &length, resDoc, ssPtr);
    if (!resDoc)
    {
        wxEndBusyCursor();
        wxLogError(_("Failed to to read the processed document!"));
        goto cleanup;
    }

cleanup:

    // Cleanup
    if (resDoc)
        xmlFreeDoc(resDoc);

    if (xmlDoc)
        xmlFreeDoc(xmlDoc);

    if (ssPtr)
        xsltFreeStylesheet(ssPtr);

    // This crashes - dunno why :-(
    // if (ssDoc)
    //  xmlFreeDoc(ssDoc); 

    xsltCleanupGlobals();
    xmlCleanupParser();

    wxEndBusyCursor();

    if (output)
        return WXSTRING_FROM_XML(output);
    else
        return wxEmptyString;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END XML FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Report base
///////////////////////////////////////////////////////
wxWindow *reportBaseFactory::StartDialog(frmMain *form, pgObject *obj)
{
    parent = form;

    wxBeginBusyCursor();
    frmReport *report = new frmReport(GetFrmMain());

    // Generate the report header
    wxDateTime now = wxDateTime::Now();
    report->XmlAddHeaderValue(wxT("generated"), now.Format(wxT("%c")));
    if (obj->GetServer())
        report->XmlAddHeaderValue(wxT("server"), obj->GetServer()->GetFullIdentifier());
    if (obj->GetDatabase())
        report->XmlAddHeaderValue(wxT("database"), obj->GetDatabase()->GetName());
    if (obj->GetSchema())
        report->XmlAddHeaderValue(wxT("schema"), obj->GetSchema()->GetName());
    if (obj->GetJob())
        report->XmlAddHeaderValue(wxT("job"), obj->GetJob()->GetName());
    if (obj->GetTable())
        report->XmlAddHeaderValue(wxT("table"), obj->GetTable()->GetName());

    GenerateReport(report, obj);
    wxEndBusyCursor();

    report->ShowModal();
    return 0;   
}

///////////////////////////////////////////////////////
// Properties report
///////////////////////////////////////////////////////
reportObjectPropertiesFactory::reportObjectPropertiesFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar)
: reportBaseFactory(list)
{
    mnu->Append(id, _("&Properties report"), _("Generate a properties report for this object."));
}

bool reportObjectPropertiesFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        if (obj->IsCollection())
            return false;
        else
            return true;
    }
    return false;
}

void reportObjectPropertiesFactory::GenerateReport(frmReport *report, pgObject *object)
{
    wxString title = object->GetTypeName();
    title += _(" properties report - ");
    title += object->GetIdentifier();
    report->SetReportTitle(title);

    int section = report->XmlCreateSection(object->GetTypeName() + _(" properties"));

    ctlListView *list = GetFrmMain()->GetProperties();
    object->ShowProperties();

    report->XmlAddSectionTableFromListView(section, list);

    report->XmlSetSectionSql(section, object->GetSql(NULL));
}

///////////////////////////////////////////////////////
// DDL report
///////////////////////////////////////////////////////
reportObjectDdlFactory::reportObjectDdlFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar)
: reportBaseFactory(list)
{
    mnu->Append(id, _("&DDL report"), _("Generate a DDL report for this object."));
}

bool reportObjectDdlFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        if (obj->GetMetaType() == PGM_SERVER || obj->IsCollection())
            return false;
        else
            return true;
    }
    return false;
}

void reportObjectDdlFactory::GenerateReport(frmReport *report, pgObject *object)
{
    wxString title = object->GetTypeName();
    title += _(" DDL report - ");
    title += object->GetIdentifier();
    report->SetReportTitle(title);

    int section = report->XmlCreateSection(object->GetTypeName() + _(" DDL"));

    report->XmlSetSectionSql(section, object->GetSql(NULL));
}

///////////////////////////////////////////////////////
// Data dictionary report
///////////////////////////////////////////////////////
reportObjectDataDictionaryFactory::reportObjectDataDictionaryFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar)
: reportBaseFactory(list)
{
    mnu->Append(id, _("&Data dictionary report"), _("Generate a data dictionary report for this object."));
}

bool reportObjectDataDictionaryFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        if (obj->GetMetaType() == PGM_TABLE && !obj->IsCollection())
            return true;
        else
            return false;
    }
    return false;
}

void reportObjectDataDictionaryFactory::GenerateReport(frmReport *report, pgObject *object)
{

    pgTable *table = (pgTable *)object;

    wxString title = object->GetTypeName();
    title += _(" Data dictionary report - ");
    title += object->GetIdentifier();
    report->SetReportTitle(title);

    // Columns
    int section = report->XmlCreateSection(_("Columns"));

    report->XmlSetSectionTableHeader(section, 6, _("Name"), _("Data type"), _("Not Null?"), _("Primary key?"), _("Default"), _("Comment"));

    ctlTree *browser = GetFrmMain()->GetBrowser();
    pgCollection *columns = table->GetColumnCollection(browser);

    treeObjectIterator colIt(browser, columns);

    pgColumn *column;
    bool haveInherit = false;
    wxString colName;
    while ((column = (pgColumn*)colIt.GetNextObject()) != 0)
    {
        column->ShowTreeDetail(browser);
        if (column->GetColNumber() > 0)
        {
            colName = column->GetName();
            if (column->GetInheritedCount() > 0)
            {
                colName += _("*");
                haveInherit = true;
            }

            report->XmlAddSectionTableRow(section, 
                                          column->GetColNumber(), 
                                          6,
                                          colName.c_str(),
                                          column->GetVarTypename().c_str(),
                                          BoolToYesNo(column->GetNotNull()).c_str(),
                                          BoolToYesNo(column->GetIsPK()).c_str(),
                                          column->GetDefault().c_str(),
                                          column->GetComment().c_str());
        }
    }
    if (haveInherit)
    {
        wxString info;
        info.Printf(_("* Inherited columns from %s."), table->GetInheritedTables().c_str());
        report->XmlSetSectionTableInfo(section, info);
    }

    // Constraints
    pgCollection *constraints = table->GetConstraintCollection(browser);

    treeObjectIterator conIt(browser, constraints);

    pgObject *constraint;
    long x = 1;
    wxString definition, type;
    while ((constraint = (pgObject*)conIt.GetNextObject()) != 0)
    {
        if (x == 1)
        {
            section = report->XmlCreateSection(_("Constraints"));
            report->XmlSetSectionTableHeader(section, 4, _("Name"), _("Type"), _("Definition"), _("Comment"));
        }

        constraint->ShowTreeDetail(browser);

        switch (constraint->GetMetaType())
        {
            case PGM_PRIMARYKEY:
                type = _("Primary key");
                definition = ((pgIndexConstraint*)constraint)->GetDefinition();
                break;
            case PGM_UNIQUE:
                type = _("Unique");
                definition = ((pgIndexConstraint*)constraint)->GetDefinition();
                break;
            case PGM_FOREIGNKEY:
                type = _("Foreign key");
                definition = ((pgForeignKey*)constraint)->GetDefinition();
                break;
            case PGM_CHECK:
                type = _("Check");
		        definition = wxT("(") + ((pgCheck*)constraint)->GetDefinition() + wxT(")");
                break;
        }

        report->XmlAddSectionTableRow(section, 
                                      x,
                                      4,
                                      constraint->GetName().c_str(),
                                      type.c_str(),
                                      definition.c_str(),
                                      constraint->GetComment().c_str());
        x++;
    }
}

///////////////////////////////////////////////////////
// Statistics report
///////////////////////////////////////////////////////
reportObjectStatisticsFactory::reportObjectStatisticsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar)
: reportBaseFactory(list)
{
    mnu->Append(id, _("&Statistics report"), _("Generate a statistics report for this object."));
}

bool reportObjectStatisticsFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        if (!obj->HasStats())
        {
            if (obj->IsCollection())
            {
                pgaFactory *f = ((pgCollection *)obj)->GetItemFactory();

                if (f)
                {
                    if (f->GetMetaType() == PGM_TABLE ||
                        f->GetMetaType() == PGM_TABLESPACE || 
                        f->GetMetaType() == PGM_DATABASE)
                        return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

        }
        else
        {
            return true;
        }
    }

    return false;
}

void reportObjectStatisticsFactory::GenerateReport(frmReport *report, pgObject *object)
{
    wxString title = object->GetTypeName();
    title += _(" statistics report - ");
    title += object->GetIdentifier();
    report->SetReportTitle(title);

    int section = report->XmlCreateSection(object->GetTypeName() + _(" statistics"));

    ctlListView *list = GetFrmMain()->GetStatisticsCtl();
    object->ShowStatistics(GetFrmMain(), list);

    report->XmlAddSectionTableFromListView(section, list);
}

///////////////////////////////////////////////////////
// Dependencies report
///////////////////////////////////////////////////////
reportObjectDependenciesFactory::reportObjectDependenciesFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar)
: reportBaseFactory(list)
{
    mnu->Append(id, _("&Dependencies report"), _("Generate a dependencies report for this object."));
}

bool reportObjectDependenciesFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        if (!obj->HasDepends())
            return false;
        else
            return true;
    }
    return false;
}

void reportObjectDependenciesFactory::GenerateReport(frmReport *report, pgObject *object)
{
    wxString title = object->GetTypeName();
    title += _(" dependencies report - ");
    title += object->GetIdentifier();
    report->SetReportTitle(title);

    int section = report->XmlCreateSection(object->GetTypeName() + _(" dependencies"));
    ctlListView *list = GetFrmMain()->GetDependenciesCtl();
    object->ShowDependencies(parent, list);

    report->XmlAddSectionTableFromListView(section, list);
}

///////////////////////////////////////////////////////
// Dependents report
///////////////////////////////////////////////////////
reportObjectDependentsFactory::reportObjectDependentsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar)
: reportBaseFactory(list)
{
    mnu->Append(id, _("&Dependents report"), _("Generate a dependents report for this object."));
}

bool reportObjectDependentsFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        if (!obj->HasReferences())
            return false;
        else
            return true;
    }
    return false;
}

void reportObjectDependentsFactory::GenerateReport(frmReport *report, pgObject *object)
{
    wxString title = object->GetTypeName();
    title += _(" dependents report - ");
    title += object->GetIdentifier();
    report->SetReportTitle(title);

    int section = report->XmlCreateSection(object->GetTypeName() + _(" dependents"));

    ctlListView *list = GetFrmMain()->GetReferencedByCtl();
    object->ShowDependents(parent, list);

    report->XmlAddSectionTableFromListView(section, list);
}

///////////////////////////////////////////////////////
// Object list report
///////////////////////////////////////////////////////
reportObjectListFactory::reportObjectListFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar)
: reportBaseFactory(list)
{
    mnu->Append(id, _("&Object list report"), _("Generate an object list report for this collection."));
}

bool reportObjectListFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        if (!obj->IsCollection())
        {
            return false;
        }
        else
        {
            pgaFactory *f = ((pgCollection *)obj)->GetItemFactory();
            if (obj)
            {
                if (f->GetMetaType() == PGM_SERVER)
                    return false;
            }
            else
                return false;

            return true;
        }
    }
    return false;
}

void reportObjectListFactory::GenerateReport(frmReport *report, pgObject *object)
{
    wxString title;
    title.Printf(_("%s list report"), object->GetIdentifier().c_str());
    report->SetReportTitle(title);

    int section = report->XmlCreateSection(object->GetFullIdentifier());

    ctlListView *list = GetFrmMain()->GetProperties();
    object->ShowProperties();

    report->XmlAddSectionTableFromListView(section, list);
}

