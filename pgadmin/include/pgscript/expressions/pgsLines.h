//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsLines.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSLINES_H_
#define PGSLINES_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsLines : public pgsExpression
{
	
private:
	
	wxString m_name;
	
public:

	pgsLines(const wxString & name);

	virtual ~pgsLines();

	/* pgsLines(const pgsLines & that); */

	/* pgsLines & operator=(const pgsLines & that); */

	virtual pgsExpression * clone() const;
	
	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap & vars) const;
	
};

#endif /*PGSLINES_H_*/
