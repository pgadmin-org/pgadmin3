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
    // REWRITE THIS - IT'S BLOODY INEFFICIENT!!

    // Quick bailout
    if (ch >= 'a' && ch <= 'z' ||
        ch >= 'A' && ch <= 'Z' ||
        ch >= '0' && ch <= '9')
        return wxString(ch);

    typedef struct __htmlEntities
    {
        unsigned short code;
        const wxChar *entity;
    } htmlEntities;

    htmlEntities ents[] = {
        { 198, wxT("AElig") },
        { 193, wxT("Aacute") },
        { 194, wxT("Acirc") },
        { 192, wxT("Agrave") },
        { 913, wxT("Alpha") },
        { 197, wxT("Aring") },
        { 195, wxT("Atilde") },
        { 196, wxT("Auml") },
        { 914, wxT("Beta") },
        { 199, wxT("Ccedil") },
        { 935, wxT("Chi") },
        { 8225, wxT("Dagger") },
        { 916, wxT("Delta") },
        { 208, wxT("ETH") },
        { 201, wxT("Eacute") },
        { 202, wxT("Ecirc") },
        { 200, wxT("Egrave") },
        { 917, wxT("Epsilon") },
        { 919, wxT("Eta") },
        { 203, wxT("Euml") },
        { 915, wxT("Gamma") },
        { 205, wxT("Iacute") },
        { 206, wxT("Icirc") },
        { 204, wxT("Igrave") },
        { 921, wxT("Iota") },
        { 207, wxT("Iuml") },
        { 922, wxT("Kappa") },
        { 923, wxT("Lambda") },
        { 924, wxT("Mu") },
        { 209, wxT("Ntilde") },
        { 925, wxT("Nu") },
        { 338, wxT("OElig") },
        { 211, wxT("Oacute") },
        { 212, wxT("Ocirc") },
        { 210, wxT("Ograve") },
        { 937, wxT("Omega") },
        { 927, wxT("Omicron") },
        { 216, wxT("Oslash") },
        { 213, wxT("Otilde") },
        { 214, wxT("Ouml") },
        { 934, wxT("Phi") },
        { 928, wxT("Pi") },
        { 8243, wxT("Prime") },
        { 936, wxT("Psi") },
        { 929, wxT("Rho") },
        { 352, wxT("Scaron") },
        { 931, wxT("Sigma") },
        { 222, wxT("THORN") },
        { 932, wxT("Tau") },
        { 920, wxT("Theta") },
        { 218, wxT("Uacute") },
        { 219, wxT("Ucirc") },
        { 217, wxT("Ugrave") },
        { 933, wxT("Upsilon") },
        { 220, wxT("Uuml") },
        { 926, wxT("Xi") },
        { 221, wxT("Yacute") },
        { 376, wxT("Yuml") },
        { 918, wxT("Zeta") },
        { 225, wxT("aacute") },
        { 226, wxT("acirc") },
        { 180, wxT("acute") },
        { 230, wxT("aelig") },
        { 224, wxT("agrave") },
        { 8501, wxT("alefsym") },
        { 945, wxT("alpha") },
        { 38, wxT("amp") },
        { 8743, wxT("and") },
        { 8736, wxT("ang") },
        { 229, wxT("aring") },
        { 8776, wxT("asymp") },
        { 227, wxT("atilde") },
        { 228, wxT("auml") },
        { 8222, wxT("bdquo") },
        { 946, wxT("beta") },
        { 166, wxT("brvbar") },
        { 8226, wxT("bull") },
        { 8745, wxT("cap") },
        { 231, wxT("ccedil") },
        { 184, wxT("cedil") },
        { 162, wxT("cent") },
        { 967, wxT("chi") },
        { 710, wxT("circ") },
        { 9827, wxT("clubs") },
        { 8773, wxT("cong") },
        { 169, wxT("copy") },
        { 8629, wxT("crarr") },
        { 8746, wxT("cup") },
        { 164, wxT("curren") },
        { 8659, wxT("dArr") },
        { 8224, wxT("dagger") },
        { 8595, wxT("darr") },
        { 176, wxT("deg") },
        { 948, wxT("delta") },
        { 9830, wxT("diams") },
        { 247, wxT("divide") },
        { 233, wxT("eacute") },
        { 234, wxT("ecirc") },
        { 232, wxT("egrave") },
        { 8709, wxT("empty") },
        { 8195, wxT("emsp") },
        { 8194, wxT("ensp") },
        { 949, wxT("epsilon") },
        { 8801, wxT("equiv") },
        { 951, wxT("eta") },
        { 240, wxT("eth") },
        { 235, wxT("euml") },
        { 8364, wxT("euro") },
        { 8707, wxT("exist") },
        { 402, wxT("fnof") },
        { 8704, wxT("forall") },
        { 189, wxT("frac12") },
        { 188, wxT("frac14") },
        { 190, wxT("frac34") },
        { 8260, wxT("frasl") },
        { 947, wxT("gamma") },
        { 8805, wxT("ge") },
        { 62, wxT("gt") },
        { 8660, wxT("hArr") },
        { 8596, wxT("harr") },
        { 9829, wxT("hearts") },
        { 8230, wxT("hellip") },
        { 237, wxT("iacute") },
        { 238, wxT("icirc") },
        { 161, wxT("iexcl") },
        { 236, wxT("igrave") },
        { 8465, wxT("image") },
        { 8734, wxT("infin") },
        { 8747, wxT("int") },
        { 953, wxT("iota") },
        { 191, wxT("iquest") },
        { 8712, wxT("isin") },
        { 239, wxT("iuml") },
        { 954, wxT("kappa") },
        { 8656, wxT("lArr") },
        { 955, wxT("lambda") },
        { 9001, wxT("lang") },
        { 171, wxT("laquo") },
        { 8592, wxT("larr") },
        { 8968, wxT("lceil") },
        { 8220, wxT("ldquo") },
        { 8804, wxT("le") },
        { 8970, wxT("lfloor") },
        { 8727, wxT("lowast") },
        { 9674, wxT("loz") },
        { 8206, wxT("lrm") },
        { 8249, wxT("lsaquo") },
        { 8216, wxT("lsquo") },
        { 60, wxT("lt") },
        { 175, wxT("macr") },
        { 8212, wxT("mdash") },
        { 181, wxT("micro") },
        { 183, wxT("middot") },
        { 8722, wxT("minus") },
        { 956, wxT("mu") },
        { 8711, wxT("nabla") },
        { 160, wxT("nbsp") },
        { 8211, wxT("ndash") },
        { 8800, wxT("ne") },
        { 8715, wxT("ni") },
        { 172, wxT("not") },
        { 8713, wxT("notin") },
        { 8836, wxT("nsub") },
        { 241, wxT("ntilde") },
        { 957, wxT("nu") },
        { 243, wxT("oacute") },
        { 244, wxT("ocirc") },
        { 339, wxT("oelig") },
        { 242, wxT("ograve") },
        { 8254, wxT("oline") },
        { 969, wxT("omega") },
        { 959, wxT("omicron") },
        { 8853, wxT("oplus") },
        { 8744, wxT("or") },
        { 170, wxT("ordf") },
        { 186, wxT("ordm") },
        { 248, wxT("oslash") },
        { 245, wxT("otilde") },
        { 8855, wxT("otimes") },
        { 246, wxT("ouml") },
        { 182, wxT("para") },
        { 8706, wxT("part") },
        { 8240, wxT("permil") },
        { 8869, wxT("perp") },
        { 966, wxT("phi") },
        { 960, wxT("pi") },
        { 982, wxT("piv") },
        { 177, wxT("plusmn") },
        { 163, wxT("pound") },
        { 8242, wxT("prime") },
        { 8719, wxT("prod") },
        { 8733, wxT("prop") },
        { 968, wxT("psi") },
        { 34, wxT("quot") },
        { 8658, wxT("rArr") },
        { 8730, wxT("radic") },
        { 9002, wxT("rang") },
        { 187, wxT("raquo") },
        { 8594, wxT("rarr") },
        { 8969, wxT("rceil") },
        { 8221, wxT("rdquo") },
        { 8476, wxT("real") },
        { 174, wxT("reg") },
        { 8971, wxT("rfloor") },
        { 961, wxT("rho") },
        { 8207, wxT("rlm") },
        { 8250, wxT("rsaquo") },
        { 8217, wxT("rsquo") },
        { 8218, wxT("sbquo") },
        { 353, wxT("scaron") },
        { 8901, wxT("sdot") },
        { 167, wxT("sect") },
        { 173, wxT("shy") },
        { 963, wxT("sigma") },
        { 962, wxT("sigmaf") },
        { 8764, wxT("sim") },
        { 9824, wxT("spades") },
        { 8834, wxT("sub") },
        { 8838, wxT("sube") },
        { 8721, wxT("sum") },
        { 8835, wxT("sup") },
        { 185, wxT("sup1") },
        { 178, wxT("sup2") },
        { 179, wxT("sup3") },
        { 8839, wxT("supe") },
        { 223, wxT("szlig") },
        { 964, wxT("tau") },
        { 8756, wxT("there4") },
        { 952, wxT("theta") },
        { 977, wxT("thetasym") },
        { 8201, wxT("thinsp") },
        { 254, wxT("thorn") },
        { 732, wxT("tilde") },
        { 215, wxT("times") },
        { 8482, wxT("trade") },
        { 8657, wxT("uArr") },
        { 250, wxT("uacute") },
        { 8593, wxT("uarr") },
        { 251, wxT("ucirc") },
        { 249, wxT("ugrave") },
        { 168, wxT("uml") },
        { 978, wxT("upsih") },
        { 965, wxT("upsilon") },
        { 252, wxT("uuml") },
        { 8472, wxT("weierp") },
        { 958, wxT("xi") },
        { 253, wxT("yacute") },
        { 165, wxT("yen") },
        { 255, wxT("yuml") },
        { 950, wxT("zeta") },
        { 8205, wxT("zwj") },
        { 8204, wxT("zwnj") },
        { 0, NULL }};

    int elem = 0;

    while (ents[elem].code != 0)
    {
        if (ch == ents[elem].code)
        {
            wxString ret = wxT("&");
            ret += ents[elem].entity;
            ret += wxT(";");
            return ret;
        }
        elem++;
    }

    return ch;
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

