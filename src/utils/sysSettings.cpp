//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// sysSettings.cpp - Settings handling class
//
// Note: This class stores and manages all the applications settings.
//       Settings are all read in the ctor, but may be written either in
//       the relevant SetXXX() member function for rarely written settings
//       or in the dtor for reguarly changed settings such as form sizes.
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"


// wxWindows headers
#include <wx/wx.h>
#include <wx/config.h>
#include <wx/url.h>

// App headers
#include "sysSettings.h"
#include "sysLogger.h"
#include "misc.h"

extern wxString docPath;



sysSettings::sysSettings(const wxString& name) : wxConfig(name)
{
    // Tip Of The Day
    Read(wxT("ShowTipOfTheDay"), &showTipOfTheDay, TRUE); 
    Read(wxT("NextTipOfTheDay"), &nextTipOfTheDay, 0); 

    // Log
    Read(wxT("LogFile"), &logFile, wxT("pgadmin.log")); 
    Read(wxT("LogLevel"), &logLevel, LOG_ERRORS);

    // Last Connection
    Read(wxT("LastServer"), &lastServer, wxT("localhost")); 
    Read(wxT("LastDatabase"), &lastDatabase, wxT("template1")); 
	Read(wxT("LastDescription"), &lastDescription, wxT("PostgreSQL Server")); 
    Read(wxT("LastUsername"), &lastUsername, wxT("postgres")); 
    Read(wxT("LastPort"), &lastPort, 5432);
    Read(wxT("LastSSL"), &lastSSL, 0);

    // Show System Objects
    Read(wxT("ShowSystemObjects"), &showSystemObjects, FALSE); 

//    Read(wxT("SqlHelpSite"), &sqlHelpSite, docPath + wxT("/en_US/pg/"));
    Read(wxT("SqlHelpSite"), &sqlHelpSite, wxT(""));
    if (sqlHelpSite.length() > 0) {
        if (sqlHelpSite.Last() != '/' && sqlHelpSite.Last() != '\\')
            sqlHelpSite += wxT("/");
    }
    Read(wxT("Proxy"), &proxy, wxGetenv(wxT("HTTP_PROXY")));
    SetProxy(proxy);

    maxRows=Read(wxT("frmQuery/MaxRows"), 100L);
    maxColSize=Read(wxT("frmQuery/MaxColSize"), 256L);
    Read(wxT("frmQuery/ExplainVerbose"), &explainVerbose, false);
    Read(wxT("frmQuery/ExplainAnalyze"), &explainAnalyze, false);
    askSaveConfirmation=StrToBool(Read(wxT("AskSaveConfirmation"), wxT("Yes")));
    confirmDelete=StrToBool(Read(wxT("ConfirmDelete"), wxT("Yes")));
    showUsersForPrivileges=StrToBool(Read(wxT("ShowUsersForPrivileges"), wxT("No")));
    autoRowCountThreshold=Read(wxT("AutoRowCount"), 2000);
    Read(wxT("StickySql"), &stickySql, true);
    Read(wxT("DoubleClickProperties"), &doubleClickProperties, true);
    Read(wxT("WriteUnicodeFile"), &unicodeFile, false);
    Read(wxT("SearchPath"), &searchPath, wxEmptyString);

    const wxLanguageInfo *langInfo;
    langInfo = wxLocale::GetLanguageInfo(Read(wxT("LanguageId"), wxLANGUAGE_UNKNOWN));
    if (langInfo)
        canonicalLanguage=langInfo->CanonicalName;

	wxString fontName;
    Read(wxT("Font"), &fontName, wxEmptyString);

    if (fontName.IsEmpty())
        systemFont = wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT);
    else
        systemFont = wxFont(fontName);

    Read(wxT("frmQuery/Font"), &fontName, wxEmptyString);

    if (fontName.IsEmpty())
    {
#ifdef __WXMSW__
        sqlFont = wxFont(9, wxTELETYPE, wxNORMAL, wxNORMAL);
#else
        sqlFont = wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
#endif
    }
    else
    	sqlFont = wxFont(fontName);
}


sysSettings::~sysSettings()
{
    wxLogInfo(wxT("Destroying sysSettings object and saving settings"));
    // frMain size/position
	Save();
}

void sysSettings::Save()
{
    Write(wxT("LogFile"), logFile);
    Write(wxT("LogLevel"), logLevel);

    Write(wxT("frmQuery/MaxRows"), maxRows);
    Write(wxT("frmQuery/MaxColSize"), maxColSize);
    Write(wxT("frmQuery/ExplainVerbose"), explainVerbose);
    Write(wxT("frmQuery/ExplainAnalyze"), explainAnalyze);
	Write(wxT("frmQuery/Font"), sqlFont.GetNativeFontInfoDesc());
    Write(wxT("AskSaveConfirmation"), BoolToStr(askSaveConfirmation));
    Write(wxT("ConfirmDelete"), BoolToStr(confirmDelete));
    Write(wxT("ShowUsersForPrivileges"), BoolToStr(showUsersForPrivileges));
    Write(wxT("SqlHelpSite"), sqlHelpSite);
    Write(wxT("Proxy"), proxy);
    Write(wxT("AutoRowCount"), autoRowCountThreshold);
    Write(wxT("WriteUnicodeFile"), unicodeFile);
    Write(wxT("SearchPath"), searchPath);
	wxString fontName = systemFont.GetNativeFontInfoDesc();

	if (fontName == wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT).GetNativeFontInfoDesc())
        Write(wxT("Font"), wxEmptyString);
    else
        Write(wxT("Font"), fontName);
}


