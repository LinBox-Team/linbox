#ifndef _TIMER_H_
#define _TIMER_H_
// ==========================================================================
// $Source$
// Copyright(c)'94-97 by Givaro Team
// see the copyright file.
// Authors: T. Gautier
// $Id$
// ==========================================================================
// Description:

#include <iostream.h>


class BaseTimer { 
public:
enum { 
  MSPSEC = 1000000  // microsecond per second
};

   // -- Clear timer :
  inline void clear() { _t = 0; }

  // -- total amount of second spent 
  inline double time() const { return _t; }

  // -- Return a value to initialize random generator
static long seed();

  // -- basic methods:
  ostream& print( ostream& ) const;
       
  // -- Some arithmetic operators to compute cumulative time :
  BaseTimer& operator = (const BaseTimer & T) ;
  const BaseTimer operator - (const BaseTimer & T)  const;
  const BaseTimer operator - () ;
  const BaseTimer operator +  (const BaseTimer & T)  const;
  const BaseTimer operator += (const BaseTimer & T) { return *this = *this + T; };
  const BaseTimer operator -= (const BaseTimer & T) { return *this = *this - T; };

public:
   double _t;  // time  
};
inline ostream& operator<< (ostream& o, const BaseTimer& BT)
{ return BT.print(o);}


class RealTimer : public BaseTimer {
public:
  inline RealTimer( const BaseTimer& BT ): BaseTimer(BT) {};
  inline RealTimer( ){};
  void start();
  void stop();
};


class UserTimer : public BaseTimer {
public:
  inline UserTimer( const BaseTimer& BT ): BaseTimer(BT) {};
  inline UserTimer( ) {};
  void start();
  void stop();
};


class SysTimer : public BaseTimer {
public:
  inline SysTimer( const BaseTimer& BT ): BaseTimer(BT) {};
  inline SysTimer( ) {};
  void start();
  void stop();
};


class Timer {
public :

   // Clear timer :
  void clear(); 

   // Start timer
  void start();

  // Stop timer 
  void stop();

  // total amount of second spent in user mode
  double usertime() const { return ut.time(); }

  // total amount of second spent in system mode
  double systime () const { return st.time(); }

  // real total amount of second spent.  
  double realtime () const { return rt.time(); }

  // retourne une petite graine
  // long seed() const { return RealTimer::seed(); }

  // Some arithmetic operators to compute cumulative time :
  Timer& operator = (const Timer & T) ;
  const Timer operator - (const Timer & T)  const;
  const Timer operator - () ;
  const Timer operator + (const Timer & T)  const;
  const Timer operator += (const Timer & T) { return *this = *this + T; };
  const Timer operator -= (const Timer & T) { return *this = *this - T; };


  // -- methods :
  ostream& print( ostream& ) const;

public:
 RealTimer rt;
 UserTimer ut;
 SysTimer  st;
};
inline ostream& operator<<( ostream& o, const Timer& T)
{ return T.print(o);}

#include "givtimer.C"

#endif 
