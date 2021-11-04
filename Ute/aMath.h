#ifndef _AMATH_H_
#define _AMATH_H_

//
// DST Ute Library (Utilities Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2018 Commonwealth of Australia
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

//
// Title:      Mathematics Class
// Class:      aMath
// File:       aMath.h
// Originally: CMath.h
//
// Description:
// This files contains the class specifiers for miscellaneous mathematical
// procedures. Reformatted to comply with the flight systems guidelines.
//
// Written by: Geoff Brian (Geoff.Brian@dsto.defence.gov.au)
//             Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 2004
//

// C++ Includes
#include <limits>
#include <algorithm>
#include <cmath>
#include <cfloat>

#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
  #define _DSTO_MATH_RANGE_CHECK_
#endif

//
// Math Macros
//
//
// math_range_code(): only execute in checking math ranges.
//
#ifdef _DSTO_MATH_RANGE_CHECK_
  #ifndef math_range_check
    #define math_range_check( code) \
      code
  #endif
  #ifndef else_no_math_range_check
    #define else_no_math_range_check( code)
  #endif
#else
  #ifndef math_range_check
    #define math_range_check( code)
  #endif
  #ifndef else_no_math_range_check
    #define else_no_math_range_check( code) \
      code
  #endif
#endif

namespace dstomath {

  // Constants
  const double piX2    = 6.283185307179586476925286766559;
  const double pi      = 3.1415926535897932384626433832795;
  const double pi_2    = 1.5707963267948966192313216916398;
  const double pi_3    = 1.0471975511965977461542144610932;
  const double pi_4    = 0.78539816339744830961566084581988;
  const double pi_6    = 0.52359877559829887307710723054658;
  const double pi_180  = 0.017453292519943295769236907684886;
  const double d180_pi = 57.295779513082320876798154814105;

  inline double zero()     {return DBL_EPSILON * 100.0;}
  inline double nan()      {return std::numeric_limits<double>::quiet_NaN();}
  inline double infinity() {return std::numeric_limits<double>::infinity();}

#if defined(_MSC_VER)
  template <typename T> T abs( const T &t)                  {if (t  < T(0)) return -t; return t;}
  template <typename T> T min( const T &t1, const T &t2)    {if (t1 < t2)   return t1; return t2;}
  template <typename T> T max( const T &t1, const T &t2)    {if (t1 > t2)   return t1; return t2;}

  template <typename T> bool isnan( const T &v)             {return ( 0 != _isnan(v));}
  template <typename T> bool isValid( const T &v)           {return ( 0 == _isnan(v));}
  template <typename T> int  finite( const T &v)            {return _finite(v);}
  template <typename T> T nearbyintpos( const T &v)         {return T( long((v) + 0.5));}
  template <typename T> T nearbyintneg( const T &v)         {return T( long((v) - 0.5));}
  template <typename T> T nearbyint( const T &v)            {if (v > T(0)) return nearbyintpos(v); return nearbyintneg(v);}
  template <typename T> T copysign( const T &a, const T &b) {return _copysign( a, b);}
  template <typename T> T asinh( const T &v)                {return ::log(v+sqrt(v*v+1.0));}
  template <typename T> T acosh( const T &v)                {return ::log(v+sqrt(v*v-1.0));}
  template <typename T> T atanh( const T &v)                {return 0.5*(::log(1.0+v)-::log(1.0-v));}
  #if (_MSC_VER < 1900)
    template <typename T> bool isinf( const T &v)           {return !_finite(v) ? 1 : 0;}
  #else
    template <typename T> bool isinf( const T &v)           {return std::isinf(v) ? 1 : 0;}
  #endif

#else
  template <typename T> T abs( const T &t)                  {return std::abs(t);}
  template <typename T> T min( const T &t1, const T &t2)    {return std::min(t1,t2);}
  template <typename T> T max( const T &t1, const T &t2)    {return std::max(t1,t2);}

  template <typename T> bool isnan( const T &v)             {return !(v == v);}
  template <typename T> bool isValid( const T &v)           {return  (v == v);}
  template <typename T> int  finite( const T &v)            {return std::isfinite(v) ? 1 : 0;}
  template <typename T> bool isinf( const T &v)             {return std::isinf(v) ? 1 : 0;}
  template <typename T> T nearbyint( const T &v)            {return ::nearbyint(v);}
  template <typename T> T copysign( const T &a, const T &b) {return ::copysign( a, b);}
  template <typename T> T asinh( const T &v)                {return ::asinh(v);}
  template <typename T> T acosh( const T &v)                {return ::acosh(v);}
  template <typename T> T atanh( const T &v)                {return ::atanh(v);}
#endif

#if __cplusplus > 199711L || _MSC_VER >= 1800
  template <typename T> inline T min( const T& t1) { return t1;}
  template <typename T, typename... Ts> 
  T min( const T& t1, const T& t2, const Ts&... args) 
  { 
    return min( min( t1, t2), min( args...)); // In this form so as the two parameter version above can remain untouched
  }

