//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSNUMBER_H_
#define PGSNUMBER_H_

#include "pgscript/pgScript.h"
#include "pgscript/objects/pgsVariable.h"

class pgsRecord;
class pgsString;

/**
 * A pgsNumber represents either a number or a string. If the data (a string)
 * matches with a regular expression that represents a number then it is a
 * number otherwise it is a string. The difference between a string stored
 * in this object and a string stored in pgsString is that a string in pgsNumber
 * cannot be concatenated with another one in pgsPlus.
 */
class pgsNumber : public pgsVariable
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

	explicit pgsNumber(const wxString &data, const bool &is_real = pgsInt);

	virtual ~pgsNumber();

	virtual pgsVariable *clone() const;

	pgsNumber(const pgsNumber &that);

	pgsNumber &operator =(const pgsNumber &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

public:

	bool is_valid() const;

	static pgsTypes num_type(const wxString &num);

public:

	virtual pgsNumber number() const;

	virtual pgsRecord record() const;

	virtual pgsString string() const;

};

#endif /*PGSNUMBER_H_*/
