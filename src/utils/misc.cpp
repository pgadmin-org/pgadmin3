//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// misc.cpp - Miscellaneous Utilities
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/timer.h>
#include <wx/xrc/xmlres.h>

// Standard headers
#include <stdlib.h>

// App headers
#include "misc.h"
#include "pgAdmin3.h"
#include "frmMain.h"

// Global Vars - yuch!
wxStopWatch stopwatch;
wxString timermsg;

void StartMsg(const wxString& msg)
{
    extern frmMain *winMain;

    if (!timermsg.IsEmpty()) return;
    timermsg.Printf("%s...", msg.c_str());
    wxBeginBusyCursor();
    stopwatch.Start(0);
    wxLogStatus(timermsg);
    winMain->statusBar->SetStatusText(timermsg, 1);
}

void EndMsg()
{

    extern frmMain *winMain;

    if (timermsg.IsEmpty()) return;

    // Get the execution time & display it
    float timeval = stopwatch.Time();
    wxString time, msg;
    time.Printf("%.2f Secs", (timeval/1000));
    winMain->statusBar->SetStatusText(time, 2);

    // Display the 'Done' message
    timermsg.Append(" Done.");
    msg.Printf("%s (%s)", timermsg.c_str(), time.c_str());
    wxLogStatus(msg);
    winMain->statusBar->SetStatusText(timermsg, 1);
    wxEndBusyCursor();
    timermsg.Empty();
    
}

// Conversions

wxString StrToYesNo(const wxString& value)
{
    wxString result;
    if (value.StartsWith(wxT("t"))) {
        result.Printf("Yes");
    } else if (value.StartsWith(wxT("T"))) {
        result.Printf("Yes");
    } else if (value.StartsWith(wxT("1"))) {
        result.Printf("Yes");
    } else if (value.StartsWith(wxT("Y"))) {
        result.Printf("Yes");
    } else if (value.StartsWith(wxT("y"))) {
        result.Printf("Yes");
    } else {
        result.Printf("No");
    }

    return result;
}

wxString BoolToYesNo(bool value)
{
    wxString result;
    if (value) {
        result.Printf("Yes");
    } else {
        result.Printf("No");
    }
    return result;
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
    result.Printf("%ld", value);
    return result;
}

long StrToLong(const wxString& value)
{
    return atol(value.c_str());
}

wxString NumToStr(double value)
{
    wxString result;
    static wxString decsep;
    
    if (decsep.Length() == 0) {
        decsep.Printf("%lf", 1.2);
        decsep = decsep[(unsigned int)1];
    }

    result.Printf("%lf", value);
    result.Replace(decsep,".");

    // Get rid of excessive decimal places
    if (result.Contains(wxT(".")))
        while (result.Right(1) == "0")
            result.RemoveLast();
    if (result.Right(1) == ".")
        result.RemoveLast();

    return result;
}

double StrToDouble(const wxString& value)
{
    return strtod(value.c_str(), 0);
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
    int scrW=wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    int scrH=wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

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

    result.Replace("\\", "\\\\");
    result.Replace("'", "\\'");
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
    result.Replace("\"", "\"\"");
	
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
		return FALSE;

	const char *first = 
		wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	const char *second = 
		wxT("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (strchr(first, (wxChar)ident[0U]) == NULL)
		return FALSE;

	unsigned int si;
	for ( si = 1; si < len; si++)
	{
	if (strchr(second, (wxChar)ident[si]) == NULL)
		return FALSE;
	}

	return TRUE;
}


queryTokenizer::queryTokenizer(const wxString& str, const char delim)
: wxStringTokenizer()
{
    if (delim == ' ')
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
