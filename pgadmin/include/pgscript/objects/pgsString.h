//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSSTRING_H_
#define PGSSTRING_H_

#include "pgscript/pgScript.h"
#include "pgscript/objects/pgsVariable.h"

class pgsRecord;
class pgsString;

class pgsString : public pgsVariable
{

public:

	virtual pgsOperand pgs_plus(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_minus(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_times(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_over(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_modulo(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_equal(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_different(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_greater(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_lower(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_lower_equal(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_greater_equal(const pgsVariable &rhs) const;

	virtual pgsOperand pgs_not() const;

	virtual bool pgs_is_true() const;

	virtual pgsOperand pgs_almost_equal(const pgsVariable &rhs) const;

protected:

	wxString m_data;

public:

	explicit pgsString(const wxString &data);

	virtual ~pgsString();

	virtual pgsVariable *clone() const;

	/* pgsString(const pgsString & that); */

	/* pgsString & operator =(const pgsString & that); */

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

public:

	virtual pgsNumber number() const;

	virtual pgsRecord record() const;

	virtual pgsString string() const;

};

#endif /*PGSSTRING_H_*/
