//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.cpp - Miscellaneous Utilities
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/timer.h>
#include <wx/xrc/xmlres.h>
#include <wx/file.h>
#include <wx/textbuf.h>
#include <wx/help.h>
#include <wx/fontenc.h>
#include "utffile.h"
#include <locale.h>

#ifdef __WXMSW__
#include <wx/msw/helpchm.h>
#endif

// Standard headers
#include <stdlib.h>

// App headers
#include "misc.h"
#include "menu.h"
#include "frmMain.h"
#include "frmHelp.h"

extern "C"
{
#include "parser/keywords.h"
}

// we dont have an appropriate wxLongLong method
#ifdef __WIN32__
#define atolonglong _atoi64
#else
#ifdef __WXMAC__
#define atolonglong(str) strtoll(str, (char **)NULL, 10) 
#else
#ifdef __FreeBSD__
#define atolonglong(str) strtoll(str, (char **)NULL, 10)
#else
#define atolonglong atoll
#endif
#endif
#endif



// Conversions


wxString BoolToYesNo(bool value)
{
    return value ? _("Yes") : _("No");
}


wxString BoolToStr(bool value)
{
    return value ? wxT("true") : wxT("false");
}



bool StrToBool(const wxString& value)
{
    if (value.StartsWith(wxT("t"))) {
        return TRUE;
    } else if (value.StartsWith(wxT("T"))) {
        return TRUE;
    } else if (value.StartsWith(wxT("1"))) {
        return TRUE;
    } else if (value.StartsWith(wxT("Y"))) {
        return TRUE;
    } else if (value.StartsWith(wxT("y"))) {
        return TRUE;
    } 

    return FALSE;
}

wxString NumToStr(long value)
{
    wxString result;
    result.Printf(wxT("%ld"), value);
    return result;
}


wxString NumToStr(OID value)
{
    wxString result;
    result.Printf(wxT("%u"), (long)value);
    return result;
}


long StrToLong(const wxString& value)
{
    return atol(value.ToAscii());
}


OID StrToOid(const wxString& value)
{
    return (OID)atol(value.ToAscii());
}

wxString NumToStr(double value)
{
    wxString result;
    static wxString decsep;
    
    if (decsep.Length() == 0) {
        decsep.Printf(wxT("%lf"), 1.2);
        decsep = decsep[(unsigned int)1];
    }

    result.Printf(wxT("%lf"), value);
    result.Replace(decsep, wxT("."));

    // Get rid of excessive decimal places
    if (result.Contains(wxT(".")))
        while (result.Right(1) == wxT("0"))
            result.RemoveLast();
    if (result.Right(1) == wxT("."))
        result.RemoveLast();

    return result;
}

double StrToDouble(const wxString& value)
{
    wxCharBuffer buf = value.ToAscii();
    char *p=strchr(buf, '.');
    if (p)
        *p = localeconv()->decimal_point[0];

    return strtod(buf, 0);
}


wxULongLong StrToLongLong(const wxString &value)
{
    return atolonglong(value.ToAscii());
}


wxString DateToAnsiStr(const wxDateTime &datetime)
{
    if (!datetime.IsValid())
        return wxEmptyString;

    return datetime.FormatISODate() + wxT(" ") + datetime.FormatISOTime();
}


wxString DateToStr(const wxDateTime &datetime)
{
    if (!datetime.IsValid())
        return wxEmptyString;

    return datetime.FormatDate() + wxT(" ") + datetime.FormatTime();
}


wxDateTime StrToDateTime(const wxString &value)
{
    wxDateTime dt;
    /* This hasn't just been used. ( Is not infinity ) */
    if ( !value.IsEmpty() )
    dt.ParseDateTime(value);
    return dt;
}


