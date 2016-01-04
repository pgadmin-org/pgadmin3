//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsIntegerGen.h"

pgsIntegerGen::pgsSequentialIntGen::pgsSequentialIntGen(const MAPM &range,
        const long &seed) :
	pgsNumberGen(range), m_state(seed), m_m(2), m_remainder(m_range)
{
	MAPM _2 = 2;
	while (m_m < m_range)
	{
		m_m = m_m * _2;
	}
	m_buffer.Alloc(BUFFER_SIZE);
}

MAPM pgsIntegerGen::pgsSequentialIntGen::random()
{
	// Bufferizer BUFFER_SIZE values in the vector
	if (m_buffer.GetCount() == 0)
	{
		// Calculate the number of elements to bufferize
		MAPM min = wxMin(m_remainder, MAPM(BUFFER_SIZE));

		// Generate those elements
		for (MAPM i = 0; i < min; i = i + 1)
		{
			do
			{
				m_state = (m_state * arg_a);
				m_state = (m_state + arg_c) % m_m;
			}
			while (m_state >= m_range);
			m_buffer.Add(m_state);
			m_remainder -= 1;
		}

		class pgsRandInt
		{

		private:

			long m_state;

		public:

			pgsRandInt(long n)
				: m_state(n)
			{

			}

			long rand()
			{
				m_state = (1103515245L * m_state + 12345L) % 2147483647L;
				return m_state;
			}

		};

		// Shuffle the vector of generated values
		pgsRandInt rand_int(BUFFER_SIZE);
		for (size_t i = 0; i < m_buffer.GetCount(); i++)
		{
			size_t r = i + (rand_int.rand() % (m_buffer.GetCount() - i));
			const MAPM a = m_buffer.Item(i);
			m_buffer.Item(i) = m_buffer.Item(r);
			m_buffer.Item(r) = a;
		}
	}

	if (m_remainder == 0)
	{
		m_remainder = m_range;
	}

	// Take the last bufferized value
	MAPM data = m_buffer.Last();
	m_buffer.RemoveAt(m_buffer.GetCount() - 1);

	// Return random value
	return data;
}

pgsIntegerGen::pgsSequentialIntGen::~pgsSequentialIntGen()
{

}

pgsNumberGen *pgsIntegerGen::pgsSequentialIntGen::clone()
{
	return pnew pgsIntegerGen::pgsSequentialIntGen(*this);
}

const MAPM pgsIntegerGen::pgsSequentialIntGen::arg_a = 5;
const MAPM pgsIntegerGen::pgsSequentialIntGen::arg_c = 1;

pgsIntegerGen::pgsNormalIntGen::pgsNormalIntGen(const MAPM &range,
        const long &seed) :
	pgsNumberGen(range), m_state(seed), m_top(arg_m - 1)
{
	for (int i = 0; i < 10; i++)
		random(); // Beginning of the sequence is garbage
}

MAPM pgsIntegerGen::pgsNormalIntGen::random()
{
	m_state = (m_state * arg_a);
	m_state = (m_state + arg_c) % arg_m;
	return (m_state * m_range).div(m_top);
}

pgsIntegerGen::pgsNormalIntGen::~pgsNormalIntGen()
{

}

pgsNumberGen *pgsIntegerGen::pgsNormalIntGen::clone()
{
	return pnew pgsIntegerGen::pgsNormalIntGen(*this);
}

const MAPM pgsIntegerGen::pgsNormalIntGen::arg_a = 16807L;
const MAPM pgsIntegerGen::pgsNormalIntGen::arg_c = 0;
const MAPM pgsIntegerGen::pgsNormalIntGen::arg_m = 2147483647L;

pgsIntegerGen::pgsIntegerGen(const MAPM &min, const MAPM &max,
                             const bool &sequence, const long &seed) :
	pgsObjectGen(seed), m_min(wxMin(min, max)), m_max(wxMax(min, max)),
	m_range(m_max - m_min + 1), m_sequence(sequence)
{
	m_randomizer = is_sequence()
	               ? pgsRandomizer(pnew pgsSequentialIntGen(m_range, m_seed))
	               : pgsRandomizer(pnew pgsNormalIntGen(m_range, m_seed));
}

bool pgsIntegerGen::is_sequence() const
{
	return m_sequence;
}

wxString pgsIntegerGen::random()
{
	MAPM data = m_randomizer->random();
	data = data + m_min;
	wxASSERT(data >= m_min && data <= m_max);
	return pgsMapm::pgs_mapm_str(data);
}

long pgsIntegerGen::random_long()
{
	long result;
	random().ToLong(&result);
	return result;
}

pgsIntegerGen::~pgsIntegerGen()
{

}

pgsIntegerGen *pgsIntegerGen::clone()
{
	return pnew pgsIntegerGen(*this);
}
