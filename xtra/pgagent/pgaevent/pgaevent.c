//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaevent.c - win32 message format dll
//
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <olectl.h>
#include <string.h>

HANDLE		g_module = NULL;

BOOL WINAPI DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

//
// DllMain --- is an optional entry point into a DLL.
//
BOOL WINAPI
DllMain(HANDLE hModule,	DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		g_module = hModule;
	return TRUE;
}
