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
 * @ingroup EGalgMinCut */
/** @addtogroup EGalgMinCut */
/** @{ */
#include "EGlib.h"
static char *dbl_file_name = 0;
static unsigned int dbl_lvl = 5;
/* ========================================================================= */
/** @brief display usage of this program */
static void dbl_mc_usage (char **argv)
{
	fprintf (stderr, "Usage: %s [options]\n", argv[0]);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "     -l n   level of padberg-rinaldi shrinking (0-5).\n");
	fprintf (stdout, "     -f n   file name.\n");
}

/* ========================================================================= */
/** @brief structure to store a set of cuts */
typedef struct dbl_mc_all_cuts_t
{
	unsigned int sz;
	unsigned int max_sz;
	double *cut_val;
	unsigned int *cut_sz;
	unsigned int **cut;
}
dbl_mc_all_cuts_t;

/* ========================================================================= */
/** @brief call-back function to store all cuts fund during the execution of
 * the min-cut algorithm. */
int dbl_mc_store_cuts (double value,
									 const unsigned int c_sz,
									 const unsigned int *const cut,
									 void *data)
{
	dbl_mc_all_cuts_t *cuts = (dbl_mc_all_cuts_t *) data;
	/* of the cut pool is too small, enlarge it */
	if (cuts->sz == cuts->max_sz)
	{
		unsigned int *l_cut_sz,
		**l_cut;
		double *l_cut_val;
		cuts->max_sz += 100;
		l_cut_sz = EGsMalloc (unsigned int,
													cuts->max_sz);
		l_cut = EGsMalloc (unsigned int *,
											 cuts->max_sz);
		l_cut_val = EGsMalloc (double, cuts->max_sz);
		if (cuts->sz)
		{

			memcpy (l_cut_sz, cuts->cut_sz, sizeof (unsigned int) * cuts->sz);
			memcpy (l_cut, cuts->cut, sizeof (unsigned int *) * cuts->sz);
			memcpy (l_cut_val, cuts->cut_val, sizeof (double) * cuts->sz);
			EGfree (cuts->cut);
			EGfree (cuts->cut_sz);
			EGfree (cuts->cut_val);
		}
		cuts->cut_sz = l_cut_sz;
		cuts->cut = l_cut;
		cuts->cut_val = l_cut_val;
	}
	/* now we copy the new cut */
	dbl_EGlpNumInitVar (cuts->cut_val[cuts->sz]);
	dbl_EGlpNumCopy (cuts->cut_val[cuts->sz], value);
	cuts->cut_sz[cuts->sz] = c_sz;
	cuts->cut[cuts->sz] = EGsMalloc (unsigned int,
																	 c_sz);
	memcpy (cuts->cut[cuts->sz], cut, sizeof (unsigned int) * c_sz);
	cuts->sz++;
	return 0;
}

/* ========================================================================= */
/** @brief parse input */
static inline int dbl_mc_parseargs (int argc,
																char **argv)
{
	int c;
	while ((c = getopt (argc, argv, "f:l:")) != EOF)
	{
		switch (c)
		{
		case 'f':
			dbl_file_name = optarg;
			break;
		case 'l':
			dbl_lvl = atoi (optarg);
			break;
		default:
			dbl_mc_usage (argv);
			return 1;
		}
	}
	/* test that we have an input file */
	if (!dbl_file_name)
	{
		dbl_mc_usage (argv);
		return 1;
	}
	/* report options */
	fprintf (stderr, "reading graph from %s\nusing shrinking level %u\n",
					 dbl_file_name, dbl_lvl);
	return 0;
}

/* ========================================================================= */
/** @brief example of usage of the Min Cut algorithm as implemented in
 * (EGalgMinCut).
 * 
 * We read a file from the input whose two first numbers are the number of nodes
 * and edges (we assume that the graph is undirected ), and then a 3-tupla with
 * head tail capacity. we use the last field (capacity) as the capacity of the
 * algorithm, and compute the minimum global cut. */
