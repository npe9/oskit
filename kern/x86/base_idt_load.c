/*
 * Copyright (c) 1996, 1998 University of Utah and the Flux Group.
 * All rights reserved.
 * 
 * This file is part of the Flux OSKit.  The OSKit is free software, also known
 * as "open source;" you can redistribute it and/or modify it under the terms
 * of the GNU General Public License (GPL), version 2, as published by the Free
 * Software Foundation (FSF).  To explore alternate licensing terms, contact
 * the University of Utah at csl-dist@cs.utah.edu or +1-801-585-3271.
 * 
 * The OSKit is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GPL for more details.  You should have
 * received a copy of the GPL along with the OSKit; see the file COPYING.  If
 * not, write to the FSF, 59 Temple Place #330, Boston, MA 02111-1307, USA.
 */

#include <oskit/x86/proc_reg.h>
#include <oskit/x86/base_vm.h>
#include <oskit/x86/base_idt.h>

void base_idt_load(void)
{
	struct pseudo_descriptor pdesc;

	/* Create a pseudo-descriptor describing the GDT.  */
	pdesc.limit = sizeof(base_idt) - 1;
	pdesc.linear_base = kvtolin(&base_idt);

	/* Load the IDT.  */
	set_idt(&pdesc);
}
