
/*
 *  M_APM  -  mapmutl1.c
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
 *      This file contains the utility function 'M_apm_log_error_msg'
 *
 *	This is the only function in this file so a user can supply
 *	their own custom version easily without changing the base library.
 */

#include "pgAdmin3.h"
#include "pgscript/utilities/mapm-lib/m_apm_lc.h"

/****************************************************************************/
void	M_apm_log_error_msg(int fatal, const char *message)
{
	if (fatal)
	{
		fprintf(stderr, "MAPM Error: %s\n", message);
		exit(100);
	}
	else
	{
		fprintf(stderr, "MAPM Warning: %s\n", message);
	}
}
/****************************************************************************/