void CheckOnScreen(wxPoint &pos, wxSize &size, const int w0, const int h0)
{
    wxSize screenSize = wxGetDisplaySize();
    int scrW = screenSize.x;
    int scrH = screenSize.y;

    if (pos.x < 0)
        pos.x = 0;
    if (pos.y < 0)
        pos.y = 0;

    if (pos.x > scrW-w0)
        pos.x = scrW-w0;
    if (pos.y > scrH-h0)
        pos.y = scrH-h0;
    
    if (size.GetWidth() < w0)
        size.SetWidth(w0);
    if (size.GetHeight() < h0)
        size.SetHeight(h0);

    if (size.GetWidth() > scrW)
        size.SetWidth(scrW);
    if (size.GetHeight() > scrH)
        size.SetHeight(scrH);
}


wxString qtString(const wxString& value)
{
    wxString result = value;	

    result.Replace(wxT("\\"), wxT("\\\\"));
    result.Replace(wxT("'"), wxT("\\'"));
    result.Append(wxT("'"));
    result.Prepend(wxT("'"));
	
    return result;
}


wxString qtDocumentHere(const wxString &value)
{
    wxString qtDefault=wxT("BODY");
    wxString qt=qtDefault;
    int counter=1;
    if (value.Find('\'') < 0 && value.Find('\n') < 0)
        return qtString(value);

    while (value.Find(wxT("$") + qt + wxT("$")) >= 0)
        qt.Printf(wxT("%s%d"), qtDefault.c_str(), counter++);


    return wxT("$") + qt + wxT("$") 
        +  value 
        +  wxT("$") + qt + wxT("$");
}


wxString qtIdent(const wxString& value)
{
    wxString result = value;
    
    if (result.Length() == 0)
        return result;

    int pos = 0;

#if 0
    if (ScanKeywordLookup(value.ToAscii()))
    {
        result.Append(wxT("\""));
        result.Prepend(wxT("\""));
        return result;
    }
#endif
    // Replace Double Quotes
    result.Replace(wxT("\""), wxT("\"\""));
	
    // Is it a number?
    if (result.IsNumber()) {
        result.Append(wxT("\""));
        result.Prepend(wxT("\""));
        return result;
    } else {
        while (pos < (int)result.length()) {
            if (!((result.GetChar(pos) >= '0') && (result.GetChar(pos) <= '9')) && 
                !((result.GetChar(pos)  >= 'a') && (result.GetChar(pos)  <= 'z')) && 
                !(result.GetChar(pos)  == '_')){
            
                result.Append(wxT("\""));
                result.Prepend(wxT("\""));
                return result;	
            }
            pos++;
        }
    }	
    return result;
}

