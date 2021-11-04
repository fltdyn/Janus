#ifndef __AMAP_H__
#define __AMAP_H__

// C++ Includes
#include <map>
#include <sstream>
#include <type_traits>

// Local Includes
#include "aString.h"
#include "aMessageStream.h"
#include "aOptional.h"

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
// Title:      aMap - An extended version of std::map
// Class:      aMap - Template Class
// Module:     aMap.h
// Based on:   aBiMap.h
//
// Description:
// This class is an extension of the std::map class.
// It provides extra capability, especially the way in which
// the map can be populated with data.
//
// Written by: Shane Hill (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 20/08/2014
//

namespace dstoute {

  /*! \brief An extended template based std::map container.
   * aMap is a template based container used to hold pairs of data that require fast one-directional lookup.
   * aMap has all the functionality of std::map< _Key, _Tp, _Compare, _Alloc >, with a bunch of extra handy stuff.
   *
      @code
      typedef aMap< int, aString> IntStringMap;
      const IntStringMap cDaysOfWeek = ( IntStringMap() =
        << IntStringMap::pair( 0, "Monday")
        << IntStringMap::pair( 1, "Tuesday")
        << IntStringMap::pair( 2, "Wednesday")
        << IntStringMap::pair( 3, "Thursday")
        << IntStringMap::pair( 4, "Friday")
        << IntStringMap::pair( 5, "Saturday")
        << IntStringMap::pair( 6, "Sunday")
      );

      int main( int, char**)
      {
        IntStringMap daysOfWeek;
        daysOfWeek =
          0, "Monday",
          1, "Tuesday",
          2, "Wednesday",
          3, "Thusday",
          4, "Friday",
          5, "Saturday",
          6, "Sunday",
      }
      @endcode
   */
  template <typename _Key, typename _Tp, typename _Compare = std::less< _Key >,
    typename _Alloc = std::allocator< std::pair< const _Key, _Tp> > >
  class aMap
   : public std::map< _Key, _Tp, _Compare, _Alloc >
  {
   public:
     #if !defined( _MSC_VER) || _MSC_VER > 1900
     typedef _Key                       key_type;
     typedef _Tp                        mapped_type;
     typedef std::pair<const _Key, _Tp> value_type;
     typedef _Compare                   key_compare;
     typedef _Alloc                     allocator_type;
     #endif

     typedef std::map< key_type, mapped_type, key_compare, allocator_type > fwd_map;
     typedef std::pair<const key_type, mapped_type> pair;

     //
     // Constructors
     //
     aMap() : fwd_map() {}
     aMap( const aMap &__am) : fwd_map( __am) {}
     template<typename _InputIterator>
     aMap( _InputIterator __first, _InputIterator __last) : fwd_map( __first, __last) {}

     // C11 Support
     #if __cplusplus > 199711L || _MSC_VER >= 1800

     aMap( std::initializer_list< value_type> __l, const key_compare& __comp = key_compare(), const allocator_type& __a = allocator_type())
       : fwd_map( __l, __comp, __a) {}

     #endif

     //
     // Functions to populate the Map.
     //
     /*! Set and existing pair or add a new pair.
      * @code
          aMap< int, aString> daysOfWeek;
          daysOfWeek.set( 0, "Monday");
          daysOfWeek.set( 1, "Tuesday");
          daysOfWeek.set( 2, "Wednesday");
          daysOfWeek.set( 3, "Thusday");
          daysOfWeek.set( 4, "Friday");
          daysOfWeek.set( 5, "Saturday");
          daysOfWeek.set( 6, "Sunday");
      * @endcode
      */
     void set( const key_type& __k, const mapped_type& __t)
     {
       (*this)[ __k] = __t;
     }

     // Required since newer gcc version > 8.1.0
     aMap& operator= ( const aMap &__am)
     {
       fwd_map::operator=( __am);
       return *this;
     }

     /*! Return a modifiable reference to a mapped type given the input key.
      *
      * @code
          daysOfWeek[ 3] = "Thu".
      * @endcode
      */
     mapped_type& operator[]( const key_type &__k)
     {
       return fwd_map::operator[]( __k);
     }

     /*! Add the content of another aMap to the current aMap.
      *
      * @code
          aMap< int, aString> moreDaysOfWeek;
          moreDaysOfWeek.set( 8, "Crumpday");
          moreDaysOfWeek.set( 9, "Animationday");
          daysOfWeek << moreDaysOfWeek;
      * @endcode
      */
     aMap& operator<<( const aMap &__am) {
       for ( typename fwd_map::const_iterator iter = __am.begin(); iter != __am.end(); ++iter) {
         (*this)[ iter->first] = iter->second;
       }
       return *this;
     }

     /*! Add the content of a pair to the current aMap.
      *
      * @code
          daysOfWeek << std::pair< int, aString>( 8, "Crumpday");
          daysOfWeek << daysOfWeek::pair( 9, "Animationday");
      * @endcode
      */
     aMap& operator<<( const value_type &__vt) {
       (*this)[ __vt.first] = __vt.second;
       return *this;
     }

     //
     // Map retrieval functions.
     //
     /*! Return true if aMap contains given key type.
      * @param __k
      * @return bool
      */
     bool contains_key( const key_type& __k) const
     {
       return fwd_map::find( __k) != fwd_map::end();
     }

     /*! Return true if aMap contains given mapped type.
      * @param __t
      * @return bool
      */
     bool contains_map( const mapped_type& __t) const
     {
       return fwd_map::find( __t) != fwd_map::end();
     }

     /*! Return a constant reference to a mapped type given the input key.
      *
      * @code
          aString day = daysOfWeek[ 3];
      * @endcode
      */
     const mapped_type&  operator[]( const key_type &__k) const
     {
       typename fwd_map::const_iterator item = fwd_map::find( __k);
       if ( item == fwd_map::end()) {
         throw_message( std::out_of_range, aString( "aMap::operator[key_type] - Can't find key \"%\".").arg( __k));
       }
       return item->second;
     }

     /*! Return a constant reference to a mapped type given the input key.
     * Returns the default value if input key is not found in map.
     *
     * @code
         aString day = daysOfWeek.get( 11, aString( "Crumpday"));
     * @endcode
     */
     const mapped_type& get( const key_type& __k, const mapped_type& __default) const
     {
       typename fwd_map::const_iterator item = fwd_map::find( __k);
       if ( item == fwd_map::end()) return __default;
       return item->second;
     }

     template<typename __Tp = _Tp>
     typename std::enable_if<sizeof( aOptional<__Tp>)!=0,aOptional<__Tp>>::type
     get( const key_type &__k) const
     {
       return aOptional<__Tp>( get( __k, aOptional<__Tp>::invalidValue()));
     }

     /*! Return an aMap iterator based on map index value.
      * @code
          cout << daysOfWeek( 0)->second << " should be \"Monday\"" << endl;
      * @endcode
      */
     typename fwd_map::const_iterator operator()( size_t __i) const {
       if ( __i >= fwd_map::size()) {
         throw_message( std::out_of_range, aString( "aMap::operator(index) - Index greater than aMap size."));
       }
       typename fwd_map::const_iterator iter = fwd_map::begin();
       while ( __i--) {
         ++iter;
       }
       return iter;
     }

     //
     // Comma operator for assignment of a aMap from a list of data.
     //
     struct CommaAssignment {
       CommaAssignment( aMap& __am, const key_type& __k)
       {
         am_ = &__am;
         k_  = __k;
       }

       CommaAssignment& operator,( const key_type& __k)
       {
         k_ = __k;
         return *this;
       }

       CommaAssignment& operator,( const mapped_type& __t)
       {
         am_[ k_] = __t;
         return *this;
       }

       aMap* am_;
       key_type k_;
     };

     /*! Add data pairs to an aMap using comma separated data.
      * @code
          aMap< int, aString> daysOfWeek;
          daysOfWeek << 0, "Monday"
                        1, "Tuesday"
                        2, "Wednesday"
                        3, "Thursday"
                        4, "Friday"
                        5, "Saturday"
                        6, "Sunday"
      * @endcode
      */
     CommaAssignment operator<<( const key_type& __k)
     {
       return CommaAssignment( *this, __k);
     }

     //
     // Stream functions.
     //
     /*! Output stream friend function to output the contents of an aMap to a std::ostream.
      * @code
          cout << "Days of week are:\n" << daysOfWeek << endl;
      * @endcode
      */
     friend std::ostream& operator<< ( std::ostream &os, const aMap& __bm)
     {
       for ( typename fwd_map::const_iterator iter = __bm.fwdMap_.begin(); iter != __bm.fwdMap_.end(); ++iter) {
         os << iter->first << " == " << iter->second << '\n';
       }
       return os;
     }

  };

} // end namespace dstoute

#endif // End __AMAP_H__
