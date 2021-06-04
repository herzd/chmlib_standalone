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
#ifndef int32___EG_DBASIS_REDUCTION__
#define int32___EG_DBASIS_REDUCTION__
#include "eg_config.h"
#include "eg_macros.h"
#include "eg_mem.h"
#include "eg_lpnum.h"
#include "int32_eg_dmatrix.h"
#include "int32_eg_numutil.h"
/* ========================================================================= */
/** @defgroup EGdBasisRed LLL Basis Reduction
 * Here we define a common interface for dense matrices (i.e. a structure), and
 * some common operations over dense matrices. The definition uses EGlpNum as
 * reference number type, this allow for template initializations.
 * 
 * @par History:
 * Revision 0.0.2
 *  - 2005-10-28
 *  					- First implementation.
 * */
/**  @{ */
/** @file
 * @brief This file provide the user interface and function definitions for
 * the so-called LLL Basis Reduction Algorithm. This algorithm was first
 * presented in the paper "Factoring polynomials with rational coefficients",
 * Mathematische Annalen 261 (1981), p515-534. and has been extensivelly
 * studied elsewere. for more details just Google-it.
 * */
/** @example int32_eg_dmatrix.ex.c */
/* ========================================================================= */
/** @brief verbosity level */
#define int32_EG_DBSRED_VERBOSE 0

/* ========================================================================= */
/** @name Profiling structures and functions for the basis reduction algorithm.
 * */
/* @{ */
/* ========================================================================= */
/** @brief where to hold the profile information */
extern uintmax_t int32_EGdBsRedStats[10];

/* ========================================================================= */
/** @brief where we store the number of calls to #int32_EGdBsRed */
#define int32_EG_BSRED_CALLS 0

/* ========================================================================= */
/** @brief where we store the total number of size reductions performed in 
 * #int32_EGdBsRed */
#define int32_EG_BSRED_SZRED 1

/* ========================================================================= */
/** @brief where we store the total number of interchanges performed in 
 * #int32_EGdBsRed */
#define int32_EG_BSRED_INTR 2

/* ========================================================================= */
/** @brief where we store the total number of innermost loops performed in 
 * #int32_EGdBsRed */
#define int32_EG_BSRED_ITT 3

/* ========================================================================= */
/** @brief Print into the given file stream, the current statistics related
 * to the #int32_EGdBsRed algorithm. And reset all counters to zero.
 * @param __ofile where we want to print the profile information. */
#define int32_EGdBsRedProfile(__ofile) do{\
	fprintf(__ofile,"LLL Basis Reduction Statistics:\n");\
	fprintf(__ofile,"\tNumber Calls    : %ju\n", int32_EGdBsRedStats[int32_EG_BSRED_CALLS]);\
	fprintf(__ofile,"\tLoops           : %ju\n", int32_EGdBsRedStats[int32_EG_BSRED_ITT]);\
	fprintf(__ofile,"\tSize Reductions : %ju\n", int32_EGdBsRedStats[int32_EG_BSRED_SZRED]);\
	fprintf(__ofile,"\tInterchanges    : %ju\n", int32_EGdBsRedStats[int32_EG_BSRED_INTR]);\
	memset(int32_EGdBsRedStats,0,sizeof(int32_EGdBsRedStats));} while(0)

/* @} */

/* ========================================================================= */
/** @brief Value used in condition two of the LLL algorithm, remember that this
 * number should be between \f$(1/4,1)\f$. By default we choose \f$\lambda =
 * \frac{2^{20}-1}{2^{20}} \approx .99999904632568359375 \f$. */
#define int32_EG_DBSRED_ALPHA 0x7ffffp-20

/* ========================================================================= */
/** @brief structure to hold all necesary data to perform the LLL's basis
 * reduction algorithm. */
typedef struct int32_EGdBsRed_t
{
	size_t dim;				/**< @brief Number of elements in the basis */
	size_t length;		/**< @brief Length of the vectors in the basis, note that
												 it should be that length >= dim */
	size_t basis_sz;	/**< @brief Actual length of the #int32_EGdBsRed_t::basis 
												 array */
	int32_t **basis;/**< @brief array of pointers to arrays containing the 
												 vector basis in extended (including zero coef) form. 
												 The vectors themselves are considered as allocated 
												 outside. everything else is considered as internally
												 allocated. */
	int32_EGdMatrix_t GM;		/**< @brief Here we store and compute the Gram-Schmidt 
												 needed for the LLL basis reduction algorithm */
}
int32_EGdBsRed_t;

/* ========================================================================= */
/** @brief Initialize an #int32_EGdBsRed_t structure, as a basis with zero elements
 * of dimension zero.
 * @param __bsred pointer to an #int32_EGdBsRed_t structure.
 * */
