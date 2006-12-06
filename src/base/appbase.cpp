//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// base.cpp - Miscellaneous Basic Utilities
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/socket.h>
#include <wx/xrc/xmlres.h>
#include <wx/stdpaths.h>

#include "copyright.h"
#include "misc.h"
#include "base/appbase.h"
#include "pgConn.h"
#include "base/sysLogger.h"

wxPathList path;                // The search path
wxString loadPath;              // Where the program is loaded from
wxString docPath;               // Where docs are stored
wxString uiPath;                // Where ui data is stored
wxString i18nPath;              // Where i18n data is stored
wxLog *logger;

#define DOC_DIR     wxT("/docs")
#define UI_DIR      wxT("/ui")
#define I18N_DIR    wxT("/i18n")

#define HELPER_DIR  wxT("/helper")


void pgAppBase::InitPaths()
{
    loadPath=wxPathOnly(argv[0]);
	if (loadPath.IsEmpty())
		loadPath = wxT(".");

    // Look in the app directory for things first
    path.Add(loadPath);

#if defined(__WXMSW__)

    // Search for the right paths. We check the following locations:
    //
    // 1) ./xxx               - Running as a standalone install
    // 2) ../pgAdmin/xxx      - Running in a pgInstaller 8.1 installation 
    //                          (with the .exe and dlls in the main bin dir)
    // 3) ../../xxx or ../xxx - Running in a development environment
    
    if (wxDir::Exists(loadPath + I18N_DIR))
        i18nPath = loadPath + I18N_DIR;
    else if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") + I18N_DIR))
        i18nPath = loadPath + wxT("/../pgAdmin III") + I18N_DIR;
    else 
        i18nPath = loadPath + wxT("/../..") + I18N_DIR;

    if (wxDir::Exists(loadPath + DOC_DIR))
        docPath = loadPath + DOC_DIR;
    else if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") DOC_DIR))
        docPath = loadPath + wxT("/../pgAdmin III") DOC_DIR;
    else
        docPath = loadPath + wxT("/../..") DOC_DIR;

    if (wxDir::Exists(loadPath + UI_DIR))
        uiPath = loadPath + UI_DIR;
    if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") + UI_DIR))
        uiPath = loadPath + wxT("/../pgAdmin III") + UI_DIR;
    else
        uiPath = loadPath + wxT("/..") UI_DIR;

    // Look for a path 'hint' on Windows. This registry setting may
    // be set by the Win32 PostgreSQL installer which will generally
    // install pg_dump et al. in the PostgreSQL bindir rather than
    // the pgAdmin directory.
    wxRegKey hintKey(wxT("HKEY_LOCAL_MACHINE\\Software\\") APPNAME_L);
    if (hintKey.HasValue(wxT("Helper Path")))
    {
        wxString hintPath;
        hintKey.QueryValue(wxT("Helper Path"), hintPath);
        path.Add(hintPath);
    }
    
#else
    wxString dataDir;

#if defined(__WXMAC__)

    //When using wxStandardPaths on OSX, wx defaults to the unix,
    //not to the mac variants. Therefor, we request wxStandardPathsCF
    //directly.
    wxStandardPathsCF stdPaths ;
    dataDir = stdPaths.GetDataDir() ;

    if (wxDir::Exists(dataDir + HELPER_DIR))
        path.Add(dataDir + HELPER_DIR) ;

#else // other *ixes

// Data path (defined by configure under Unix).
#ifndef DATA_DIR
#define DATA_DIR "./"
#endif

    dataDir = wxString::FromAscii(DATA_DIR);
#endif


    if (dataDir)
    {
        if (wxDir::Exists(dataDir + I18N_DIR))
            i18nPath = dataDir + I18N_DIR;
        
        if (wxDir::Exists(dataDir + UI_DIR))
            uiPath = dataDir + UI_DIR;

        if (wxDir::Exists(dataDir + DOC_DIR))
            docPath = dataDir + DOC_DIR ;
    }

    if (i18nPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + I18N_DIR))
            i18nPath = loadPath + I18N_DIR;
        else
            i18nPath = loadPath + wxT("/..") I18N_DIR;
    }
    if (uiPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + UI_DIR))
            uiPath = loadPath + UI_DIR;
        else 
            uiPath = loadPath + wxT("/..") UI_DIR;
    }

    if (docPath.IsEmpty())
    {
        if (wxDir::Exists(loadPath + DOC_DIR))
            docPath = loadPath + DOC_DIR ;
        else
            docPath = loadPath + wxT("/..") DOC_DIR ;
    }
#endif

    path.AddEnvList(wxT("PATH"));
}


void pgAppBase::InitXml()
{
#define chkXRC(id) XRCID(#id) == id
    wxASSERT_MSG(
        chkXRC(wxID_OK) &&
        chkXRC(wxID_CANCEL) && 
        chkXRC(wxID_HELP) &&
        chkXRC(wxID_APPLY) &&
        chkXRC(wxID_ADD) &&
        chkXRC(wxID_STOP) &&
        chkXRC(wxID_REMOVE)&&
        chkXRC(wxID_REFRESH) &&
        chkXRC(wxID_CLOSE), 
        wxT("XRC ID not correctly assigned."));
    // if this assert fires, some event table uses XRCID(...) instead of wxID_... directly
        


#ifdef EMBED_XRC

    // resources are loaded from memory
    extern void InitXmlResource();
    InitXmlResource();

#else

    // for debugging, dialog resources are read from file
    wxXmlResource::Get()->Load(uiPath + wxT("/*.xrc"));
#endif

}


void pgAppBase::InitLogger()
{
    logger = new sysLogger();
    wxLog::SetActiveTarget(logger);
}


void pgAppBase::InitNetwork()
{
    // Startup the windows sockets if required
#ifdef __WXMSW__
    WSADATA    wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        wxLogFatalError(__("Cannot initialise the networking subsystem!"));   
    }
#endif
    wxSocketBase::Initialize();

    pgConn::ExamineLibpqVersion();
}


int pgAppBase::OnExit()
{
#ifdef __WXMSW__
    WSACleanup();
#endif

    return 1;
}