  template <typename T> inline T max( const T& t1) { return t1;}
  template <typename T, typename... Ts>
  T max( const T& t1, const T& t2, const Ts&... args) 
  { 
    return max( max( t1, t2), max( args...));
  }
#endif

  template <typename T> bool isZero( const T &v)
  {
    if ( dstomath::isnan( v)) {
      return false;
    }
    return ( abs(v) < zero());
  }
  template <typename T> bool isNotZero( const T &v) {return !isZero(v);}
  template <typename T> bool isPositive( const T& v) { return v >= zero();}
  template <typename T> bool isNegative( const T& v) { return v <= -zero();}

  /** A value of zero scaled by the size of the value (greater than 1.0) being tested.
   *
   */
  inline double zero( const double value)
  {
    return DBL_EPSILON * 10.0 * dstomath::max( 1.0, ::pow( 10.0, int( ::log10( abs( value)))));
  }

  /** Round to the nearest integer.
   * @param dataValue : the value to round.
   * @return data     : the rounded value of dataValue.
   * @code data = math.round( dataValue); @endcode.
   */
  template <typename T> inline int round( const T &dataValue)
  {
    if ( dataValue > T(0)) {
      return ( int( dataValue + T(0.5)));
    }

    return ( int( dataValue - T(0.5)));
  }

  //
  // Compute Functions
  //
  /** Compute data using linear interpolation.
   * @param f  : the f fraction for the interpolation point ( x data not required).
   * @param x  : the x data interpolation point.
   * @param x1 : the x1 bound for the interpolation interval.
   * @param x2 : the x2 bound for the interpolation interval.
   * @param y1 : the y1 bound for the interpolation interval.
   * @param y2 : the y2 bound for the interpolation interval.
   * @return interpolation : the interpolated value.
   * @code interpolation = math.vint( x, x1, x2, y1, y2); @endcode
   * @code interpolation = math.vint( f, y1, y2); @endcode
   */
  template <typename T> inline T  vint( const T &x, const T &x1, const T &x2,
         const T &y1, const T &y2)
  {
    T y = y1;
    if ( dstomath::isNotZero( x2 - x1)) {
      y = ( x - x1) * ( y2 - y1) / ( x2 - x1) + y1;
    }
    return y;
  }
  template <typename T> inline T  vint( const T &f, const T &y1, const T &y2)
  {
    return (f * ( y2 - y1) + y1);
  }

  /** Compute the mid point of an interval.
   * @param bound1    : the 1st boundary for the interval.
   * @param bound2    : the 2nd boundary for the interval.
   * @return midPoint : the mid point data value for the interval.
   * @code midPoint = math.mid_pnt( bound1, bound2); @endcode
   */
  template <typename T> inline T  mid_pnt( const T &bound1, const T &bound2)
  {
    return dstomath::vint( T( 0.5), T( 0), T( 1), bound1, bound2);
  }

  /** Check if data is within an interval.
   * @param checkData   : the data value to check.
   * @param bound1      : the 1st boundary for the interval.
   * @param bound2      : the 2nd boundary for the interval.
   * @return true/false : a logical indicating if data is between
   *                      or outside interval.
   * @code true/false = math.isBetween( checkData, bound1, bound2); @endcode
   * @code true/false = math.between( checkData, bound1, bound2); @endcode -- legacy code
   */
  template <typename T> inline bool isBetween( const T &checkData, const T &bound1, const T &bound2)
  {
    return ( checkData >= dstomath::min( bound1, bound2) && checkData <= dstomath::max( bound1, bound2));
  }
  template <typename T> inline int between( const T &checkData, const T &bound1, const T &bound2)
  {
    return int( dstomath::isBetween( checkData, bound1, bound2));
  }

