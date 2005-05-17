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

#include <windows.h>
#include <process.h>
#include <stdio.h>

// for debugging purposes, we can start the service paused

#define START_SUSPENDED 1


static SERVICE_STATUS serviceStatus;
static SERVICE_STATUS_HANDLE serviceStatusHandle;
static string serviceName;
static string user=".\\Administrator", password;
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

void LogMessage(char *msg, int level)
{
    if (eventHandle)
    {
        // FIXME - This path should use the event log!
        switch (level)
        {
            case LOG_DEBUG:
                fprintf(stderr, "DEBUG: %s\n", msg);
                break;
            case LOG_WARNING:
                fprintf(stderr, "WARNING: %s\n", msg);
                break;
            case LOG_ERROR:
                fprintf(stderr, "ERROR: %s\n", msg);
                exit(1);
                break;
        }
    }
    else
    {
        switch (level)
        {
            case LOG_DEBUG:
                fprintf(stderr, "DEBUG: %s\n", msg);
                break;
            case LOG_WARNING:
                fprintf(stderr, "WARNING: %s\n", msg);
                break;
            case LOG_ERROR:
                fprintf(stderr, "ERROR: %s\n", msg);
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
    serviceName = strdup(argv[0]);
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
bool installService(const char *serviceName, const char *exePath, const char *displayname, const char *user, const char *password)
{
    bool done=false;
    SC_HANDLE manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (manager)
    {
        SC_HANDLE service = CreateService(manager, serviceName, displayname, SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
            exePath, 0, 0, 0, user, password);

        if (service)
        {
            done = true;
            CloseServiceHandle(service);
        }
        CloseServiceHandle(manager);
    }
    return done;
}


bool removeService(const char *serviceName)
{
    bool done=false;
    SC_HANDLE manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (manager)
    {
        SC_HANDLE service = OpenService(manager, serviceName, SERVICE_ALL_ACCESS);
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
    return done;
}



void usage()
{
    printf(
        "Usage:\n"
        "pgAgent REMOVE <serviceName>\n"
        "pgAgent INSTALL <serviceName> [options] <connect-string>\n"
        "options:\n"
        "-u <user>\n"
        "-p <password>\n"
        "-d <displayname>\n"
        "-t <poll time interval in seconds (default 10)>\n"
        "-r <retry period after connection abort in seconds (>=10, default 30)>\n"
        "-c <connection pool size (>=5, default 5)>\n"
        );
}



////////////////////////////////////////////////////////////

void setupForRun(int argc, char **argv)
{
    eventHandle = RegisterEventSource(0, serviceName.c_str());
    if (!eventHandle)
        LogMessage("Couldn't register event handle.", LOG_ERROR);

    setOptions(argc, argv);

    DBconn *conn=DBconn::InitConnection(connectString);
    if (!conn->IsValid())
    {
        char tmp[255];
        _snprintf(tmp, 254, "Connection not valid: %s", conn->GetLastError().c_str());
        LogMessage(tmp, LOG_ERROR);
    }

    serviceDBname = conn->GetDBname();
}

        
void main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage();
        return;
    }

    string executable = *argv++;
    string command = *argv++;
    serviceName = *argv++;
    argc -= 3;

    if (command == "INSTALL")
    {
        string displayname = "pgAgent " + serviceName;
        string arg = executable + " RUN " + serviceName;

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
                        arg += string(" ") + *argv;
                        break;
                    }
                }
            }
            else
            {
                arg += " ";
                arg += *argv;
            }

            argv++;
        }

        bool rc=installService(serviceName.c_str(), arg.c_str(), displayname.c_str(), user.c_str(), password.c_str());
    }
    else if (command == "REMOVE")
    {
        bool rc=removeService(serviceName.c_str());
    }
    else if (command == "DEBUG")
    {
        setupForRun(argc, argv);

        initService();
#if START_SUSPENDED
        continueService();
#endif

        WaitForSingleObject(threadHandle, INFINITE);
    }
    else if (command == "RUN")
    {
        SERVICE_TABLE_ENTRY serviceTable[] = 
            { "pgAgent service", serviceMain, 0, 0};
        
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
        usage();
    }

    return;
}
