/*-
 * Copyright (c) 1997 FreeBSD Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id: timelocal.c,v 1.2 1998/09/16 04:17:45 imp Exp $
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <fcntl.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "setlocale.h"
#include "timelocal.h"

struct lc_time_T _time_localebuf;
int _time_using_locale;

const struct lc_time_T	_C_time_locale = {
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	}, {
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	}, {
		"Sun", "Mon", "Tue", "Wed",
		"Thu", "Fri", "Sat"
	}, {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
	},

	/* X_fmt */
	"%H:%M:%S",

	/*
	** x_fmt
	** Since the C language standard calls for
	** "date, using locale's date format," anything goes.
	** Using just numbers (as here) makes Quakers happier;
	** it's also compatible with SVR4.
	*/
	"%m/%d/%y",

	/*
	** c_fmt (ctime-compatible)
	** Note that
	**	"%a %b %d %H:%M:%S %Y"
	** is used by Solaris 2.3.
	*/
	"%a %b %e %X %Y",

	/* am */
	"AM",

	/* pm */
	"PM",

	/* date_fmt */
	"%a %b %e %X %Z %Y"
};


int
__time_load_locale(const char *name)
{
	static char *		locale_buf;
	static char		locale_buf_C[] = "C";

	int			fd;
	char *			lbuf;
	char *			p;
	const char **		ap;
	const char *		plim;
	char                    filename[PATH_MAX];
	struct stat		st;
	size_t			namesize;
	size_t			bufsize;
	int                     save_using_locale;

	save_using_locale = _time_using_locale;
	_time_using_locale = 0;

	if (name == NULL)
		goto no_locale;

	if (!strcmp(name, "C") || !strcmp(name, "POSIX"))
		return 0;

	/*
	** If the locale name is the same as our cache, use the cache.
	*/
	lbuf = locale_buf;
	if (lbuf != NULL && strcmp(name, lbuf) == 0) {
		p = lbuf;
		for (ap = (const char **) &_time_localebuf;
			ap < (const char **) (&_time_localebuf + 1);
				++ap)
					*ap = p += strlen(p) + 1;
		_time_using_locale = 1;
		return 0;
	}
	/*
	** Slurp the locale file into the cache.
	*/
	namesize = strlen(name) + 1;

	if (!_PathLocale)
		goto no_locale;
	/* Range checking not needed, 'name' size is limited */
	strcpy(filename, _PathLocale);
	strcat(filename, "/");
	strcat(filename, name);
	strcat(filename, "/LC_TIME");
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		goto no_locale;
	if (fstat(fd, &st) != 0)
		goto bad_locale;
	if (st.st_size <= 0)
		goto bad_locale;
	bufsize = namesize + st.st_size;
	locale_buf = NULL;
	lbuf = (lbuf == NULL || lbuf == locale_buf_C) ?
		malloc(bufsize) : reallocf(lbuf, bufsize);
	if (lbuf == NULL)
		goto bad_locale;
	(void) strcpy(lbuf, name);
	p = lbuf + namesize;
	plim = p + st.st_size;
	if (read(fd, p, (size_t) st.st_size) != st.st_size)
		goto bad_lbuf;
	if (close(fd) != 0)
		goto bad_lbuf;
	/*
	** Parse the locale file into localebuf.
	*/
	if (plim[-1] != '\n')
		goto bad_lbuf;
	for (ap = (const char **) &_time_localebuf;
		ap < (const char **) (&_time_localebuf + 1);
			++ap) {
				if (p == plim)
					goto reset_locale;
				*ap = p;
				while (*p != '\n')
					++p;
				*p++ = '\0';
	}
	/*
	** Record the successful parse in the cache.
	*/
	locale_buf = lbuf;

	_time_using_locale = 1;
	return 0;

reset_locale:
	/*
	 * XXX - This may not be the correct thing to do in this case.
	 * setlocale() assumes that we left the old locale alone.
	 */
	locale_buf = locale_buf_C;
	_time_localebuf = _C_time_locale;
	save_using_locale = 0;
bad_lbuf:
	free(lbuf);
bad_locale:
	(void) close(fd);
no_locale:
	_time_using_locale = save_using_locale;
	return -1;
}
