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

    Copyright (C) 2007 David Harvey, William Hart
    Copyright (C) 2010 Sebastian Pancratz
    Copyright (C) 2013 Tom Bachmann (C++ adaptation)

******************************************************************************/

/*
    Demo FLINT program for computing the q-expansion of the delta function.
*/

#include <iostream>
#include <cstdlib>
#include "fmpzxx.h"
#include "arithxx.h"

using namespace flint;
using namespace std;

int main(int argc, char* argv[])
{
    slong N = 0;

    if (argc == 2)
        N = atol(argv[1]);

    if (argc != 2 || N < 1)
    {
        flint_printf("Syntax: delta_qexp <integer>\n");
        flint_printf("where <integer> is the (positive) number of terms to compute\n");
        return 1;
    }

    std::cout << "Coefficient of q^" << N << " is "
              << ramanujan_tau(fmpzxx(N)) << '\n';

    return 0;
}

