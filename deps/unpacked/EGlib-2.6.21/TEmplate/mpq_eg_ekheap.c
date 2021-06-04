/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005 Daniel Espinoza and Marcos Goycoolea.
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public 
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 
 */
#include "mpq_eg_ekheap.h"
/** @file 
 * @ingroup EGeKHeap */
/** @addtogroup EGeKHeap */
/** @{ */
/* ========================================================================= */
void mpq_EGeKHeapCopyVal(mpq_t*const dst, const mpq_t*const src)
{
	register int i = mpq_EG_EKHEAP_ENTRY;
	for( ; i-- ; )
	{
		mpq_EGlpNumCopy(dst[i],src[i]);
	}
}
/* ========================================================================= */
/** @} */
