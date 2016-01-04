//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// sysProcess.cpp - External process
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>


// App headers
#include "pgAdmin3.h"
#include "utils/sysProcess.h"


sysProcess::sysProcess(wxEvtHandler *evh, wxMBConv &conv)
	: wxProcess(evh), m_conv(conv)
{
	pid = 0;
	Redirect();
}


sysProcess *sysProcess::Create(const wxString &exec, wxEvtHandler *evh, wxArrayString *env, wxMBConv &conv)
{
	sysProcess *proc = new sysProcess(evh, conv);
	if (env)
		proc->SetEnvironment(*env);

	if (!proc->Run(exec))
	{
		delete proc;
		proc = 0;
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
	for (i = 0 ; i < environment.GetCount() ; i++)
	{
		wxString str = environment.Item(i);
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

void sysProcess::WriteOutputStream(const wxString &out)
{
	// With wxEOL_DOS (=wxEOL_NATIVE in Windows) WriteString() will turn each '\n'
	//   into "\r\n", thus making "\r\n" a wrong "\r\r\n".
	// With wxEOL_UNIX it passes EOL characters as-is, which is preferable.
	wxTextOutputStream tos(*GetOutputStream(), wxEOL_UNIX);
	tos.WriteString(out);
}

wxString sysProcess::ReadStream(wxInputStream *input)
{
	wxString str;

	char buffer[1000 + 1];
	size_t size = 1;
	while (size && !input->Eof())
	{
		input->Read(buffer, sizeof(buffer) - 1);
		size = input->LastRead();
		if (size)
		{
			buffer[size] = 0;
			str.Append(wxString::Format(wxT("%s"), wxString(buffer, m_conv).c_str()));
		}
	}
	return str;
}

