// ========================================================================
// Copyright(c)'2001 by LinBox Team
// see the copyright file.
// Authors: M. Samama, T. Gautier
// Time-stamp: <28 Aug 02 16:16:09 Jean-Guillaume.Dumas@imag.fr> 
// ========================================================================
// Description: 
// Integer class definition based on Gmp (>V2.0 or 1.3.2)

#include "givaro-config.h"
#ifndef _GMPplusplus_INTEGER_H_
#define _GMPplusplus_INTEGER_H_

#ifndef __DONOTUSE_64_bits__
#define __USE_64_bits__
#endif

// If GMP is at least version 4, do not need extern
#ifdef GMP_VERSION_4
#define __GMP_CPLUSPLUS__
#include <gmpxx.h>
#endif

#ifndef GMP_VERSION_4
extern "C" {
#endif

#include "gmp.h"

#ifndef GMP_VERSION_4
}
#endif

#include <vector>
#include <list>
#include <string>


#ifdef __USE_64_bits__
#    define __USE_GMPPLUSPLUS_64__
#endif

#ifdef __USE_ISOC99
#    define __USE_GMPPLUSPLUS_64__
#endif


  //------------------------------------------------------ Class Integer
class Integer {

public:
  typedef std::vector<mp_limb_t> vect_t;
  Integer( const std::vector<mp_limb_t>& vect_t );
  //--------------------------------------cstors & dstors
  Integer(int n = 0);
  Integer(long n);
  Integer(unsigned int n);
  Integer(unsigned long n);
#ifdef __USE_GMPPLUSPLUS_64__
  Integer(long long n);
  Integer(unsigned long long n);
#endif
  Integer(double d);
  Integer(const char *s);
  Integer(const Integer& n);
  ~Integer();

  //------------------------------------- predefined null and one
  static const Integer zero;
  static const Integer one;

  // -- Assignment and copy operators
  Integer& operator = (const Integer& n);
  Integer& logcpy(const Integer& n);
  Integer& copy(const Integer& n);
  
  //------------------Equalities and inequalities between integers and longs
  int operator != (const int l) const;
  int operator != (const long l) const;
  
  friend int compare(const Integer& a, const Integer& b);
  friend int absCompare(const Integer& a, const Integer& b);

  int operator > (const int l) const;
  int operator > (const long l) const;
  int operator < (const int l) const;
  int operator < (const long l) const;

  //----------------Elementary arithmetic between Integers & longs
  Integer& operator += (const Integer& n);  
  Integer& operator += (const unsigned long l);  
  Integer& operator += (const long l);  
  Integer  operator + (const Integer& n) const;  
  Integer  operator + (const unsigned long l) const;
  Integer  operator + (const long l) const;

  Integer& operator -= (const Integer& n);  
  Integer& operator -= (const unsigned long l);  
  Integer& operator -= (const long l);  
  Integer  operator - (const Integer& n) const;
  Integer  operator - (const unsigned long l) const;
  Integer  operator - (const long l) const;
  Integer  operator -() const;

  Integer& operator *= (const Integer& n);  
  Integer& operator *= (const unsigned long l);  
  Integer& operator *= (const long l);  
  Integer  operator * (const Integer& n) const;
  Integer  operator * (const unsigned long l) const;
  Integer  operator * (const long l) const;

  // -- Euclidian division of a/b: returns q or r such that
  // - a=b*q + r, with |r| < |b|, a*r >=0
  Integer& operator /= (const Integer& n);  
  Integer& operator /= (const unsigned long l);
  Integer& operator /= (const long l);
  Integer  operator /  (const Integer& n) const;
  Integer  operator /  (const unsigned long l) const;
  Integer  operator /  (const long l) const;

  Integer& operator %= (const Integer& n);  
  Integer& operator %= (const unsigned long l);
  Integer& operator %= (const long l);
  Integer  operator % (const Integer& n) const;
  long  operator % (const unsigned long l) const;
  long  operator % (const long l) const;

  // - Methods
static Integer& addin (Integer& res, const Integer& n);  
static Integer& addin (Integer& res, const long n);  
static Integer& addin (Integer& res, const unsigned long n);  
static Integer& add   (Integer& res, const Integer& n1, const Integer& n2);  
static Integer& add   (Integer& res, const Integer& n1, const long n2);  
static Integer& add   (Integer& res, const Integer& n1, const unsigned long n2);  

static Integer& subin (Integer& res, const Integer& n);  
static Integer& subin (Integer& res, const long n);  
static Integer& subin (Integer& res, const unsigned long n);  
static Integer& sub   (Integer& res, const Integer& n1, const Integer& n2);  
static Integer& sub   (Integer& res, const Integer& n1, const long n2);  
static Integer& sub   (Integer& res, const Integer& n1, const unsigned long n2);  
static Integer& negin (Integer& res);  
static Integer& neg   (Integer& res, const Integer& n);  

static Integer& mulin (Integer& res, const Integer& n);  
static Integer& mulin (Integer& res, const long n);  
static Integer& mulin (Integer& res, const unsigned long n);  
static Integer& mul   (Integer& res, const Integer& n1, const Integer& n2);  
static Integer& mul   (Integer& res, const Integer& n1, const long n2);  
static Integer& mul   (Integer& res, const Integer& n1, const unsigned long n2);  
static Integer& axpy   (Integer& res, const Integer& a, const Integer& x, const Integer& y );  
static Integer& axpyin   (Integer& res, const Integer& a, const Integer& x);  
static Integer& axmy   (Integer& res, const Integer& a, const Integer& x, const Integer& y );  
static Integer& axmyin   (Integer& res, const Integer& a, const Integer& x);  

static Integer& divin (Integer& q, const Integer& n);  
static Integer& divin (Integer& q, const long n);  
static Integer& divin (Integer& q, const unsigned long n);  
static Integer& div   (Integer& q, const Integer& n1, const Integer& n2);  
static Integer& div   (Integer& q, const Integer& n1, const long n2);  
static Integer& div   (Integer& q, const Integer& n1, const unsigned long n2);  
static Integer& divexact  (Integer& q, const Integer& n1, const Integer& n2);  
static Integer  divexact  (const Integer& n1, const Integer& n2);  

static Integer& modin (Integer& r, const Integer& n);  
static Integer& modin (Integer& r, const long n);  
static Integer& modin (Integer& r, const unsigned long n);  
static Integer& mod   (Integer& r, const Integer& n1, const Integer& n2);  
static Integer& mod   (Integer& r, const Integer& n1, const long n2);  
static Integer& mod   (Integer& r, const Integer& n1, const unsigned long n2);  

