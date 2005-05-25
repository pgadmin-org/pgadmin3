//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#define YYSTYPE_IS_DECLARED
#define DECIMAL DECIMAL_P
  typedef int YYSTYPE;
#include "parser/keywords.h"
#include "parser/parse.h"
}

wxString IdAndName(long id, const wxString &name)
{
    wxString str;
    str.Printf(wxT("%d - %s"), id, name.BeforeFirst('\n').c_str());
    return str;
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


void FillKeywords(wxString &str)
{
    // unfortunately, the keyword list is static. 
    // If the first or the last word change, these both need to get updated.
    const ScanKeyword *keyword=ScanKeywordLookup("abort");
    const ScanKeyword *last=ScanKeywordLookup("zone");

    wxASSERT(keyword && last);

    str = wxString::FromAscii(keyword->name);

    while (keyword++ < last)
        str += wxT(" ") + wxString::FromAscii(keyword->name);
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
            if (!((pos > 0) && (c >= '0' && c <= '9')) && 
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
        wxString helpdir=docPath + wxT("/") + settings->GetCanonicalLanguage();

        if (!wxFile::Exists(helpdir + wxT("/pgadmin3..hhp")) &&
#if defined(__WXMSW__) || wxUSE_LIBMSPACK
            !wxFile::Exists(helpdir + wxT("/pgadmin3.chm")) &&
#endif
            !wxFile::Exists(helpdir + wxT("/pgadmin3.zip")))
            helpdir=docPath + wxT("/en_US");

#ifdef __WXMSW__
#ifndef __WXDEBUG__
       if (wxFile::Exists(helpdir + wxT("/pgadmin3.chm")))
        {
            helpCtl=new wxCHMHelpController();
            helpCtl->Initialize(helpdir + wxT("/pgadmin3"));
        }
        else
#endif
#endif
#if wxUSE_LIBMSPACK
        if (wxFile::Exists(helpdir + wxT("/pgadmin3.chm")) ||
            wxFile::Exists(helpdir + wxT("/pgadmin3.hhp")) || wxFile::Exists(helpdir + wxT("/pgadmin3.zip")))
#else
        if (wxFile::Exists(helpdir + wxT("/pgadmin3.hhp")) || wxFile::Exists(helpdir + wxT("/pgadmin3.zip")))
#endif
        {
            helpCtl=new wxHtmlHelpController();
			((wxHtmlHelpController*)helpCtl)->SetTempDir(helpdir);
            helpCtl->Initialize(helpdir + wxT("/pgadmin3"));
        }
    }

    wxString page;
    int hashPos = helpTopic.Find('#');
    if (hashPos < 0)
        page = helpTopic + wxT(".html");
    else
        page = helpTopic.Left(hashPos) + wxT(".html") + helpTopic.Mid(hashPos);

    if (helpCtl)
    {
        if (helpTopic == wxT("index.html"))
            helpCtl->DisplayContents();
        else
            helpCtl->DisplaySection(page);
    }
    else
    {
        while (wnd->GetParent())
            wnd=wnd->GetParent();
    
        frmHelp::LoadLocalDoc(wnd, page, icon);
    }
}


void DisplaySqlHelp(wxWindow *wnd, const wxString &helpTopic, char **icon)
{
    if (settings->GetSqlHelpSite().length() != 0) 
        frmHelp::LoadSqlDoc(wnd, helpTopic.Mid(3)  + wxT(".html"));
    else
        DisplayHelp(wnd, helpTopic, icon);
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