int main (int argc,
					char **argv)
{
	int rval = 0,
	  n_read;
	EGioFile_t *in_file = 0;
	register unsigned int i;
	unsigned int head,
	  tail;
	double cap;
	EGtimer_t cut_timer;
	dbl_EGalgMCgraph_t G;
	char l_str[4096];
	char*l_argv[128];
	dbl_EGalgMCcbk_t cb;
	dbl_mc_all_cuts_t all_cuts;
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	all_cuts.max_sz = all_cuts.sz = 0;
	EGtimerReset (&cut_timer);
	dbl_EGalgMCcbkInit (&cb);
	cb.do_fn = dbl_mc_store_cuts;
	dbl_EGlpNumSet (cb.cutoff, 2.0);
	cb.param = &all_cuts;
	dbl_EGalgMCgraphInit (&G);
	dbl_EGlpNumInitVar (cap);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	/* test we have an input file */
	rval = dbl_mc_parseargs (argc, argv);
	CHECKRVALG (rval, CLEANUP);
	in_file = EGioOpen (dbl_file_name, "r");
	if (!in_file)
	{
		fprintf (stderr, "Can't open file %s\n", argv[1]);
		dbl_mc_usage (argv);
		rval = 1;
		goto CLEANUP;
	}
	/* now we start reading the file */
	EGioGets(l_str,(size_t)4095,in_file);
	EGioNParse(l_str,128," ","%#",&n_read,l_argv);
	TESTG ((rval = (n_read != 2)), CLEANUP, "Can't read n_nodes and n_endges "
				 "from file");
	G.G.n_onodes = atoi(l_argv[0]);
	G.G.n_oedges = atoi(l_argv[1]);
	if (!G.G.n_oedges || !G.G.n_onodes)
	{
		fprintf (stderr, "Problem is trivial with %u nodes and %u edges\n",
						 G.G.n_onodes, G.G.n_oedges);
		G.G.n_onodes = G.G.n_oedges = 0;
		goto CLEANUP;
	}
	fprintf (stderr, "Reading graph on %u nodes and %u edges...", G.G.n_onodes,
					 G.G.n_oedges);
	G.all_nodes = EGsMalloc (dbl_EGalgMCnode_t, G.G.n_onodes);
	G.all_edges = EGsMalloc (dbl_EGalgMCedge_t, G.G.n_oedges);
	for (i = G.G.n_onodes; i--;)
	{
		dbl_EGalgMCnodeInit (G.all_nodes + i);
		EGeListAddAfter (&(G.all_nodes[i].lvl_cn), G.lvl_list);
		G.all_nodes[i].id = i;
		dbl_EGsrkAddNode (&(G.G), &(G.all_nodes[i].node));
	}
	for (i = G.G.n_oedges; i--;)
	{
		dbl_EGalgMCedgeInit (G.all_edges + i);
		G.all_edges[i].id = i;
		EGioGets(l_str,(size_t)4095,in_file);
		EGioNParse(l_str,128," ","%#",&n_read,l_argv);
		TESTG ((rval = (n_read != 3)), CLEANUP, "Can't read head tail capacity");
		head = atoi(l_argv[0]);
		tail = atoi(l_argv[1]);
		dbl_EGlpNumReadStr(cap,l_argv[2]);
		dbl_EGlpNumCopy (G.all_edges[i].edge.weight, cap);
		dbl_EGsrkAddEdge (&(G.G), &(G.all_nodes[head].node), &(G.all_nodes[tail].node),
									&(G.all_edges[i].edge));
	}
	dbl_EGlpNumCopySum (G.cut_val, G.all_nodes[0].node.weight, dbl_oneLpNum);
	dbl_EGlpNumCopy (G.epsilon, dbl_epsLpNum);
	G.cut = EGsMalloc (unsigned int,
										 G.G.n_onodes);
	EGioClose (in_file);
	in_file = 0;

	/* now we call the min cut algorithm */
	fprintf (stderr, "...done\nComputing Min Cut...");
	EGtimerStart (&cut_timer);
	rval = dbl_EGalgMC (&G, &cb, dbl_lvl);
	EGtimerStop (&cut_timer);
	fprintf (stderr, "...done in %lf seconds\n\tValue: %lf\n", cut_timer.time,
					 dbl_EGlpNumToLf (G.cut_val));

	/* now we display all cuts */
#if 0
	fprintf (stderr, "Number of cuts found: %u\n", all_cuts.sz);
	for (i = all_cuts.sz; i--;)
	{
		fprintf (stderr, "cut %u: val %lf, sz %u, elems ", i,
						 dbl_EGlpNumToLf (all_cuts.cut_val[i]), all_cuts.cut_sz[i]);
		for (j = all_cuts.cut_sz[i]; j--;)
			fprintf (stderr, " %u", all_cuts.cut[i][j]);
		fprintf (stderr, "\n");
	}
#endif

	/* ending */
CLEANUP:
	if (in_file) EGioClose (in_file);
	dbl_EGlpNumClearVar (cap);
	if (all_cuts.sz)
	{
		for (i = all_cuts.sz; i--;)
		{
			dbl_EGlpNumClearVar (all_cuts.cut_val[i]);
			EGfree (all_cuts.cut[i]);
		}
		EGfree (all_cuts.cut);
		EGfree (all_cuts.cut_val);
		EGfree (all_cuts.cut_sz);
	}
	if (G.G.n_onodes)
	{
		for (i = G.G.n_onodes; i--;)
			dbl_EGalgMCnodeClear (G.all_nodes + i);
		EGfree (G.all_nodes);
	}
	if (G.G.n_oedges)
	{
		for (i = G.G.n_oedges; i--;)
			dbl_EGalgMCedgeClear (G.all_edges + i);
		EGfree (G.all_edges);
	}
	dbl_EGalgMCprofile();
	dbl_EGalgPRprofile();
	if (G.cut)
		EGfree (G.cut);
	dbl_EGalgMCgraphClear (&G);
	EGlpNumClear();
	return rval;
}

/* ========================================================================= */
/** @} */
