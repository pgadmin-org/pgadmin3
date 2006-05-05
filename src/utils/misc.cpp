//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: misc.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    if (helpTopic.Left(3) == wxT("pg/") && settings->GetSqlHelpSite().length() != 0) 
        frmHelp::LoadSqlDoc(wnd, helpTopic.Mid(3)  + wxT(".html"));
    else
        DisplayHelp(wnd, helpTopic, icon);
}

wxString GetHtmlEntity(const wxChar ch)
{
    // REWRITE THIS - IT'S (STILL) BLOODY INEFFICIENT!!

    // Quick bailout
    if (ch >= 'a' && ch <= 'z' ||
        ch >= 'A' && ch <= 'Z' ||
        ch >= '0' && ch <= '9')
        return wxString(ch);

    unsigned short ents[] = {
                            34,
                            39,
                            38,
                            60,
                            62,
                            160,
                            161,
                            164,
                            162,
                            163,
                            165,
                            166,
                            167,
                            168,
                            169,
                            170,
                            171,
                            172,
                            173,
                            174,
                            8482,
                            175,
                            176,
                            177,
                            178,
                            179,
                            180,
                            181,
                            182,
                            183,
                            184,
                            185,
                            186,
                            187,
                            188,
                            189,
                            190,
                            191,
                            215,
                            247,
                            192,
                            193,
                            194,
                            195,
                            196,
                            197,
                            198,
                            199,
                            200,
                            201,
                            202,
                            203,
                            204,
                            205,
                            206,
                            207,
                            208,
                            209,
                            210,
                            211,
                            212,
                            213,
                            214,
                            216,
                            217,
                            218,
                            219,
                            220,
                            221,
                            222,
                            223,
                            224,
                            225,
                            226,
                            227,
                            228,
                            229,
                            230,
                            231,
                            232,
                            233,
                            234,
                            235,
                            236,
                            237,
                            238,
                            239,
                            240,
                            241,
                            242,
                            243,
                            244,
                            245,
                            246,
                            248,
                            249,
                            250,
                            251,
                            252,
                            253,
                            254,
                            255,
                            338,
                            339,
                            352,
                            353,
                            376,
                            710,
                            732,
                            8194,
                            8195,
                            8201,
                            8204,
                            8205,
                            8206,
                            8207,
                            8211,
                            8212,
                            8216,
                            8217,
                            8218,
                            8220,
                            8221,
                            8222,
                            8224,
                            8225,
                            8230,
                            8240,
                            8249,
                            8250,
                            8364,
                            0};

    int elem = 0;

    while (ents[elem] != 0)
    {
        if (ch == ents[elem])
        {
            wxString ret = wxT("&#");
            ret += NumToStr((long)ents[elem]);
            ret += wxT(";");
            return ret;
        }
        elem++;
    }

    return wxString(ch);
}

wxString HtmlEntities(const wxString &str)
{
    wxString ret;

    for (unsigned int x=0; x < str.Length(); x++)
    {
        ret += GetHtmlEntity(str[x]);
    }

    return ret;
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

#ifdef WIN32
#if (_MSC_VER < 1300)
/* _ftol2 is more than VC7. */
extern "C" long _ftol( double ); 
extern "C" long _ftol2( double dblSource ) { return _ftol( dblSource ); } 
#endif
#endif

