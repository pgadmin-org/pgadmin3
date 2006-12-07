//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAdmin3.h - The main application header
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGADMIN3_H
#define PGADMIN3_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/xrc/xmlres.h>

#include "utils/misc.h"
#include <ctl/ctlTree.h>
#include "ctl/ctlSQLBox.h"
#include "ctl/ctlListView.h"
#include "ctl/ctlComboBox.h"
#include "dlg/dlgClasses.h"
#include "db/pgConn.h"
#include "db/pgSet.h"
#include "utils/factory.h"

#include "precomp.h"

// App headers
#include "utils/sysSettings.h"

#ifdef __WXMSW__
#else
#include "config.h"
#undef VERSION
#endif

// Check the wxWidgets config
#if !wxCHECK_VERSION(2, 8, 0)
#error wxWidgets 2.8.0 or higher is required to compile this version of pgAdmin.
#endif

#if !wxUSE_UNICODE
#error wxWidgets must be compiled with Unicode support to build pgAdmin.
#endif

const float SERVER_MIN_VERSION = 7.3f;

// Some redefines for modern Microsoft compilers
#if defined(_MSC_VER) && (_MSC_VER >= 1400) 
#define creat _creat
#define close _close
#define mkdir _mkdir
#define sprintf _sprintf
#define snprintf _snprintf
#define strcat _strcat
#define strdup _strdup
#define stricmp _stricmp
#define strincmp _strincmp
#endif

extern wxPathList path;                  // The search path
extern wxString loadPath;              // Where the program is loaded from
extern wxString docPath;               // Where docs are stored
extern wxString uiPath;                // Where ui data is stored
extern wxString i18nPath;              // Where i18n data is stored

// Class declarations
class pgAdmin3 : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

private:
    bool LoadAllXrc(const wxString dir);

protected:
    void InitPaths();
    void InitLogger();
    void InitNetwork();
    void InitXml();

};

class pgAppearanceFactory
{
public:
    pgAppearanceFactory();

    void SetIcons(wxDialog *dlg);
    void SetIcons(wxTopLevelWindow *dlg);
    char **GetSmallIconImage();
    char **GetBigIconImage();
    char **GetSplashImage();
    wxFont GetSplashTextFont();
    wxColour GetSplashTextColour();
    int GetSplashTextOffset();
    wxPoint GetSplashTextPos();
};

extern pgAppearanceFactory *appearanceFactory;


#endif // PGADMIN3_H

