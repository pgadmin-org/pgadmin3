//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef MISC_H
#define MISC_H

// wxWindows headers
#include <wx/wx.h>

// Global Stuff
void StartMsg(const wxString& msg);
void EndMsg();

// Conversions
wxString StrToYesNo(const wxString& value);
wxString BoolToYesNo(bool value);
bool StrToBool(const wxString& value);
wxString NumToStr(long value);
long StrToLong(const wxString& value);
wxString NumToStr(double value);
double StrToDouble(const wxString& value);

// Quoting
wxString qtString(const wxString& value);
wxString qtIdent(const wxString& value);

#endif