  /** Compute the Euclidean normal of data.
   * @param data1 :
   * @param data2 :
   * @param data3 :
   * @return norm : the Euclidean norm.
   * @code norm = math.enorm( data1, data2); @endcode
   * @code norm = math.enorm( data1, data2, data3); @endcode
   */
  template <typename T> inline T enorm( const T &data1, const T &data2, const T &data3)
  {
    // Calculate the normal of three values.
    return ( ::sqrt(( data1 * data1) + ( data2 * data2) + ( data3 * data3)));
  }
  template <typename T> inline T enorm( const T &data1, const T &data2)
  {
    // Calculate the normal of two values.
    return ( ::sqrt(( data1 * data1) + ( data2 * data2)));
  }

  /** Computes the factorial of a number.
   * @param data       : the number.
   * @return factorial : the factorial of the data.
   * @code factorial = math.fact( data); @endcode
   */
  template <typename T> inline double fact( const T &indata)
  {
    int    idata = dstomath::round( indata);
    double data  = double( idata);
    if ( idata > 15) {
      return ::sqrt( 2.0 * pi * data) * ::pow(( data / 2.718281828), data);
    }
    for ( int i = idata - 1; i > 1; --i) {
      data *= double(i);
    }
    return data;
  }

  /** Determine whether data is an even integer number.
   * @param data : the integer to check.
   * @return true/false : logical indicating if the data is even.
   * @code true/false = math.isEven( data); @endcode
   * @code true/false = math.even( data); @endcode -- legacy code
   */
  inline bool isEven( const int &data){
    return ((data % 2) == 0);
  }
  inline int even( const int &data) { return int( dstomath::isEven( data));}

  /** Determine whether data is an odd integer number.
   * @param data        : the integer to check.
   * @return true/false : logical indicating if the data is odd.
   * @code true/false = math.isOdd( data); @endcode
   * @code true/false = math.odd( data); @endcode -- legacy code
   */
  inline bool isOdd( const int &data) { return !dstomath::isEven( data);}
  inline int odd( const int &data)    { return int( !dstomath::isEven( data));}

  /** Determine whether data is divisible by a divisor integer number.
   * @param data    : the integer to check.
   * @param divisor : the divisor integer.
   * @return true/false : logical indicating if the data is divisible.
   * @code true/false = math.isDivisible( data, divisor); @endcode
   * @code true/false = math.divisible( data, divisor); @endcode -- legacy code
   */
  inline bool isDivisible( const int &data, const int &divisor){
    return ((data % divisor) == 0);
  }
  inline int divisible( const int &data, const int &divisor)
  { return int( dstomath::isDivisible( data, divisor));}

  /** Return the sign of the data value.
   * @param data  : the data value to check.
   * @return sign : the sign of the data value.
   * @code sign = math.sign( data); @endcode
   */
  template <typename T> inline T sign( const T &data) { if ( data < T(0)) return T(-1); return T(1);}

  /** Compute the sum of squares of data.
   * @param data1 :
   * @param data2 :
   * @param data3 :
   * @return sumSquare : the sum of squares of the x data.
   * @code sumSquare = math.ssqr( data1, data2); @endcode
   * @code sumSquare = math.ssqr( data1, data2, data3); @endcode
   */
#if __cplusplus > 199711L || _MSC_VER >= 1800
  template <typename T> 
  inline T ssqr( const T &data1)
  {
    return ( data1 * data1);
  }

  template <typename T, typename... Ts> 
  inline T ssqr( const T &data1, const Ts&... args)
  {
    return ( ( data1 * data1) + ssqr( args...));
  }
#else
  template <typename T> inline T ssqr( const T &data1, const T &data2)
  {
    return (( data1 * data1) + ( data2 * data2));
  }
  template <typename T> inline T ssqr( const T &data1, const T &data2, const T &data3)
  {
    return (( data1 * data1) + ( data2 * data2) + ( data3 * data3));
  }
#endif

  /** Bound data to be within limits.
   * @param checkData  : the data to bound.
   * @param lowerLimit : the lower bound of the interval.
   * @param upperLimit : the upper bound of the interval.
   * @return data      : either the value of inData or one of the bounds.
   * @code data = math.bound( checkData, lowerLimit, upperLimit); @endcode
   */
  template <typename T> inline T bound( T value, const T &lowerLimit, const T &upperLimit)
  {
    value = dstomath::max( value, dstomath::min( lowerLimit, upperLimit));
    value = dstomath::min( value, dstomath::max( lowerLimit, upperLimit));
    return value;
  }

  template<typename T> inline T boundPM( const T& value, const T& limit)
  {
    return bound<T>( value, -limit, limit);
  }

