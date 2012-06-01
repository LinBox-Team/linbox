

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

#include <stdio.h>
#include <stdlib.h>
#include "linbox/gen_superlu/sp_defs.h"
#include "linbox/gen_superlu/util.h"

// #define DEBUG // A. Duran

/* 
 * Function prototypes 
 */
template <class Field>
void usolve(int, int, typename Field::Element*, typename Field::Element*, Field& F);
template <class Field>
void lsolve(int, int, typename Field::Element*, typename Field::Element*, Field& F);
template <class Field>
void matvec(int, int, int, typename Field::Element*, typename Field::Element*, typename Field::Element*, Field& F);



/* Return value:   0 - successful return
 *               > 0 - number of bytes allocated when run out of space
 */
template <class Field>
int
column_bmod (
	     const int  jcol,	  /* in */
	     const int  nseg,	  /* in */
	     typename Field::Element *dense,	  /* in */
	     typename Field::Element *tempv,	  /* working array */
	     int        *segrep,  /* in */
	     int        *repfnz,  /* in */
	     int        fpanelc,  /* in -- first column in the current panel */
	     GlobalLU_t<Field> *Glu,      /* modified */
	     Field& F
	     )
{
  /*
   * Purpose:
   * ========
   *    Performs numeric block updates (sup-col) in topological order.
   *    It features: col-col, 2cols-col, 3cols-col, and sup-col updates.
   *    Special processing on the supernodal portion of L\U[*,j]
   *
   */

#ifdef DEBUG
  printf("column_bmode visited\n");
#endif

#ifdef _CRAY
  _fcd ftcs1 = _cptofcd("L", strlen("L")),
    ftcs2 = _cptofcd("N", strlen("N")),
    ftcs3 = _cptofcd("U", strlen("U"));
#endif
  int         incx = 1, incy = 1;
  typename Field::Element alpha, beta;
  
  /* krep = representative of current k-th supernode
   * fsupc = first supernodal column
   * nsupc = no of columns in supernode
   * nsupr = no of rows in supernode (used as leading dimension)
   * luptr = location of supernodal LU-block in storage
   * kfnz = first nonz in the k-th supernodal segment
   * no_zeros = no of leading zeros in a supernodal U-segment
   */
  typename Field::Element ukj, ukj1, ukj2;
  int          luptr, luptr1, luptr2;
  int          fsupc, nsupc, nsupr, segsze;
  int          nrow;	  /* No of rows in the matrix of matrix-vector */
  int          jcolp1, jsupno, k, ksub, krep, krep_ind, ksupno;
  register int lptr, kfnz, isub, irow, i;
  register int no_zeros, new_next; 
  int          ufirst, nextlu;
  int          fst_col; /* First column within small LU update */
  int          d_fsupc; /* Distance between the first column of the current
			   panel and the first column of the current snode. */
  int          *xsup, *supno;
  int          *lsub, *xlsub;
  typename Field::Element *lusup;
  typename Field::Element temp_mul; // A.Duran 8/15/2002
  typename Field::Element temp_mul1; // A.Duran 8/15/2002
  typename Field::Element temp_add; // A.Duran 8/15/2002
  typename Field::Element temp_sub; // A.Duran 8/15/2002
  int          *xlusup;
  int          nzlumax;
  typename Field::Element *tempv1;
  typename Field::Element zero; // A.Duran
  F.init(zero, 0);
  typename Field::Element one;  // A.Duran
  F.init(one, 1);
  typename Field::Element none; // A.Duran
  F.init(none, -1);
  int          mem_error;
  extern SuperLUStat_t SuperLUStat;
  flops_t  *ops = SuperLUStat.ops;
  
  xsup    = Glu->xsup;
  supno   = Glu->supno;
  lsub    = Glu->lsub;
  xlsub   = Glu->xlsub;
  lusup   = Glu->lusup; // Field::Element * ,  L supernodes
  xlusup  = Glu->xlusup;
  nzlumax = Glu->nzlumax;
  jcolp1  = jcol + 1;
  jsupno  = supno[jcol];
  
  /* 
   * For each nonz supernode segment of U[*,j] in topological order 
   */
  k = nseg - 1;
  for (ksub = 0; ksub < nseg; ksub++) {
    
    krep = segrep[k];
    k--;
    ksupno = supno[krep];
    if ( jsupno != ksupno ) { /* Outside the rectangular supernode */
      
      fsupc = xsup[ksupno];
      fst_col = SUPERLU_MAX ( fsupc, fpanelc );
      
      /* Distance from the current supernode to the current panel; 
	 d_fsupc=0 if fsupc > fpanelc. */
      d_fsupc = fst_col - fsupc; 
      
      luptr = xlusup[fst_col] + d_fsupc;
      lptr = xlsub[fsupc] + d_fsupc;
      
      kfnz = repfnz[krep];
      kfnz = SUPERLU_MAX ( kfnz, fpanelc );
      
      segsze = krep - kfnz + 1;
      nsupc = krep - fst_col + 1;
      nsupr = xlsub[fsupc+1] - xlsub[fsupc];	/* Leading dimension */
      nrow = nsupr - d_fsupc - nsupc;
      krep_ind = lptr + nsupc - 1;
      
      ops[TRSV] += segsze * (segsze - 1);
      ops[GEMV] += 2 * nrow * segsze;
      
      
      /* 
       * Case 1: Update U-segment of size 1 -- col-col update 
       */
      if ( segsze == 1 ) {
	F.assign(ukj, dense[lsub[krep_ind]]); // F.assign is correct!
	luptr += nsupr*(nsupc-1) + nsupc;

#ifdef DEBUG
	  cout <<"At column_bmod Case1 ukj "<< ukj <<"\n";
#endif
	
	for (i = lptr + nsupc; i < xlsub[fsupc+1]; ++i) {
	  irow = lsub[i];

#ifdef DEBUG
	  cout <<"At column_bmod lusup[luptr] "<< lusup[luptr] <<"\n";
#endif
	  // dense[irow] -=  ukj*lusup[luptr];
	  F.mul(temp_mul, ukj, lusup[luptr]); // A.Duran 8/15/2002
	  F.subin(dense[irow], temp_mul);
#ifdef DEBUG
	  cout <<"At column_bmod dense[irow] "<< dense[irow] <<"\n";
#endif
	  luptr++;
	}
	
      } else if ( segsze <= 3 ) {
	F.assign(ukj, dense[lsub[krep_ind]]);
	luptr += nsupr*(nsupc-1) + nsupc-1;
	F.assign(ukj1, dense[lsub[krep_ind - 1]]);
	luptr1 = luptr - nsupr;
	
	if ( segsze == 2 ) { /* Case 2: 2cols-col update */
	  // ukj -= ukj1 * lusup[luptr1];
	  F.mul(temp_mul, ukj1, lusup[luptr1]); // A.Duran 8/15/2002
	  F.subin(ukj, temp_mul);
	  F.assign(dense[lsub[krep_ind]], ukj);
	  for (i = lptr + nsupc; i < xlsub[fsupc+1]; ++i) {
	    irow = lsub[i];
	    luptr++;
	    luptr1++;
	    // dense[irow] -= ( ukj*lusup[luptr]
	    //	     + ukj1*lusup[luptr1] );
	    F.mul(temp_mul, ukj, lusup[luptr]); // A.Duran 8/15/2002
	    F.mul(temp_mul1, ukj1, lusup[luptr1]);
	    F.add(temp_add, temp_mul, temp_mul1);
	    F.subin(dense[irow], temp_add);
#ifdef DEBUG
	  cout <<"At column_bmod segsze == 2 dense[irow] "<< dense[irow] <<"\n";
#endif

	  }
	} else { /* Case 3: 3cols-col update */
	  F.assign(ukj2, dense[lsub[krep_ind - 2]]);
	  luptr2 = luptr1 - nsupr;
	  // ukj1 -= ukj2 * lusup[luptr2-1];
	  F.mul(temp_mul, ukj2, lusup[luptr2-1]); // A.Duran 8/15/2002
	  F.subin(ukj1, temp_mul);
	  // ukj = ukj - ukj1*lusup[luptr1] - ukj2*lusup[luptr2];
	  F.mul(temp_mul, ukj1, lusup[luptr1]);
	  F.sub(temp_sub, ukj, temp_mul);
	  F.mul(temp_mul, ukj2, lusup[luptr2]);
	  F.sub(ukj, temp_sub, temp_mul);
	  F.assign(dense[lsub[krep_ind]], ukj);
	  F.assign(dense[lsub[krep_ind-1]], ukj1);
	  for (i = lptr + nsupc; i < xlsub[fsupc+1]; ++i) {
	    irow = lsub[i];
	    luptr++;
	    luptr1++;
	    luptr2++;
	    // dense[irow] -= ( ukj*lusup[luptr]
	    //	     + ukj1*lusup[luptr1] + ukj2*lusup[luptr2] );
	    F.mul(temp_mul, ukj, lusup[luptr]); // A.Duran 8/15/2002
	    F.mul(temp_mul1, ukj1, lusup[luptr1]);
	    F.add(temp_add, temp_mul, temp_mul1);
	    F.mul(temp_mul, ukj2, lusup[luptr2]);
	    F.addin(temp_add, temp_mul);
	    F.subin(dense[irow], temp_add);
	  }
	}
	
	
	
      } else {
	/*
	 * Case: sup-col update
	 * Perform a triangular solve and block update,
	 * then scatter the result of sup-col update to dense
	 */
	
	no_zeros = kfnz - fst_col;
	
	/* Copy U[*,j] segment from dense[*] to tempv[*] */
	isub = lptr + no_zeros;
	for (i = 0; i < segsze; i++) {
	  irow = lsub[isub];
	  F.assign(tempv[i], dense[irow]);
	  ++isub; 
	}
	
	/* Dense triangular solve -- start effective triangle */
	luptr += nsupr * no_zeros + no_zeros; 
	
#ifdef USE_VENDOR_BLAS
#ifdef _CRAY
	STRSV( ftcs1, ftcs2, ftcs3, &segsze, &lusup[luptr], 
	       &nsupr, tempv, &incx );
#else		
	dtrsv_( "L", "N", "U", &segsze, &lusup[luptr], 
		&nsupr, tempv, &incx );
#endif		
	luptr += segsze;  /* Dense matrix-vector */
	tempv1 = &tempv[segsze];
	F.assign(alpha, one);
	F.assign(beta, zero);
#ifdef _CRAY
	SGEMV( ftcs2, &nrow, &segsze, &alpha, &lusup[luptr], 
	       &nsupr, tempv, &incx, &beta, tempv1, &incy );
#else
	dgemv_( "N", &nrow, &segsze, &alpha, &lusup[luptr], 
		&nsupr, tempv, &incx, &beta, tempv1, &incy );
#endif
#else
	lsolve ( nsupr, segsze, &lusup[luptr], tempv, F );
	
	luptr += segsze;  /* Dense matrix-vector */
	tempv1 = &tempv[segsze];
	matvec (nsupr, nrow , segsze, &lusup[luptr], tempv, tempv1, F);
#endif
	
	
	/* Scatter tempv[] into SPA dense[] as a temporary storage */
	isub = lptr + no_zeros;
	for (i = 0; i < segsze; i++) {
	  irow = lsub[isub];
	  F.assign(dense[irow], tempv[i]);
	  F.assign(tempv[i], zero);
	  ++isub;
	}
	
	/* Scatter tempv1[] into SPA dense[] */
	for (i = 0; i < nrow; i++) {
	  irow = lsub[isub];
	  // dense[irow] -= tempv1[i];
	  F.subin(dense[irow], tempv1[i]); // A.Duran 8/15/2002
	  F.assign(tempv1[i], zero);
	  ++isub;
	}
      }
      
    } /* if jsupno ... */
    
  } /* for each segment... */
  
  /*
   *	Process the supernodal portion of L\U[*,j]
   */
  nextlu = xlusup[jcol];
  fsupc = xsup[jsupno];
  
  /* Copy the SPA dense into L\U[*,j] */
  new_next = nextlu + xlsub[fsupc+1] - xlsub[fsupc];
  while ( new_next > nzlumax ) {
    if (mem_error = FLUMemXpand(jcol, nextlu, LUSUP, &nzlumax, Glu, F))
      return (mem_error);
    lusup = Glu->lusup;
    lsub  = Glu->lsub;
  }
  
  for (isub = xlsub[fsupc]; isub < xlsub[fsupc+1]; isub++) {
    irow = lsub[isub];
    F.assign(lusup[nextlu], dense[irow]);
    F.assign(dense[irow], zero);
    ++nextlu;
  }
  
  xlusup[jcolp1] = nextlu;	/* Close L\U[*,jcol] */
  
  /* For more updates within the panel (also within the current supernode), 
   * should start from the first column of the panel, or the first column 
   * of the supernode, whichever is bigger. There are 2 cases:
   *    1) fsupc < fpanelc, then fst_col := fpanelc
   *    2) fsupc >= fpanelc, then fst_col := fsupc
   */
  fst_col = SUPERLU_MAX ( fsupc, fpanelc );
  
  if ( fst_col < jcol ) {
    
    /* Distance between the current supernode and the current panel.
       d_fsupc=0 if fsupc >= fpanelc. */
    d_fsupc = fst_col - fsupc;
    
    lptr = xlsub[fsupc] + d_fsupc;
    luptr = xlusup[fst_col] + d_fsupc;
    nsupr = xlsub[fsupc+1] - xlsub[fsupc];	/* Leading dimension */
    nsupc = jcol - fst_col;	/* Excluding jcol */
    nrow = nsupr - d_fsupc - nsupc;
    
    /* Points to the beginning of jcol in snode L\U(jsupno) */
    ufirst = xlusup[jcol] + d_fsupc;	
    
    ops[TRSV] += nsupc * (nsupc - 1);
    ops[GEMV] += 2 * nrow * nsupc;
    
#ifdef USE_VENDOR_BLAS
#ifdef _CRAY
    STRSV( ftcs1, ftcs2, ftcs3, &nsupc, &lusup[luptr], 
	   &nsupr, &lusup[ufirst], &incx );
#else
    dtrsv_( "L", "N", "U", &nsupc, &lusup[luptr], 
	    &nsupr, &lusup[ufirst], &incx );
#endif
    
    F.assign(alpha, none); F.assign(beta, one); /* y := beta*y + alpha*A*x */
    
#ifdef _CRAY
    SGEMV( ftcs2, &nrow, &nsupc, &alpha, &lusup[luptr+nsupc], &nsupr,
	   &lusup[ufirst], &incx, &beta, &lusup[ufirst+nsupc], &incy );
#else
    dgemv_( "N", &nrow, &nsupc, &alpha, &lusup[luptr+nsupc], &nsupr,
	    &lusup[ufirst], &incx, &beta, &lusup[ufirst+nsupc], &incy );
#endif
#else
    lsolve ( nsupr, nsupc, &lusup[luptr], &lusup[ufirst], F );
    
    matvec ( nsupr, nrow, nsupc, &lusup[luptr+nsupc],
	     &lusup[ufirst], tempv, F );
    
#ifdef DEBUG
	  cout <<"At column_bmod after matvec lusup[luptr] "<< lusup[luptr] <<"\n";

#endif




    /* Copy updates from tempv[*] into lusup[*] */
    isub = ufirst + nsupc;
    for (i = 0; i < nrow; i++) {
      // lusup[isub] -= tempv[i];
      F.subin(lusup[isub], tempv[i]); // A.Duran 8/15/2002
      F.assign(tempv[i], zero);
      ++isub;
    }
    
#endif
    
    
  } /* if fst_col < jcol ... */ 
  
  return 0;
}