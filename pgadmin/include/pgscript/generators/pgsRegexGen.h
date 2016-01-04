//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSREGEXGEN_H_
#define PGSREGEXGEN_H_

#include "pgscript/pgScript.h"
#include "pgscript/generators/pgsObjectGen.h"
#include "pgscript/generators/pgsStringGen.h"

WX_DECLARE_OBJARRAY(pgsStringGen, pgsVectorStringGen);

class pgsRegexGen : public pgsObjectGen
{

private:

	class pgsRegex
	{

	private:

		pgsVectorChar m_characters;

		long m_first;
		long m_second;

	public:

		pgsRegex(const pgsVectorChar &characters, const long &first,
		         const long &second);

		pgsRegex();

		~pgsRegex();

		pgsRegex *clone();

		/* pgsRegex & operator =(const pgsRegex & that); */

		/* pgsRegex(const pgsRegex & that); */

		void set_characters(const pgsVectorChar &characters);

		void add_character(const wxChar &c);

		void set_first(const long &first);

		void set_second(const long &second);

		const pgsVectorChar &get_characters() const;

		const long &get_first() const;

		const long &get_second() const;

	};

private:

	wxString m_regex;

	bool m_valid;

	pgsVectorStringGen m_string_gens;

public:

	pgsRegexGen(const wxString &regex, const long &seed = wxDateTime::GetTimeNow());

	virtual wxString random();

	virtual ~pgsRegexGen();

	virtual pgsRegexGen *clone();

	/* pgsRegexGen & operator =(const pgsRegexGen & that); */

	/* pgsRegexGen(const pgsRegexGen & that); */

	const bool &is_valid() const;

	const pgsVectorStringGen &string_gens() const;

	size_t string_gens_size() const;

private:

	static wxString espace_xml_char(const wxChar &c);

	static wxString char_range(const wxChar &b, const wxChar &c);

};

#endif /*PGSREGEXGEN_H_*/
