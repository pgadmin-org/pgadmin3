//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.cpp - Miscellaneous Utilities
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/timer.h>
#include <wx/xrc/xmlres.h>
#include <wx/file.h>

// Standard headers
#include <stdlib.h>

// App headers
#include "misc.h"
#include "menu.h"
#include "pgAdmin3.h"
#include "frmMain.h"
#include "frmHelp.h"


// we dont have an appropriate wxLongLong method
#ifdef __WIN32__
#define atolonglong _atoi64
#else
#ifdef __WXMAC__
#define atolonglong(str) strtoll(str, (char **)NULL, 10) 
#else
#define atolonglong atoll
#endif
#endif



// Global Vars - yuch!
wxStopWatch stopwatch;
wxString timermsg;

void StartMsg(const wxString& msg)
{
    extern frmMain *winMain;

    if (!timermsg.IsEmpty()) return;
    timermsg.Printf(wxT("%s..."), msg.c_str());
    wxBeginBusyCursor();
    stopwatch.Start(0);
    wxLogStatus(timermsg);
    winMain->statusBar->SetStatusText(timermsg, 1);
    winMain->statusBar->SetStatusText(wxT(""), 2);
}

void EndMsg()
{

    extern frmMain *winMain;

    if (!timermsg.IsEmpty())
    {
        // Get the execution time & display it
        float timeval = stopwatch.Time();
        wxString time;
        time.Printf(wxT("%.2f secs"), (timeval/1000));
        winMain->statusBar->SetStatusText(time, 2);

        // Display the 'Done' message
        winMain->statusBar->SetStatusText(timermsg + _(" Done."), 1);
        wxLogStatus(wxT("%s (%s)"), timermsg.c_str(), time.c_str());
        timermsg.Empty();
        wxEndBusyCursor();
    }
}

// Conversions


wxString BoolToYesNo(bool value)
{
    return value ? _("Yes") : _("No");
}


wxString BoolToStr(bool value)
{
    return value ? wxT("Yes") : wxT("No");
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
    return strtod(value.ToAscii(), 0);
}


wxULongLong StrToLongLong(const wxString &value)
{
    return atolonglong(value.ToAscii());
}


wxString GetListText(wxListCtrl *lst, long row, long col)
{
    wxListItem item;
    item.SetId(row);
    item.SetColumn(col);
    item.SetMask(wxLIST_MASK_TEXT);
    lst->GetItem(item);
    return item.GetText();
}


long GetListSelected(wxListCtrl *lst)
{
    return lst->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
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

wxString qtIdent(const wxString& value)
{
    wxString result = value;
    
    if (result.Length() == 0)
        return result;

    int pos = 0;

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


wxString FileRead(const wxString &filename)
{
    wxString str;
    wxFile file(filename);
    if (file.IsOpened())
    {
        int len=file.Length();
        char *buf=new char[len+1];
        memset(buf, 0, len+1);
        file.Read(buf, len);
        file.Close();
        if (settings->GetUnicodeFile())
            str=wxString(buf, wxConvUTF8);
        else
            str=wxString(buf, wxConvLibc);
        delete[] buf;
        str.Replace(wxT("\r"), wxT(""));
    }
    return str;
}


bool FileWrite(const wxString &filename, const wxString &data)
{
    wxFile file(filename, wxFile::write);
    if (file.IsOpened())
    {
        wxString buf=data;
#ifdef __WIN32__
        buf.Replace(wxT("\n"), wxT("\r\n"));
#endif
        if (settings->GetUnicodeFile())
            file.Write(buf, wxConvUTF8);
        else
            file.Write(buf, wxConvLibc);
        file.Close();
        return true;
    }
    return false;
}



BEGIN_EVENT_TABLE(DialogWithHelp, wxDialog)
    EVT_MENU(MNU_HELP,                              DialogWithHelp::OnHelp)
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
    wxString helpSite=settings->GetSqlHelpSite();
    wxString page=GetHelpPage();

    if (!page.IsEmpty())
    {
        frmHelp *h=new frmHelp(mainForm);
        h->Show(true);
        if (!h->Load(helpSite + page))
            h->Destroy();
    }
}
