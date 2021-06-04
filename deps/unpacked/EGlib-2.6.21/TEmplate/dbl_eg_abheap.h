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
 * */
/* ========================================================================= */
/** @defgroup EGaBheap EGaBheap
 *
 * Here we define the basic interface for binary heaps as an array-based 
 * structure.
 * In this implementation the heap does not grow on the fly, meaning that it 
 * may fill-up during an add call, to avoid that, the user must 
 * re-allocate when necesary. the heap start as a heap of size zero. 
 * This implementatioon is a minimum-heap implementatiton.
 *
 * @version 0.0.1
 * @par History:
 * - 2013-04-12
 * 						- First implementation, starting from base dbl_EGeHeap_t 
 * @note 
 * This implementatiton is designed as a template using as base the types of
 * @ref EGlpNum
 * */
/** @file 
 * @ingroup EGaBheap */
/** @addtogroup EGaBheap */
/** @{ */
/** @example dbl_eg_abheap.ex.c
 * This is a simple example of the usage of heaps using @ref EGaBheap */
/* ========================================================================= */
#ifndef dbl___EG_ABHEAP__
#define dbl___EG_ABHEAP__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_lpnum.h"
/* ========================================================================= */
/** @brief Debug level for the heap */
#ifndef dbl_EG_ABHEAP_DEBUG
#define dbl_EG_ABHEAP_DEBUG 1000
#endif

/* ========================================================================= */
/** @name Test macros, enabled only if debug level is high enough. */
/** @{ */
#if dbl_EG_ABHEAP_DEBUG <= DEBUG
#define dbl_EGaBheapCHECK_CN(__hp2,__hcn2) EXIT(__hcn2->pos>=__hp2->sz,"Heap Connector possition %d out of range [0,%d]",__hcn2->pos,__hp2->sz)
#define dbl_EGaBheapCHECK_NF(__hp2) EXIT(__hp2->sz >= __hp2->max_sz,"Heap "#__hp2" Is full, can't add an element")
#else
#define dbl_EGaBheapCHECK_CN(__hp,__hcn) 
#define dbl_EGaBheapCHECK_NF(__hp) 
#endif
/** @} */
/* ========================================================================= */
/** @brief Structure to store the information relevant to an element in the
 * heap. */
typedef struct 
{
	double val;		/**< Value of this node in the heap */
	uint32_t pos;					/**< Position in the heap-array for this node, if set to
												 #dbl_EG_ABHEAP_POISON, then the connector is not in any 
												 heap.*/
}
dbl_EGaBheapCn_t;

/* ========================================================================= */
/** @brief Poison position for heap connector not in a heap. */
#define dbl_EG_ABHEAP_POISON UINT32_MAX

/* ========================================================================= */
/** @brief Initialize a heap conector structure. This function will allocate any
 * interal memory not allocated by the user, it should be called only once, or
 * after a clear function call.
 * @param __hcn conector to initialize.
 * */
#define dbl_EGaBheapCnInit(__hcn) do{dbl_EGlpNumInitVar((__hcn)->val);(__hcn)->pos = dbl_EG_ABHEAP_POISON;}while(0)

/* ========================================================================= */
/** @brief Reset a heap conector to the same state as after an init call, this
 * function is provided only for completness.
 * @param __hcn conector to reset
 * */
#define dbl_EGaBheapCnReset(__hcn) ((__hcn)->pos = dbl_EG_ABHEAP_POISON)

/* ========================================================================= */
/** @brief Free all internal memory used by this structured not allocated by the
 * user. This function should be called after an init call, and only once.
 * @param __hcn conector to clear.
 * */
#define dbl_EGaBheapCnClear(__hcn) dbl_EGlpNumClearVar((__hcn)->val)

/* ========================================================================= */
/** @brief Structure to hold a whole heap structure, this structure is designed
 * so that it can grow on the fly with a low cost */
