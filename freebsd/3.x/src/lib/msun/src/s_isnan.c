/* We've already got an isnan in libc.x */
#ifndef OSKIT
/* @(#)s_isnan.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#ifndef lint
static char rcsid[] = "$Id: s_isnan.c,v 1.4 1997/02/22 15:11:23 peter Exp $";
#endif

/*
 * isnan(x) returns 1 is x is nan, else 0;
 * no branching!
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	int isnan(double x)
#else
	int isnan(x)
	double x;
#endif
{
	int32_t hx,lx;
	EXTRACT_WORDS(hx,lx,x);
	hx &= 0x7fffffff;
	hx |= (u_int32_t)(lx|(-lx))>>31;
	hx = 0x7ff00000 - hx;
	return (int)((u_int32_t)(hx))>>31;
}
#endif
