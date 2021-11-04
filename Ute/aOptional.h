#ifndef __AOPTIONAL_H__
#define __AOPTIONAL_H__

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
// Title:      aOptional - a lightweight alternative to std::optional
// Class:      aOptional - Template Class
// Module:     aOptional.h
//
// Description:
// This class is a lightweight alternative to std::optional. Choosing to sacrifice
// a single value of the type being wrapped rather than increasing the size of the
// combined type.
//
// Written by: Rob Porter (Robert.Porter@dst.defence.gov.au)
//
// First Date: 10/05/2018
//

// C++ Includes
#include <functional>
#include <limits>
#include <string>
#include <type_traits>

namespace dstoute {

  // Default aOptionalValidator, compares numeric types to numeric_limits<T>::max()
  // Note the workaround of std::numeric_limits<enum>::max() == 0.
  template<typename T>
  struct aOptionalValidator
  {
    template<typename U = T>
    static typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
    isValid( const T& v)
    {
      return v != std::numeric_limits<T>::max();
    }
    template<typename U = T>
    static typename std::enable_if< std::is_enum<U>::value, bool>::type
    isValid( const T& v)
    {
      return v != std::numeric_limits<size_t>::max();
    }
    template<typename U = T>
    static typename std::enable_if<std::is_arithmetic<U>::value, T>::type
    invalidValue()
    {
      return std::numeric_limits<T>::max();
    }
    template<typename U = T>
    static typename std::enable_if< std::is_enum<U>::value, T>::type
    invalidValue()
    {
      return T( std::numeric_limits<size_t>::max());
    }
  };

  // Specialisation of aOptionalValidator for std::string
  template<>
  struct aOptionalValidator<std::string>
  {
    static bool isValid( const std::string& v)
    {
      return v != std::string( "\0\0");
    }
    static std::string invalidValue()
    {
      return std::string( "\0\0");
    }
  };

  // Helper class to test if aOptional __validator has a method called "makeInvalid"
  template<typename T>
  class __has_make_invalid
  {
    template<typename V> static char test( decltype( &V::makeInvalid));
    template<typename V> static long test( ...);
  public:
    enum { value = sizeof( test<T>( 0)) == sizeof( char)};
  };

#define __AOPTIONAL_ASSIGNMENT_OPERATOR( op, ...)                                                                        \
    template<typename U = T>                                                                                             \
    typename std::enable_if<__VA_ARGS__, aOptional<T, __validator>>::type&                                               \
    operator op ( const aOptional<T, __validator>& rhs)                                                                  \
    { if ( !isValid() || !rhs.isValid()) { value_ = invalidValue();} else { value_ op rhs.value_;} return *this;}        

#define __AOPTIONAL_BINARY_OPERATOR( op, ...)                                                                            \
    template<typename U = T>                                                                                             \
    typename std::enable_if<__VA_ARGS__, aOptional<T, __validator>>::type                                                \
    operator op ( const aOptional<T, __validator>& rhs) const                                                            \
    { aOptional<T, __validator> val; if ( isValid() && rhs.isValid()) { val.value_ = value_ op rhs.value_;} return val;} 

  // aOptional class
  template<typename T, typename __validator = aOptionalValidator<T>>
  class aOptional
  {
  public:
    aOptional() 
      : value_( __validator::invalidValue()) 
    {}

    aOptional( const T& value) 
      : value_( value) 
    {}

    template<typename U>
    typename std::enable_if<std::is_constructible<T, U>::value && !std::is_convertible<U, T>::value, aOptional<T,__validator>>::type&
    operator= ( const U& rhs)
    { value_ = rhs; return *this;}

    operator T() const
    { return value_;}

    __AOPTIONAL_ASSIGNMENT_OPERATOR ( += , std::is_arithmetic<U>::value)
    __AOPTIONAL_ASSIGNMENT_OPERATOR ( -= , std::is_arithmetic<U>::value)
    __AOPTIONAL_ASSIGNMENT_OPERATOR ( *= , std::is_arithmetic<U>::value)
    __AOPTIONAL_ASSIGNMENT_OPERATOR ( /= , std::is_arithmetic<U>::value)
    __AOPTIONAL_ASSIGNMENT_OPERATOR ( %= , std::is_integral<U>::value)
    __AOPTIONAL_ASSIGNMENT_OPERATOR ( ^= , std::is_integral<U>::value)
    __AOPTIONAL_ASSIGNMENT_OPERATOR ( &= , std::is_integral<U>::value)
    __AOPTIONAL_ASSIGNMENT_OPERATOR ( |= , std::is_integral<U>::value)

    __AOPTIONAL_BINARY_OPERATOR     ( +  , std::is_arithmetic<U>::value)
    __AOPTIONAL_BINARY_OPERATOR     ( -  , std::is_arithmetic<U>::value)
    __AOPTIONAL_BINARY_OPERATOR     ( *  , std::is_arithmetic<U>::value)
    __AOPTIONAL_BINARY_OPERATOR     ( /  , std::is_arithmetic<U>::value)
    __AOPTIONAL_BINARY_OPERATOR     ( %  , std::is_integral<U>::value)
    __AOPTIONAL_BINARY_OPERATOR     ( ^  , std::is_integral<U>::value)
    __AOPTIONAL_BINARY_OPERATOR     ( &  , std::is_integral<U>::value)
    __AOPTIONAL_BINARY_OPERATOR     ( |  , std::is_integral<U>::value)

    bool isValid() const
    { return __validator::isValid( value_);}

    static bool isValid( const T& value)
    { return __validator::isValid(value);}

