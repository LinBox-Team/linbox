

/*
 * -- SuperLU routine (version 2.0) --
 * Univ. of California Berkeley, Xerox Palo Alto Research Center,
 * and Lawrence Berkeley National Lab.
 * November 15, 1997
 *
 */
/*
  Copyright (c) 1994 by Xerox Corporation.  All rights reserved.
 
  THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY
  EXPRESSED OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 
  Permission is hereby granted to use or copy this program for any
  purpose, provided the above notices are retained on all copies.
  Permission to modify the code and to distribute modified code is
  granted, provided the above notices are retained, and a notice that
  the code was modified is included with the above copyright notice.
*/

#include "linbox/gen_superlu/sp_defs.h"
#include "linbox/gen_superlu/util.h"
#include "linbox/gen_superlu/snode_bmod.h"
#include "linbox/gen_superlu/panel_bmod.h"
#include "linbox/gen_superlu/column_bmod.h"
#include "linbox/gen_superlu/pivotL.h"
#include "linbox/gen_superlu/pruneL.h"
#include "linbox/gen_superlu/copy_to_ucol.h"
#include "linbox/gen_superlu/panel_dfs.h"
#include "linbox/gen_superlu/column_dfs.h"
#include "linbox/gen_superlu/snode_dfs.h"
#include "linbox/gen_superlu/relax_snode.h"

// #define DEBUG   // A.Duran
// #define DEBUGPERMR // A.Duran