// Keith 2003.03.11
// We need an identifier validation function
bool IsValidIdentifier(wxString ident)
{
    // compiler complains if not unsigned
	unsigned int len = ident.length();
	if (!len)
		return false;

	wxString first = 
		wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	wxString second = 
		wxT("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (!first.Contains(ident.Left(1)))
		return false;

	unsigned int si;
	for ( si = 1; si < len; si++)
	{
	    if (!second.Contains(ident.Mid(si, 1)))
		    return false;
	}

	return true;
}


queryTokenizer::queryTokenizer(const wxString& str, const wxChar delim)
: wxStringTokenizer()
{
    if (delim == (wxChar)' ')
        SetString(str, wxT(" \n\r\t"), wxTOKEN_RET_EMPTY_ALL);
    else
        SetString(str, delim, wxTOKEN_RET_EMPTY_ALL);
    delimiter=delim;
}


void AppendIfFilled(wxString &str, const wxString &delimiter, const wxString &what)
{
    if (!what.IsNull())
        str+= delimiter + what;
}


wxString queryTokenizer::GetNextToken()
{
    // we need to override wxStringTokenizer, because we have to handle quotes
    wxString str;

    bool foundQuote=false;
    do
    {
        wxString s=wxStringTokenizer::GetNextToken();
        str.Append(s);
        int quotePos;
        do
        {
            quotePos = s.Find('"');
            if (quotePos >= 0)
            {
                foundQuote = !foundQuote;
                s = s.Mid(quotePos+1);
            }
        }
        while (quotePos >= 0);

        if (foundQuote)
            str.Append(delimiter);
    }
    while (foundQuote & HasMoreTokens());
 
    return str;
}


wxString FileRead(const wxString &filename, int format)
{
    wxString str;

    wxFontEncoding encoding;
    if (format > 0)
        encoding = wxFONTENCODING_UTF8;
    else
        encoding=wxFONTENCODING_DEFAULT;

    wxUtfFile file(filename, wxFile::read, encoding);

    if (file.IsOpened())
        file.Read(str);

    return wxTextBuffer::Translate(str, wxTextFileType_Unix);   // we want only \n
}


bool FileWrite(const wxString &filename, const wxString &data, int format)
{
    wxFontEncoding encoding;
    if (format < 0)
        encoding = settings->GetUnicodeFile() ? wxFONTENCODING_UTF8 : wxFONTENCODING_SYSTEM;
    else
        encoding = format ? wxFONTENCODING_UTF8 : wxFONTENCODING_SYSTEM;

    wxUtfFile file;

    if (format < 0) // first try using the old encoding
        file.Open(filename, wxFile::read_write, wxS_DEFAULT, encoding);

    if (!file.IsOpened())
        file.Open(filename, wxFile::write, wxS_DEFAULT, encoding);

    if (file.IsOpened())
        return file.Write(wxTextBuffer::Translate(data));

    return false;
}


void DisplayHelp(wxWindow *wnd, const wxString &helpTopic, char **icon)
{
    extern wxString docPath;
    static wxHelpControllerBase *helpCtl=0;
    static bool firstCall=true;

    if (firstCall)
    {
        firstCall=false;
        wxString helpfile=docPath + wxT("/") + settings->GetCanonicalLanguage() + wxT("/pgadmin3");

        if (!wxFile::Exists(helpfile + wxT(".hhp")) &&
#ifdef __WXMSW__
            !wxFile::Exists(helpfile + wxT(".chm")) &&
#endif
            !wxFile::Exists(helpfile + wxT(".zip")))
            helpfile=docPath + wxT("/en_US/pgadmin3");

#ifdef __WXMSW__
#ifndef __WXDEBUG__
        if (wxFile::Exists(helpfile + wxT(".chm")))
        {
            helpCtl=new wxCHMHelpController();
            helpCtl->Initialize(helpfile);
        }
        else
#endif
#endif
        if (wxFile::Exists(helpfile + wxT(".hhp")) || wxFile::Exists(helpfile + wxT(".zip")))
        {
            helpCtl=new wxHtmlHelpController();
            helpCtl->Initialize(helpfile);
        }
    }

    if (helpCtl)
    {
        if (helpTopic == wxT("index"))
            helpCtl->DisplayContents();
        else
            helpCtl->DisplaySection(helpTopic + wxT(".html"));
    }
    else
    {
        while (wnd->GetParent())
            wnd=wnd->GetParent();
    
        frmHelp::LoadLocalDoc(wnd, helpTopic + wxT(".html"), icon);
    }
}


void DisplaySqlHelp(wxWindow *wnd, const wxString &helpTopic, char **icon)
{
    if (settings->GetSqlHelpSite().length() != 0) 
        frmHelp::LoadSqlDoc(wnd, helpTopic  + wxT(".html"));
    else
        DisplayHelp(wnd, wxT("pg/") + helpTopic, icon);
}



BEGIN_EVENT_TABLE(DialogWithHelp, wxDialog)
    EVT_MENU(MNU_HELP,                  DialogWithHelp::OnHelp)
    EVT_BUTTON(XRCID("btnHelp"),        DialogWithHelp::OnHelp)
END_EVENT_TABLE();


DialogWithHelp::DialogWithHelp(frmMain *frame) : wxDialog()
{
    mainForm = frame;

    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_NORMAL, WXK_F1, MNU_HELP);
// this is for GTK because Meta (usually Numlock) is interpreted like Alt
// there are too many controls to reset m_Meta in all of them
    entries[1].Set(wxACCEL_ALT, WXK_F1, MNU_HELP);
    wxAcceleratorTable accel(2, entries);

    SetAcceleratorTable(accel);
}


void DialogWithHelp::OnHelp(wxCommandEvent& ev)
{
    wxString page=GetHelpPage();

    if (!page.IsEmpty())
        DisplaySqlHelp(this, page);
}
