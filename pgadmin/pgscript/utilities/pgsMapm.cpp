//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/utilities/pgsMapm.h"
#include <string>
#include <wx/sstream.h>
#include <wx/txtstrm.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(pgsVectorMapm);

wxString pgsMapm::pgs_mapm_str(const MAPM &m, const bool &as_int)
{
	if (m.is_integer() || as_int)
	{
		wxString str = pgs_mapm_str_fixed(m);
		str = str.substr(0, str.find_last_of('.')); // Remove .0 at the end of an integer
		return str;
	}
	else
	{
		return pgs_mapm_str_float(m);
	}
}

MAPM pgsMapm::pgs_mapm_round(const MAPM &m)
{
	return MAPM(pgs_mapm_str(m, true).mb_str());
}

wxString pgsMapm::pgs_mapm_str_fixed(const MAPM &m)
{
	char *const res = m.toFixPtStringExp(-1, '.', ' ', INT_MAX);
	wxString str(res, wxConvUTF8);
	free(res);
	return str;
}

wxString pgsMapm::pgs_mapm_str_float(const MAPM &m)
{
	int str_len = m.significant_digits();

	{
		wxStringOutputStream sos;
		wxTextOutputStream tos(sos);
		tos << abs(m.exponent());
		str_len += sos.GetString().Length();
	}

	{
		wxStringOutputStream sos;
		wxTextOutputStream tos(sos);
		tos << str_len;
		str_len = m.significant_digits() + 10 + sos.GetString().Length();
	}

	char *res = pnew char[str_len];
	m.toString(res, -1);
	wxString result(res, wxConvUTF8);
	pdeletea(res);

	return result;
}

MAPM pgsMapm::pgs_str_mapm(const wxString &s)
{
	return std::string(s.mb_str()).c_str();
}