void sysSettings::SetProxy(const wxString &s)
{
    proxy=s;
    if (!s.IsEmpty() && s.Find(':') < 0)
        proxy += wxT(":80");
    wxURL::SetDefaultProxy(proxy);
}


bool sysSettings::Write(const wxString &key, const wxPoint &value)
{
    bool rc=wxConfig::Write(key + wxT("/Left"), value.x);
    if (rc)
        rc=wxConfig::Write(key + wxT("/Top"), value.y);
    return rc;
}


bool sysSettings::Write(const wxString &key, const wxSize &value)
{
    bool rc=wxConfig::Write(key + wxT("/Width"), value.x);
    if (rc)
        rc=wxConfig::Write(key + wxT("/Height"), value.y);
    return rc;
}


wxPoint sysSettings::Read(const wxString& key, const wxPoint &defaultVal) const
{
    return wxPoint(wxConfig::Read(key + wxT("/Left"), defaultVal.x), 
                   wxConfig::Read(key + wxT("/Top"), defaultVal.y));
}


wxSize sysSettings::Read(const wxString& key, const wxSize &defaultVal) const
{
    return wxSize(wxConfig::Read(key + wxT("/Width"), defaultVal.x), 
                  wxConfig::Read(key + wxT("/Height"), defaultVal.y));
}

//////////////////////////////////////////////////////////////////////////
// Tip of the Day
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetShowTipOfTheDay(const bool newval)
{
    showTipOfTheDay = newval;
    Write(wxT("ShowTipOfTheDay"), showTipOfTheDay);
}

void sysSettings::SetNextTipOfTheDay(const int newval)
{
    nextTipOfTheDay = newval;
    Write(wxT("NextTipOfTheDay"), nextTipOfTheDay);
}

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetLogFile(const wxString& newval)
{
    logFile = newval;
}

void sysSettings::SetLogLevel(const int newval)
{
    logLevel = newval;
}

//////////////////////////////////////////////////////////////////////////
// Last Connection
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetLastServer(const wxString& newval)
{
    lastServer = newval;
    Write(wxT("LastServer"), lastServer);
}

void sysSettings::SetLastDescription(const wxString& newval)
{
    lastDescription = newval;
    Write(wxT("LastDescription"), lastDescription);
}

void sysSettings::SetLastDatabase(const wxString& newval)
{
    lastDatabase = newval;
    Write(wxT("LastDatabase"), lastDatabase);
}

void sysSettings::SetLastUsername(const wxString& newval)
{
    lastUsername = newval;
    Write(wxT("LastUsername"), lastUsername);
}

void sysSettings::SetLastPort(const int newval)
{
    lastPort = newval;
    Write(wxT("LastPort"), lastPort);
}

void sysSettings::SetLastSSL(const int newval)
{
    lastSSL = newval;
    Write(wxT("LastSSL"), lastSSL);
}

//////////////////////////////////////////////////////////////////////////
// Show System Objects
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetShowSystemObjects(const bool newval)
{
    showSystemObjects = newval;
    Write(wxT("ShowSystemObjects"), showSystemObjects);
}


//////////////////////////////////////////////////////////////////////////
// Sticky SQL
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetStickySql(const bool newval)
{
    stickySql = newval;
    Write(wxT("StickySql"), stickySql);
}

//////////////////////////////////////////////////////////////////////////
// Double click for properties
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetDoubleClickProperties(const bool newval)
{
    doubleClickProperties = newval;
    Write(wxT("DoubleClickProperties"), doubleClickProperties);
}

//////////////////////////////////////////////////////////////////////////
// The size of the query builder
//////////////////////////////////////////////////////////////////////////

wxSize sysSettings::GetFrmQueryBuilderSize()
{
    int width = Read(wxT("frmQueryBuilder/Width"), 750);
    int height = Read(wxT("frmQueryBuilder/Height"), 550);

	return wxSize(width, height);
}

void sysSettings::SetFrmQueryBuilderSize(wxSize size)
{
    Write(wxT("frmQueryBuilder/Width"), size.GetWidth());
    Write(wxT("frmQueryBuilder/Height"), size.GetHeight());
}

//////////////////////////////////////////////////////////////////////////
// The position of the query builder
//////////////////////////////////////////////////////////////////////////

wxPoint sysSettings::GetFrmQueryBuilderPos()
{
    int top = Read(wxT("frmQueryBuilder/Top"), 50);
    int left = Read(wxT("frmQueryBuilder/Left"), 50);

	return wxPoint(top, left);
}

void sysSettings::SetFrmQueryBuilderPos(wxPoint pos)
{
    Write(wxT("frmQueryBuilder/Top"), pos.x);
    Write(wxT("frmQueryBuilder/Left"), pos.y);
}
