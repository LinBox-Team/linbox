/* File: src/wrapper/by_library/Givaro/Field/lin_zpz_giv.h
 * Author: Pascal Giorgi for the LinBox group
 */



/* WARNING !!!
 * 
 * This wrapper doesn't work properly.
 * there are conflicts between Integer of Givaro and Integer of LinBox.
 * So this wrapper can be used only without Integer of LinBox and 
 * without LinBox field archetype.
 * But it runs properly with the Integer of Givaro.
 * We are working on this problem !!!
 * Thanks for your comprehension.
 */

#ifndef _LIN_GFQ_GIV_
#define _LIN_GFQ_GIV_

//#include "LinBox/integer.h"

//------------------------------------
// Files of Givaro library
#include <givgfq.h>

//------------------------------------

// Namespace in which all LinBox code resides
namespace LinBox 
{ 

  typedef Integer integer;

  /** This template class is define just to be in phase with the LinBox
   *  archetype.
   *  Most of all methods are inherited from GFqDom<long>  class
   *  of Givaro.
   *  these class allow to construct only finite field with a prime modulus.
   */   
 class givaro_gfq : public GFqDom<long>
  {
  public:

    /** Element type.
     *  This type is inherited from the Givaro class GFqDom<long>
     */
    typedef  GFqDom<long>::Rep element;
    

    /** Constructor from an integer
     *  this constructor use the ZpzDom<TAG> constructor
     */
    givaro_gfq(const integer& p, const integer& k) :
      GFqDom<long>(static_cast<UTT>(Integer2long(p)), static_cast<UTT>(Integer2long(k))) {}
    

    /** Characteristic.
     * Return integer representing characteristic of the domain.
     * Returns a positive integer to all domains with finite characteristic,
     * and returns 0 to signify a domain of infinite characteristic.
     * @return integer representing characteristic of the domain.
     */
    integer& characteristic(integer& c) const
      {return c=*(new integer(static_cast<int>(GFqDom<long>::caracteristic())));}
      
      
    /** Cardinality. 
     * Return integer representing cardinality of the domain.
     * Returns a non-negative integer for all domains with finite
     * cardinality, and returns -1 to signify a domain of infinite
     * cardinality.
     * @return integer representing cardinality of the domain
     */
    integer& cardinality(integer& c) const
      { return c=*(new integer(static_cast<int>(GFqDom<long>::size())));}
 

   /** Conversion of field base element to an integer.
     * This function assumes the output field base element x has already been
     * constructed, but that it is not already initialized.
     * @return reference to an integer.
     * @param x integer to contain output (reference returned).
     * @param y constant field base element.
     */
    integer& convert(integer& x, const element& y) const
      {return x = integer(static_cast<int>(y));}
      

    /** Initialization of field base element from an integer.
     * Behaves like C++ allocator construct.
     * This function assumes the output field base element x has already been
     * constructed, but that it is not already initialized.
     * We assume that the type of element is short int.
     * this methos is just a simple cast.
     * @return reference to field base element.
     * @param x field base element to contain output (reference returned).
     * @param y integer.
     */  
    element& init(element& x , const integer& y=0) const
      { return x=  element(static_cast<int>(Integer2long(y)));}
      
    
  }; // class givaro_gfq
 

} // namespace LinBox

#endif // _LIN_GFQ_GIV_
