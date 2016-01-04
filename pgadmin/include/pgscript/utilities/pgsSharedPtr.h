//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSSHAREDPTR_H_
#define PGSSHAREDPTR_H_

template <typename T> class pgsSharedPtr
{

private:

	struct count
	{
		long c;

		T *q;

		count(T *q) :
			c(1), q(q)
		{

		}

		~count()
		{
			pdelete(q);
		}
	};

	count *p;

public:

	pgsSharedPtr(T *q) :
		p(pnew count(q))
	{

	}

	pgsSharedPtr() :
		p(pnew count(0))
	{

	}

	pgsSharedPtr(const pgsSharedPtr &that) :
		p(that.p)
	{
		++p->c;
	}

	~pgsSharedPtr()
	{
		if (!--p->c)
		{
			pdelete(p);
		}
	}

	pgsSharedPtr &operator =(pgsSharedPtr that)
	{
		std::swap(p, that.p);
		return (*this);
	}

	T &operator *()
	{
		return *(p->q);
	}

	const T &operator *() const
	{
		return *(p->q);
	}

	T *operator ->()
	{
		return p->q;
	}

	const T *operator ->() const
	{
		return p->q;
	}

	const T *get() const
	{
		return p->q;
	}
};

#endif /*PGSSHAREDPTR_H_*/
