//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: misc.cpp 4054 2005-03-28 16:43:01Z andreas $
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// base.cpp - Miscellaneous Basic Utilities
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>


// Standard headers
#include <locale.h>

// App headers
#include "base/base.h"


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
        return true;
    } else if (value.StartsWith(wxT("T"))) {
        return true;
    } else if (value.StartsWith(wxT("1"))) {
        return true;
    } else if (value.StartsWith(wxT("Y"))) {
        return true;
    } else if (value.StartsWith(wxT("y"))) {
        return true;
    } 

    return false;
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
    result.Printf(wxT("%lu"), (long)value);
    return result;
}


long StrToLong(const wxString& value)
{
    return atol(value.ToAscii());
}


OID StrToOid(const wxString& value)
{
    return (OID)strtoul(value.ToAscii(), 0, 10);
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
