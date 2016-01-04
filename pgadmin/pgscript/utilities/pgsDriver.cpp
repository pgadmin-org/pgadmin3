//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/utilities/pgsDriver.h"
#include "pgscript/utilities/pgsThread.h"
#include "pgscript/utilities/pgsUtilities.h"

#include <wx/ffile.h>
#include <wx/filename.h>

namespace pgscript
{

pgsDriver::pgsDriver(class pgsContext &_context, class pgsProgram &_program,
                     class pgsThread &_thread) :
	trace_scanning(false), trace_parsing(false), context(_context),
	program(_program), thread(_thread)
{
	wxLogScript(wxT("Driver created"));
}

pgsDriver::~pgsDriver()
{
	wxLogScript(wxT("Driver destroyed"));
}

bool pgsDriver::parse_stream(std::istream &in)
{
	pgsScanner scanner(wxConvUTF8, &in);
	scanner.set_debug(trace_scanning);
	this->lexer = &scanner;

	pgsParser parser(*this);
	parser.set_debug_level(trace_parsing);
	return (parser.parse() == 0);
}

bool pgsDriver::parse_file(const wxString &filename, wxMBConv &conv)
{
	wxFileName file_path(filename);
	if (file_path.FileExists() && file_path.IsFileReadable())
	{
		wxFFile file(filename);
		wxString input;
		file.ReadAll(&input, conv);
		return parse_string(input);
	}
	else
	{
		wxLogError(wxT("PGSCRIPT: File %s does not exist"), filename.c_str());
		return false;
	}
}

bool pgsDriver::parse_string(const wxString &input)
{
	std::istringstream iss(std::string(input.mb_str(wxConvUTF8)));
	return parse_stream(iss);
}

void pgsDriver::error(const class location &l, const wxString &m)
{
	std::ostringstream oss;
	oss << l;
	thread.last_error_line(l.begin.line);
	thread.LockOutput();
	context.m_cout << PGSOUTERROR
	               << wx_static_cast(const wxString, wxString(oss.str()
	                                 .c_str(), wxConvUTF8)) << wxT(": ") << m << wxT("\n");
	thread.UnlockOutput();
}

} // namespace pgscript
