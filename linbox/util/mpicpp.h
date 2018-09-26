/* Copyright (C) 2018 The LinBox group
 * Updated by Hongguang Zhu <zhuhongguang2014@gmail.com>
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
  * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */

#ifndef __LINBOX_mpicpp_H
#define __LINBOX_mpicpp_H

#ifndef __LINBOX_HAVE_MPI
typedef int Communicator;
#else
#include <iterator>

// problem of mpi(ch2) in C++
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#include <mpi.h>

namespace LinBox {
    /* Idea:  Only use ssend for send.
    */
    class Communicator {
    public:
        // constructor from existing communicator
        //`Communicator(MPI_Comm comm = MPI_COMM_NULL);
        Communicator(MPI_Comm comm);
        // MPI_initializing constructor
        // When this communicator is destroyed MPI is shut down (finalized).
        Communicator(int* ac, char*** av);

        Communicator(const Communicator& D);

        ~Communicator();

        // Accessors
        int size() const;
        int rank() const;
        MPI_Status status() const;
        MPI_Comm mpi_communicator() const;

        // peer to peer communication
        template <class Ptr>
        void send(Ptr b, Ptr e, int dest, int tag);

        template <class Ptr>
        void ssend(Ptr b, Ptr e, int dest, int tag);

        template <class Ptr>
        void recv(Ptr b, Ptr e, int dest, int tag);

        template <class X>
        void recv(X* b, X* e, int dest, int tag);

        // whole object send and recv
        template <class X>
        void send(X& b, int dest /*, int tag = 0 */);

        template <class Field>
        void send(BlasMatrix<Field>& b, int dest);
        template <class Field>
        void send(SparseMatrix<Field>& b, int dest);
        template <class Field>
        void send(BlasVector<Field>& b, int dest);

        template <class X>
        void isend(X& b, int dest /*, int tag = 0 */);

        template <class Field>
        void isend(BlasMatrix<Field>& b, int dest);
        template <class Field>
        void isend(SparseMatrix<Field>& b, int dest);
        template <class Field>
        void isend(BlasVector<Field>& b, int dest);

        template <class X>
        void ssend(X& b, int dest /*, int tag = 0 */);

        template <class Field>
        void ssend(BlasMatrix<Field>& b, int dest);
        template <class Field>
        void ssend(SparseMatrix<Field>& b, int dest);
        template <class Field>
        void ssend(BlasVector<Field>& b, int dest);

        template <class X>
        void bsend(X& b, int dest);

        template <class X>
        void recv(X& b, int dest /*, int tag = 0*/);

        template <class Field>
        void recv(BlasMatrix<Field>& b, int src);
        template <class Field>
        void recv(SparseMatrix<Field>& b, int src);
        template <class Field>
        void recv(BlasVector<Field>& b, int src);

        /*
           template < vector < class X > >
           void send( X& b, int dest );
        */

        template <class X>
        void buffer_attach(X b);

        template <class X>
        int buffer_detach(X& b, int* size);

        // collective communication
        template <class X>
        void bcast(X& b, int src);

        template <class Field>
        void bcast(BlasMatrix<Field>& b, int src);
        template <class Field>
        void bcast(SparseMatrix<Field>& b, int src);
        template <class Field>
        void bcast(BlasVector<Field>& b, int src);

        template <class X>
        void bcast(X* b, X* e, int src);

        template <class Ptr, class Function_object>
        void reduce(Ptr bloc, Ptr eloc, Ptr bres, Function_object binop, int root);

        // member access
        MPI_Status get_stat();

    protected:
        MPI_Comm _mpi_comm; // MPI's handle for the communicator
        bool _mpi_boss;     // true of an MPI initializing communicator
        // There is at most one initializing communicator.
        MPI_Status stat; // status from most recent receive
        MPI_Request req; // request from most recent send

        template <class X>
        void send_integerVec(X& b, int dest);
        template <class X>
        void send_integerMat(X& b, int dest);
        template <class X>
        void send_integerSparseMat(X& b, int dest);

        template <class X>
        void ssend_integerVec(X& b, int dest);
        template <class X>
        void ssend_integerMat(X& b, int dest);
        template <class X>
        void ssend_integerSparseMat(X& b, int dest);

        template <class X>
        void isend_integerVec(X& b, int dest);
        template <class X>
        void isend_integerMat(X& b, int dest);
        template <class X>
        void isend_integerSparseMat(X& b, int dest);

        template <class X>
        void recv_integerVec(X& b, int src);
        template <class X>
        void recv_integerMat(X& b, int src);
        template <class X>
        void recv_integerSparseMat(X& b, int src);
        template <class X>
        void bcast_integerVec(X& b, int src);
        template <class X>
        void bcast_integerMat(X& b, int src);
        template <class X>
        void bcast_integerSparseMat(X& b, int src);
    };
}

#include "mpicpp.inl"

#endif // __LINBOX_HAVE_MPI
#endif // __LINBOX_mpicpp_H

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
