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
/** @file 
 * @ingroup EGaHeap */
/** @addtogroup EGaHeap */
/** @{ */
/* ========================================================================= */
/** @brief This program reads a list of double values from a text file and 
 * uses a heap to sort them. It also allows the user to change the value of 
 * one of the read numbers after they have been placed in the heap. The 
 * purpose of this program is to illustrate the use of the @ref EGaHeap 
 * structure and its associated functions. 
 * 
 * The input file format is: 
 * n
 * Value_0
 * Value_1
 * Value_2
 * Value_3
 *   ...  
 * Value_n 
 * */
/* ========================================================================= */
#include "EGlib.h"

/* ========================================================================= */
/** @brief display the usage message for this program */
void mpq_aheap_usage (char *program)
{
	fprintf (stdout, "Usage: %s [options]\n", program);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -d n   'd' value.\n");
	fprintf (stdout, "     -f n   file name.\n");
	fprintf (stdout, "     -c n   item whose value to change.\n");
	fprintf (stdout, "     -v n   new item value (0 by default).\n");

}

/* ========================================================================= */
/** @brief parse the input argumenbts for the program */
int mpq_aheap_parseargs (int argc,
										 char **argv,
										 unsigned int *d,
										 unsigned int *ch,
										 mpq_t * v,
										 char **mpq_file_name)
{
	int c;
	while ((c = getopt (argc, argv, "f:d:c:v:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			*mpq_file_name = optarg;
			break;
		case 'd':
			*d = atoi (optarg);
			break;
		case 'c':
			*ch = atoi (optarg);
			break;
		case 'v':
			mpq_EGlpNumReadStr (*v, optarg);
			break;
		default:
			mpq_aheap_usage (argv[0]);
			return 1;
		}
	}
	/* reporting the options */
	if (!*mpq_file_name)
	{
		mpq_aheap_usage (argv[0]);
		return 1;
	}
	fprintf (stdout, "Parsed Options:\n");
	fprintf (stdout, "input         : %s\n", *mpq_file_name);
	fprintf (stdout, "d             : %u\n", *d);
	if (*ch != UINT_MAX)
	{
		fprintf (stdout, "c             : %u\n", *ch);
		fprintf (stdout, "v             : %lf\n", mpq_EGlpNumToLf (*v));
	}
	return 0;
}

/* ========================================================================= */
/** @brief main function */
int main (int argc,
					char **argv)
{

	int rval = 0;
	unsigned int i,
	  c = UINT_MAX,
	  d = 2;
	char *mpq_file_name = 0,
	  str1[1024];
	FILE *file;
	unsigned int nval,ccn;
	mpq_t v;
	mpq_EGaHeap_t my_heap;
	mpq_EGaHeapCn_t *all_cn = 0;
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	mpq_EGlpNumInitVar (v);
	mpq_EGlpNumZero (v);

	/* Parse command line input to get 'file name' and 'd'. */
	EGcallD(mpq_aheap_parseargs(argc, argv, &d, &c, &v, &mpq_file_name));
	mpq_EGaHeapInit (&my_heap,d);

	/* Read the objects to sort from the file */
	file = fopen (mpq_file_name, "r");
	TEST (!file, "unable to open file %s", mpq_file_name);
	fscanf (file, "%u", &nval);
	all_cn=EGsMalloc(mpq_EGaHeapCn_t, nval);
	mpq_EGaHeapResize(&my_heap, nval);
	IFMESSAGE (1, "Inserting %u elements into the heap", nval);
	for (i = 0; i < nval; i++)
	{
		mpq_EGaHeapCnInit(all_cn+i);
		fscanf (file, "%s", str1);
		mpq_EGlpNumReadStr (all_cn[i].val, str1);
		IFMESSAGE (1, "Adding value (%s,%lg) to the heap", str1, mpq_EGlpNumToLf (all_cn[i].val));
		mpq_EGaHeapAdd (all_cn,sizeof(mpq_EGaHeapCn_t),&my_heap,i);
	}
	fclose (file);

	/* Check if change value is in range */
	TESTG (c != UINT_MAX && c >= nval, CLEANUP,
				 "Change item (%u) is out of range (only %u objects)", c, nval);

	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		ccn = mpq_EGaHeapGetMin (&my_heap);
		IFMESSAGE (1, "%u: item %u : %lg", i, ccn, mpq_EGlpNumToLf (all_cn[ccn].val));
		mpq_EGaHeapDel (all_cn,sizeof(mpq_EGaHeapCn_t),&my_heap, ccn);
	}

	if (c == UINT_MAX)
		goto CLEANUP;

	/* Re-inserting the values into the heap */
	fprintf (stderr, "\nRe-Inserting.\n\n");
	for (i = 0; i < nval; i++) mpq_EGaHeapAdd (all_cn,sizeof(mpq_EGaHeapCn_t),&my_heap, i);

	/* Changing value of an item */
	fprintf (stderr, "Changing value of item %u from %lf to %lf.\n", c, mpq_EGlpNumToLf (all_cn[c].val), mpq_EGlpNumToLf (v));
	mpq_EGaHeapChangeVal (all_cn,sizeof(mpq_EGaHeapCn_t),&my_heap,c,v);
	/* Popping the values from the heap */
	fprintf (stderr, "\nRemoving:\n\n");
	for (i = 0; i < nval; i++)
	{
		ccn = my_heap.cn[my_heap.sz-1];
		IFMESSAGE (1, "%u: item %u : %lf", i, ccn, mpq_EGlpNumToLf (all_cn[ccn].val));
		mpq_EGaHeapDel (all_cn,sizeof(mpq_EGaHeapCn_t),&my_heap, ccn);
	}

	/* Liberating allocated memory */
CLEANUP:
	if (all_cn) EGfree(all_cn);
	mpq_EGaHeapClear(&my_heap);
	mpq_EGlpNumClearVar(v);
	EGlpNumClear();
	return rval;
}
/* ========================================================================= */
/** @} */
