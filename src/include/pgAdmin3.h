//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgAdmin3.h - The main application header
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGADMIN3_H
#define PGADMIN3_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "sysSettings.h"

// Application Versions & Name
#define VERSION wxT("1.0.0 Devel")
#define APPNAME_L wxT("pgAdmin III")
#define APPNAME_S wxT("pgadmin3")
#define SERVER_MIN_VERSION 7.3

// XRC Path
#ifdef __WXMSW__
  #define XRC_PATH "ui/win32"
#else
  #define XRC_PATH "ui/gtk"
#endif

// Class declarations
class pgAdmin3 : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

private:
    void LoadXrc(const wxString szFile);
};

// Global Stuff
void StartMsg(const wxString& szMsg);
void EndMsg();
int wxCALLBACK ListSort(long itm1, long itm2, long sortData);

// Conversions
wxString StrToYesNo(const wxString& szVal);
wxString BoolToYesNo(bool bVal);
bool StrToBool(const wxString& szVal);
wxString NumToStr(long nVal);
long StrToLong(const wxString& szVal);
wxString NumToStr(double nVal);
double StrToDouble(const wxString& szVal);

#endif
