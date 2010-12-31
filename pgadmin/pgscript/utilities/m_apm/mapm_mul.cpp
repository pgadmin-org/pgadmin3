
/*
 *  M_APM  -  mapm_mul.c
 *
 *  Copyright (C) 1999 - 2007   Michael C. Ring
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted,
 *  provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted. Permission to distribute
 *  the modified code is granted. Modifications are to be distributed by
 *  using the file 'license.txt' as a template to modify the file header.
 *  'license.txt' is available in the official MAPM distribution.
 *
 *  This software is provided "as is" without express or implied warranty.
 */

/*
 *
 *      This file contains basic multiplication function.
 *
 */

#include "pgAdmin3.h"
#include "pgscript/utilities/mapm-lib/m_apm_lc.h"

extern void M_fast_multiply(M_APM, M_APM, M_APM);

/****************************************************************************/
void	m_apm_multiply(M_APM r, M_APM a, M_APM b)
{
	int	ai, itmp, sign, nexp, ii, jj, indexa, indexb, index0, numdigits;
	UCHAR   *cp, *cpr, *cp_div, *cp_rem;
	void	*vp;

	sign = a->m_apm_sign * b->m_apm_sign;
	nexp = a->m_apm_exponent + b->m_apm_exponent;

	if (sign == 0)      /* one number is zero, result is zero */
	{
		M_set_to_zero(r);
		return;
	}

	numdigits = a->m_apm_datalength + b->m_apm_datalength;
	indexa = (a->m_apm_datalength + 1) >> 1;
	indexb = (b->m_apm_datalength + 1) >> 1;

	/*
	 *	If we are multiplying 2 'big' numbers, use the fast algorithm.
	 *
	 *	This is a **very** approx break even point between this algorithm
	 *      and the FFT multiply. Note that different CPU's, operating systems,
	 *      and compiler's may yield a different break even point. This point
	 *      (~96 decimal digits) is how the test came out on the author's system.
	 */

	if (indexa >= 48 && indexb >= 48)
	{
		M_fast_multiply(r, a, b);
		return;
	}

	ii = (numdigits + 1) >> 1;     /* required size of result, in bytes */

	if (ii > r->m_apm_malloclength)
	{
		if ((vp = MAPM_REALLOC(r->m_apm_data, (ii + 32))) == NULL)
		{
			/* fatal, this does not return */

			M_apm_log_error_msg(M_APM_FATAL, "\'m_apm_multiply\', Out of memory");
		}

		r->m_apm_malloclength = ii + 28;
		r->m_apm_data = (UCHAR *)vp;
	}

	M_get_div_rem_addr(&cp_div, &cp_rem);

	index0 = indexa + indexb;
	cp = r->m_apm_data;
	memset(cp, 0, index0);
	ii = indexa;

	while (TRUE)
	{
		index0--;
		cpr = cp + index0;
		jj  = indexb;
		ai  = (int)a->m_apm_data[--ii];

		while (TRUE)
		{
			itmp = ai * b->m_apm_data[--jj];

			*(cpr - 1) += cp_div[itmp];
			*cpr     += cp_rem[itmp];

			if (*cpr >= 100)
			{
				*cpr     -= 100;
				*(cpr - 1) += 1;
			}

			cpr--;

			if (*cpr >= 100)
			{
				*cpr     -= 100;
				*(cpr - 1) += 1;
			}

			if (jj == 0)
				break;
		}

		if (ii == 0)
			break;
	}

	r->m_apm_sign       = sign;
	r->m_apm_exponent   = nexp;
	r->m_apm_datalength = numdigits;

	M_apm_normalize(r);
}
/****************************************************************************/
