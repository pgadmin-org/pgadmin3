//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsDictionaryGen.h"

#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/filename.h>

pgsDictionaryGen::pgsDictionaryGen(const wxString &file_path,
                                   const bool &sequence, const long &seed, wxCSConv conv) :
	pgsObjectGen(seed), m_file_path(file_path), m_conv(conv)
{
	m_nb_lines = this->count_lines();
	m_randomizer = pgsRandomizer(pnew pgsIntegerGen(1, m_nb_lines,
	                             sequence, seed));
}

wxString pgsDictionaryGen::random()
{
	return this->get_line(m_randomizer->random_long());
}

pgsDictionaryGen::~pgsDictionaryGen()
{

}

pgsDictionaryGen *pgsDictionaryGen::clone()
{
	return pnew pgsDictionaryGen(*this);
}

const long &pgsDictionaryGen::nb_lines() const
{
	return m_nb_lines;
}

long pgsDictionaryGen::count_lines()
{
	long result = 0;

	wxFileName file_path(m_file_path);
	if (file_path.FileExists() && file_path.IsFileReadable())
	{
		wxFFileInputStream input(m_file_path);
		if (input.IsOk())
		{
#if wxUSE_UNICODE
			wxTextInputStream text(input, wxT(" \t"), m_conv);
#else
			wxTextInputStream text(input, wxT(" \t"));
#endif
			wxString line;
			while (!(line = text.ReadLine()).IsEmpty() && !input.Eof())
			{
				++result;
			}
		}
	}

	if (result < 0) result = 0;
	wxASSERT(result >= 0);
	return result;
}

wxString pgsDictionaryGen::get_line(long line_nb)
{
	long current_line = 1;

	if (m_nb_lines > 0)
	{
		wxFFileInputStream input(m_file_path);
		if (input.IsOk())
		{
#if wxUSE_UNICODE
			wxTextInputStream text(input, wxT(" \t"), m_conv);
#else
			wxTextInputStream text(input, wxT(" \t"));
#endif
			while (current_line < line_nb && !input.Eof())
			{
				text.ReadLine();
				++current_line;
			}
			return text.ReadLine();
		}
	}

	return wxString();
}
