//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsAssertException.h,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSASSERTEXCEPTION_H_
#define PGSASSERTEXCEPTION_H_

#include "pgscript/pgScript.h"
#include "pgscript/exceptions/pgsException.h"

class pgsAssertException : public pgsException
{
	
protected:
		
	const wxString m_message;
	
public:
	
	pgsAssertException(const wxString & message);
	
	virtual ~pgsAssertException();
	
	virtual const wxString message() const;
	
};

#endif /*PGSASSERTEXCEPTION_H_*/
