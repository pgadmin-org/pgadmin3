//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: sysProcess.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// sysProcess.cpp - External process
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>


// App headers
#include "pgAdmin3.h"
#include "sysProcess.h"


sysProcess::sysProcess(wxEvtHandler *evh)
: wxProcess(evh)
{
    pid=0;
    Redirect();
}


sysProcess *sysProcess::Create(const wxString &exec, wxEvtHandler *evh, wxArrayString *env)
{
    sysProcess *proc=new sysProcess(evh);
    if (env)
        proc->SetEnvironment(*env);
    
    if (!proc->Run(exec))
    {
        delete proc;
        proc=0;
    }
    return proc;
}


bool sysProcess::Run(const wxString &exec)
{
    pid = wxExecute(exec, wxEXEC_ASYNC, this);

    return (pid != 0);
}


void sysProcess::SetEnvironment(const wxArrayString &environment)
{
    size_t i;
    for (i=0 ; i < environment.GetCount() ; i++)
    {
        wxString str=environment.Item(i);
        wxSetEnv(str.BeforeFirst('='), str.AfterFirst('='));
    }
}


void sysProcess::Abort()
{
    if (pid)
        wxKill(pid, wxSIGTERM);
}


wxString sysProcess::ReadInputStream()
{
    if (IsInputAvailable())
        return ReadStream(GetInputStream());
    return wxEmptyString;
}


wxString sysProcess::ReadErrorStream()
{
    if (IsErrorAvailable())
        return ReadStream(GetErrorStream());
    return wxEmptyString;
}


wxString sysProcess::ReadStream(wxInputStream *input)
{
    wxString str;

    char buffer[1000+1];
    size_t size=1;
    while (size && !input->Eof())
    {
        input->Read(buffer, sizeof(buffer)-1);
        size=input->LastRead();
        if (size)
        {
            buffer[size]=0;
            str.Append(wxString::Format(wxT("%s"),wxString(buffer,wxConvLibc)));
        }
    }
    return str;
}

