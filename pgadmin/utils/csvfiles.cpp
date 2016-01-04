//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// csvfiles.cpp - CSV file parsing
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"
#include "utils/sysLogger.h"
#include "utils/csvfiles.h"

// PostgreSQL and GPDB now support CSV format logs.
// So, we need a way to parse the CSV files into lines, and lines into tokens (fields).

bool CSVTokenizer::HasMoreTokens() const
{
	if ( m_string.length() > 0)
	{
		if ( m_pos >= m_string.length())
			return false;

		if ( m_string.find_first_not_of(wxT(','), m_pos) != wxString::npos )
			// there are non delimiter characters left, so we do have more tokens
			return true;

		if (m_string[m_pos] == wxT('\n'))
			return false;
	}
	return m_pos == 0 && !m_string.empty();
}

wxString CSVTokenizer::GetNextToken()
{
	wxString token;

	if ( !HasMoreTokens() )
		return token;

	// skip leading blanks if not quoted.
	while (m_pos < m_string.length() && m_string[m_pos] == wxT(' '))
		m_pos ++;

	// Are we a quoted field?  Must handle this special.
	bool quoted_string = (m_string[m_pos] == wxT('\"'));
	bool inquote = false;

	size_t pos = m_pos;

	// find the end of this token.
	for (; pos < m_string.length(); pos++)
	{
		if (quoted_string && m_string[pos] == wxT('\"'))
			inquote = !inquote;

		if (!inquote)
		{
			// Check to see if we have found the end of this token.
			// Tokens normally end with a ',' delimiter.
			if (m_string[pos] == wxT(','))
				break;

			// Last token is delimited by '\n' or by end of string.
			if (m_string[pos] == wxT('\n') && pos == m_string.length() - 1)
				break;
		}
	}

	if (quoted_string && !inquote)
	{
		token.assign(m_string, m_pos + 1, pos - m_pos - 2);  // Remove leading and trailing quotes

		// Remove double doublequote chars, replace with single doublequote chars
		token.Replace(wxT("\"\""), wxT("\""), true);
	}
	else
		token.assign(m_string, m_pos, pos - m_pos);

	if (quoted_string && inquote)
	{
		wxLogNotice(wxT("unterminated double quoted string: %s\n"), token.c_str());
	}

	m_pos = pos + 1;    // Skip token and delimiter

	if (m_pos > m_string.length())  // Perhaps no delimiter if at end of string if orig string didn't have '\n'.
		m_pos = m_string.length();

	return token;
}

bool CSVLineTokenizer::HasMoreLines() const
{
	if ( m_string.find_first_not_of(wxT('\n'), m_pos) != wxString::npos )
		// there are non line-end characters left, so we do have more lines
		return true;
	return false;
}

wxString CSVLineTokenizer::GetNextLine(bool &partial)
{
	wxString token;
	partial = true;

	if ( !HasMoreLines() )
		return token;

	// find the end of this line.  CSV lines end in "\n", but
	// CSV lines may have "\n" chars inside double-quoted strings, so we need to find that out.

	bool inquote = false;
	for (size_t pos = m_pos; pos < m_string.length(); pos++)
	{
		if (m_string[pos] == wxT('\"'))
			inquote = !inquote;

		if (m_string[pos] == wxT('\n') && !inquote)
		{
			// Good, we found a complete log line terminated
			// by "\n", and the "\n" wasn't in a quoted string.

			size_t len = pos - m_pos + 1;   // return the line, including the trailing "\n"
			token.assign(m_string, m_pos, len);
			m_pos = pos + 1;                // point to next line.
			partial = false;
			return token;
		}
	}

	// no more delimiters, so the line is everything till the end of
	// string, but we don't have all of the CSV the line... Some must still be coming.

	token.assign(m_string, m_pos, wxString::npos);
	partial = true;

	m_pos = m_string.length();

	return token;
}
