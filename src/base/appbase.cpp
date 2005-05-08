//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: misc.cpp 4054 2005-03-28 16:43:01Z andreas $
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// base.cpp - Miscellaneous Basic Utilities
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/socket.h>
#include <wx/xrc/xmlres.h>
#include <wx/stdpaths.h>

#include "base/base.h"
#include "base/appbase.h"
#include "base/pgConnBase.h"
#include "base/sysLogger.h"

wxString loadPath;              // Where the program is loaded from
wxString docPath;               // Where docs are stored
wxString uiPath;                // Where ui data is stored
wxString i18nPath;              // Where i18n data is stored
wxLog *logger;

#define DOC_DIR     wxT("/docs")
#define UI_DIR      wxT("/ui")
#define COMMON_DIR  wxT("/common")
#define I18N_DIR    wxT("/il8n")

#define SCRIPT_DIR  wxT("/scripts")
#define HELPER_DIR  wxT("/helper")


void pgAppBase::InitPaths()
{
    loadPath=wxPathOnly(argv[0]);
	if (loadPath.IsEmpty())
		loadPath = wxT(".");

#if defined(__WXMSW__)

    if (wxDir::Exists(loadPath + I18N_DIR))
        i18nPath = loadPath + I18N_DIR;
    else if (wxDir::Exists(loadPath + wxT("/../..") + I18N_DIR))
        i18nPath = loadPath + wxT("/../..") + I18N_DIR;

    if (wxDir::Exists(loadPath + UI_DIR))
        uiPath = loadPath + UI_DIR;
    else
        uiPath = loadPath + wxT("/..") UI_DIR;

    if (wxDir::Exists(loadPath + DOC_DIR))
        docPath = loadPath + DOC_DIR;
    else
        docPath = loadPath + wxT("/../..") DOC_DIR;
    
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
    if (wxDir::Exists(dataDir + SCRIPT_DIR))
        path.Add(dataDir + SCRIPT_DIR) ;

#else // other *ixes
    dataDir = DATA_DIR;
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
		i18nPath = loadPath + I18N_DIR;

    if (uiPath.IsEmpty())
        uiPath = loadPath + UI_DIR))

    if (docPath.IsEmpty())
        docPath = loadPath + wxT("/..") DOC_DIR ;
#endif
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
    wxXmlResource::Get()->Load(uiPath+COMMON_DIR + wxT("/*.xrc"));
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
    WSADATA	wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        wxLogFatalError(__("Cannot initialise the networking subsystem!"));   
    }
#endif
    wxSocketBase::Initialize();

    pgConnBase::ExamineLibpqVersion();
}


int pgAppBase::OnExit()
{
#ifdef __WXMSW__
	WSACleanup();
#endif

    return 1;
}