    void makeInvalid()
    { __makeInvalid();}

    T& value() 
    { return value_;}

    const T& value() const 
    { return value_;}

    static T invalidValue()
    { return __validator::invalidValue();}

    // Following method included to align with C++11 standard
    const T& valueOr( const T& defValue) const
    { return isValid() ? value_ : defValue;}

    // Following three methods included to align with C++17 standard
    template <typename U, typename validator = aOptionalValidator<U>>
    aOptional<U,validator> transform( std::function<U(const T&)> func) const
    { return isValid() ? aOptional<U,validator>( func( value_)) : aOptional<U,validator>();}

    template <typename U, typename validator = aOptionalValidator<U>>
    aOptional<U,validator> transformOptional( std::function<aOptional<U,validator>(const T&)> func) const
    { return isValid() ? func( value_) : aOptional<U,validator>();}

    void call( std::function<void(T)> func) const
    { if ( isValid()) func( value_);}

  private:
#if defined( _MSC_VER) && _MSC_VER < 1900

    void __makeInvalid()
    { value_ = __validator::invalidValue();}

#else

    template<typename __T = T>
    void __makeInvalid( typename std::enable_if<!__has_make_invalid<__validator>::value,__T>::type* = 0)
    { value_ = __validator::invalidValue();}

    template<typename __T = T>
    void __makeInvalid( typename std::enable_if< __has_make_invalid<__validator>::value,__T>::type* = 0)
    { __validator::makeInvalid( value_);}

#endif

    T value_;
  };

#undef __AOPTIONAL_ASSIGNMENT_OPERATOR
#undef __AOPTIONAL_BINARY_OPERATOR

  /** Special handling for bool, not being tricky and packing it into a single
   * char as I want to keep the bool& value() method.
   */
  template<>
  class aOptional<bool>
  {
  public:
    aOptional() 
      : value_( false),
        isValid_( false)
    {}

    aOptional( const bool& value) 
      : value_( value),
        isValid_( true)
    {}

    aOptional<bool>& operator= ( const aOptional<bool>& rhs)
    { value_ = rhs.value_; isValid_ = rhs.isValid_; return *this;}

    operator bool() const
    { return value_;}

    bool isValid() const
    { return isValid_;}

    void makeInvalid()
    { isValid_ = false;}

    bool& value() 
    { return value_;}

    const bool& value() const 
    { return value_;}

  private:
    bool value_;
    bool isValid_;
  };

  /** Special handling for tuples to support arbitrary types
    */
  template <typename... Ts>
  class aOptional<std::tuple<Ts...>>
  {
  public:
    aOptional() 
      : value_{},
      isValid_( false)
    {}

    aOptional( const std::tuple<Ts...>& value) 
      : value_( value),
      isValid_( true)
    {}

    aOptional<std::tuple<Ts...>>& operator= ( const aOptional<std::tuple<Ts...>>& rhs)
    { value_ = rhs.value_; isValid_ = rhs.isValid_; return *this;}

    operator std::tuple<Ts...>() const
    { return value_;}

    bool isValid() const
    { return isValid_;}

    void makeInvalid()
    { isValid_ = false;}

    std::tuple<Ts...>& value() 
    { return value_;}

    const std::tuple<Ts...>& value() const 
    { return value_;}

  private:
    std::tuple<Ts...> value_;
    bool isValid_;
  };

  /** Special handling for pairs to support arbitrary types
  */
  template <typename T1, typename T2>
  class aOptional<std::pair<T1,T2>>
  {
  public:
    aOptional() 
      : value_{},
      isValid_( false)
    {}

    aOptional( const std::pair<T1,T2>& value) 
      : value_( value),
      isValid_( true)
    {}

    aOptional<std::pair<T1,T2>>& operator= ( const aOptional<std::pair<T1,T2>>& rhs)
    { value_ = rhs.value_; isValid_ = rhs.isValid_; return *this;}

    operator std::pair<T1,T2>() const
    { return value_;}

    bool isValid() const
    { return isValid_;}

    void makeInvalid()
    { isValid_ = false;}

    std::pair<T1,T2>& value() 
    { return value_;}

    const std::pair<T1,T2>& value() const 
    { return value_;}

  private:
    std::pair<T1,T2> value_;
    bool isValid_;
  };

  // Similar to std::all_of
  template<typename T, typename... Args>
  inline bool allOf( aOptional<T> o, Args... args)
  {
    return o.isValid() && allOf( args...);
  }

  template<typename T>
  inline bool allOf( aOptional<T> o)
  {
    return o.isValid();
  }

  // Similar to std::any_of
  template<typename T, typename... Args>
  inline bool anyOf( aOptional<T> o, Args... args)
  {
    return o.isValid() || anyOf( args...);
  }

  template<typename T>
  inline bool anyOf( aOptional<T> o)
  {
    return o.isValid();
  }

  template<typename T, typename... Args>
  inline size_t countValid( aOptional<T> o, Args... args)
  {
    return ( o.isValid() ? 1 : 0) + countValid( args...);
  }

  template<typename T>
  inline size_t countValid( aOptional<T> o)
  {
    return o.isValid() ? 1 : 0;
  }

  // Typedefs for common use
  typedef aOptional<bool>        aOptionalBool;
  typedef aOptional<int>         aOptionalInt;
  typedef aOptional<long>        aOptionalLong;
  typedef aOptional<size_t>      aOptionalSizeT;
  typedef aOptional<double>      aOptionalDouble;
  typedef aOptional<std::string> aOptionalString;

} // namespace dstoute

#endif /* __AOPTIONAL_H__*/
