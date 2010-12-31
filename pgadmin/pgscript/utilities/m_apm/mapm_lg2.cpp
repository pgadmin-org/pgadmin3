
/*
 *  M_APM  -  mapm_lg2.c
 *
 *  Copyright (C) 2003 - 2007   Michael C. Ring
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
 *      This file contains the iterative function to compute the LOG
 *	This is an internal function to the library and is not intended
 *	to be called directly by the user.
 *
 */

#include "pgAdmin3.h"
#include "pgscript/utilities/mapm-lib/m_apm_lc.h"

/****************************************************************************/

/*
 *      compute rr = log(nn)
 *
 *	input is assumed to not exceed the exponent range of a normal
 *	'C' double ( |exponent| must be < 308)
 */

/****************************************************************************/
void	M_log_solve_cubic(M_APM rr, int places, M_APM nn)
{
	M_APM   tmp0, tmp1, tmp2, tmp3, guess;
	int	ii, maxp, tolerance, local_precision;

	guess = M_get_stack_var();
	tmp0  = M_get_stack_var();
	tmp1  = M_get_stack_var();
	tmp2  = M_get_stack_var();
	tmp3  = M_get_stack_var();

	M_get_log_guess(guess, nn);

	tolerance       = -(places + 4);
	maxp            = places + 16;
	local_precision = 18;

	/*    Use the following iteration to solve for log :

	                        exp(X) - N
	      X     =  X - 2 * ------------
	       n+1              exp(X) + N


	      this is a cubically convergent algorithm
	      (each iteration yields 3X more digits)
	*/

	ii = 0;

	while (TRUE)
	{
		m_apm_exp(tmp1, local_precision, guess);

		m_apm_subtract(tmp3, tmp1, nn);
		m_apm_add(tmp2, tmp1, nn);

		m_apm_divide(tmp1, local_precision, tmp3, tmp2);
		m_apm_multiply(tmp0, MM_Two, tmp1);
		m_apm_subtract(tmp3, guess, tmp0);

		if (ii != 0)
		{
			if (((3 * tmp0->m_apm_exponent) < tolerance) || (tmp0->m_apm_sign == 0))
				break;
		}

		m_apm_round(guess, local_precision, tmp3);

		local_precision *= 3;

		if (local_precision > maxp)
			local_precision = maxp;

		ii = 1;
	}

	m_apm_round(rr, places, tmp3);
	M_restore_stack(5);
}
/****************************************************************************/
/*
 *      find log(N)
 *
 *      if places < 360
 *         solve with cubically convergent algorithm above
 *
 *      else
 *
 *      let 'X' be 'close' to the solution   (we use ~110 decimal places)
 *
 *      let Y = N * exp(-X) - 1
 *
 *	then
 *
 *	log(N) = X + log(1 + Y)
 *
 *      since 'Y' will be small, we can use the efficient log_near_1 algorithm.
 *
 */
void	M_log_basic_iteration(M_APM rr, int places, M_APM nn)
{
	M_APM   tmp0, tmp1, tmp2, tmpX;

	if (places < 360)
	{
		M_log_solve_cubic(rr, places, nn);
	}
	else
	{
		tmp0 = M_get_stack_var();
		tmp1 = M_get_stack_var();
		tmp2 = M_get_stack_var();
		tmpX = M_get_stack_var();

		M_log_solve_cubic(tmpX, 110, nn);

		m_apm_negate(tmp0, tmpX);
		m_apm_exp(tmp1, (places + 8), tmp0);
		m_apm_multiply(tmp2, tmp1, nn);
		m_apm_subtract(tmp1, tmp2, MM_One);

		M_log_near_1(tmp0, (places - 104), tmp1);

		m_apm_add(tmp1, tmpX, tmp0);
		m_apm_round(rr, places, tmp1);

		M_restore_stack(4);
	}
}
/****************************************************************************/
