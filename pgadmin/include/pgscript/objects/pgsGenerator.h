//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENERATOR_H_
#define PGSGENERATOR_H_

#include "pgscript/pgScript.h"
#include "pgscript/generators/pgsObjectGen.h"
#include "pgscript/objects/pgsVariable.h"
#include "pgscript/utilities/pgsSharedPtr.h"

class pgsNumber;
class pgsRecord;
class pgsString;

class pgsGenerator : public pgsVariable
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

	typedef pgsSharedPtr<pgsObjectGen> pgsRandomizer;

	mutable pgsRandomizer m_randomizer;

public:

	pgsGenerator(const pgsTypes &generator_type, pgsObjectGen *randomizer);

	virtual ~pgsGenerator();

	virtual pgsVariable *clone() const;

	/* pgsGenerator(const pgsGenerator & that); */

	/* pgsGenerator & operator =(const pgsGenerator & that); */

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

protected:

	pgsOperand operand() const;

public:

	virtual pgsNumber number() const;

	virtual pgsRecord record() const;

	virtual pgsString string() const;

};

#endif /*PGSGENERATOR_H_*/
