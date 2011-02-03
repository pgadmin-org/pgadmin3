
/*
 *  M_APM  -  mapmcbrt.c
 *
 *  Copyright (C) 2000 - 2007   Michael C. Ring
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
 *      This file contains the CBRT (cube root) function.
 *
 */

#include "pgAdmin3.h"
#include "pgscript/utilities/mapm-lib/m_apm_lc.h"

/****************************************************************************/
void	m_apm_cbrt(M_APM rr, int places, M_APM aa)
{
	M_APM   last_x, guess, tmpN, tmp7, tmp8, tmp9;
	int	ii, nexp, bflag, tolerance, maxp, local_precision;

	/* result is 0 if input is 0 */

	if (aa->m_apm_sign == 0)
	{
		M_set_to_zero(rr);
		return;
	}

	last_x = M_get_stack_var();
	guess  = M_get_stack_var();
	tmpN   = M_get_stack_var();
	tmp7   = M_get_stack_var();
	tmp8   = M_get_stack_var();
	tmp9   = M_get_stack_var();

	/* compute the cube root of the positive number, we'll fix the sign later */

	m_apm_absolute_value(tmpN, aa);

	/*
	    normalize the input number (make the exponent near 0) so
	    the 'guess' function will not over/under flow on large
	    magnitude exponents.
	*/

	nexp = aa->m_apm_exponent / 3;
	tmpN->m_apm_exponent -= 3 * nexp;

	M_get_cbrt_guess(guess, tmpN);

	tolerance       = places + 4;
	maxp            = places + 16;
	bflag           = FALSE;
	local_precision = 14;

	m_apm_negate(last_x, MM_Ten);

	/*   Use the following iteration to calculate 1 / cbrt(N) :

	                                 4
	         X     =  [ 4 * X - N * X ] / 3
	          n+1
	*/

	ii = 0;

	while (TRUE)
	{
		m_apm_multiply(tmp8, guess, guess);
		m_apm_multiply(tmp7, tmp8, tmp8);
		m_apm_round(tmp8, local_precision, tmp7);
		m_apm_multiply(tmp9, tmpN, tmp8);

		m_apm_multiply(tmp8, MM_Four, guess);
		m_apm_subtract(tmp7, tmp8, tmp9);
		m_apm_divide(guess, local_precision, tmp7, MM_Three);

		if (bflag)
			break;

		/* force at least 2 iterations so 'last_x' has valid data */

		if (ii != 0)
		{
			m_apm_subtract(tmp8, guess, last_x);

			if (tmp8->m_apm_sign == 0)
				break;

			if ((-4 * tmp8->m_apm_exponent) > tolerance)
				bflag = TRUE;
		}

		local_precision *= 2;

		if (local_precision > maxp)
			local_precision = maxp;

		m_apm_copy(last_x, guess);
		ii = 1;
	}

	/* final cbrt = N * guess ^ 2 */

	m_apm_multiply(tmp9, guess, guess);
	m_apm_multiply(tmp8, tmp9, tmpN);
	m_apm_round(rr, places, tmp8);

	rr->m_apm_exponent += nexp;
	rr->m_apm_sign = aa->m_apm_sign;
	M_restore_stack(6);
}
/****************************************************************************/

