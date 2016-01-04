//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSRECORD_H_
#define PGSRECORD_H_

#include "pgscript/pgScript.h"
#include "pgscript/objects/pgsVariable.h"

WX_DECLARE_OBJARRAY(pgsOperand, pgsVectorRecordLine);
WX_DECLARE_OBJARRAY(pgsVectorRecordLine, pgsVectorRecord);

class pgsNumber;
class pgsString;

class pgsRecord : public pgsVariable
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

	pgsVectorRecord m_record;

	wxArrayString m_columns;

public:

	explicit pgsRecord(const USHORT &nb_columns);

	virtual ~pgsRecord();

	virtual pgsVariable *clone() const;

	/* pgsRecord(const pgsRecord & that); */

	/* pgsRecord & operator =(const pgsRecord & that); */

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

public:

	USHORT count_lines() const;

	USHORT count_columns() const;

	/**
	 * Inserts a new element at line.column. If there is something then
	 * it is deleted before inserting the new element.
	 */
	bool insert(const USHORT &line, const USHORT &column,
	            pgsOperand value);

	/**
	 * Retrieves the element at line.column. If it does not exist it
	 * returns an empty string.
	 */
	pgsOperand get(const USHORT &line,
	               const USHORT &column) const;

	pgsOperand get_line(const USHORT &line) const;

	/**
	 * Sets the name of a column. If the index is too high or if the name
	 * already exists then false is returned.
	 */
	bool set_column_name(const USHORT &column, wxString name);

	/**
	 * Gets the index of a given column. If this column does not exist then it
	 * returns count_columns() (the number of columns) which means that this value
	 * is unusable. So if get_column(...) == count_colums() an error occurred.
	 */
	USHORT get_column(wxString name) const;

	bool remove_line(const USHORT &line);

private:

	bool newline();

	bool valid() const;

public:

	bool operator==(const pgsRecord &rhs) const;

	bool operator!=(const pgsRecord &rhs) const;

	bool operator<(const pgsRecord &rhs) const;

	bool operator>(const pgsRecord &rhs) const;

	bool operator<=(const pgsRecord &rhs) const;

	bool operator>=(const pgsRecord &rhs) const;

	bool almost_equal(const pgsRecord &rhs) const;

private:

	bool records_equal(const pgsRecord &lhs, const pgsRecord &rhs,
	                   bool case_sensitive = true) const;

	bool lines_equal(const pgsVectorRecordLine &lhs,
	                 const pgsVectorRecordLine &rhs, bool case_sensitive = true) const;

public:

	virtual pgsNumber number() const;

	virtual pgsRecord record() const;

	virtual pgsString string() const;

};

#endif /*PGSRECORD_H_*/