  /** Compute Modulo of x between [0..y]
   * @param data1 : the modulo data ( x).
   * @param data2 : the interval data ( y).
   * @return data : the modulo of the x data in interval [0..y].
   * @code data = math.mod( data1, data2); @endcode
   */
  template <typename T> inline T mod( const T &data1, const T &data2)
  {
    // Modulo of x between [0..y]
    T returnValue = 0.0;

    if ( dstomath::abs( data2) < dstomath::zero()) {
      returnValue = T(-4.2E20); // Cannot solve for y = 0
    }
    else {
      returnValue = data1 - (( T( int( data1 / data2))) * data2);

      if ( dstomath::sign( data2) != dstomath::sign( returnValue)) {
        returnValue += data2;
      }
    }

    return returnValue;
  }

  /** Compute Modulo of x between [-y..y]
   * @param data1 : the modulo data ( x).
   * @param data2 : the interval data ( y).
   * @return data : the modulo of the x data in interval [-y..y].
   * @code data = math.modPM( data1, data2); @endcode
   */
  template <typename T> inline T modPM( const T &data1, const T &data2)
  {
    return dstomath::mod( data1 + data2, T(2) * data2) - data2;
  }

  /** Locate lower bound index of an element.
   * This function locates the lower bound index for an array of discrete values x(i)
   * for given value x. This is usually used to locate the nearest discrete elements
   * before performing linear interpolation. This function is based on the STL std::lower_bound().
   * @param first : the first element.
   * @param last  : the last element.
   * @param val   : the value to locate.
   * @return      : index of Xi such that Xi <= X < Xi+1.
   * @code index = lower_bound_index( &x[0], &x[N], xValue);
   * @code index = lower_bound_index( x.begin(), x.end(), xValue);
   */
  template <typename _ForwardIterator, typename _Tp> inline size_t lower_bound_index(
    _ForwardIterator __first, _ForwardIterator __last, const _Tp& __val)
  {
    size_t __maxIndex = __last - __first - 2; // This can wrap. All okay.
    size_t __index = std::lower_bound( __first, __last, __val) - __first;
    if ( __index > 0) --__index;
    if ( __index > __maxIndex) --__index;
    return __index;
  }

  template <typename _ForwardIterator, typename _Tp> inline size_t reverse_lower_bound_index(
    _ForwardIterator __first, _ForwardIterator __last, const _Tp& __val)
  {
    size_t __maxIndex = __last - __first - 2; // This can wrap. All okay.
    size_t __index = std::lower_bound( __first, __last, __val) - __first;
    if ( __index > 0) --__index;
    if ( __index > __maxIndex) --__index;
    return __last - __first - __index - 2;
  }

  template <typename _ForwardIterator, typename _Tp> inline _ForwardIterator lower_bound_iterator(
    _ForwardIterator __first, _ForwardIterator __last, const _Tp& __val)
  {
    _ForwardIterator __iterator = std::lower_bound( __first, __last, __val);
    size_t __maxIndex = __last - __first - 2; // This can wrap. All okay.
    size_t __index = __iterator - __first;
    if ( __index > 0) --__iterator;
    if ( __index > __maxIndex) --__iterator;
    return __iterator;
  }

  inline size_t next_pow2( size_t x)
  {
    if ( x) {
      size_t N = 1;
      while ( N < x) N <<= 1;
      return N;
    }

    return 0;
  }

  //// http://locklessinc.com/articles/next_pow2/
  //// Apparently the fastest way. Need arch. test though for 32/64bit systems.
  //size_t FFT::next_pow2( size_t x)
  //{
  //  --x;
  //  x |= ( x >> 1);
  //  x |= ( x >> 2);
  //  x |= ( x >> 4);
  //  x |= ( x >> 8);
  //  x |= ( x >> 16);
  //  x |= ( x >> 32); // Gives warning under 32bit system.
  //
  //  return x + 1;
  //}

#if __cplusplus > 199711L || _MSC_VER >= 1800
  template<typename T>
  inline T polynomial( const T& /* x */, const T& c)
  {
    return c;
  }

  template<typename T, typename... Ts>
  inline T polynomial( const T& x, const T& c, const Ts&... args)
  {
    return x * polynomial( x, args...) + c;
  }
#endif

template<typename T> 
inline T safe_asin( const T& val)
{
  return asin( bound( val, T( -1), T( 1)));
}

template<typename T> 
inline T safe_acos( const T& val)
{
  return acos( bound( val, T( -1), T( 1)));
}

template<typename T>
inline T safe_sqrt( const T& val)
{
  return sqrt( max( 0.0, val));
}

} // end namespace dstomath

#endif // End _AMATH_H_
