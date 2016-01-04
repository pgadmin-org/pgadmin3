//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSVARIABLE_H_
#define PGSVARIABLE_H_

#include "pgscript/pgScript.h"
#include "pgscript/utilities/pgsMapm.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsNumber;
class pgsRecord;
class pgsString;

class pgsVariable : public pgsExpression
{

	friend pgsOperand operator+(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator-(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator*(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator/(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator%(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator==(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator!=(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator<(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator>(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator<=(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator>=(const pgsVariable &lhs, const pgsVariable &rhs);

	friend pgsOperand operator!(const pgsVariable &lhs);

	friend pgsOperand operator&=(const pgsVariable &lhs, const pgsVariable &rhs);

public:

	virtual pgsOperand pgs_plus(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_minus(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_times(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_over(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_modulo(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_equal(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_different(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_greater(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_lower(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_lower_equal(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_greater_equal(const pgsVariable &rhs) const = 0;

	virtual pgsOperand pgs_not() const = 0;

	virtual bool pgs_is_true() const = 0;

	virtual pgsOperand pgs_almost_equal(const pgsVariable &rhs) const = 0;

public:

	enum pgsTypes
	{
		pgsTReal, pgsTInt, pgsTString, pgsTRecord
	};

protected:

	pgsVariable(const pgsTypes &type);

	pgsTypes m_type;

public:

	virtual ~pgsVariable();

	virtual pgsVariable *clone() const = 0;

	/* pgsVariable(const pgsVariable & that); */

	/* pgsVariable & operator =(const pgsVariable & that); */

	static MAPM num(const pgsOperand &var);

	static MAPM num(const wxString &var);

public:

	virtual wxString value() const = 0;

	virtual pgsOperand eval(pgsVarMap &vars) const = 0;

public:

	bool is_number() const;

	bool is_integer() const;

	bool is_real() const;

	bool is_string() const;

	bool is_record() const;

	const pgsTypes &type() const;

public:

	virtual pgsNumber number() const = 0;

	virtual pgsRecord record() const = 0;

	virtual pgsString string() const = 0;

};

#endif /*PGSVARIABLE_H_*/
