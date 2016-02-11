//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/textbuf.h>
#include <wx/help.h>
#include <wx/html/helpctrl.h>
#include <wx/fontenc.h>
#include <wx/display.h>

#include "utils/utffile.h"
#include <locale.h>

#ifdef __WXMSW__
#include <wx/msw/helpchm.h>
#endif

// Standard headers
#include <stdlib.h>

// App headers
#include "utils/misc.h"

#if !defined(PGSCLI)

// App headers
#include "frm/frmMain.h"

wxImageList *imageList = 0;

#endif // PGSCLI

extern "C"
{
#define YYSTYPE_IS_DECLARED
#define DECIMAL DECIMAL_P
	typedef int YYSTYPE;
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



bool StrToBool(const wxString &value)
{
	if (value.StartsWith(wxT("t")))
	{
		return true;
	}
	else if (value.StartsWith(wxT("T")))
	{
		return true;
	}
	else if (value.StartsWith(wxT("1")))
	{
		return true;
	}
	else if (value.StartsWith(wxT("Y")))
	{
		return true;
	}
	else if (value.StartsWith(wxT("y")))
	{
		return true;
	}
	else if (value == wxT("on"))
		return true;

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


long StrToLong(const wxString &value)
{
	return atol(value.ToAscii());
}


OID StrToOid(const wxString &value)
{
	return (OID)strtoul(value.ToAscii(), 0, 10);
}

wxString generate_spaces(int length)
{
	return wxString().Pad(length);
}

wxString NumToStr(double value)
{
	wxString result;
	static wxString decsep;

	if (decsep.Length() == 0)
	{
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


wxString NumToStr(wxLongLong value)
{
	wxString str;
#if wxCHECK_VERSION(2, 9, 0)
	str.Printf("%" wxLongLongFmtSpec "d", value.GetValue());
#else
	str.Printf(wxT("%") wxLongLongFmtSpec wxT("d"), value.GetValue());
#endif
	return str;
}


double StrToDouble(const wxString &value)
{
	wxCharBuffer buf = value.ToAscii();
	char *p = (char *)strchr(buf, '.');
	if (p)
		*p = localeconv()->decimal_point[0];

	return strtod(buf, 0);
}


wxLongLong StrToLongLong(const wxString &value)
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


wxString ElapsedTimeToStr(wxLongLong msec)
{
	wxTimeSpan tsMsec(0, 0, 0, msec);

	int days = tsMsec.GetDays();
	int hours = (wxTimeSpan(tsMsec.GetHours(), 0, 0, 0) - wxTimeSpan(days * 24)).GetHours();
	int minutes = (wxTimeSpan(0, tsMsec.GetMinutes(), 0, 0) - wxTimeSpan(hours)).GetMinutes();
	long seconds = (wxTimeSpan(0, 0, tsMsec.GetSeconds(), 0) - wxTimeSpan(hours, minutes)).GetSeconds().ToLong();
	long milliseconds = (wxTimeSpan(0, 0, 0, tsMsec.GetMilliseconds()) - wxTimeSpan(0, 0, seconds)).GetMilliseconds().ToLong();

	if (days > 0)
		return wxString::Format(
		           wxT("%d %s, %02d:%02d:%02ld hours"),
		           days, wxT("days"), hours, minutes, seconds
		       );
	else if (hours > 0)
		return wxString::Format(
		           wxT("%02d:%02d:%02ld hours"), hours, minutes, seconds
		       );
	else if (msec >= 1000 * 60)
		return wxString::Format(wxT("%02d:%02ld minutes"), minutes, seconds);
	else if (msec >= 1000)
		return wxString::Format(
		           wxT("%ld.%ld secs"), seconds, milliseconds / 100
		       );
	else
		return msec.ToString() + wxT(" msec");
}

wxDateTime StrToDateTime(const wxString &value)
{
	wxDateTime dt;
	/* This hasn't just been used. ( Is not infinity ) */
	if ( !value.IsEmpty() )
		dt.ParseDateTime(value);
	return dt;
}

#if !defined(PGSCLI)

void CheckOnScreen(wxWindow *win, wxPoint &pos, wxSize &size, const int w0, const int h0)
{
	wxRect rect;
	int scrH, scrW;

	wxSize screenSize = wxGetDisplaySize();
	scrW = screenSize.x;
	scrH = screenSize.y;

	if (pos.x > scrW - w0)
		pos.x = scrW - w0;
	if (pos.y > scrH - h0)
		pos.y = scrH - h0;

	if (pos.x < 0)
		pos.x = 0;
	if (pos.y < 0)
		pos.y = 0;

	if (size.GetWidth() < w0)
		size.SetWidth(w0);
	if (size.GetHeight() < h0)
		size.SetHeight(h0);

	if (size.GetWidth() > scrW)
		size.SetWidth(scrW);
	if (size.GetHeight() > scrH)
		size.SetHeight(scrH);
}

#endif // PGSCLI

wxString qtConnString(const wxString &value)
{
	wxString result = value;

	result.Replace(wxT("\\"), wxT("\\\\"));
	result.Replace(wxT("'"), wxT("\\'"));
	result.Append(wxT("'"));
	result.Prepend(wxT("'"));

	return result;
}

#if !defined(PGSCLI)

wxString IdAndName(long id, const wxString &name)
{
	wxString str;
	str.Printf(wxT("%ld - %s"), id, name.BeforeFirst('\n').c_str());
	return str;
}


wxString qtDbStringDollar(const wxString &value)
{
	wxString qtDefault = wxT("BODY");
	wxString qt = qtDefault;
	int counter = 1;
	if (value.Find('\'') < 0 && value.Find('\n') < 0 && value.Find('\r') < 0)
	{
		wxString ret = value;
		ret.Replace(wxT("\\"), wxT("\\\\"));
		ret.Replace(wxT("'"), wxT("''"));
		ret.Append(wxT("'"));
		ret.Prepend(wxT("'"));
		return ret;
	}

	while (value.Find(wxT("$") + qt + wxT("$")) >= 0)
		qt.Printf(wxT("%s%d"), qtDefault.c_str(), counter++);


	return wxT("$") + qt + wxT("$")
	       +  value
	       +  wxT("$") + qt + wxT("$");
}


void FillKeywords(wxString &str)
{
	// unfortunately, the keyword list is static.
	int i;

	str = wxString();

	for (i = 0; i < NumScanKeywords; i++)
	{
		str += wxT(" ") + wxString::FromAscii(ScanKeywords[i].name);
	}
	for (i = 0; i < NumScanKeywordsExtra; i++)
	{
		str += wxT(" ") + wxString::FromAscii(ScanKeywordsExtra[i].name);
	}
}

#endif // PGSCLI

static bool needsQuoting(wxString &value, bool forTypes)
{
	// Is it a number?
	if (value.IsNumber())
		return true;
	else
	{
		// certain types should not be quoted even though it contains a space. Evilness.
		wxString valNoArray;
		if (forTypes && value.Right(2) == wxT("[]"))
			valNoArray = value.Mid(0, value.Len() - 2);
		else
			valNoArray = value;

		if (forTypes &&
		        (!valNoArray.CmpNoCase(wxT("character varying")) ||
		         !valNoArray.CmpNoCase(wxT("\"char\"")) ||
		         !valNoArray.CmpNoCase(wxT("bit varying")) ||
		         !valNoArray.CmpNoCase(wxT("double precision")) ||
		         !valNoArray.CmpNoCase(wxT("timestamp without time zone")) ||
		         !valNoArray.CmpNoCase(wxT("timestamp with time zone")) ||
		         !valNoArray.CmpNoCase(wxT("time without time zone")) ||
		         !valNoArray.CmpNoCase(wxT("time with time zone")) ||
		         !valNoArray.CmpNoCase(wxT("\"trigger\"")) ||
		         !valNoArray.CmpNoCase(wxT("\"unknown\""))))
			return false;

		int pos = 0;
		while (pos < (int)valNoArray.length())
		{
			wxChar c = valNoArray.GetChar(pos);
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
	const ScanKeyword *sk = ScanKeywordLookup(value.ToAscii());
	if (!sk)
		return false;
	if (sk->category == UNRESERVED_KEYWORD)
		return false;
	if (forTypes && sk->category == COL_NAME_KEYWORD)
		return false;
	return true;
}

wxString qtTypeIdent(const wxString &value)
{
	if (value.Length() == 0)
		return value;

	wxString result = value;

	if (needsQuoting(result, true))
	{
		result.Replace(wxT("\""), wxT("\"\""));
		return wxT("\"") + result + wxT("\"");
	}
	else
		return result;
}


wxString qtIdent(const wxString &value)
{
	if (value.Length() == 0)
		return value;

	wxString result = value;

	if (needsQuoting(result, false))
	{
		result.Replace(wxT("\""), wxT("\"\""));
		return wxT("\"") + result + wxT("\"");
	}
	else
		return result;
}

#if !defined(PGSCLI)

wxString qtStrip(const wxString &str)
{
	if (str.Left(2) == wxT("\\\""))
		return str.Mid(2, str.Length() - 4);
	else
		return str;
}


wxString TransformToNewDatconfig(const wxString &list)
{
	const wxChar *cp = list.c_str();

	wxString config = wxEmptyString;
	wxString str = wxEmptyString;
	bool quote = false;

	while (*cp)
	{
		switch (*cp)
		{
			case '\\':
			{
				if (cp[1] == '"')
				{
					str.Append(*cp);
					cp++;
					str.Append(*cp);
				}
				break;
			}
			case '"':
			case '\'':
			{
				quote = !quote;
				str.Append(*cp);
				break;
			}
			case ',':
			{
				if (!quote)
				{
					if (!config.IsEmpty())
						config += wxT(",");
					config += wxT("=") + str;
					str = wxEmptyString;
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
	{
		if (!config.IsEmpty())
			config += wxT(",");
		config += wxT("=") + str;
	}

	return config;
}


void FillArray(wxArrayString &array, const wxString &list)
{
	const wxChar *cp = list.c_str();

	wxString str;
	bool quote = false;

	while (*cp)
	{
		switch (*cp)
		{
			case '\\':
			{
				if (cp[1] == '"')
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
					str = wxEmptyString;
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


queryTokenizer::queryTokenizer(const wxString &str, const wxChar delim)
	: wxStringTokenizer()
{
	if (delim == (wxChar)' ')
		SetString(str, wxT(" \n\r\t"), wxTOKEN_RET_EMPTY_ALL);
	else
		SetString(str, delim, wxTOKEN_RET_EMPTY_ALL);
	delimiter = delim;
}


void AppendIfFilled(wxString &str, const wxString &delimiter, const wxString &what)
{
	if (!what.IsNull())
		str += delimiter + what;
}


wxString queryTokenizer::GetNextToken()
{
	// we need to override wxStringTokenizer, because we have to handle quotes
	wxString str;

	bool foundQuote = false;
	do
	{
		wxString s = wxStringTokenizer::GetNextToken();
		str.Append(s);
		int quotePos;
		do
		{
			quotePos = s.Find('"');
			if (quotePos >= 0)
			{
				foundQuote = !foundQuote;
				s = s.Mid(quotePos + 1);
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
		encoding = wxFONTENCODING_DEFAULT;

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


wxString CleanHelpPath(const wxString &path)
{
	wxString thePath = path;

	if (thePath.IsEmpty())
		return wxEmptyString;

	// If the filename ends in .chm, .hhp, .pdf or .zip, it's
	// a file based help system and we can assumes it's
	// correct.
	if (thePath.Lower().EndsWith(wxT(".hhp")) ||
#if defined (__WXMSW__) || wxUSE_LIBMSPACK
	        thePath.Lower().EndsWith(wxT(".chm")) ||
#endif
	        thePath.Lower().EndsWith(wxT(".pdf")) ||
	        thePath.Lower().EndsWith(wxT(".zip")))
		return thePath;

	// In all other cases we must have a directory
	wxString sep;

	// Figure out the appropriate seperator
	if (thePath.Lower().Contains(wxT("://")))
		sep = wxT("/");
	else
		sep = wxFileName::GetPathSeparator();

	// First, correct the path separators
	thePath.Replace(wxT("/"), sep);
	thePath.Replace(wxT("\\"), sep);

	// Now, append the seperator if it's not there
	if (!thePath.EndsWith(sep))
		thePath += sep;

	return thePath;
}

bool HelpPathValid(const wxString &path)
{
	if (path.IsEmpty())
		return true;

	// If the filename contains any of the sensible URL schemes
	// we just assume it's correct if the end also looks right.
	if ((path.Lower().StartsWith(wxT("http://")) ||
	        path.Lower().StartsWith(wxT("https://")) ||
	        path.Lower().StartsWith(wxT("file://")) ||
	        path.Lower().StartsWith(wxT("ftp://"))) &&
	        (path.Lower().EndsWith(wxT(".hhp")) ||
#if defined (__WXMSW__) || wxUSE_LIBMSPACK
	         path.Lower().EndsWith(wxT(".chm")) ||
#endif
	         path.Lower().EndsWith(wxT(".pdf")) ||
	         path.Lower().EndsWith(wxT(".zip")) ||
	         path.Lower().EndsWith(wxT("/"))))
		return true;

	// Otherwise, we're looking for a file (or directory) that
	// actually exists.
	wxString sep = wxFileName::GetPathSeparator();
	if (path.Lower().EndsWith(wxT(".hhp")) ||
#if defined (__WXMSW__) || wxUSE_LIBMSPACK
	        path.Lower().EndsWith(wxT(".chm")) ||
#endif
	        path.Lower().EndsWith(wxT(".pdf")) ||
	        path.Lower().EndsWith(wxT(".zip")))
		return wxFile::Exists(path);
	else if (path.Lower().EndsWith(sep))
		return wxDir::Exists(path);
	else
		return false;
}

void DisplayHelp(const wxString &helpTopic, const HelpType helpType)
{
	static wxHelpControllerBase *pgHelpCtl = 0;
	static wxHelpControllerBase *edbHelpCtl = 0;
	static wxHelpControllerBase *greenplumHelpCtl = 0;
	static wxHelpControllerBase *slonyHelpCtl = 0;
	static wxString pgInitPath = wxEmptyString;
	static wxString edbInitPath = wxEmptyString;
	static wxString gpInitPath = wxEmptyString;
	static wxString slonyInitPath = wxEmptyString;

	switch (helpType)
	{
		case HELP_PGADMIN:
			DisplayPgAdminHelp(helpTopic);
			break;

		case HELP_POSTGRESQL:
			DisplayExternalHelp(helpTopic, settings->GetPgHelpPath(), pgHelpCtl, (pgInitPath != settings->GetPgHelpPath() ? true : false));
			pgInitPath = settings->GetPgHelpPath();
			break;

		case HELP_ENTERPRISEDB:
			DisplayExternalHelp(helpTopic, settings->GetEdbHelpPath(), edbHelpCtl, (edbInitPath != settings->GetEdbHelpPath() ? true : false));
			edbInitPath = settings->GetEdbHelpPath();
			break;

		case HELP_GREENPLUM:
		{
			// the old help path (stored in the settings) is no longer working
			static wxString gpHelpPath = settings->GetGpHelpPath();

			// Note: never end the URL on "index.html"
			// InitHelp() does obscure magic with this ending
			if (gpHelpPath.CmpNoCase(wxT("http://docs.gopivotal.com/gpdb/")) == 0)
			{
				gpHelpPath = wxT("http://gpdb.docs.pivotal.io/");
				// Replace the path to the old domain with the link to
				// the new documentation path
				// The old link is working for now, but there is no guarantee
				// that it will stay this way
				// Also the new link automatically redirects to the latest version
				settings->SetGpHelpPath(gpHelpPath);
			}

			if (gpHelpPath.CmpNoCase(wxT("http://www.greenplum.com/docs/3300/")) == 0)
			{
				gpHelpPath = wxT("http://gpdb.docs.pivotal.io/");
				// this is the old link, update the link to the new documentation link
				// problem: this saves the link into the configuration file
				settings->SetGpHelpPath(gpHelpPath);
			}
			DisplayExternalHelp(helpTopic, settings->GetGpHelpPath(), greenplumHelpCtl, (gpInitPath != settings->GetGpHelpPath() ? true : false));
			gpInitPath = settings->GetGpHelpPath();
		}
		break;

		case HELP_SLONY:
			DisplayExternalHelp(helpTopic, settings->GetSlonyHelpPath(), slonyHelpCtl, (slonyInitPath != settings->GetSlonyHelpPath() ? true : false));
			slonyInitPath = settings->GetSlonyHelpPath();
			break;

		default:
			DisplayPgAdminHelp(helpTopic);
			break;
	}
}

void DisplayPgAdminHelp(const wxString &helpTopic)
{
	static wxHelpControllerBase *helpCtl = 0;
	static bool firstCall = true;

	// Startup the main help system
	if (firstCall)
	{
		firstCall = false;
		wxString helpdir = docPath + wxT("/") + settings->GetCanonicalLanguageName();

		if (!wxFile::Exists(helpdir + wxT("/pgadmin3.hhp")) &&
#if defined(__WXMSW__) || wxUSE_LIBMSPACK
		        !wxFile::Exists(helpdir + wxT("/pgadmin3.chm")) &&
#endif
		        !wxFile::Exists(helpdir + wxT("/pgadmin3.zip")))
			helpdir = docPath + wxT("/en_US");

#ifdef __WXMSW__
#ifndef __WXDEBUG__
		if (wxFile::Exists(helpdir + wxT("/pgadmin3.chm")))
		{
			helpCtl = new wxCHMHelpController();
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
				helpCtl = new wxHtmlHelpController();
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
		wxLaunchDefaultBrowser(page);
	}
}

void DisplayExternalHelp(const wxString &helpTopic, const wxString &docPath, wxHelpControllerBase *helpCtl, const bool init)
{
	// Build the page name
	wxString page;
	int hashPos = helpTopic.Find('#');
	if (hashPos < 0)
		page = helpTopic + wxT(".html");
	else
		page = helpTopic.Left(hashPos) + wxT(".html") + helpTopic.Mid(hashPos);

	// If the docPath ends in .pdf, then open the file in the browser. No
	// bookmarks though :-(
	if (docPath.Lower().EndsWith(wxT(".pdf")))
	{
		wxLaunchDefaultBrowser(docPath);
		return;
	}

	// If the docPath doesn't end in .chm, .zip or .hhp, then we must be using
	// plain HTML files, so just fire off the browser and be done with it.
	if (!docPath.Lower().EndsWith(wxT(".hhp")) &&
#if defined (__WXMSW__) || wxUSE_LIBMSPACK
	        !docPath.Lower().EndsWith(wxT(".chm")) &&
#endif
	        !docPath.Lower().EndsWith(wxT(".zip")))
	{
		wxLaunchDefaultBrowser(docPath + page);
		return;
	}

	// We must be using HTML Help, so init the appropriate help controller
	// Note the path that we init for - if it changes, we need to init a
	// new controller in case it's no longer the same type
	if (init || !helpCtl)
	{
		// Get shot of the old help controller if there is one.
		if (helpCtl)
			delete helpCtl;

#ifdef __WXMSW__
		// For Windows builds we us the MS HTML Help viewer for .chm files
		if (docPath.Lower().EndsWith(wxT(".chm")) && wxFile::Exists(docPath))
		{
			helpCtl = new wxCHMHelpController();
			helpCtl->Initialize(docPath);
		}
		else
#endif
#if wxUSE_LIBMSPACK
			// If we can use a .chm file...
			if ((docPath.Lower().EndsWith(wxT(".chm")) && wxFile::Exists(docPath)) ||
			        (docPath.Lower().EndsWith(wxT(".hhp")) && wxFile::Exists(docPath)) ||
			        (docPath.Lower().EndsWith(wxT(".zip")) && wxFile::Exists(docPath)))
#else
			// Otherwise...
			if ((docPath.Lower().EndsWith(wxT(".hhp")) && wxFile::Exists(docPath)) ||
			        (docPath.Lower().EndsWith(wxT(".zip")) && wxFile::Exists(docPath)))
#endif
			{
				helpCtl = new wxHtmlHelpController();
				helpCtl->Initialize(docPath);
			}
	}

	// Display the page using the help controller
	// If it's foobar'ed, use the browser.
	if (helpCtl)
	{
		if (helpTopic == wxT("index.html"))
			helpCtl->DisplayContents();
		else
			helpCtl->DisplaySection(page);
	}
	else
	{
		wxLogError(_("The help source (\"%s\") could not be opened. Please check the help configuration options."), docPath.c_str());
	}
}

wxString GetHtmlEntity(const wxChar ch)
{
	// REWRITE THIS - IT'S (STILL) BLOODY INEFFICIENT!!

	// Quick bailout
	if ((ch >= 'a' && ch <= 'z') ||
	        (ch >= 'A' && ch <= 'Z') ||
	        (ch >= '0' && ch <= '9'))
		return wxString(ch);

	unsigned short ents[] =
	{
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
		0
	};

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

	for (unsigned int x = 0; x < str.Length(); x++)
	{
		if (str[x] != 13)
			ret += GetHtmlEntity(str[x]);
	}

	return ret;
}


#ifndef WIN32
wxString ExecProcess(const wxString &cmd)
{
	wxString res;
	FILE *f = popen(cmd.ToAscii(), "r");

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

int ExecProcess(const wxString &command, wxArrayString &result)
{
	FILE *fp_command;
	char buf[4098];

	fp_command = popen(command.mb_str(wxConvUTF8), "r");

	if (!fp_command)
		return -1;

	while(!feof(fp_command))
	{
		if (fgets(buf, 4096, fp_command) != NULL)
			result.Add(wxString::FromAscii(buf));
	}

	return pclose(fp_command);
}
#endif

#ifdef WIN32
#if (_MSC_VER < 1300)
/* _ftol2 is more than VC7. */
extern "C" long _ftol( double );
extern "C" long _ftol2( double dblSource )
{
	return _ftol( dblSource );
}
#endif
#endif

wxString firstLineOnly(const wxString &str)
{
	wxString ip, tmp;
	ip = str;
	ip = ip.Trim(true).Trim(false);

	if (ip.Contains(wxT("\r\n")) && (ip.First(wxT("\r")) < ip.First(wxT("\n"))))
	{
		tmp = ip.BeforeFirst('\r');
		if (ip.BeforeFirst('\r').Length() != ip.Length())
			tmp += wxT("...");
	}
	else
	{
		tmp = ip.BeforeFirst('\n');
		if (ip.BeforeFirst('\n').Length() != ip.Length())
			tmp += wxT("...");
	}

	return tmp;
}

bool pgAppMinimumVersion(const wxString &cmd, const int majorVer, const int minorVer)
{
	wxArrayString output;
	bool isEnterpriseDB = false;

#ifdef __WXMSW__
	if (wxExecute(cmd + wxT(" --version"), output, 0) != 0)
#else
	if (ExecProcess(cmd + wxT(" --version"), output) != 0)
#endif
	{
		wxLogError(_("Failed to execute: %s --version"), cmd.c_str());
		return false;
	}

	// Is this an EDB utility?
	if (output[0].Contains(wxT("EnterpriseDB")))
		isEnterpriseDB = true;

	wxString version = output[0].AfterLast(' ');
	long actualMajor = 0, actualMinor = 0;

	wxString tmp = wxT("");
	int x = 0;
	while(version[x] == '0' || version[x] == '1' || version[x] == '2' || version[x] == '3' || version[x] == '4' ||
	        version[x] == '5' || version[x] == '6' || version[x] == '7' || version[x] == '8' || version[x] == '9')
	{
		tmp += version[x];
		x++;
	}
	tmp.ToLong(&actualMajor);
	x++;

	tmp = wxT("");
	while(version[x] == '0' || version[x] == '1' || version[x] == '2' || version[x] == '3' || version[x] == '4' ||
	        version[x] == '5' || version[x] == '6' || version[x] == '7' || version[x] == '8' || version[x] == '9')
	{
		tmp += version[x];
		x++;
	}

	tmp.ToLong(&actualMinor);

	// EnterpriseDB's 8.3R1 utilties are based on PG8.2, so correct the version number here.
	// This will need more work when 8.3R2 is released :-(
	if (isEnterpriseDB && actualMajor == 8 && actualMinor == 3)
		actualMinor = 2;

	if (actualMajor > majorVer)
		return true;

	if (actualMajor == majorVer && actualMinor >= minorVer)
		return true;

	return false;
}

bool isPgApp(const wxString &app)
{
	if (!wxFile::Exists(app))
		return false;

	wxArrayString output;

#ifdef __WXMSW__
	if (wxExecute(app + wxT(" --version"), output, 0) != 0)
#else
	if (ExecProcess(app + wxT(" --version"), output) != 0)
#endif
	{
		wxLogError(_("Failed to execute: %s --version"), app.c_str());
		return false;
	}

	if (output[0].Contains(wxT("PostgreSQL")))
		return true;

	return false;
}

bool isEdbApp(const wxString &app)
{
	if (!wxFile::Exists(app))
		return false;

	wxArrayString output;

#ifdef __WXMSW__
	if (wxExecute(app + wxT(" --version"), output, 0) != 0)
#else
	if (ExecProcess(app + wxT(" --version"), output) != 0)
#endif
	{
		wxLogError(_("Failed to execute: %s --version"), app.c_str());
		return false;
	}

	if (output[0].Contains(wxT("EnterpriseDB")))
		return true;

	return false;
}

bool isGpApp(const wxString &app)
{
	if (!wxFile::Exists(app))
		return false;

	wxArrayString output;

#ifdef __WXMSW__
	if (wxExecute(app + wxT(" --version"), output, 0) != 0)
#else
	if (ExecProcess(app + wxT(" --version"), output) != 0)
#endif
	{
		wxLogError(_("Failed to execute: %s --version"), app.c_str());
		return false;
	}

	if (output[0].Contains(wxT("8.2")))  // Ugly... No way to tell Greenplum app from PostgreSQL 8.2 app
		return true;

	return false;
}

wxString sanitizePath(const wxString &path)
{
	if (path.Length())
	{
		wxFileName fn = path;
		fn.Normalize();
		return fn.GetLongPath();
	}

	return wxEmptyString;
}

/**
 * FUNCTION: commandLineCleanOption
 * INPUTS:
 *       option       - input string needs to be reformatted
 *       schemaObject - Is this an object related to schema?
 * PURPOSE:
 *  - Fixup a (double-quoted) string for use on the command line
 */
wxString commandLineCleanOption(const wxString &option, bool schemaObject)
{
	wxString tmp = option;

	if (schemaObject)
	{
		// Replace double-quote with slash & double-quote
		tmp.Replace(wxT("\""), wxT("\\\""));
	}
	else
	{
		// If required, clean the string to know the real object name
		if (option.StartsWith(wxT("\"")) && option.EndsWith(wxT("\"")))
			tmp = option.AfterFirst((wxChar)'"').BeforeLast((wxChar)'"');

		// Replace single splash to double-splash
		tmp.Replace(wxT("\\"), wxT("\\\\"));

		// Replace double-quote with slash & double-quote
		tmp.Replace(wxT("\""), wxT("\\\""));

		// Replace double (slash & double-quote) combination to single (slash & double-quote) combination
		tmp.Replace(wxT("\\\"\\\""), wxT("\\\""));

		// Add the double quotes
		tmp = wxT("\"") + tmp + wxT("\"");
	}

	return tmp;
}

#endif // PGSCLI

// Get an array from a comma separated list
bool getArrayFromCommaSeparatedList(const wxString &str, wxArrayString &res)
{
	size_t len = str.Len(), index = 0, nBracketLevel = 0, startArray = 0;
	bool inSingleQuote = false, inDoubleQuote = false;

	if (len == 0)
		return true;

	for(; index < len; index++)
	{
		wxChar curr = str.GetChar(index);
		if (!inDoubleQuote && curr == (wxChar)'\'')
			inSingleQuote = !inSingleQuote;
		else if (!inSingleQuote && curr == (wxChar)'"')
			inDoubleQuote = !inDoubleQuote;
		else if (!inDoubleQuote && !inSingleQuote && curr == (wxChar)'(')
			nBracketLevel++;
		else if (!inDoubleQuote && !inSingleQuote && curr == (wxChar)')')
			nBracketLevel--;
		else if (!inDoubleQuote && !inSingleQuote && nBracketLevel == 0 && curr == (wxChar)',')
		{
			if (index != startArray)
				res.Add(str.SubString(startArray, index - 1).Trim(true).Trim(false));
			else
				res.Add(wxEmptyString);
			startArray = index + 1;
		}
	}
	if (inDoubleQuote || inSingleQuote || nBracketLevel != 0)
		return false;

	// Add last value to array
	res.Add(str.SubString(startArray, index).Trim(true).Trim(false));

	return true;
}

