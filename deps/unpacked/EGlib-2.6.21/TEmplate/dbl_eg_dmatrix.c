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
#include "dbl_eg_dmatrix.h"
/** @file
 * @ingroup EGdMatrix */
/** @addtogroup EGdMatrix */
/** @{ */
/* ========================================================================= */
int dbl_EGdMatrixGaussianElimination (dbl_EGdMatrix_t * const dmatrix,
																	const unsigned do_col_perm,
																	const unsigned do_row_perm,
																	unsigned *const ext_rank,
																	const double zero_tol,
																	int *const ext_status)
{
	const size_t col_sz = dmatrix->col_sz;
	const size_t row_sz = dmatrix->row_sz;
	int *const col_ord = dmatrix->col_ord;
	int *const row_ord = dmatrix->row_ord;
	double **const rowval = dmatrix->matrow;
	double pivot,
	  tmpval;
	int rval = 0,
	  tmpint = 0,
	  tmpint2 = 0;
	int status = EG_ALGSTAT_SUCCESS;
	size_t rank = 0;
	size_t prow,
	  pcol;
	register size_t i,
	  j;
	dbl_EGlpNumInitVar (pivot);
	dbl_EGlpNumInitVar (tmpval);
	/* main loop */
	while (1)
	{
		/* test if we are done */
		if (rank == col_sz || rank == row_sz)
			break;
		/* choose wich pivot to do */
		pcol = rank;
		prow = rank;
		dbl_EGlpNumCopyAbs (pivot, rowval[row_ord[prow]][col_ord[pcol]]);
		/* in this case we are forced to choose as pivot element a_kk */
		if (!do_col_perm && !do_row_perm)
		{
			if (!dbl_EGlpNumIsNeqZero (pivot, zero_tol))
			{
				status = EG_ALGSTAT_NUMERROR;
				break;
			}
		}
		/* in this case we choose the best possible pivot value as the largest
		 * entry in the remaining sub-matrix */
		else if (do_col_perm && do_row_perm)
		{
			for (i = rank; i < row_sz; i++)
				for (j = rank; j < col_sz; j++)
				{
					dbl_EGlpNumCopyAbs (tmpval, rowval[row_ord[i]][col_ord[j]]);
					if (dbl_EGlpNumIsLess (pivot, tmpval))
					{
						pcol = j;
						prow = i;
						dbl_EGlpNumCopy (pivot, tmpval);
					}
				}
			if (!dbl_EGlpNumIsNeqZero (pivot, zero_tol))
			{
				status = EG_ALGSTAT_PARTIAL;
				break;
			}
		}
		/* in this case we choose the best possible value as the largest entry in
		 * the remaining row */
		else if (do_col_perm && !do_row_perm)
		{
			tmpint = row_ord[prow];
			for (j = rank; j < col_sz; j++)
			{
				dbl_EGlpNumCopyAbs (tmpval, rowval[tmpint][col_ord[j]]);
				if (dbl_EGlpNumIsLess (pivot, tmpval))
				{
					pcol = j;
					dbl_EGlpNumCopy (pivot, tmpval);
				}
			}
			if (!dbl_EGlpNumIsNeqZero (pivot, zero_tol))
			{
				status = EG_ALGSTAT_NUMERROR;
				break;
			}
		}
		/* in this case we choose the best possible value as the largest entry in
		 * the remaining column */
		else
		{
			tmpint = col_ord[pcol];
			for (i = rank; i < row_sz; i++)
			{
				dbl_EGlpNumCopyAbs (tmpval, rowval[row_ord[i]][tmpint]);
				if (dbl_EGlpNumIsLess (pivot, tmpval))
				{
					prow = i;
					dbl_EGlpNumCopy (pivot, tmpval);
				}
			}
			if (!dbl_EGlpNumIsNeqZero (pivot, zero_tol))
			{
				status = EG_ALGSTAT_NUMERROR;
				break;
			}
		}
		/* exchange row/columns if neccesary */
		if (pcol != rank)
		{
			tmpint = col_ord[rank];
			col_ord[rank] = col_ord[pcol];
			col_ord[pcol] = tmpint;
		}
		if (prow != rank)
		{
			tmpint = row_ord[rank];
			row_ord[rank] = row_ord[prow];
			row_ord[prow] = tmpint;
		}
		/* now we are done choosing a non-zero pivot, and permutating columns 
		 * and or rows */
		dbl_EGlpNumCopy (pivot, rowval[row_ord[rank]][col_ord[rank]]);
		/* now we do de pivot */
		tmpint = row_ord[rank];
		for (i = rank + 1; i < row_sz; i++)
		{
			tmpint2 = row_ord[i];
			dbl_EGlpNumCopy (tmpval, rowval[tmpint2][col_ord[rank]]);
			/* check wether we don't need to update this row */
			if (dbl_EGlpNumIsNeqZero (tmpval, zero_tol))
			{
				/* compute the row multiplier */
				dbl_EGlpNumDivTo (tmpval, pivot);
				/* set row_i = row_i - tmpval row_rank */
				dbl_EGlpNumCopy (rowval[tmpint2][col_ord[rank]], dbl_zeroLpNum);
				for (j = rank + 1; j < col_sz; j++)
					dbl_EGlpNumSubInnProdTo (rowval[tmpint2][col_ord[j]], tmpval,
															 rowval[tmpint][col_ord[j]]);
			}
		}
		/* increase the proven rank */
		rank++;
	}
	/* ending */
	dbl_EGlpNumClearVar (tmpval);
	dbl_EGlpNumClearVar (pivot);
	*ext_rank = (unsigned)rank;
	*ext_status = status;
	return rval;
}

/* ========================================================================= */
/** @} */
