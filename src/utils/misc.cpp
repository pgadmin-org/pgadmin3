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
#define YYSTYPE
#define DECIMAL DECIMAL_P
#include "parser/keywords.h"
#include "parser/parse.h"
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


wxString qtStringDollar(const wxString &value)
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



static bool needsQuoting(wxString& value, bool forTypes)
{
    // Replace Double Quotes
    if (value.Replace(wxT("\""), wxT("\"\"")) > 0)
        return true;

    // Is it a number?
    if (value.IsNumber()) 
        return true;
    else
    {
        int pos = 0;
        while (pos < (int)value.length())
        {
            wxChar c=value.GetChar(pos);
            if (!(c >= '0' && c <= '9') && 
                !(c >= 'a' && c  <= 'z') && 
                !(c == '_'))
            {
                return true;
            }
            pos++;
        }
    }

    // is it a keyword?
    const ScanKeyword *sk=ScanKeywordLookup(value.ToAscii());
    if (sk)
    {
        if (forTypes)
        {
            switch (sk->value)
            {
                case ANY:
                case BIGINT:
                case BIT:
                case BOOLEAN_P:
                case CHAR_P:
                case CHARACTER:
                case DECIMAL:
                case DOUBLE_P:
                case FLOAT_P:
                case INT_P:
                case INTEGER:
                case INTERVAL:
                case NUMERIC:
                case SET:
                case SMALLINT:
                case TIME:
                case TIMESTAMP:
                case TRIGGER:
                case VARCHAR:
                    break;
                default:
                    return true;
            }
        }
        else
            return true;
    }

    return false;
}


wxString qtTypeIdent(const wxString& value)
{
    if (value.Length() == 0)
        return value;

    wxString result = value;

    if (needsQuoting(result, true))
        return wxT("\"") + result + wxT("\"");
    else
        return result;
}


wxString qtIdent(const wxString& value)
{
    if (value.Length() == 0)
        return value;

    wxString result = value;

    if (needsQuoting(result, false))
        return wxT("\"") + result + wxT("\"");
    else
        return result;
}


wxString qtStrip(const wxString &str)
{
    if (str.Left(2) == wxT("\\\""))
        return str.Mid(2, str.Length()-4);
    else
        return str;
}


void FillArray(wxArrayString &array, const wxString &list)
{
    const wxChar *cp=list.c_str();

    wxString str;
    bool quote=false;
    
    while (*cp)
    {
        switch (*cp)
        {
            case '\\':
            {
                if (cp[1]== '"')
                {
                    cp++;
                    str.Append(*cp);
                }
                break;
            }
            case '"':
            case '\'':
            {
                quote = !quote;
                break;
            }
            case ',':
            {
                if (!quote)
                {
                    array.Add(str);
                    str=wxEmptyString;
                    break;
                }
            }
            default:
            {
                str.Append(*cp);
                break;
            }
        }
        cp++;
    }
    if (!str.IsEmpty())
        array.Add(str);
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
    wxFontEncoding encoding = wxFONTENCODING_DEFAULT;
    wxUtfFile file;

    if (format < 0)
    {
        if (wxFile::Access(filename, wxFile::read))
        {
            file.Open(filename);
            encoding = file.GetEncoding();
            file.Close();
        }
        if (encoding == wxFONTENCODING_DEFAULT)
            encoding = settings->GetUnicodeFile() ? wxFONTENCODING_UTF8 : wxFONTENCODING_SYSTEM;
    }
    else
        encoding = format ? wxFONTENCODING_UTF8 : wxFONTENCODING_SYSTEM;


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
    if (helpTopic.Left(8) == wxT("pgadmin/"))
        DisplayHelp(wnd, helpTopic.Mid(8), icon);
    else if (settings->GetSqlHelpSite().length() != 0) 
        frmHelp::LoadSqlDoc(wnd, helpTopic  + wxT(".html"));
    else
        DisplayHelp(wnd, wxT("pg/") + helpTopic, icon);
}


#ifndef WIN32
wxString ExecProcess(const wxString &cmd)
{
    wxString res;
	FILE *f=popen(cmd.ToAscii(), "r");

	if (f)
	{
		char buffer[1024];
		int cnt;
		while ((cnt = fread(buffer, 1, 1024, f)) > 0)
		{
			res += wxString::FromAscii(buffer);
		}
		pclose(f);
	}
	return res;
}
#endif
