#ifndef __MPICPP_H_
#define __MPICPP_H_
#include <iterator>
#include <mpi.h>

namespace LinBox {
/* Idea:  Only use ssend for send.
*/
class Communicator
{   public:
    // constructors and destructor

	// constructor from existing communicator 
	//`Communicator(MPI_Comm comm = MPI_COMM_NULL);
   Communicator(MPI_Comm comm);
	// MPI_initializing constructor
	// When this communicator is destroyed MPI is shut down (finalized).
	Communicator(int* ac, char*** av);

	// copy constructor
	Communicator(const Communicator& D);

	~Communicator();

    // accessors
	int size();

int rank();

	MPI_Status status(); 

	MPI_Comm mpi_communicator();

    // peer to peer communication
	template < class Ptr >
	void send( Ptr b, Ptr e, int dest, int tag);

	template < class Ptr >
	void ssend( Ptr b, Ptr e, int dest, int tag);

	template < class Ptr >
	void recv( Ptr b, Ptr e, int dest, int tag);

	template < class X >
	void send( X *b, X *e, int dest, int tag);

	template < class X >
	void recv( X *b, X *e, int dest, int tag);

	// whole object send and recv
	template < class X >
	void send( X& b, int dest /*, int tag = 0 */);

	template < class X >
	void ssend( X& b, int dest /*, int tag = 0 */);

	template < class X >
	void bsend( X& b, int dest);

	template < class X >
	void recv( X& b, int dest /*, int tag = 0*/);

	template < class X >
	void buffer_attach( X &b);

	template < class X >
	int buffer_detach( X &b, int *size);


    // collective communication
	template < class Ptr, class Function_object >
	void reduce( Ptr bloc, Ptr eloc, Ptr bres, Function_object binop, int root);

    protected:
	MPI_Comm _mpi_comm; // MPI's handle for the communicator
	bool _mpi_boss; // true of an MPI initializing communicator
                       // There is at most one initializing communicator.
	MPI_Status stat; // status from most recent receive

};
}// namespace LinBox

#include "mpicpp.inl"
#endif
