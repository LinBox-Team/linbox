/* -*- mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/** file: blackbox_parallel.h
 *  Author: Zhendong Wan
 */

#ifndef __BLACKBOX_PARALLEL_H__
#define __BLACKBOX_PARALLEL_H__

/** parallel apply and apply transpose
 */
#include <linbox/vector/subvector.h>
#include <linbox/vector/vector-domain.h>
#include <linbox/algorithms/density.h>
#include <linbox/blackbox/subrowmatrix.h>
#include <linbox/blackbox/blackbox_thread.h>

#include <typeinfo>

namespace LinBox {

	/** General case
	 *  Matrix needs to be RowMatrix
	 */
	template <class Out, class Matrix, class In>
	Out& BlackboxParallel(Out& out,const Matrix& m,
			      const In& in, BBBase::BBType type);


	template <class Out, class Matrix, class In>
	Out& BlackboxParallel(Out& out, const Matrix& cm, const In& in, BBBase::BBType type) {

		typedef SubRowMatrix<Matrix> SubMatrix;

		typedef typename Out::iterator OutIterator;

		typedef typename In::const_iterator InIterator;

		typename BB_list_list::const_iterator cp;

		const std::type_info * key = & typeid (Out& (*) (Out&, const Matrix&, const In&));

		// search the threads info location
		cp = cm. sub_list. find (key);

		// allocate threads for this apply function
		if (cp == cm. sub_list. end()) {

			Matrix& m = const_cast<Matrix&>(cm);

			m. sub_list. insert (std::pair<const std::type_info*, 
						    BB_list>
						(key, BB_list()));
	
			typename BB_list_list::iterator p;

			p = m. sub_list. find (key);

			cp = p;

			pthread_sigmask (SIG_BLOCK, &(Thread::sigset), 0);

			const In* inaddr = NULL;

			int nnz = 0;

			int aver_load, cur_load;

			typename Matrix::ConstRowIterator row_p;

			for (row_p = m. rowBegin(); row_p != m. rowEnd(); ++ row_p)

				nnz += density(*row_p);

			aver_load = (int)ceil((double)nnz / (double)LINBOX_NTHR);

			cur_load = 0;
		
			int len = 0;

			int pos = 0;

			SubMatrix* subp;

			Out* outaddr = NULL;

			BBBase* t;

			for (row_p = m. rowBegin(); row_p != m. rowEnd(); ++ row_p) {
			
				cur_load += density(*row_p);

				++ len;

				if ((cur_load >= aver_load) || (((size_t)(pos + len) == cm. rowdim()) && (cur_load > 0))) {

					cur_load = 0;

					subp = new SubMatrix(&m, pos, len);

					t = createBBThread(subp, outaddr, inaddr);

					p -> second. push_back (t);
				
					pos += len;

					len = 0; 
				}

			}

		}

		switch (type) {

		case BBBase::Apply :  {

			std::vector<Subvector<OutIterator>*> outv(cp -> second.size());

			typename std::vector<Subvector<OutIterator>*>::iterator outv_p = outv. begin();

			int len = 0;

			int pos = 0;

			BB_list::const_iterator ti_p;

			const In* inaddr = &in;

			BBBase* bp = NULL;

			// give the input for each threads, and signal them
			for (ti_p = cp -> second. begin(); ti_p != cp -> second. end(); ++ ti_p, ++ outv_p) {

				bp = *(ti_p);

				len = ((const SubMatrix*)bp -> bb_addr) -> rowdim();

				*outv_p = new Subvector<OutIterator>(out. begin() + pos,
							  out. begin() + (pos + len));

				bp -> bb_outaddr = (void*) *outv_p;

				bp -> bb_inaddr = (const void*) inaddr;

				bp -> bb_type = BBBase::Apply;

				Thread::signal_thread (bp);

				pos += len;
			}

			OutIterator out_p;

			for (out_p = out. begin() + pos; out_p != out. end(); ++ out_p) 

				cm. field(). init (*out_p, 0);
	
			for (ti_p = cp ->second. begin(); ti_p != cp -> second. end(); ++ ti_p)
		
			Thread::wait_thread (*ti_p);	
			
			for (outv_p = outv. begin(); outv_p != outv. end(); ++ outv_p)

			delete (*outv_p);
			
			break; }

		case BBBase::ApplyTranspose : {

			int nthr = cp -> second. size ();

			OutIterator out_p;
			
			 for (out_p = out. begin(); out_p != out. end(); ++ out_p) 

                                cm. field(). init (*out_p, 0);

			std::vector<std::vector<typename Matrix::Field::Element> > out_v(nthr);

			std::vector<Subvector<InIterator>*> in_v(nthr);

			typename std::vector<std::vector<typename Matrix::Field::Element> >::iterator out_v_p;

			typename std::vector<Subvector<InIterator>*>::iterator in_v_p;

                        int len = 0;

                        int pos = 0;

                        BB_list::const_iterator ti_p;

                        BBBase* bp = NULL;

			for (ti_p = cp -> second. begin (), in_v_p = in_v. begin(), out_v_p = out_v. begin();
			     ti_p != cp -> second. end();
		  	     ++ ti_p, ++ in_v_p, ++ out_v_p) {

				bp = *ti_p;

				len = ((const SubMatrix*)bp -> bb_addr) -> rowdim();

				*in_v_p = new Subvector<InIterator>(in. begin() + pos, in. begin() + (pos + len));
				
				out_v_p -> resize (cm. coldim());
				
				bp -> bb_outaddr = &(*out_v_p);

				bp -> bb_inaddr = *in_v_p;

				bp -> bb_type = BBBase::ApplyTranspose;

				Thread::signal_thread (bp);

				pos += len;
			}

			for (ti_p = cp ->second. begin(); ti_p != cp -> second. end(); ++ ti_p)

                       		Thread::wait_thread (*ti_p);    

			VectorDomain<typename Matrix::Field> vd(cm. field());

			for (out_v_p = out_v. begin(), in_v_p = in_v. begin();
			     out_v_p != out_v. end(); ++ out_v_p, ++ in_v_p) {

				vd. addin (out, *out_v_p);

				delete (*in_v_p);

			}

			break; }

		default :
			break;
		}		
	
		return out;
	}

}

#endif
