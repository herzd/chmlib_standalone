/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2010 William Hart
    Copyright (C) 2011 Fredrik Johansson
    Copyright (C) 2014 Abhinav Baid

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "d_mat.h"
#include "ulong_extras.h"

int
main(void)
{
    int i;
    FLINT_TEST_INIT(state);


    flint_printf("entry....");
    fflush(stdout);

    /* check if entries are accessed correctly by setting their values
       and then comparing them */
    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        d_mat_t A;
        slong j, k;
        slong rows = n_randint(state, 10);
        slong cols = n_randint(state, 10);

        d_mat_init(A, rows, cols);

        for (j = 0; j < rows; j++)
        {
            for (k = 0; k < cols; k++)
            {
                d_mat_entry(A, j, k) = 3 * j + 7 * k;
            }
        }

        for (j = 0; j < rows; j++)
        {
            for (k = 0; k < cols; k++)
            {
                if (d_mat_entry(A, j, k) != 3 * j + 7 * k)
                {
                    flint_printf("FAIL: get/set entry %wd, %wd\n", j, k);
                    abort();
                }
            }
        }

        d_mat_clear(A);
    }



    FLINT_TEST_CLEANUP(state);
    flint_printf("PASS\n");
    return 0;
}
