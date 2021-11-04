#ifndef ALIST_H_
#define ALIST_H_

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
// Title:      A List Class
// Class:      aList
//
// Description:
//  Provides a list container for any variable, structure or class.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 11/05/2012
//

// C++ Includes
#include <iostream>
#include <vector>

namespace dstoute {

  template < typename _Tp, typename _Alloc = std::allocator< _Tp > >
  class aList : public std::vector< _Tp, _Alloc >
  {
   public:
#if !defined( _MSC_VER) || _MSC_VER > 1900
   typedef _Tp    value_type;
   typedef _Alloc allocator_type;
   typedef size_t size_type;
#endif
   typedef std::vector< _Tp, _Alloc > fwd_list;

   aList() : fwd_list() {}
   aList( const allocator_type &__a) : fwd_list( __a) {}
   aList( size_type __n, const value_type &__v = value_type(),
     const allocator_type &__a = allocator_type())
     : fwd_list( __n, __v, __a) {}
   aList( const aList &__al) : fwd_list( __al) {}
   aList( const fwd_list &__al) : fwd_list( __al) {}

   // C11 Support
#if __cplusplus > 199711L || _MSC_VER >= 1800

   aList( const std::initializer_list<_Tp>& __il) : fwd_list( __il) {}

#endif

   template < typename _Tp2, size_t _N >
   void assign( const _Tp2 (&ra)[_N])
   {
     this->clear();
     append( ra);
   }

   template <typename _Tp2>
   void assign( const _Tp2 &__container)
   {
     this->clear();
     append( __container);
   }

   template < typename _Tp2, size_t _N >
   void append( const _Tp2 (&ra)[_N])
   {
     this->clear();
     for ( size_t i = 0; i < _N; ++i) {
       this->push_back( ra[i]);
     }
   }

   template <typename _Tp2>
   void append( const _Tp2 &__container)
   {
     typename _Tp2::const_iterator iter     = __container.begin();
     typename _Tp2::const_iterator iter_end = __container.end();

     this->clear();
     for ( ; iter != iter_end; ++iter) {
       this->push_back( *iter);
     }
   }

   typename fwd_list::iterator find( const value_type &__v)
   {
     for ( typename fwd_list::iterator iter = this->begin(); iter != this->end(); ++iter) {
       if ( *iter == __v) return iter;
     }
     return this->end();
   }

   typename fwd_list::const_iterator find( const value_type &__v) const
   {
     for ( typename fwd_list::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
       if ( *iter == __v) return iter;
     }
     return this->end();
   }

   int indexOf( const value_type &__v) const
   {
     typename fwd_list::const_iterator iter = find( __v);
     if ( iter != this->end()) {
       return int( iter - this->begin());
     }

     return -1;
   }

   size_t contains( const value_type &__v) const
   {
     size_t count = 0;
     for ( typename fwd_list::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
       if ( *iter == __v) return count + 1;
       ++count;
     }
     return 0;
   }

   // Required since newer gcc version > 8.1.0
   aList& operator= ( const aList &__al)
   {
     fwd_list::operator=( __al);
     return *this;
   }

   template < typename _Tp2, size_t _N >
   aList& operator= ( const _Tp2 (&ra)[_N])
   {
     this->clear();
     for ( size_t i = 0; i < _N; ++i) {
       this->push_back( ra[i]);
     }
     return *this;
   }

   aList operator+ ( const aList &__al)
   {
     aList addList = *this;
     addList += __al;
     return addList;
   }

   aList& operator+= ( const aList &__al)
   {
     for ( typename fwd_list::const_iterator iter = __al.begin(); iter != __al.end(); ++iter) {
       this->push_back( *iter);
     }
     return *this;
   }

   aList& operator+= ( const value_type &__v)
   {
     this->push_back( __v);
     return *this;
   }

   aList& operator<< ( const aList &__al)
   {
     for ( typename fwd_list::const_iterator iter = __al.begin(); iter != __al.end(); ++iter) {
       this->push_back( *iter);
     }
     return *this;
   }

   aList& operator<< ( const value_type &__v)
   {
     this->push_back( __v);
     return *this;
   }

   aList& operator, ( const value_type &__v)
   {
     this->push_back( __v);
     return *this;
   }

   aList mask( const aList< bool > &__b) const
   {
     aList al;
     size_t limit = __b.size();
     if ( limit > this->size()) {
       limit = this->size();
     }
     for ( size_t i = 0; i < limit; ++i) {
       if ( __b[i]) {
         al.push_back( (*this)[ i]);
       }
     }
     return al;
   }

   //
   // Friend operators.
   //
   friend std::ostream& operator<< ( std::ostream &os, const aList &__al)
   {
     for ( size_t i = 0; i < __al.size(); ++i) {
       os << i << ": " << __al[i] << '\n';
     }
     return os;
   }

  };

  typedef aList<double>       aDoubleList;
  typedef aList<long double>  aLongDoubleList;
  typedef aList<bool>         aBoolList;
  typedef aList<int>          aIntList;
  typedef aList<unsigned int> aUnsignedIntList;
  typedef aList<size_t>       aSizeTList;

}

#endif /* ALIST_H_ */