  // -- return q, the quotient
static Integer& divmod   (Integer& q, Integer& r, const Integer& n1, const Integer& n2);  
static Integer& divmod   (Integer& q, Integer& r, const Integer& n1, const long n2);  
static Integer& divmod   (Integer& q, Integer& r, const Integer& n1, const unsigned long n2);  

  
  //------------------------------------- Arithmetic functions
  friend Integer gcd (const Integer& a, const Integer& b);
  friend Integer gcd (const Integer& a, const Integer& b, 
                            Integer& u, Integer& v);
  friend Integer& gcd (Integer& g, const Integer& a, const Integer& b);
  friend Integer& gcd (Integer& g, const Integer& a, const Integer& b, 
                            Integer& u, Integer& v);

  friend Integer pp( const Integer& P, const Integer& Q );


  // - return n^l 
  friend Integer pow(const Integer& n, const long l);
  friend Integer pow(const Integer& n, const unsigned long l);
  friend Integer pow(const Integer& n, const int l) { return pow(n, (long)l ); }
  friend Integer pow(const Integer& n, const unsigned int l) { return pow(n, (unsigned long)l ); }

  // - return n^e % m
  friend Integer powmod(const Integer& n, const unsigned long e, const Integer& m);
  friend Integer powmod(const Integer& n, const long e, const Integer& m);
  friend Integer powmod(const Integer& n, const unsigned int e, const Integer& m) { return powmod(n, (unsigned long)e, m); }
  friend Integer powmod(const Integer& n, const int e, const Integer& m)  { return powmod(n, (long)e, m); }
  friend Integer powmod(const Integer& n, const Integer& e, const Integer& m);

  friend Integer fact ( unsigned long l);
  
  friend Integer sqrt(const Integer& p);
  friend Integer sqrt(const Integer& p, Integer& r);
  friend long logp(const Integer& a, const Integer& p) ;

  //-----------------------------------------Miscellaneous
  friend inline int sign   (const Integer& a);
  friend inline int iszero (const Integer& a);
  friend inline int isone  (const Integer& a);

  friend Integer abs(const Integer& n);

  friend int probab_prime(const Integer& p);
  friend int probab_prime(const Integer& p, int r);
  friend int jacobi(const Integer& u, const Integer& v) ;
  friend int legendre(const Integer& u, const Integer& v) ;


  Integer operator << (unsigned int l) const; // lshift
  Integer operator >> (unsigned int l) const; // rshift
  Integer operator << (unsigned long l) const; // lshift
  Integer operator >> (unsigned long l) const; // rshift

  // - return the size in byte
  friend inline unsigned long length (const Integer& a); 
  // - return the size in word.
  size_t size() const;
  // - return the i-th word of the integer. Word 0 is lowest word.
  unsigned long operator[](size_t i) const; 

  // -- Convert an Integer to a basic C++ type
  // -- Cast operators
  friend long   Integer2long  ( const Integer& n);
  friend vect_t& Integer2vector  (vect_t& v, const Integer& n);
  friend double Integer2double( const Integer& n);
  friend std::string& Integer2string(std::string&, const Integer&, int base = 10);
  operator short() const 
	  { return (int) *this; }
  operator unsigned short() const 
	  { return (unsigned int) *this; }
  operator unsigned int() const ;
  operator int() const ;
  operator unsigned long() const ;
  operator long() const ;
  operator std::string() const ;
  operator float() const ;
  operator double() const ;
  operator vect_t() const ;

  //--------------------Random Iterators
  // -- return a random number with sz machine word. 
  // -- To be improved.
#ifdef __GMP_CPLUSPLUS__
    static void seeding(unsigned long int s=0);
    static gmp_randclass& randstate(unsigned long int s=0);
#endif
    static Integer  random(int sz=1 );
    static Integer  nonzerorandom(int sz=1 );
    static Integer& random(Integer& r, const Integer& size );
    static Integer& nonzerorandom(Integer& r, const Integer& size );
    static Integer& random(Integer& r, long size =1 );
    static Integer& nonzerorandom(Integer& r, long size =1 );
  //----------------------------------------------I/O

  friend std::istream& operator >> (std::istream &i, Integer& n);
  friend std::ostream& operator << (std::ostream &o, const Integer& n);
  friend std::ostream& absOutput (std::ostream &o, const Integer& n);

  std::ostream& print( std::ostream& o ) const;
  
protected:

    typedef MP_INT Rep;

    Rep gmp_rep;

    int priv_sign() const;

    // -- Creates a new Integer from a size sz and a array of unsigned long d 
    Integer(unsigned long* d, long size);

}; //----------------------------------------------- End of Class Integer



#include "gmp++_int.inl"

#endif
