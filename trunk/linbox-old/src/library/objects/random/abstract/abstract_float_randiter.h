/* File: src/library/objects/random/abstract/abstract_float_randiter.h
 * Author: William J Turner for the LinBox group
 */

#ifndef _ABSTRACT_FLOAT_RANDITER_
#define _ABSTRACT_FLOAT_RANDITER_

#include <iostream>
#include <vector>
#include "LinBox/integer.h"
#include "LinBox/abstract_float.h"
#include "LinBox/abstract_float_element.h"
#include "LinBox/randiter_abstract.h"

// Namespace in which all LinBox code resides
namespace LinBox 
{ 

  /** Random field base element generator.
   * This encapsulated class is a generator of random field base elements for 
   * the encapsulating field.
   * It is required to contain constructors from a field object and
   * two integers.  The first integer being a cardinality of a set to 
   * draw the random elements from, and the second being a seed for the 
   * random number generator.
   * It is also required to contain a copy constructor, a destructor, and
   * an operator() which acts on a reference to a field base element.  In this 
   * operator(), the random element is placed into the input field base element 
   * and also returned as a reference.
   */
  class abstract_float_randIter : public RandIter_abstract
  {
  public:

    /// Element type
    typedef abstract_float_element element;

    /** Constructor from field, sampling size, and seed.
     * The random field element iterator works in the field F, is seeded
     * by seed, and it returns any one element with probability no more
     * than 1/min(size, F.cardinality()).
     * A sampling size of zero means to sample from the entire field.
     * A seed of zero means to use some arbitrary seed for the generator.
     * Purely virtual.
     * @param F LinBox field archetype object in which to do arithmetic
     * @param size constant integer reference of sample size from which to 
     *             sample (default = 0)
     * @param seed constant integer reference from which to seed random number
     *             generator (default = 0)
     */
    abstract_float_randIter(const abstract_float& F, 
			     const integer& size = 0, 
			     const integer& seed = 0)
      : _F(F), _size(size), _seed(seed)
    { if (_seed == 0) _seed = time(NULL); }

    /** Copy constructor.
     * Constructs abstract_float_randIter object by copying the random field
     * element generator.
     * This is required to allow generator objects to be passed by value
     * into functions.
     * @param  R abstract_float_randIter object.
     */
    abstract_float_randIter(const abstract_float_randIter& R) 
      : _F(R._F), _size(R._size), _seed(R._seed) {}

    /** Destructor.
     * Required by abstract base class.
     * This destructs the random field element generator object.
     */
    ~abstract_float_randIter() {}
    
    /** Assignment operator.
     * Assigns abstract_float_randIter object R to generator.
     * Required by abstract base class.
     * @param  R abstract_float_randIter object.
     */
    RandIter_abstract& operator=(const RandIter_abstract& R)
    {
      if (this != &R) // guard against self-assignment
      {
	_size = static_cast<const abstract_float_randIter&>(R)._size;
	_seed = static_cast<const abstract_float_randIter&>(R)._seed;
      }

      return *this;
    }
 
    /** Virtual constructor from field, sampling size, and seed.
     * Required because constructors cannot be virtual.
     * Passes construction on to derived classes.
     * The random field element iterator works in the field F, is seeded
     * by seed, and it returns any one element with probability no more
     * than 1/min(size, F.cardinality()).
     * A sampling size of zero means to sample from the entire field.
     * A seed of zero means to use some arbitrary seed for the generator.
     * Purely virtual.
     * @param F LinBox field archetype object in which to do arithmetic
     * @param size constant integer reference of sample size from which to 
     *             sample (default = 0)
     * @param seed constant integer reference from which to seed random number
     *             generator (default = 0)
     */
    RandIter_abstract* construct(const Field_abstract& F, 
				 const integer& size = 0, 
				 const integer& seed = 0) const
    { 
      return new 
	abstract_float_randIter(static_cast<const abstract_float&>(F),
				 size,
				 seed);
    } // RandIter_abstract* construct(const Field_abstract&, ...)

    /** Virtual copy constructor.
     * Required because constructors cannot be virtual.
     * Passes construction on to derived classes.
     * Required by abstract base class.
     * @return pointer to new RandIter_abstract object in dynamic memory.
     */
    RandIter_abstract* clone(void) const
    { return new abstract_float_randIter(*this); }

    /** Random field element creator.
     * This returns a random field element from the information supplied
     * at the creation of the generator.
     * Required by abstract base class.
     * @return reference to random field element
     */
    Element_abstract& operator() (void) 
    {
      // Create new random elements
      if (_size == 0)
	return *(new element(rand()));
      else
	return *(new element(static_cast<long>((float(rand())/RAND_MAX)*double(_size))));

    } // element& operator() (void)

  private:

    /// Field in which arithmetic is done
    abstract_float _F;

    /// Sampling size
    integer _size;
    
    /// Seed
    integer _seed;

  }; // class abstract_float_randIter : public RandIter_abstract

} // namespace LinBox 

#endif // _ABSTRACT_FLOAT_RANDITER_