typedef struct 
{
	uint32_t*cn;							/**<@brief position in the base array of the connector */
	uint32_t sz;
	uint32_t max_sz;
}
dbl_EGaBheap_t;

/* ========================================================================= */
/** @brief Return one if the heap is full, zero otherwise.
 * @param __hp heat to check */
#define dbl_EGaBheapIsFull(__hp) ({dbl_EGaBheap_t*const __EGehp = (__hp); __EGehp->sz == __EGehp->max_sz;})

/* ========================================================================= */
/** @brief set the number of elements in hte heap to zero.
 * @param __hp heap to empty.
 * */
#define dbl_EGaBheapEmpty(__hp) ((__hp)->sz = 0)

/* ========================================================================= */
/** @brief Initialize a heap as an empty heap (with no space for conectors).
 * @param __hp heap to initialize.
 * */
#define dbl_EGaBheapInit(__hp) (*(__hp) = (dbl_EGaBheap_t){0,0,0})

/* ========================================================================= */
/** @brief Reset the given heap as an empty heap (just as returned by the init
 * call.
 * @param __hp heap to reset 
 * */
#define dbl_EGaBheapReset(__hp) dbl_EGaBheapResize(__hp,0)

/* ========================================================================= */
/** @brief Clear a heap structure, and free any internal memory (not allocated
 * by the user).
 * @param __hp heap to clear.
 * */
#define dbl_EGaBheapClear(__hp) dbl_EGaBheapResize(__hp,0)

/* ========================================================================= */
/** @brief get the minimum conector in the heap, if the heap is empty, return
 * dbl_EG_ABHEAP_POISON.
 * @param __hp eap where we are working.
 * @return position (in the base array) of the minimum element in the heap.
 * */
#define dbl_EGaBheapGetMin(__hp) ({\
	dbl_EGaBheap_t*const __EGehp = (__hp);\
	__EGehp->sz ? __EGehp->cn[0] : dbl_EG_ABHEAP_POISON;})

/* ========================================================================= */
/** @brief resize the heap cn array to the given size, if the new size is zero,
 * it is equivalent to free the internal memory, and left the heap as an empty
 * heap with zero space.
 * @param __hp heap where we are working.
 * @param __new_sz new size for the  cn array .
 * */
#define dbl_EGaBheapResize(__hp,__new_sz) ({\
	dbl_EGaBheap_t*const __EGehp = (__hp);\
	const uint32_t __EGehp_nsz = (uint32_t)(__new_sz);\
	__EGehp->cn = EGrealloc((__EGehp->cn),((size_t)(__EGehp_nsz*sizeof(uint32_t))));\
	__EGehp->max_sz = __EGehp_nsz;})

/* ========================================================================= */
/** @brief return the index of the father of the given index.
 * @param __id position in the heap-array to wich we want to compute it's father.
 * */
#define dbl_EGaBheapFatherId(__id) ((uint32_t)((__id)?(((uint32_t)((__id)-1))>>1U):0))

/* ========================================================================= */
/** @brief move an element in the heap up in the heap (position 0 is the top,
 * this kind of move is neded whenever we decrease the value in a heap element).
 * @note this is an INTERNAL function; and MUST NOT be called directly by
 * users.
 * @param __pbase3 pointer to he base array of connectors.
 * @param __psz3 size of each structure holding the heap connector sub-structure
 * @param __hp3 heap where we are working.
 * @param __hcn3 element in the heap to move.
 * */
