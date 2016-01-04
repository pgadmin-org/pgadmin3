//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// sysProcess.h - External process
//
//////////////////////////////////////////////////////////////////////////

#ifndef SYSPROCESS_H
#define SYSPROCESS_H

#include <wx/wx.h>
#include "wx/process.h"
#include <wx/txtstrm.h>

class sysProcess : public wxProcess
{
public:
	sysProcess(wxEvtHandler *evh, wxMBConv &conv = wxConvLibc);

	bool Run(const wxString &exec);
	void Abort();
	void SetEnvironment(const wxArrayString &environment);
	wxString ReadInputStream();
	wxString ReadErrorStream();
	void WriteOutputStream(const wxString &out);

	static sysProcess *Create(const wxString &exec, wxEvtHandler *evh = 0, wxArrayString *env = 0, wxMBConv &conv = wxConvLibc);

private:
	int pid;
	wxMBConv &m_conv;
	void OnTerminate(int pid, int status) const;
	wxString ReadStream(wxInputStream *input);
};



#endif
