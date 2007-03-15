//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: factory.h 5884 2007-01-23 12:22:27Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// debugger.h - Debugger factories
//
//////////////////////////////////////////////////////////////////////////

#ifndef DEBUGGER_H
#define DEBUGGER_H

// wxWindows headers
#include <wx/wx.h>

///////////////////////////////////////////////////
// Debugger factory
///////////////////////////////////////////////////
class debuggerFactory : public actionFactory
{
public:
    debuggerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

///////////////////////////////////////////////////
// Breakpoint factory
///////////////////////////////////////////////////
class breakpointFactory : public actionFactory
{
public:
    breakpointFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif