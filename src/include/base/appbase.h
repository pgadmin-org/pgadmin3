//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// appbase.h - Miscellaneous Basic Utilties
//
//////////////////////////////////////////////////////////////////////////

extern wxPathList path;                  // The search path
extern wxString loadPath;              // Where the program is loaded from
extern wxString docPath;               // Where docs are stored
extern wxString uiPath;                // Where ui data is stored
extern wxString i18nPath;              // Where i18n data is stored

class pgAppBase : public wxApp
{
protected:
    void InitPaths();
    void InitLogger();
    void InitNetwork();
    void InitXml();
    int OnExit();
};
