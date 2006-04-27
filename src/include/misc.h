//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: misc.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef MISC_H
#define MISC_H

#include "base/base.h"


// compile ID and Name into one string
wxString IdAndName(long id, const wxString &name);

// Quoting
wxString qtIdent(const wxString& value);    // add " if necessary
wxString qtTypeIdent(const wxString& value);    // add " if necessary
wxString qtStringDollar(const wxString &value);
wxString qtStrip(const wxString& value);    // remove \"


// string build helper
void AppendIfFilled(wxString &str, const wxString &delimiter, const wxString &what);

// Create keyword list from PostgreSQL list
void FillKeywords(wxString &str);

// Fill array, splitting the string separated by commas (maybe quoted elements)
void FillArray(wxArrayString &array, const wxString &str);


// splitting of strings, obeying quotes
class queryTokenizer : public wxStringTokenizer
{
public:
    queryTokenizer(const wxString& str, const wxChar delim=(wxChar)' ');
    wxString GetNextToken();
private:
    char delimiter;
};


// File handling including encoding according to sysSettings if format<0,
// 0-> local charset, 1->utf8
wxString FileRead(const wxString &filename, int format=-1);
bool FileWrite(const wxString &filename, const wxString &data, int format=-1);

void DisplayHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);
void DisplaySqlHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);

#ifndef WIN32
wxString ExecProcess(const wxString &cmd);
#endif

wxString GetHtmlEntity(const wxChar ch);
wxString HtmlEntities(const wxString &str);

enum 
{
    PGM_SERVER,
    PGM_TABLESPACE,
    PGM_DATABASE,
    PGM_SCHEMA,
    PGM_UNKNOWN,
    PGM_FUNCTION,
    PGM_TABLE,
    PGM_VIEW,
    PGM_SEQUENCE,
    PGM_CONSTRAINT,
    PGM_INDEX,
    PGM_UNIQUE,
    PGM_PRIMARYKEY,
    PGM_FOREIGNKEY,
    PGM_CHECK,
    PGM_COLUMN,

    PGM_STEP,
    PGM_SCHEDULE,

    SLM_LISTEN,
    SLM_NODE,
    SLM_PATH,
    SLM_SEQUENCE,
    SLM_SET,
    SLM_SUBSCRIPTION,
    SLM_TABLE,
};


enum        // depends on pgaFactory::addImage order!
{
    PGICON_PROPERTY,
    PGICON_STATISTICS,
    PGICON_PUBLIC,
};



#endif
