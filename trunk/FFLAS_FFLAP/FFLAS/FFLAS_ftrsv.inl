/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
//----------------------------------------------------------------------
//                FFLAS: Finite Field Fast Linear Algebra Subroutines 
//                FFLAS_trsv: Triangular Matrix Solve with Vector
//----------------------------------------------------------------------
// by Clement PERNET ( clement.pernet@imag.fr )
// 25/04/2003
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// ftrsv: TRiangular System solve with vector
// Computes  X <- op(A^-1).X
// size of X is m
//---------------------------------------------------------------------
template<class Field>
inline void
FFLAS::ftrsv(const Field& F, const enum FFLAS_UPLO Uplo, 
	     const enum FFLAS_TRANSPOSE TransA, const enum FFLAS_DIAG Diag,
	     const size_t N,const typename Field::element * A, size_t lda,
	     typename Field::element * X, int incX){
	
	typename Field::element * Xi,* Xj, * Ximax;
	const typename Field::element * Ai, * Aj;
	double x;
	if ( Uplo == FflasLower ){
		if ( TransA == FflasTrans){
			Ai = A+(N-1)*(lda+1); // bottom right entry of A
			Ximax = Xi = X+(N-1)*incX;
			for( ; Xi>=X; Ai-=lda+1,Xi-=incX ){
				F.negin( *Xi );
				for ( Xj = Xi+incX, Aj=Ai+lda; Xj<=Ximax; 
				      Xj+=incX, Aj+=lda){
					F.axpyin( *Xi, *Xj, *Aj );
				}				
				if ( Diag==FflasNonUnit ){
					F.divin(*Xi,*Ai);
				}
				F.negin( *Xi );
			}
		} // FflasTrans
		else{
			Ai = A;
		        Xi = X;
			for( ; Xi<X+incX*N; Ai+=lda+1,Xi+=incX ){
				F.negin( *Xi );
				for ( Xj = Xi-incX, Aj=Ai-1; Xj>=X; 
				      Xj-=incX, Aj--){
					F.axpyin( *Xi, *Xj, *Aj );
				}
				if ( Diag==FflasNonUnit )
					F.divin(*Xi,*Ai);
				F.negin( *Xi );
			}
		}
	} // FflasLower
	else{
	if ( TransA == FflasTrans){
			Ai = A; 
			Xi = X;
			for( ; Xi<X+N*incX; Ai+=lda+1,Xi+=incX ){
				F.negin( *Xi );
				for ( Xj = Xi-incX, Aj=Ai-lda; Xj>=X;
				      Xj-=incX, Aj-=lda){
					F.axpyin( *Xi, *Xj, *Aj );
				}
				
				if ( Diag==FflasNonUnit )
					F.divin(*Xi,*Ai);
				F.negin( *Xi );
			}

		} // FflasTrans
		else{
			Ai = A+(lda+1)*(N-1); 
			Ximax = Xi = X+incX*(N-1);
			for( ; Xi>=X; Ai-=lda+1,Xi-=incX ){
				F.negin( *Xi );
				for ( Xj = Xi+incX, Aj=Ai+1; Xj<=Ximax;
				      Xj+=incX, Aj++){
					F.axpyin( *Xi, *Xj, *Aj );
				}
				if ( Diag==FflasNonUnit )
					F.divin(*Xi,*Ai);
				F.negin( *Xi );
			}
		}
	}
}
