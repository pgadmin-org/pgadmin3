//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsRegexGen.h"

#include <wx/sstream.h>
#include <wx/regex.h>
#include <wx/xml/xml.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(pgsVectorStringGen);

pgsRegexGen::pgsRegex::pgsRegex(const pgsVectorChar &characters,
                                const long &first, const long &second) :
	m_characters(characters), m_first(wxMin(first, second)),
	m_second(wxMax(first, second))
{

}

pgsRegexGen::pgsRegex::pgsRegex() :
	m_characters(pgsVectorChar()), m_first(0), m_second(0)
{

}

pgsRegexGen::pgsRegex::~pgsRegex()
{

}

pgsRegexGen::pgsRegex *pgsRegexGen::pgsRegex::clone()
{
	return pnew pgsRegexGen::pgsRegex(*this);
}

void pgsRegexGen::pgsRegex::set_characters(const pgsVectorChar &characters)
{
	m_characters = characters;
}

void pgsRegexGen::pgsRegex::add_character(const wxChar &c)
{
	m_characters.Add(c);
}

void pgsRegexGen::pgsRegex::set_first(const long &first)
{
	m_first = first;
	m_second = first;
}

void pgsRegexGen::pgsRegex::set_second(const long &second)
{
	long first = m_first;
	m_first = wxMin(first, second);
	m_second = wxMax(first, second);
}

const pgsVectorChar &pgsRegexGen::pgsRegex::get_characters() const
{
	return m_characters;
}

const long &pgsRegexGen::pgsRegex::get_first() const
{
	return m_first;
}

const long &pgsRegexGen::pgsRegex::get_second() const
{
	return m_second;
}

pgsRegexGen::pgsRegexGen(const wxString &regex, const long &seed) :
	pgsObjectGen(seed), m_regex(regex), m_valid(true), m_string_gens(pgsVectorStringGen())
{
	wxLogScriptVerbose(wxT("REGEXGEN: %s"), m_regex.c_str());

	// Transform regular expression into XML structure
	bool escape = false, first_regex = true, list = false;
	wxString result = wxT("<regexpressions>\n");
	size_t i = 0;
	while (i < m_regex.Length())
	{
		if (escape)
		{
			if (list == true)
			{
				result.Append(espace_xml_char(m_regex[i]));
			}
			else
			{
				if (!first_regex)
					result.Append(wxT(" </regex>\n"));
				else
					first_regex = false;
				result.Append(wxT(" <regex>\n  <characters>"));
				result.Append(espace_xml_char(m_regex[i]));
				result.Append(wxT("</characters>\n"));
			}
			escape = false;
		}
		else if (list == true && m_regex[i] == wxT('-'))
		{
			if ((i + 1) < m_regex.Length())
			{
				result.Append(char_range(m_regex[i - 1], m_regex[i + 1]));
			}
		}
		else if (m_regex[i] == wxT('['))
		{
			if (!first_regex)
				result.Append(wxT(" </regex>\n"));
			else
				first_regex = false;
			result.Append(wxT(" <regex>\n  <characters>"));
			list = true;
		}
		else if (m_regex[i] == wxT(']'))
		{
			result.Append(wxT("</characters>\n"));
			list = false;
		}
		else if (m_regex[i] == wxT('{'))
		{
			result.Append(wxT("  <range>"));
			list = true;
		}
		else if (m_regex[i] == wxT('}'))
		{
			result.Append(wxT("</range>\n"));
			list = false;
		}
		else if (m_regex[i] == wxT('\\'))
		{
			escape = true;
		}
		else
		{
			if (list == true)
			{
				result.Append(espace_xml_char(m_regex[i]));
			}
			else
			{
				if (!first_regex)
					result.Append(wxT(" </regex>\n"));
				else
					first_regex = false;
				result.Append(wxT(" <regex>\n  <characters>"));
				result.Append(espace_xml_char(m_regex[i]));
				result.Append(wxT("</characters>\n"));
			}
		}

		++i;
	}
	if (result != wxT("<regexpressions>\n"))
		result.Append(wxT(" </regex>\n"));
	result.Append(wxT("</regexpressions>\n"));

	wxLogScriptVerbose(wxT("REGEXGEN: %s"), result.c_str());

	// Load this XML structure with the wxXmlDocument from wxWidgets
	wxStringInputStream input(result);
	wxXmlDocument doc;
	if (!doc.Load(input, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
	{
		m_valid = false;
	}
	else
	{
		// Start processing the XML file
		if (doc.GetRoot()->GetName() != wxT("regexpressions"))
		{
			m_valid = false;
		}
		else
		{
			// Go through XML nodes
			wxXmlNode *xml_regexpressions = doc.GetRoot()->GetChildren();
			while (xml_regexpressions && m_valid)
			{
				if (xml_regexpressions->GetName() == wxT("regex"))
				{
					wxXmlNode *xml_regex = xml_regexpressions->GetChildren();

					pgsRegex regex;
					regex.set_first(1);

					while (xml_regex && m_valid)
					{
						if (xml_regex->GetName() == wxT("characters"))
						{
							wxString content = xml_regex->GetNodeContent();
							for (size_t i = 0; i < content.Length(); i++)
							{
								regex.add_character(content[i]);
							}
						}
						else if (xml_regex->GetName() == wxT("range"))
						{
							wxString content = xml_regex->GetNodeContent();
							wxRegEx ex(wxT("^([0-9]+)(,([0-9]+))?$"));
							wxASSERT(ex.IsValid());
							if (ex.Matches(content))
							{
								long min;
								ex.GetMatch(content, 1).ToLong(&min);
								regex.set_first(min);
								wxString smax = ex.GetMatch(content, 3);
								if (!smax.IsEmpty())
								{
									long max;
									smax.ToLong(&max);
									regex.set_second(max);
								}
							}
							else
							{
								// m_valid = false;
							}
						}

						xml_regex = xml_regex->GetNext();
					}

					m_string_gens.Add(pgsStringGen(regex.get_first(),
					                               regex.get_second(), 1, seed,
					                               regex.get_characters()));
				}

				xml_regexpressions = xml_regexpressions->GetNext();
			}
		}
	}
}

wxString pgsRegexGen::random()
{
	wxString result;
	for (size_t i = 0; i < string_gens_size(); i++)
	{
		result.Append(m_string_gens.Item(i).random());
	}
	return result;
}

const pgsVectorStringGen &pgsRegexGen::string_gens() const
{
	return m_string_gens;
}

size_t pgsRegexGen::string_gens_size() const
{
	return m_string_gens.size();
}

const bool &pgsRegexGen::is_valid() const
{
	return m_valid;
}

wxString pgsRegexGen::espace_xml_char(const wxChar &c)
{
	if (c == wxT('<'))
		return wxT("&lt;");
	else if (c == wxT('&'))
		return wxT("&amp;");
	else if (c == wxT('>'))
		return wxT("&gt;");
	else if (c == wxT('"'))
		return wxT("&quot;");
	else if (c == wxT('\''))
		return wxT("&apos;");
	else return wxString(c);
}

wxString pgsRegexGen::char_range(const wxChar &b, const wxChar &c)
{
	wxChar min = wxMin(b, c);
	++min;
	wxChar max = wxMax(b, c);
	wxString result;
	for (wxChar i = min; i < max; i++)
	{
		result.Append(espace_xml_char(i));
	}
	return result;
}

pgsRegexGen::~pgsRegexGen()
{

}

pgsRegexGen *pgsRegexGen::clone()
{
	return pnew pgsRegexGen(*this);
}