#define dbl___EGaBheapSiftUp(__pbase3,__psz3,__hp3,__hcn3) do{\
	char*const __EGbase3=((char*)(__pbase3));\
	const uint32_t __EGstsz3=((uint32_t)(__psz3));\
	dbl_EGaBheap_t*const __EGehp3 = (__hp3);\
	const uint32_t __EGapos3 = __EGehp3->cn[__hcn3];\
	dbl_EGaBheapCn_t*const __EGecn3 = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGapos3)));\
	uint32_t __EGcpos3 = __EGecn3->pos;\
	uint32_t __EGfpos3 = dbl_EGaBheapFatherId(__EGcpos3);\
	dbl_EGaBheapCn_t*__EGfcn3 = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGehp3->cn[__EGfpos3])));\
	dbl_EGaBheapCHECK_CN(__EGehp3,__EGecn3);\
	while(__EGcpos3 && \
				dbl_EGlpNumIsLess(__EGecn3->val,__EGfcn3->val))\
	{\
		__EGfcn3->pos = __EGcpos3;\
		__EGehp3->cn[__EGcpos3] = ((uint32_t)(EGaGetPos(__EGbase3,__EGstsz3,__EGfcn3)));\
		__EGcpos3 = __EGfpos3;\
		__EGfpos3 = dbl_EGaBheapFatherId(__EGcpos3);\
		__EGfcn3 = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase3,__EGstsz3,__EGehp3->cn[__EGfpos3])));\
	}\
	__EGecn3->pos = __EGcpos3;\
	__EGehp3->cn[__EGcpos3] = __EGapos3;\
	}while(0)
/* ========================================================================= */
/** @brief Add an element to the heap
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where to add the element.
 * @param __hcn element to be added.
 * */
#define dbl_EGaBheapAdd(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const uint32_t __EGstsz2=((uint32_t)(__psz));\
	dbl_EGaBheap_t*const __EGlhp = (__hp);\
	const uint32_t __EGapos2=(uint32_t)(__hcn);\
	dbl_EGaBheapCn_t*const __EGlcn = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	dbl_EGaBheapCHECK_NF(__EGlhp);\
	__EGlcn->pos=__EGlhp->sz,__EGlhp->cn[__EGlhp->sz]=__EGapos2;\
	__EGlhp->sz +=1;\
	dbl___EGaBheapSiftUp(__EGbase2,__EGstsz2,__EGlhp,__EGlhp->sz-1);}while(0)
/* ========================================================================= */
/** @brief Give the first child for a given position.
 * @param __id position that we want to get the first child.
 * */
#define dbl_EGaBheapFirstChildId(__id) ((uint32_t)(((uint32_t)(__id))<<1U)+1U)
/* ========================================================================= */
/** @brief Move an element down in the heap (position 0 is the
 * top), this kind of operation is needed whenever we increase the value in a
 * heap element.
 * @note this is an INTERNAL function; and MUST NOT be called directly by
 * users.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element in the heap to move.
 * */
#define dbl___EGaBheapSiftDown(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase=((char*)(__pbase));\
	const uint32_t __EGstsz=((uint32_t)(__psz));\
	dbl_EGaBheap_t*const __EGehp = (__hp);\
	const uint32_t __EGapos = __EGehp->cn[__hcn];\
	dbl_EGaBheapCn_t*const __EGecn = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGapos)));\
	const uint32_t __EGhsz = __EGehp->sz;\
	uint32_t __EGcpos = __EGecn->pos;\
	uint32_t __EGfchd = dbl_EGaBheapFirstChildId(__EGcpos);\
	uint32_t __EGlchd = ((uint32_t)(__EGfchd + 2U));\
	dbl_EGaBheapCn_t*__EGcchd=0,*__EGnchd=0;\
	dbl_EGaBheapCHECK_CN(__EGehp,__EGecn);\
	while(__EGfchd < __EGhsz)\
	{\
		/* detect the minimum child */\
		__EGcchd = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGehp->cn[__EGfchd])));\
		if(__EGlchd<=__EGhsz){\
			__EGnchd = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase,__EGstsz,__EGehp->cn[__EGfchd+1])));\
			if(dbl_EGlpNumIsLeq(__EGnchd->val,__EGcchd->val)) __EGcchd=__EGnchd;}\
		/* if the minimum child is less than the current position, move the minimum\
		 * child to the position of the current element */\
		if(dbl_EGlpNumIsLess(__EGcchd->val,__EGecn->val))\
		{\
			__EGfchd = __EGcchd->pos;\
			__EGcchd->pos = __EGcpos, __EGehp->cn[__EGcpos] = ((uint32_t)(EGaGetPos(__EGbase,__EGstsz,__EGcchd)));\
			__EGecn->pos = __EGcpos = __EGfchd, __EGehp->cn[__EGfchd] = ((uint32_t)(EGaGetPos(__EGbase,__EGstsz,__EGecn)));\
			__EGfchd = dbl_EGaBheapFirstChildId(__EGcpos);\
			__EGlchd = ((uint32_t)(__EGfchd + 2U));\
		}\
		/* else we exit the main loop */\
		else __EGfchd = dbl_EG_ABHEAP_POISON;\
	}\
}while(0)