template <class Field>
void
gstrf( char *refact, SuperMatrix<Field> *A, typename Field::Element diag_pivot_thresh, 
	typename Field::Element drop_tol, int relax, int panel_size, int *etree, 
	void *work, int lwork, int *perm_r, int *perm_c, 
	SuperMatrix<Field> *L, SuperMatrix<Field> *U, int *info, int *rank, Field& F)
{
/*
 * Purpose
 * =======
 *
 * DGSTRF computes an LU factorization of a general sparse m-by-n
 * matrix A using partial pivoting with row interchanges.
 * The factorization has the form
 *     Pr * A = L * U
 * where Pr is a row permutation matrix, L is lower triangular with unit
 * diagonal elements (lower trapezoidal if A->nrow > A->ncol), and U is upper 
 * triangular (upper trapezoidal if A->nrow < A->ncol).
 *
 * See supermatrix.h for the definition of 'SuperMatrix' structure.
 *
 * Arguments
 * =========
 *
 * refact (input) char*
 *          Specifies whether we want to use perm_r from a previous factor.
 *          = 'Y': re-use perm_r; perm_r is input, and may be modified due to
 *                 different pivoting determined by diagonal threshold.
 *          = 'N': perm_r is determined by partial pivoting, and output.
 *
 * A        (input) SuperMatrix*
 *	    Original matrix A, permuted by columns, of dimension
 *          (A->nrow, A->ncol). The type of A can be:
 *          Stype = NCP; Dtype = D; Mtype = GE.
 *
 * diag_pivot_thresh (input) double
 *	    Diagonal pivoting threshold. At step j of the Gaussian elimination,
 *          if abs(A_jj) >= thresh * (max_(i>=j) abs(A_ij)), use A_jj as pivot.
 *	    0 <= thresh <= 1. The default value of thresh is 1, corresponding
 *          to partial pivoting.
 *
 * drop_tol (input) double (NOT IMPLEMENTED)
 *	    Drop tolerance parameter. At step j of the Gaussian elimination,
 *          if abs(A_ij)/(max_i abs(A_ij)) < drop_tol, drop entry A_ij.
 *          0 <= drop_tol <= 1. The default value of drop_tol is 0.
 *
 * relax    (input) int
 *          To control degree of relaxing supernodes. If the number
 *          of nodes (columns) in a subtree of the elimination tree is less
 *          than relax, this subtree is considered as one supernode,
 *          regardless of the row structures of those columns.
 *
 * panel_size (input) int
 *          A panel consists of at most panel_size consecutive columns.
 *
 * etree    (input) int*, dimension (A->ncol)
 *          Elimination tree of A'*A.
 *          Note: etree is a vector of parent pointers for a forest whose
 *          vertices are the integers 0 to A->ncol-1; etree[root]==A->ncol.
 *          On input, the columns of A should be permuted so that the
 *          etree is in a certain postorder.
 *
 * work     (input/output) void*, size (lwork) (in bytes)
 *          User-supplied work space and space for the output data structures.
 *          Not referenced if lwork = 0;
 *
 * lwork   (input) int
 *         Specifies the size of work array in bytes.
 *         = 0:  allocate space internally by system malloc;
 *         > 0:  use user-supplied work array of length lwork in bytes,
 *               returns error if space runs out.
 *         = -1: the routine guesses the amount of space needed without
 *               performing the factorization, and returns it in
 *               *info; no other side effects.
 *
 * perm_r   (input/output) int*, dimension (A->nrow)
 *          Row permutation vector which defines the permutation matrix Pr,
 *          perm_r[i] = j means row i of A is in position j in Pr*A.
 *          If refact is not 'Y', perm_r is output argument;
 *          If refact = 'Y', the pivoting routine will try to use the input
 *          perm_r, unless a certain threshold criterion is violated.
 *          In that case, perm_r is overwritten by a new permutation
 *          determined by partial pivoting or diagonal threshold pivoting.
 *
 * perm_c   (input) int*, dimension (A->ncol)
 *	    Column permutation vector, which defines the 
 *          permutation matrix Pc; perm_c[i] = j means column i of A is 
 *          in position j in A*Pc.
 *          When searching for diagonal, perm_c[*] is applied to the
 *          row subscripts of A, so that diagonal threshold pivoting
 *          can find the diagonal of A, rather than that of A*Pc.
 *
 * L        (output) SuperMatrix*
 *          The factor L from the factorization Pr*A=L*U; use compressed row 
 *          subscripts storage for supernodes, i.e., L has type: 
 *          Stype = SC, Dtype = _D, Mtype = TRLU.
 *
 * U        (output) SuperMatrix*
 *	    The factor U from the factorization Pr*A*Pc=L*U. Use column-wise
 *          storage scheme, i.e., U has types: Stype = NC, 
 *          Dtype = _D, Mtype = TRU.
 *
 * info     (output) int*
 *          = 0: successful exit
 *          < 0: if info = -i, the i-th argument had an illegal value
 *          > 0: if info = i, and i is
 *             <= A->ncol: U(i,i) is exactly zero. The factorization has
 *                been completed, but the factor U is exactly singular,
 *                and division by zero will occur if it is used to solve a
 *                system of equations.
 *             > A->ncol: number of bytes allocated when memory allocation
 *                failure occurred, plus A->ncol. If lwork = -1, it is
 *                the estimated amount of space needed, plus A->ncol.
 *
 * ======================================================================
 *
 * Local Working Arrays: 
 * ======================
 *   m = number of rows in the matrix
 *   n = number of columns in the matrix
 *
 *   xprune[0:n-1]: xprune[*] points to locations in subscript 
 *	vector lsub[*]. For column i, xprune[i] denotes the point where 
 *	structural pruning begins. I.e. only xlsub[i],..,xprune[i]-1 need 
 *	to be traversed for symbolic factorization.
 *
 *   marker[0:3*m-1]: marker[i] = j means that node i has been 
 *	reached when working on column j.
 *	Storage: relative to original row subscripts
 *	NOTE: There are 3 of them: marker/marker1 are used for panel dfs, 
 *	      see dpanel_dfs.c; marker2 is used for inner-factorization,
 *            see dcolumn_dfs.c.
 *
 *   parent[0:m-1]: parent vector used during dfs
 *      Storage: relative to new row subscripts
 *
 *   xplore[0:m-1]: xplore[i] gives the location of the next (dfs) 
 *	unexplored neighbor of i in lsub[*]
 *
 *   segrep[0:nseg-1]: contains the list of supernodal representatives
 *	in topological order of the dfs. A supernode representative is the 
 *	last column of a supernode.
 *      The maximum size of segrep[] is n.
 *
 *   repfnz[0:W*m-1]: for a nonzero segment U[*,j] that ends at a 
 *	supernodal representative r, repfnz[r] is the location of the first 
 *	nonzero in this segment.  It is also used during the dfs: repfnz[r]>0
 *	indicates the supernode r has been explored.
 *	NOTE: There are W of them, each used for one column of a panel. 
 *
 *   panel_lsub[0:W*m-1]: temporary for the nonzeros row indices below 
 *      the panel diagonal. These are filled in during dpanel_dfs(), and are
 *      used later in the inner LU factorization within the panel.
 *	panel_lsub[]/dense[] pair forms the SPA data structure.
 *	NOTE: There are W of them.
 *
 *   dense[0:W*m-1]: sparse accumulating (SPA) vector for intermediate values;
 *	    	   NOTE: there are W of them.
 *
 *   tempv[0:*]: real temporary used for dense numeric kernels;
 *	The size of this array is defined by NUM_TEMPV() in dsp_defs.h.
 *
 */
    /* Local working arrays */
    NCPformat<Field> *Astore;
    int       *iperm_r; /* inverse of perm_r; not used if refact = 'N' */
    int       *iperm_c; /* inverse of perm_c */
    int       *iwork;
    typename Field::Element   *dwork;
    int	      *segrep, *repfnz, *parent, *xplore;
    int	      *panel_lsub; /* dense[]/panel_lsub[] pair forms a w-wide SPA */
    int	      *xprune;
    int	      *marker;
    typename Field::Element   *dense, *tempv;
    int       *relax_end;
    typename Field::Element    *a;
    int       *asub;
    int       *xa_begin, *xa_end;
    int       *xsup, *supno;
    int       *xlsub, *xlusup, *xusub;
    int       nzlumax;
    static GlobalLU_t<Field> Glu; /* persistent to facilitate multiple factors. */

    /* Local scalars */
    int nullity; // Nullity of original matrix A // A. Duran
    int       pivrow;   /* pivotal row number in the original matrix A */
    int       nseg1;	/* no of segments in U-column above panel row jcol */
    int       nseg;	/* no of segments in each U-column */
    register int jcol;	
    register int kcol;	/* end column of a relaxed snode */
    register int icol;
    register int i, k, jj, new_next, iinfo;
    int       m, n, min_mn, jsupno, fsupc, nextlu, nextu;
    int       w_def;	/* upper bound on panel width */
    int       usepr, iperm_r_allocated = 0;
    int       nnzL, nnzU;
    extern SuperLUStat_t SuperLUStat;
    int       *panel_histo = SuperLUStat.panel_histo;
    flops_t   *ops = SuperLUStat.ops;

    nullity  = 0; // A. Duran
    int null = 0;
    int rnk  = 0;
    iinfo    = 0;
    m        = A->nrow;
    n        = A->ncol;
    min_mn   = SUPERLU_MIN(m, n);
    Astore   = (NCPformat<Field> *)A->Store;
    a        = (typename Field::Element   *)Astore->nzval;
    asub     = Astore->rowind;
    xa_begin = Astore->colbeg;
    xa_end   = Astore->colend;

    /* Allocate storage common to the factor routines */
    // *info = dLUMemInit(refact, work, lwork, m, n, Astore->nnz,
    //	      panel_size, L, U, &Glu, &iwork, &dwork);
    *info = FLUMemInit(refact, work, lwork, m, n, Astore->nnz,
		       panel_size, L, U, &Glu, &iwork, &dwork, F); // A.Duran 8/14/2002

    if ( *info ) return;
    
    xsup    = Glu.xsup;
    supno   = Glu.supno;
    xlsub   = Glu.xlsub;
    xlusup  = Glu.xlusup;
    xusub   = Glu.xusub;
    
    SetIWork(m, n, panel_size, iwork, &segrep, &parent, &xplore,
	     &repfnz, &panel_lsub, &xprune, &marker);
    FSetRWork(m, panel_size, dwork, &dense, &tempv, F);
    
    usepr = lsame_(refact, "Y");
    if ( usepr ) {
	/* Compute the inverse of perm_r */
	iperm_r = (int *) intMalloc(m);
	for (k = 0; k < m; ++k) iperm_r[perm_r[k]] = k;
	iperm_r_allocated = 1;
    }
    iperm_c = (int *) intMalloc(n);
    for (k = 0; k < n; ++k) iperm_c[perm_c[k]] = k;

    /* Identify relaxed snodes */
    relax_end = (int *) intMalloc(n);
    relax_snode(n, etree, relax, marker, relax_end); 
    
    ifill (perm_r, m, EMPTY);
    ifill (marker, m * NO_MARKER, EMPTY);
    supno[0] = -1;
    xsup[0]  = xlsub[0] = xusub[0] = xlusup[0] = 0;
    w_def    = panel_size;

    /* 
     * Work on one "panel" at a time. A panel is one of the following: 
     *	   (a) a relaxed supernode at the bottom of the etree, or
     *	   (b) panel_size contiguous columns, defined by the user
     */
    for (jcol = 0; jcol < min_mn; ) {

	if ( relax_end[jcol] != EMPTY ) { /* start of a relaxed snode */
   	    kcol = relax_end[jcol];	  /* end of the relaxed snode */
	    panel_histo[kcol-jcol+1]++;

	    /* --------------------------------------
	     * Factorize the relaxed supernode(jcol:kcol) 
	     * -------------------------------------- */
	    /* Determine the union of the row structure of the snode */
	    if ( (*info = snode_dfs(jcol, kcol, asub, xa_begin, xa_end,
				    xprune, marker, &Glu, F)) != 0 )
		return;

            nextu    = xusub[jcol];
	    nextlu   = xlusup[jcol];
	    jsupno   = supno[jcol];
	    fsupc    = xsup[jsupno];
	    new_next = nextlu + (xlsub[fsupc+1]-xlsub[fsupc])*(kcol-jcol+1);
	    nzlumax = Glu.nzlumax;
	    while ( new_next > nzlumax ) {
		if ( *info = FLUMemXpand(jcol, nextlu, LUSUP, &nzlumax, &Glu, F) )
		    return;
	    }
    
	    for (icol = jcol; icol<= kcol; icol++) {
		xusub[icol+1] = nextu;
		
    		/* Scatter into SPA dense[*] */
    		for (k = xa_begin[icol]; k < xa_end[icol]; k++)
		  {
        	    dense[asub[k]] = a[k];
#ifdef DEBUG
		    cout << "At gstrf dense[asub[k]] "<< a[k] << "\n";

#endif
		  }
	       	/* Numeric update within the snode */
	        snode_bmod(icol, jsupno, fsupc, dense, tempv, &Glu, F);

		if ( *info = pivotL(icol, diag_pivot_thresh, &usepr, perm_r,
				    iperm_r, iperm_c, &pivrow, &Glu, &null, F) )
                {   ++nullity;
                    if ( iinfo == 0 ) iinfo = *info;
                } else ++rnk;

		
#ifdef DEBUGPERMR
                cout <<"\nAfter first pivotL call";
		print_int_vec("\nperm_r", m, perm_r);
#endif

#ifdef DEBUG
		print_lu_col("[1]: ", icol, pivrow, xprune, &Glu, F);
#endif

	    }

	    jcol = icol;

	} else { /* Work on one panel of panel_size columns */
	    
	    /* Adjust panel_size so that a panel won't overlap with the next 
	     * relaxed snode.
	     */
	    panel_size = w_def;
	    for (k = jcol + 1; k < SUPERLU_MIN(jcol+panel_size, min_mn); k++) 
		if ( relax_end[k] != EMPTY ) {
		    panel_size = k - jcol;
		    break;
		}
	    if ( k == min_mn ) panel_size = min_mn - jcol;
	    panel_histo[panel_size]++;

	    /* symbolic factor on a panel of columns */
	    panel_dfs(m, panel_size, jcol, A, perm_r, &nseg1,
		      dense, panel_lsub, segrep, repfnz, xprune,
		      marker, parent, xplore, &Glu, F);
#ifdef DEBUGPERMR
	    cout <<"\nafter panel_dfs";
	    print_int_vec("\nperm_r", m, perm_r);
#endif
	    
	    /* numeric sup-panel updates in topological order */
	    panel_bmod(m, panel_size, jcol, nseg1, dense,
		       tempv, segrep, repfnz, &Glu, F);
	    
	    /* Sparse LU within the panel, and below panel diagonal */
    	    for ( jj = jcol; jj < jcol + panel_size; jj++) {
 		k = (jj - jcol) * m; /* column index for w-wide arrays */

		nseg = nseg1;	/* Begin after all the panel segments */

	    	if ((*info = column_dfs(m, jj, perm_r, &nseg, &panel_lsub[k],
					segrep, &repfnz[k], xprune, marker,
					parent, xplore, &Glu, F)) != 0) return;


	      	/* Numeric updates */
	    	if ((*info = column_bmod(jj, (nseg - nseg1), &dense[k],
					 tempv, &segrep[nseg1], &repfnz[k],
					 jcol, &Glu, F)) != 0) return;
#ifdef DEBUGPERMR
		cout <<"\nafter column_bmode";
		print_int_vec("\nperm_r", m, perm_r);
#endif

#ifdef DEBUG
		    cout << "At gstrf after column_bmod dense[k] "<< dense[k] << "\n";

#endif

		
	        /* Copy the U-segments to ucol[*] */
		if ((*info = Fcopy_to_ucol(jj, nseg, segrep, &repfnz[k],
					  perm_r, &dense[k], &Glu, F)) != 0)
		    return;

	    	if ( *info = pivotL(jj, diag_pivot_thresh, &usepr, perm_r,
				    iperm_r, iperm_c, &pivrow, &Glu, &null, F) )
		{   ++nullity;
		    if ( iinfo == 0 ) iinfo = *info;
                } else ++rnk;

#ifdef DEBUGPERMR
		cout <<"\nafter pivotL";
		cout <<"\nnullity : "<<nullity;
		cout <<"\nnull : "<<null;
		print_int_vec("\nperm_r", m, perm_r);
#endif

		/* Prune columns (0:jj-1) using column jj */
	    	pruneL(jj, perm_r, pivrow, nseg, segrep,
		       &repfnz[k], xprune, &Glu, F);

#ifdef DEBUGPERMR
		cout <<"\nafter pruneL";
		print_int_vec("\nperm_r", m, perm_r);
#endif

		/* Reset repfnz[] for this column */
	    	resetrep_col (nseg, segrep, &repfnz[k]);
		
#ifdef DEBUG
		print_lu_col("[2]: ", jj, pivrow, xprune, &Glu, F);
#endif

	    }

   	    jcol += panel_size;	/* Move to the next panel */

	} /* else */

    } /* for */

    *info = iinfo;
    
    if ( m > n ) {
	k = 0;
        for (i = 0; i < m; ++i) 
            if ( perm_r[i] == EMPTY ) {
    		perm_r[i] = n + k;
		++k;
	    }
    }

#ifdef DEBUGPERMR
    cout <<"\nbefore fixupL";
    print_int_vec("\nperm_r", m, perm_r);
#endif

    countnz(min_mn, xprune, &nnzL, &nnzU, &Glu);
    fixupL(min_mn, perm_r, &Glu);
#ifdef DEBUGPERMR
    cout <<"\nafter fixupL";
    print_int_vec("\nperm_r", m, perm_r);
#endif

    (*rank) = n - nullity; // A. Duran
cout <<"New rank : "<< rnk <<"\n";
cout <<"nullity : "<<nullity << "\n";
    FLUWorkFree(iwork, dwork, &Glu, F); /* Free work space and compress storage */

    if ( lsame_(refact, "Y") ) {
        /* L and U structures may have changed due to possibly different
	   pivoting, although the storage is available. 	   
	   There could also be memory expansions, so the array locations
           may have changed, */
        ((SCformat<Field> *)L->Store)->nnz = nnzL;
	((SCformat<Field> *)L->Store)->nsuper = Glu.supno[n];
	((SCformat<Field> *)L->Store)->nzval = Glu.lusup;
	((SCformat<Field> *)L->Store)->nzval_colptr = Glu.xlusup;
	((SCformat<Field> *)L->Store)->rowind = Glu.lsub;
	((SCformat<Field> *)L->Store)->rowind_colptr = Glu.xlsub;
	((NCformat<Field> *)U->Store)->nnz = nnzU;
	((NCformat<Field> *)U->Store)->nzval = Glu.ucol;
	((NCformat<Field> *)U->Store)->rowind = Glu.usub;
	((NCformat<Field> *)U->Store)->colptr = Glu.xusub;
    } else {
        FCreate_SuperNode_Matrix(L, A->nrow, A->ncol, nnzL, Glu.lusup, 
	                         Glu.xlusup, Glu.lsub, Glu.xlsub, Glu.supno,
			         Glu.xsup, SC, D_, TRLU, F);
    	FCreate_CompCol_Matrix(U, min_mn, min_mn, nnzU, Glu.ucol, 
			       Glu.usub, Glu.xusub, NC, D_, TRU, F);
    }
    
    ops[FACT] += ops[TRSV] + ops[GEMV];	
    
    if ( iperm_r_allocated ) SUPERLU_FREE (iperm_r);
    SUPERLU_FREE (iperm_c);
    SUPERLU_FREE (relax_end);

}