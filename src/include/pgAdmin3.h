//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgAdmin3.h 4875 2006-01-06 21:06:46Z dpage $
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

#include "misc.h"
#include <ctl/ctlTree.h>
#include "ctl/ctlSQLBox.h"
#include "ctl/ctlListView.h"
#include "ctl/ctlComboBox.h"
#include "dlgClasses.h"
#include "pgConn.h"
#include "pgSet.h"
#include "base/factory.h"

#include "precomp.h"

// App headers
#include "sysSettings.h"

#ifdef __WXMSW__
#else
#include "config.h"
#undef VERSION
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