/* ========================================================================= */
/** @brief Change the value of an element in the heap.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element in the heap that we are going to change it's value.
 * @param __new_val new value for the element.
 * */
#define dbl_EGaBheapChangeVal(__pbase,__psz,__hp,__hcn,__new_val) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const uint32_t __EGstsz2=((uint32_t)(__psz));\
	dbl_EGaBheap_t*const __EGlhp=(__hp);\
	const uint32_t __EGapos2=(uint32_t)(__hcn);\
	dbl_EGaBheapCn_t*__EGlcn2=((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	double __EGnum;\
	dbl_EGlpNumCopy(__EGnum,(__new_val));\
	if(dbl_EGlpNumIsLess(__EGnum,__EGlcn2->val)){\
		dbl_EGlpNumCopy(__EGlcn2->val,__EGnum);\
		dbl___EGaBheapSiftUp(__EGbase2,__EGstsz2,__EGlhp,__EGlcn2->pos);}\
	else{\
		dbl_EGlpNumCopy(__EGlcn2->val,__EGnum);\
		dbl___EGaBheapSiftDown(__EGbase2,__EGstsz2,__EGlhp,__EGlcn2->pos);}}while(0)

/* ========================================================================= */
/** @brief Eliminate an element from the heap, note that the position stored in
 * the eliminated element is reset to zero.
 * @param __pbase pointer to he base array of connectors.
 * @param __psz size of each structure holding the heap connector sub-structure
 * @param __hp heap where we are working.
 * @param __hcn element to eliminate from the heap.
 * */
#define dbl_EGaBheapDel(__pbase,__psz,__hp,__hcn) do{\
	char*const __EGbase2=((char*)(__pbase));\
	const uint32_t __EGstsz2=((uint32_t)(__psz));\
	dbl_EGaBheap_t*const __EGlhp = (__hp);\
	const uint32_t __EGapos2=(uint32_t)(__hcn);\
	dbl_EGaBheapCn_t*const __EGlhpcn = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGapos2)));\
	const uint32_t __EGlcn = __EGlhpcn->pos;\
	const uint32_t __EGlhsz = __EGlhp->sz - 1;\
	dbl_EGaBheapCn_t*const __EGllcn = ((dbl_EGaBheapCn_t*)(EGaGetElem(__EGbase2,__EGstsz2,__EGlhp->cn[__EGlhsz])));\
	__EGlhpcn->pos = dbl_EG_ABHEAP_POISON, __EGlhp->sz = __EGlhsz;\
	if(__EGlhsz && __EGlhsz != __EGlcn){\
		__EGlhp->cn[__EGlcn] = __EGlhp->cn[__EGlhsz],__EGllcn->pos = __EGlcn;\
		dbl___EGaBheapSiftDown(__EGbase2,__EGstsz2,__EGlhp,__EGlcn);}\
	__EGlhp->cn[__EGlhp->sz] = dbl_EG_ABHEAP_POISON;}while(0)

/* ========================================================================= */
/** @} */
/* end of dbl_eg_abheap.h */
#endif
