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
 * @ingroup EGsrkGraph
 * */
/** @addtogroup EGsrkGraph */
/** @{ */
#include "EGlib.h"
/* ========================================================================= */
/** @brief display usage of this program */
static inline void int_srk_usage (char **argv)
{
	fprintf (stderr, "Usage: %s input_file\n", argv[0]);
}

/* ========================================================================= */
/** @brief given a int_EGsrkGraph_t structure, display the current shrunken graph
 * and it's node weights */
static inline void int_display_srkG (int_EGsrkGraph_t * G,
																 int_EGsrkNode_t * const all_nodes,
																 int_EGsrkEdge_t * const all_edges)
{
	EGeList_t *n_it,
	 *e_it,
	 *n_end,
	 *e_end,
	 *ee_it,
	 *ee_end;
	int_EGsrkNode_t *lcn,
	 *onode;
	int_EGsrkEdge_t *ce,
	 *cee;
	EGeUgraphEP_t *cep;
	fprintf (stderr, "Graph %p: (%u nodes, %u edges)\n", (void *) G,
					 G->G.n_nodes, G->G.n_edges);
	n_end = &(G->G.nodes);
	if (!EGeListIsEmpty (n_end))
	{
		fprintf (stderr, "Nodes:\n");
		for (n_it = n_end->next; n_it != n_end; n_it = n_it->next)
		{
			lcn = EGcontainerOf (n_it, int_EGsrkNode_t, node.node_cn);
			fprintf (stderr, "\t%zd (%lf): ", lcn - all_nodes,
							 int_EGlpNumToLf (lcn->weight));
			e_end = &(lcn->members);
			if (!EGeListIsEmpty (e_end))
			{
				fprintf (stderr, "(embeded nodes) ");
				for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
				{
					onode = EGcontainerOf (e_it, int_EGsrkNode_t, members);
					fprintf (stderr, "%zd ", onode - all_nodes);
				}
			}
			e_end = &(lcn->node.edges);
			if (!EGeListIsEmpty (e_end))
			{
				fprintf (stderr, "(edges) ");
				for (e_it = e_end->next; e_it != e_end; e_it = e_it->next)
				{
					cep = EGcontainerOf (e_it, EGeUgraphEP_t, cn);
					ce = EGcontainerOf (cep, int_EGsrkEdge_t, edge.ep[cep->type]);
					fprintf (stderr, "%zd[%lf] ", ce - all_edges,
									 int_EGlpNumToLf (ce->weight));
					ee_end = &(ce->members);
					if (!EGeListIsEmpty (ee_end))
					{
						fprintf (stderr, "[(embeded edges) ");
						for (ee_it = ee_end->next; ee_it != ee_end; ee_it = ee_it->next)
						{
							cee = EGcontainerOf (ee_it, int_EGsrkEdge_t, members);
							fprintf (stderr, "%zd ", cee - all_edges);
						}
						fprintf (stderr, "] ");
					}
				}
			}
			fprintf (stderr, "\n");
		}
	}
}

/* ========================================================================= */
/** @brief This program read a graph a file from the input whose two first 
 * numbers are the number of nodes and edges (we assume that the graph is
 * undirected ), and then a 3-tupla with head tail capacity. we use the last 
 * field (capacity) as the capacity of the edge. Once this graph has been
 * read, we create a EGsrkGraph, and then we shrink the graph until two nodes
 * remain. printing the graph at every iteration. */
int main (int argc,
					char **argv)
{
	int rval = 0,
	  n_read;
	FILE *in_file = 0;
	register unsigned i;
	unsigned int head,
	  tail;
	int cap;
	int_EGsrkGraph_t G;
	int_EGsrkNode_t *all_nodes = 0;
	int_EGsrkEdge_t *all_edges = 0;
	int_EGsrkNode_t *N1,
	 *N2;
	char l_str[1024];
	/* basic initialization */
	EGlib_info();
	EGlib_version();
	EGlpNumStart();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	int_EGlpNumInitVar (cap);
	int_EGsrkGraphInit (&G);
	/* test we have an input file */
	if (argc < 2)
	{
		int_srk_usage (argv);
		rval = 1;
		goto CLEANUP;
	}
	in_file = fopen (argv[1], "r");
	if (!in_file)
	{
		fprintf (stderr, "Can't open file %s\n", argv[1]);
		int_srk_usage (argv);
		rval = 1;
		goto CLEANUP;
	}
	/* now we start reading the file */
	n_read = fscanf (in_file, "%u %u", &G.n_onodes, &G.n_oedges);
	TESTG ((rval = (n_read != 2)), CLEANUP, "couldn't read the number of nodes "
				 "and edges");
	if (!G.n_oedges || !G.n_onodes)
	{
		fprintf (stderr, "n_edges %u n_nodes %u should be more than zero",
						 G.n_oedges, G.n_onodes);
		G.n_oedges = G.n_onodes = 0;
		goto CLEANUP;
	}
	fprintf (stderr, "Reading graph on %u nodes and %u edges...",
					 G.n_onodes, G.n_oedges);
	/* create all nodes and edges */
	all_nodes = EGsMalloc (int_EGsrkNode_t, G.n_onodes);
	all_edges = EGsMalloc (int_EGsrkEdge_t, G.n_oedges);
	for (i = G.n_onodes; i--;)
	{
		int_EGsrkNodeInit (all_nodes + i);
		int_EGsrkAddNode (&G, all_nodes + i);
	}
	for (i = G.n_oedges; i--;)
	{
		int_EGsrkEdgeInit (all_edges + i);
		n_read = fscanf (in_file, "%u %u %s", &head, &tail, l_str);
		TESTG ((rval = (n_read != 3)), CLEANUP, "couldn't read three fields");
		int_EGlpNumReadStr (cap, l_str);
		int_EGlpNumCopy (all_edges[i].weight, cap);
		int_EGsrkAddEdge (&G, all_nodes + head, all_nodes + tail, all_edges + i);
	}
	fclose (in_file);
	in_file = 0;
	fprintf (stderr, "done\n");

	/* now we display the graph, and shrink the two first consecutive nodes
	 * until only two nodes remain in the shrunken graph */
	while (G.G.n_nodes > 2)
	{
		//int_display_srkG (&G, all_nodes, all_edges);
		N1 = EGcontainerOf (G.G.nodes.next, int_EGsrkNode_t, node.node_cn);
		N2 = EGcontainerOf (N1->node.node_cn.next, int_EGsrkNode_t, node.node_cn);
		int_EGsrkIdentifyNodes (&G, N1, N2);
	}
	int_display_srkG (&G, all_nodes, all_edges);
	/* ending */
CLEANUP:
	if (in_file)
		fclose (in_file);
	int_EGlpNumClearVar (cap);
	for (i = G.n_onodes; i--;)
		int_EGsrkNodeClear (all_nodes + i);
	for (i = G.n_oedges; i--;)
		int_EGsrkEdgeClear (all_edges + i);
	if (all_nodes)
		EGfree (all_nodes);
	if (all_edges)
		EGfree (all_edges);
	int_EGsrkGraphClear (&G);
	EGlpNumClear();
	return rval;
}


/* ========================================================================= */
/** @} */
