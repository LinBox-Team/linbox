/* File: src/wrapper/by_scope/field/LIDIA_randiter.h
 * Author: Pascal Giorgi for the LinBox group
 */

#ifndef __RANDITER_LIDIA_GFQ
#define __RANDITER_LIDIA_GFQ

#include "LiDIA/gf_element.h"

#include "linbox/integer.h"
#include "linbox-config.h"

#ifdef __LINBOX_XMLENABLED

#include "linbox/util/xml/linbox-reader.h"
#include "linbox/util/xml/linbox-writer.h"

using LinBox::Reader;
using LinBox::Writer;

#include <iostream>
#include <string>

using std::istream;
using std::ostream;
using std::string;

#endif

namespace LinBox
{
using namespace LiDIA;

  


 template<class field> class LidiaGfqRandIter
    {
    public:
      

      /* Element of the class Field
       */
      typedef typename field::Element Element;

      /* Constructor of the class LidiaGfqRandIter .
       * the size has to be a divisor of the degree of the Field F.
       * the seed doesn't do something for the moment.
       */
      LidiaGfqRandIter(const field& F, 
		       const integer& size = 0, 
		       const integer& seed = 0)
	: _size(size), _seed(seed) , GF(F)
	{ 
	  if (_seed == integer(0)) _seed = time(NULL);
	  
	  integer cardinality ;
	  F.cardinality(cardinality);
	  if ( (cardinality != integer(-1)) &&  (_size > cardinality) )
	    _size = cardinality;
	  
	  
#ifdef TRACE
	  cout << "created random generator with size " << _size 
	       << " and seed " << _seed << endl;
#endif // TRACE
	  
	  // Seed random number generator
	  srand(_seed);
	  
	}

#ifdef __LINBOX_XMLENABLED

      // XML Reader constructor
      LidiaGfqRandIter(Reader &R) : GF(R.Down(1))
      {
	      R.Up(1);
	      if(!R.expectTagName("randiter")) return;
	      if(!R.expectAtteributeNum("size", _size) || !R.expectAttributeNum("seed", _seed)) return;

	      return;
      }
#endif

      
      LidiaGfqRandIter(const LidiaGfqRandIter& R)
	: _size(R._size), _seed(R._seed) {}
      
      
      ~LidiaGfqRandIter(void) {}
      
      
      LidiaGfqRandIter& operator=(const LidiaGfqRandIter& R)
	{
	  if (this != &R) // guard against self-assignment
	    {
	      _size = R._size;
	      _seed = R._seed;
	    }
	  
	  return *this;
	}           


      Element& random (Element& x)  const
	{
	  Element e(GF);
	  if (_size == 0)
	    e.randomize();
	  else	  
	    e.randomize(_size);
	  
	  return x=*(new Element(e));
	  
	  /* Another method allowing generate with a sampling size
	     
	  long temp;
	  Element e;
	  if (_size==0)
	  temp= rand();
	  else
	  temp= static_cast<long>((double(rand())/RAND_MAX)*double(_size));
	  
	  GF.init(e,integer(temp));
	  return x=*(new Element(e));
	  */
	  
	}
      
      LidiaGfqRandIter(void) : _size(0), _seed(0) { time(NULL); }
      
#ifdef __LINBOX_XMLENABLED
      // XML writing rand-iter object

      ostream &write(ostream &os) const
      {

	      Writer W;
	      if( toTag(W) )
		      W.write(os);

	      return os;
      }

      bool toTag(Writer &W) const
      {
	      string s;

	      W.setTagName("randiter");
	      W.setAttribute("size", Writer::numToString(s, _size));
	      W.setAttribute("seed", Writer::numToString(s, _seed));

	      W.addTagChild();
	      if(!GF.toTag(W)) return false;
	      W.upToParent();

	      return true;
      }
#endif

      
    private:
      
      /// Sampling size
	integer _size;
	
	/// Seed
	  integer _seed;
	  
	  field GF;
	  
    }; // class LidiaGfqRandIter
 
} // namespace LinBox

#endif // -_LIDIA_RANDITER_GFQ
