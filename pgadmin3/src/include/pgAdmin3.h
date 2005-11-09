//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#include "ctlListView.h"
#include "ctlComboBox.h"
#include "dlgClasses.h"

// App headers
#include "sysSettings.h"

#define __(str) wxT(str)


// XRC Path
#ifdef __WXMSW__
#define XRC_PATH wxT("/ui/win32")
#else
#include "config.h"
#undef VERSION
#define XRC_PATH wxT("/ui/gtk")
#endif

// Data path (defined by configure under Unix).
#ifndef DATA_DIR
#define DATA_DIR wxT("./")
#endif

const float SERVER_MIN_VERSION = 7.3f;

#ifdef __WIN32__
#define SPLASH_FONTSIZE 8
#else
#if wxCHECK_VERSION(2,5,0)
#define SPLASH_FONTSIZE 9
#else
#define SPLASH_FONTSIZE 11
#endif
#endif

#define SPLASH_X0       6
#define SPLASH_Y0       114
#define SPLASH_OFFS     15


// Class declarations
class pgAdmin3 : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

private:

    bool LoadAllXrc(const wxString dir);
};


#endif // PGADMIN3_H
