//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSCOPIEDPTR_H_
#define PGSCOPIEDPTR_H_

#include <algorithm>

template <typename T> class pgsCopiedPtr
{

private:

	T *p;

public:

	pgsCopiedPtr(T *p) :
		p(p)
	{

	}

	pgsCopiedPtr() :
		p(0)
	{

	}

	pgsCopiedPtr(const pgsCopiedPtr &that) :
		p(that.p == 0 ? 0 : that.p->clone())
	{

	}

	~pgsCopiedPtr()
	{
		pdelete(p);
	}

	pgsCopiedPtr &operator =(pgsCopiedPtr that)
	{
		std::swap(p, that.p);
		return (*this);
	}

	T &operator *()
	{
		return *p;
	}

	const T &operator *() const
	{
		return *p;
	}

	T *operator ->()
	{
		return p;
	}

	const T *operator ->() const
	{
		return p;
	}

	const T *get() const
	{
		return p;
	}
};

#endif /*PGSCOPIEDPTR_H_*/
