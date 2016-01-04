//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
	debuggerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

///////////////////////////////////////////////////
// Breakpoint factory
///////////////////////////////////////////////////
class breakpointFactory : public actionFactory
{
public:
	breakpointFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif


