//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// win32.cpp - pgAgent win32 specific functions
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"

#ifndef WIN32
#error this file is for win32 only!
#endif

#include <wx/filename.h>
#include <wx/config.h>
#include <process.h>

// for debugging purposes, we can start the service paused

#define START_SUSPENDED 0


static SERVICE_STATUS serviceStatus;
static SERVICE_STATUS_HANDLE serviceStatusHandle;
static wxString serviceName;
static wxString user=wxT(".\\Administrator"), password;
static HANDLE threadHandle=0;


static bool serviceIsRunning;
static HANDLE serviceSync;
static HANDLE eventHandle;


// This will be called periodically to check if the service is to be paused.
void CheckForInterrupt()
{
    serviceIsRunning = false;
    long prevCount;
    ReleaseSemaphore(serviceSync, 1, &prevCount);

    // if prevCount is zero, the service should be paused.
    // We're waiting for the semaphore to get signaled again.
    if (!prevCount)
        WaitForSingleObject(serviceSync, INFINITE);
    serviceIsRunning = true;
}

void LogMessage(wxString msg, int level)
{
    if (eventHandle)
    {
        char *tmp;
        tmp = (char *)malloc(msg.length()+1);
        sprintf(tmp, msg.mb_str(wxConvUTF8));

        switch (level)
        {
            case LOG_DEBUG:
                if (minLogLevel >= LOG_DEBUG)
                    ReportEvent(eventHandle, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (const unsigned short **)&tmp, NULL);
                break;
            case LOG_WARNING:
                if (minLogLevel >= LOG_WARNING)
                    ReportEvent(eventHandle, EVENTLOG_WARNING_TYPE, 0, 0, NULL, 1, 0, (const unsigned short **)&tmp, NULL);
                break;
            case LOG_ERROR:
                ReportEvent(eventHandle, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, (const unsigned short **)&tmp, NULL);
                exit(1);
                break;
        }
    }
    else
    {
        switch (level)
        {
            case LOG_DEBUG:
                if (minLogLevel >= LOG_DEBUG)
                    wxPrintf(_("DEBUG: %s\n"), msg);
                break;
            case LOG_WARNING:
                if (minLogLevel >= LOG_WARNING)
                    wxPrintf(_("WARNING: %s\n"), msg);
                break;
            case LOG_ERROR:
                wxPrintf(_("ERROR: %s\n"), msg);
                exit(1);
                break;
        }
    }
}

// The main working thread of the service

unsigned int __stdcall threadProcedure(void *unused)
{
    MainLoop();

    return 0;
}



////////////////////////////////////////////////////////////
// service control functions
bool pauseService()
{
    WaitForSingleObject(serviceSync, shortWait*1000 -30);

    if (!serviceIsRunning)
    {
        SuspendThread(threadHandle);
        return true;
    }
    return false;
}


bool continueService()
{
    ReleaseSemaphore(serviceSync, 1, 0);
    ResumeThread(threadHandle);
    return true;
}

bool stopService()
{
    pauseService();
    CloseHandle (threadHandle);
    return true;
}

bool initService()
{
    serviceSync = CreateSemaphore(0, 1, 1, 0);

    unsigned int tid;
#if START_SUSPENDED
    threadHandle = (HANDLE)_beginthreadex(0, 0, threadProcedure, 0, CREATE_SUSPENDED, &tid);
#else
    threadHandle = (HANDLE)_beginthreadex(0, 0, threadProcedure, 0, 0, &tid);
#endif
    return (threadHandle != 0);
}


void CALLBACK serviceHandler(DWORD ctl)
{
    switch (ctl)
    {
        case SERVICE_CONTROL_STOP:
        {
            serviceStatus.dwCheckPoint++;
            serviceStatus.dwCurrentState=SERVICE_STOP_PENDING;
            SetServiceStatus(serviceStatusHandle, &serviceStatus);

            stopService();

            serviceStatus.dwCheckPoint=0;
            serviceStatus.dwCurrentState=SERVICE_STOPPED;
            SetServiceStatus(serviceStatusHandle, &serviceStatus);
            break;
        }
        case SERVICE_CONTROL_PAUSE:
        {
            pauseService();

            serviceStatus.dwCurrentState=SERVICE_PAUSED;
            SetServiceStatus(serviceStatusHandle, &serviceStatus);

            break;
        }
        case SERVICE_CONTROL_CONTINUE:
        {
            continueService();
            serviceStatus.dwCurrentState=SERVICE_RUNNING;
            SetServiceStatus(serviceStatusHandle, &serviceStatus);
            break;
        }
        default:
        {
            break;
        }
   }
}


void CALLBACK serviceMain(DWORD argc, LPTSTR *argv)
{
    serviceName.Printf(wxT("%s"), (const char *)argv[0]);
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 15000;
    serviceStatusHandle = RegisterServiceCtrlHandler(serviceName.c_str(), serviceHandler);
    if (serviceStatusHandle)
    {
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        if (initService())
        {
#if START_SUSPENDED
            serviceStatus.dwCurrentState = SERVICE_PAUSED;
#else
            serviceStatus.dwCurrentState = SERVICE_RUNNING;
#endif
            serviceStatus.dwWaitHint = shortWait*1000;
        }
        else
            serviceStatus.dwCurrentState = SERVICE_STOPPED;

        SetServiceStatus(serviceStatusHandle, &serviceStatus);


    }
}




////////////////////////////////////////////////////////////
// installation and removal
bool installService(const wxString &serviceName, const wxString &executable,  const wxString &args, const wxString &displayname, const wxString &user, const wxString &password)
{
    DWORD dwData;
    bool done=false;

    SC_HANDLE manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (manager)
    {
        wxString cmd = executable + wxT(" ") + args;

        wxString quser;
        if (!user.Contains(wxT("\\")))
            quser = wxT(".\\") + user;
        else
            quser = user;

        SC_HANDLE service = CreateService(manager, serviceName.c_str(), displayname.c_str(), SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
            cmd.c_str(), 0, 0, 0, quser.c_str(), password.c_str());

        if (service)
        {
            done = true;
            CloseServiceHandle(service);
        }
        else
        {
            LPVOID lpMsgBuf;
            DWORD dw = GetLastError(); 

            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL
            );
            wxString error;
            error.Printf(wxT("%s"), lpMsgBuf);
            LogMessage(error, LOG_ERROR);
        }

        CloseServiceHandle(manager);
    }

    // Setup the event message DLL
    wxRegKey *msgKey = new wxRegKey(wxT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\") + serviceName);
    if(!msgKey->Exists())
    {
        if (!msgKey->Create())
            LogMessage(_("Could not open the message source registry key."), LOG_WARNING);
    }

    wxString path = executable.BeforeLast('\\') + wxT("\\pgaevent.dll");

    if (!msgKey->SetValue(wxT("EventMessageFile"), path))
        LogMessage(_("Could not set the event message file registry value."), LOG_WARNING);

    dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;

    if (!msgKey->SetValue(wxT("TypesSupported"), dwData))
        LogMessage(_("Could not set the supported types."), LOG_WARNING);;

    return done;
}


bool removeService(const wxString &serviceName)
{
    bool done=false;

    SC_HANDLE manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (manager)
    {
        SC_HANDLE service = OpenService(manager, serviceName.c_str(), SERVICE_ALL_ACCESS);
        if (service)
        {
            SERVICE_STATUS serviceStatus;
            ControlService(service, SERVICE_CONTROL_STOP, &serviceStatus);

            int retries;
            for (retries = 0 ; retries < 5 ; retries++)
            {
                if (QueryServiceStatus(service, &serviceStatus))
                {
                    if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
                    {
                        DeleteService(service);
                        done = true;
                        break;
                    }
                    Sleep(1000L);
                }
            }
            CloseServiceHandle(service);
        }
        CloseServiceHandle(manager);
    }

    // Remove the event message DLL
    wxRegKey *msgKey = new wxRegKey(wxT("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\") + serviceName);
    msgKey->DeleteSelf();

    return done;
}



void usage(const wxString &executable)
{
    wxFileName *fn = new wxFileName(executable);

    wxPrintf(_("Usage:\n"));
    wxPrintf(fn->GetName() + _(" REMOVE <serviceName>\n"));
    wxPrintf(fn->GetName() + _(" INSTALL <serviceName> [options] <connect-string>\n"));
    wxPrintf(_("options:\n"));
    wxPrintf(_("-u <user>\n"));
    wxPrintf(_("-p <password>\n"));
    wxPrintf(_("-d <displayname>\n"));
    wxPrintf(_("-t <poll time interval in seconds (default 10)>\n"));
    wxPrintf(_("-r <retry period after connection abort in seconds (>=10, default 30)>\n"));
    wxPrintf(_("-l <logging verbosity (ERROR=0, WARNING=1, DEBUG=2, default 0)>\n"));
}



////////////////////////////////////////////////////////////

void setupForRun(int argc, char **argv, bool debug=false)
{
    if (!debug)
    {
        eventHandle = RegisterEventSource(0, serviceName.c_str());
        if (!eventHandle)
            LogMessage(_("Couldn't register event handle."), LOG_ERROR);
    }

    setOptions(argc, argv);
}


void main(int argc, char **argv)
{
    // Statup wx
    wxInitialize();

    wxFileName file = wxString::FromAscii(*argv++);
    file.MakeAbsolute();
    wxString executable = file.GetFullPath();

    if (argc < 3)
    {
        usage(executable);
        return;
    }

    wxString command;
    command = wxString::FromAscii(*argv++);

    serviceName = wxString::FromAscii(*argv++);

    argc -= 3;

    if (command == wxT("INSTALL"))
    {
        wxString displayname = _("PostgreSQL Scheduling Agent - ") + serviceName;
        wxString args = wxT("RUN ") + serviceName;

        while (argc-- > 0)
        {
            if (argv[0][0] == '-')
            {
                switch (argv[0][1])
                {
                    case 'u':
                    {
                        user = getArg(argc, argv);
                        break;
                    }
                    case 'p':
                    {
                        password = getArg(argc, argv);
                        break;
                    }
                    case 'd':
                    {
                        displayname = getArg(argc, argv);
                        break;
                    }
                    default:
                    {
                        args += wxT(" ") + wxString::FromAscii(*argv);
                        break;
                    }
                }
            }
            else
            {
                args += wxT(" ") + wxString::FromAscii(*argv);
            }

            argv++;
        }

        bool rc=installService(serviceName, executable, args, displayname, user, password);
    }
    else if (command == wxT("REMOVE"))
    {
        bool rc=removeService(serviceName);
    }
    else if (command == wxT("DEBUG"))
    {
        setupForRun(argc, argv, true);

        initService();
#if START_SUSPENDED
        continueService();
#endif

        WaitForSingleObject(threadHandle, INFINITE);
    }
    else if (command == wxT("RUN"))
    {
        SERVICE_TABLE_ENTRY serviceTable[] =
            { (unsigned short *)_("pgAgent Service"), serviceMain, 0, 0};

        setupForRun(argc, argv);

        if (!StartServiceCtrlDispatcher(serviceTable))
        {
            DWORD rc=GetLastError();
            if (rc)
            {
            }
        }
    }
    else
    {
        usage(executable);
    }

    return;
}
