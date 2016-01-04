//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsGenDictionary.h"

#include <wx/filename.h>
#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/generators/pgsDictionaryGen.h"
#include "pgscript/objects/pgsGenerator.h"

pgsGenDictionary::pgsGenDictionary(const pgsExpression *file_path, const pgsExpression *sequence,
                                   const pgsExpression *seed, const pgsExpression *wx_conv) :
	pgsExpression(), m_file_path(file_path), m_sequence(sequence), m_seed(seed),
	m_wx_conv(wx_conv)
{

}

pgsGenDictionary::~pgsGenDictionary()
{
	pdelete(m_file_path);
	pdelete(m_sequence);
	pdelete(m_seed);
	pdelete(m_wx_conv);
}

pgsExpression *pgsGenDictionary::clone() const
{
	return pnew pgsGenDictionary(*this);
}

pgsGenDictionary::pgsGenDictionary(const pgsGenDictionary &that) :
	pgsExpression(that)
{
	m_file_path = that.m_file_path->clone();
	m_sequence = that.m_sequence->clone();
	m_seed = that.m_seed->clone();
	m_wx_conv = that.m_wx_conv->clone();
}

pgsGenDictionary &pgsGenDictionary::operator =(const pgsGenDictionary &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_file_path);
		pdelete(m_sequence);
		pdelete(m_seed);
		pdelete(m_wx_conv);
		m_file_path = that.m_file_path->clone();
		m_sequence = that.m_sequence->clone();
		m_seed = that.m_seed->clone();
		m_wx_conv = that.m_wx_conv->clone();
	}
	return (*this);
}

wxString pgsGenDictionary::value() const
{
	return wxString() << wxT("file[ file = ") << m_file_path->value() << wxT(" sequence = ")
	       << m_sequence->value() << wxT(" seed = ") << m_seed->value()
	       << wxT(" encoding = ") << m_wx_conv->value() << wxT(" ]");
}

pgsOperand pgsGenDictionary::eval(pgsVarMap &vars) const
{
	// Evaluate parameters
	pgsOperand file_path(m_file_path->eval(vars));
	pgsOperand sequence(m_sequence->eval(vars));
	pgsOperand seed(m_seed->eval(vars));
	pgsOperand wx_conv(m_wx_conv->eval(vars));

	// Check parameters and create the generator
	if (file_path->is_string() && sequence->is_integer() && seed->is_integer()
	        && wx_conv->is_string())
	{
		wxFileName file(file_path->value());
		if (file.FileExists() && file.IsFileReadable())
		{
			long aux_sequence, aux_seed;
			sequence->value().ToLong(&aux_sequence);
			seed->value().ToLong(&aux_seed);
			return pnew pgsGenerator(pgsVariable::pgsTString,
			                         pnew pgsDictionaryGen(file_path->value(), aux_sequence != 0,
			                                 aux_seed, wxCSConv(wx_conv->value())));
		}
		else
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nFile <") << file_path->value()
			                            << wxT("> does not exist"));
		}
	}
	else
	{
		// Deal with errors
		if (!file_path->is_string())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nfile should be a string"));
		}
		else if (!sequence->is_integer())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nsequence should be an integer"));
		}
		else if (!seed->is_integer())
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nseed should be an integer"));
		}
		else
		{
			throw pgsParameterException(wxString() << value()
			                            << wxT(":\nencoding should be a string"));
		}
	}
}