#define int32_EGdBsRedInit(__bsred) do{\
	int32_EGdBsRed_t*const __EGdbs = (__bsred);\
	memset(__EGdbs,0,sizeof(int32_EGdBsRed_t));\
	int32_EGdMatrixInit(&(__EGdbs->GM));} while(0)

/* ========================================================================= */
/** @brief Free any internally allocated memory in a #int32_EGdBsRed_t structure.
 * @param __bsred pointer to an #int32_EGdBsRed_t structure.
 * */
#define int32_EGdBsRedClear(__bsred) do{\
	int32_EGdBsRed_t*const __EGdbs = (__bsred);\
	if(__EGdbs->basis) EGfree(__EGdbs->basis);\
	int32_EGdMatrixClear(&(__EGdbs->GM));} while(0)

/* ========================================================================= */
/** @brief reset an #int32_EGdBsRed_t structure as a basis without elements (note
 * that we do not reset the length of the vectors, just the number of vectors 
 * in the basis).
 * @param __bsred pointer to an #int32_EGdBsRed_t structure.
 * */
#define int32_EGdBsRedReset(__bsred) ((__bsred)->dim = 0)

/* ========================================================================= */
/** @brief set the length of the vectors used in the basis for an #int32_EGdBsRed_t
 * structure.
 * @param __bsred pointer to an #int32_EGdBsRed_t structure.
 * @param __new_length length of the vectors in the basis.
 * */
#define int32_EGdBsRedSetLength(__bsred,__new_length) ((__bsred)->length = (__new_length))

/* ========================================================================= */
/** @brief add a new vector to the basis.
 * @param __bsred pointer to an #int32_EGdBsRed_t structure.
 * @param __new_elem new vector to add to the basis.
 * */
#define int32_EGdBsRedAddElement(__bsred,__new_elem) do{\
	int32_EGdBsRed_t*const __EGdbs = (__bsred);\
	if(__EGdbs->basis_sz <= __EGdbs->dim){\
		__EGdbs->basis_sz += 10U;\
		EGrealloc(__EGdbs->basis,sizeof(int32_t*)*__EGdbs->basis_sz);}\
	__EGdbs->basis[__EGdbs->dim++] = (__new_elem);} while(0)

/* ========================================================================= */
/** @brief This function performs the so-called LLL basis reduction algorithm.
 * @param __bsred pointer to an #int32_EGdBsRed_t structure.
 * @param status where we return the status of the algorithm, if the algorithm
 * finish with non-zero reduced elements, the status is #EG_ALGSTAT_SUCCESS. if
 * the algorithm finish with some zero reduced vector, the status is
 * #EG_ALGSTAT_PARTIAL. if the algorithm stop because of numerical problems,
 * the status is #EG_ALGSTAT_NUMERROR.
 * @param zero_tol threshold for a number to be considered as zero.
 * @param dim pointer to a number where we return the dimension of the basis
 * that the algorithm could prove before running in any numerical problem. If
 * the algorithm stop with status #EG_ALGSTAT_SUCCESS, then this number should
 * be equal to #int32_EGdBsRed_t::dim. The vectors that we finish reducing are stored
 * in #int32_EGdMatrix_t::row_ord[0], ... , #int32_EGdMatrix_t::row_ord[dim], in the
 * #int32_EGdBsRed_t::GM matrix.
 * @return zero if the algorithm finish, non-zero if an unforeseen error occure
 * during execution. 
 * @par Details:
 * The implementation that we use introduce (as an heuristic step) the sorting
 * of the original basis vectors in increasing order according to their norms,
 * this simple step reduced the total running time of the algorithm, but does
 * not improve the theoretical running time. A second detail is that we only
 * compute the Gram-Schmidth coefficients only once (at the beggining of the
 * program), and then, we only update the changed entries for both operations
 * \a size \a reduction and \a interchange. The advantage of the approach is
 * that we save most Gram-Schmidth computations and also all the recomputations
 * of the inner products of the elements currently in the basis. Again, this
 * are improvements form the practical point of view, but not in practice. The
 * dissadvantage of this approach is that we do accumulate rounding errors in
 * the Gram-Schmidth coefficients allong the way, but if all original vectors
 * coefficients where integer (and not too big), then the error should not grow
 * too much. Still this may happen if the input basis is ill conditioned.
 * */
int int32_EGdBsRed (int32_EGdBsRed_t * const __bsred,
							unsigned *const dim,
							const int32_t zero_tol,
							int *const status);

/* ========================================================================= */
/**  @} */
#endif
