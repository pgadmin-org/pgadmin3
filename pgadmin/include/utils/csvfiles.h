//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// csvfiles.h - CSV file parsing
//
//////////////////////////////////////////////////////////////////////////

#ifndef CSVFILES_H
#define CSVFILES_H

// PostgreSQL and GPDB now support CSV format logs.
// So, we need a way to parse the CSV files into lines, and lines into tokens (fields).

#include <wx/wx.h>

class CSVTokenizer : public wxObject
{
public:
	CSVTokenizer(const wxString &str): m_string(str), m_pos(0) { }

	bool HasMoreTokens() const;

	// Get the next token (CSV field). Will return empty string if !HasMoreTokens()
	wxString GetNextToken();

protected:

	const wxString m_string;        // the string we tokenize into fields
	size_t   m_pos;                 // the current position in m_string
};

class CSVLineTokenizer : public wxObject
{
public:
	CSVLineTokenizer(const wxString &str): m_string(str), m_pos(0) { }

	bool HasMoreLines() const;

	// Get the next line.  Will return empty string if !HasMoreLines().
	// partial is set "true" if the last line returned was not a complete
	// line (no newline char at end).
	wxString GetNextLine(bool &partial);

protected:

	const wxString m_string;        // the string we tokenize into lines
	size_t   m_pos;                 // the current position in m_string
};
#endif
