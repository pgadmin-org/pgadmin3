//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// sysProcess.h - External process
//
//////////////////////////////////////////////////////////////////////////

#ifndef SYSPROCESS_H
#define SYSPROCESS_H

#include <wx/wx.h>
#include "wx/process.h"

class sysProcess : public wxProcess
{
public:
	sysProcess(wxEvtHandler *evh);

	bool Run(const wxString &exec);
	void Abort();
	void SetEnvironment(const wxArrayString &environment);
	wxString ReadInputStream();
	wxString ReadErrorStream();

	static sysProcess *Create(const wxString &exec, wxEvtHandler *evh = 0, wxArrayString *env = 0);

private:
	int pid;
	void OnTerminate(int pid, int status) const;
	wxString ReadStream(wxInputStream *input);
};



#endif
