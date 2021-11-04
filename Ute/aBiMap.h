#ifndef __ABIMAP_H__
#define __ABIMAP_H__

// C++ Includes
#include <map>
#include <ostream>
#include <type_traits>

// Local Includes
#include "aString.h"
#include "aMap.h"
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
// Title:      aBiMap - Bi-directional version of std::map class.
// Class:      aBiMap - Template Class
// Module:     BiMap.h
//
// Description:
// This class is an extension of the std::map class that allows
// bi-directional lookup of both key and value. This is done using
// two maps to ensure speed. Note the duplication of data.
//
// @TODO: Do BiMap using single data approach.
//
// Note: the code names/structure of this template class follows
//       the same as <map>, namely <bits/stl_map.h>.
//
// Written by: Shane Hill (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 22/10/2010
//

namespace dstoute {

  /*! \brief An extended template based std::map container.
   * aBiMap is a template based container used to hold pairs of data that require fast bi-directional lookup.
   * aBiMap has most of the functionality of std::map< _Key, _Tp, _Compare, _Alloc >, with a bunch of extra handy stuff.
   * No write access is given to the forard and reverse maps. Only read access is allowed.
   *
      @code
      typedef aBiMap< int, aString> IntStringMap;
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
        daysOfWeek <<
          0, "Monday"
          1, "Tuesday"
          2, "Wednesday"
          3, "Thusday"
          4, "Friday"
          5, "Saturday"
          6, "Sunday"
      }
      @endcode
   */
  template <typename _Key, typename _Tp, bool isOneToOne = true, 
    typename _Compare = std::less< _Key >,
    typename _Alloc = std::allocator< std::pair< const _Key, _Tp> > >
  class aBiMap
  {
   public:
     typedef _Key     key_type;
     typedef _Tp      mapped_type;
     typedef std::pair<key_type, mapped_type> value_type;
     typedef _Compare key_compare;
     typedef _Alloc   allocator_type;

     typedef aMap< key_type, mapped_type, key_compare, allocator_type > fwd_map;
     typedef aMap< mapped_type, key_type > rev_map;

     typedef std::pair< key_type, mapped_type > pair;
     typedef std::pair< key_type, mapped_type > fwd_pair;
     typedef std::pair< mapped_type, key_type > rev_pair;

     typedef typename fwd_map::const_iterator const_iterator;
     typedef typename fwd_map::const_iterator const_fwd_iterator;
     typedef typename rev_map::const_iterator const_rev_iterator;

     //
     // Constructors
     //
     aBiMap() {}
     aBiMap( const fwd_map& __fm)
     {
       for ( typename fwd_map::const_iterator item = __fm.begin(); item != __fm.end(); ++item) {
         set( item->first, item->second);
       }
     }

     // C11 Support
     #if __cplusplus > 199711L || _MSC_VER >= 1800

     aBiMap( std::initializer_list< value_type> __l)
     {
       for ( pair p : __l) {
         set( p.first, p.second);
       }
     }

     #endif

     //
     // Functions to populate the BiMap.
     //
     /*! Set and existing pair or add a new pair.
      * @code
          aBiMap< int, aString> daysOfWeek;
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
       typename fwd_map::const_iterator fwdItem = fwdMap_.find( __k);
       if ( fwdItem != fwdMap_.end()) {
         throw_message( std::out_of_range,
           aString( "aBiMap::set() - Key type \"%\" already exists. Tried to map to \"%\".").arg( __k).arg( __t));
       }
       typename rev_map::const_iterator revItem = revMap_.find( __t);
       if ( isOneToOne && revItem != revMap_.end()) {
         throw_message( std::out_of_range,
           aString( "aBiMap::set() - Mapped type \"%\" already exists. Tried with key \"%\".").arg( __t).arg( __k));
       }
       fwdMap_[ __k] = __t;
       if ( isOneToOne || revItem == revMap_.end()) {
         revMap_[ __t] = __k;
       }
     }

     /*! Add the content of a pair to the current aMap.
      *
      * @code
          daysOfWeek << std::pair< int, aString>( 8, "Crumpday");
          daysOfWeek << daysOfWeek::pair( 9, "Animationday");
      * @endcode
      */
     aBiMap& operator<<( const value_type &__vt)
     {
       set( __vt.first, __vt.second);
       return *this;
     }

     /*! Add the content of another aBiMap to the current aBiMap.
      *
      * @code
          aBiMap< int, aString> moreDaysOfWeek;
          moreDaysOfWeek.set( 8, "Crumpday");
          moreDaysOfWeek.set( 9, "Animationday");
          daysOfWeek << moreDaysOfWeek;
      * @endcode
      */
     aBiMap& operator<<( const aBiMap& __bm)
     {
       for ( typename fwd_map::const_iterator item = __bm.fwdMap_.begin(); item != __bm.fwdMap_.end(); ++item) {
         set( item->first, item->second);
       }
       return *this;
     }

     /*! Add the content of another std::map to the current aBiMap.
      *
      * @code
          aBiMap< int, aString> moreDaysOfWeek;
          moreDaysOfWeek.set( 8, "Crumpday");
          moreDaysOfWeek.set( 9, "Animationday");
          daysOfWeek << moreDaysOfWeek.forward();
      * @endcode
      */
     aBiMap& operator<<( const fwd_map& __fm)
     {
       for ( typename fwd_map::const_iterator item = __fm.begin(); item != __fm.end(); ++item) {
         set( item->first, item->second);
       }
       return *this;
     }

     //
     // Map retrieval functions.
     //
     /*! Return a constant reference to a mapped type given the input key.
      *
      * @code
          aString day = daysOfWeek[ 3];
      * @endcode
      */
     const mapped_type& operator[]( const key_type &__k) const
     {
       typename fwd_map::const_iterator item = fwdMap_.find( __k);
       if ( item == fwdMap_.end()) {
         throw_message( std::out_of_range, aString( "aBiMap::operator[key_type] - Can't find key \"%\".").arg( __k));
       }
       return item->second;
     }

     /*! Return a constant reference to a mapped type given the input key.
      *
      * @code
          int dayNo = daysOfWeek[ "Thursday"];
      * @endcode
      */
     const key_type& operator[]( const mapped_type &__t) const
     {
       typename rev_map::const_iterator item = revMap_.find( __t);
       if ( item == revMap_.end()) {
         throw_message( std::out_of_range, aString( "aBiMap::operator[key_type] - Can't find key \"%\".").arg( __t));
       }
       return item->second;
     }

     const mapped_type& get( const key_type &__k, const mapped_type& __default) const
     {
       typename fwd_map::const_iterator item = fwdMap_.find( __k);
       if ( item == fwdMap_.end()) return __default;
       return item->second;
     }

     const key_type& get( const mapped_type& __t, const key_type& __default) const
     {
       typename rev_map::const_iterator item = revMap_.find( __t);
       if ( item == revMap_.end()) return __default;
       return item->second;
     }

     template<typename __Tp = _Tp>
     typename std::enable_if<sizeof( aOptional<__Tp>)!=0,aOptional<__Tp>>::type 
     get( const key_type &__k) const
     {
       return aOptional<__Tp>( get( __k, aOptional<__Tp>::invalidValue()));
     }

     template<typename __Key = _Key>
     typename std::enable_if<sizeof( aOptional<__Key>)!=0,aOptional<__Key>>::type
     get( const mapped_type& __t) const
     {
       return aOptional<__Key>( get( __t, aOptional<__Key>::invalidValue()));
     }

     //
     // Access to forward and reverse maps.
     //
     /*! Provides access to the forward std::map. Useful when both key and mapped types are of the same type.
      * @code
          aBiMap< aString, aString> daysOfWeekFrench;
          daysOfWeekFrench << "Monday",    "Lundi"
                              "Tuesday",   "Mardi"
                              "Wednesday", "Mercredi"
                              "Thursday",  "Jeudi"
                              "Friday",    "Vendredi"
                              "Saturday",  "Samedi"
                              "Sunday",    "Dimanche";
          cout << "Thursday in French is " << daysOfWeekFrench.forward()[ "Thusday"] << endl;
      * @endcode
      */
     const fwd_map& forward() const { return fwdMap_;}

     /*! Provides access to the forward std::map. Useful when both key and mapped types are of the same type.
      * @code
          aBiMap< aString, aString> daysOfWeekFrench;
          daysOfWeekFrench << "Monday",    "Lundi"
                              "Tuesday",   "Mardi"
                              "Wednesday", "Mercredi"
                              "Thursday",  "Jeudi"
                              "Friday",    "Vendredi"
                              "Saturday",  "Samedi"
                              "Sunday",    "Dimanche";
          cout << "Mercredi in English is " << daysOfWeekFrench.reverse()[ "Mercredi"] << endl;
      * @endcode
      */
     const rev_map& reverse() const { return revMap_;}

     //
     // Reimplemented std::map functions.
     //
     /*! Return an iterator pointing to the beginning of the forward map.
      */
     const_fwd_iterator begin() const
     {
       return fwdMap_.begin();
     }

     /*! Return an iterator pointing to the beginning of the reverse map.
      */
     const_fwd_iterator revBegin() const
     {
       return revMap_.begin();
     }

     /*! Clear the contents of the aBiMap.
      */
     void clear()
     {
       fwdMap_.clear();
       revMap_.clear();
     }

     /*! Return true if aBiMap is empty.
      */
     bool empty() const
     {
       return ( size() == 0);
     }

     /*! Return an iterator pointing to the end of the forward map.
      */
     const_fwd_iterator end() const
     {
       return fwdMap_.end();
     }

     /*! Return an iterator pointing to the end of the reverse map.
      */
     const_rev_iterator revEnd() const
     {
       return revMap_.end();
     }

     /*! Erase the pair containing the given key type.
      */
     inline void erase( const key_type& __k)
     {
       static_assert( isOneToOne, "aBiMap::erase is not available if not a one to one mapping.");
       typename fwd_map::iterator fwdItem = fwdMap_.find( __k);
       if ( fwdItem != fwdMap_.end()) {
         revMap_.erase( fwdItem->second);
         fwdMap_.erase( fwdItem);
       }
     }

     /*! Erase the pair containing the given mapped type.
      */
     void erase( const mapped_type& __t)
     {
       static_assert( isOneToOne, "aBiMap::erase is not available if not a one to one mapping.");
       typename rev_map::iterator revItem = revMap_.find( __t);
       if ( revItem != revMap_.end()) {
         fwdMap_.erase( revItem->second);
         revMap_.erase( revItem);
       }
     }

     /*! Erase the pair at the given forward iterator.
      */
     void erase( const typename fwd_map::const_iterator& item)
     {
       static_assert( isOneToOne, "aBiMap::erase is not available if not a one to one mapping.");
       typename fwd_map::iterator fwdItem = fwdMap_.find( item->first);
       if ( fwdItem != fwdMap_.end()) {
         typename rev_map::iterator revItem = revMap_.find( item->second);
         if ( revItem != revMap_.end()) {
           fwdMap_.erase( fwdItem);
           revMap_.erase( revItem);
         }
       }
     }

     /*! Erase the pair at the given reverse iterator.
      */
     void erase( const typename rev_map::const_iterator& item)
     {
       static_assert( isOneToOne, "aBiMap::erase is not available if not a one to one mapping.");
       typename rev_map::iterator revItem = revMap_.find( item->first);
       if ( revItem != revMap_.end()) {
         typename fwd_map::iterator fwdItem = fwdMap_.find( item->second);
         if ( fwdItem != fwdMap_.end()) {
           revMap_.erase( revItem);
           fwdMap_.erase( fwdItem);
         }
       }
     }

     /*! Return the forward iterator for the given key type.
      */
     const typename fwd_map::const_iterator find( const key_type& __k) const
     {
       return fwdMap_.find( __k);
     }

     /*! Return the reverse iterator for the given mapped type.
      */
     const typename rev_map::const_iterator find( const mapped_type& __t) const
     {
       return revMap_.find( __t);
     }

     /*! Return size of the aBiMap (number of item pairs).
      */
     size_t size() const
     {
       return fwdMap_.size();
     }

     /*! Return true if aBiMap contains key type.
      */
     bool contains_key( const key_type& __k) const
     {
       return fwdMap_.find( __k) != fwdMap_.end();
     }

     /*! Return true if aBiMap contains mapped type.
      */
     bool contains_map( const mapped_type& __t) const
     {
       return revMap_.find( __t) != revMap_.end();
     }

     //
     // Boolean operators.
     //
     friend bool operator== ( const aBiMap& __bm1, const aBiMap& __bm2)
     {
       return ( __bm1.fwdMap_ == __bm2.fwdMap_) && ( __bm1.revMap_ == __bm2.revMap_);
     }

     friend bool operator!= ( const aBiMap& __bm1, const aBiMap& __bm2)
     {
       return ( __bm1.fwdMap_ != __bm2.fwdMap_) || ( __bm1.revMap_ != __bm2.revMap_);
     }

     //
     // Comma operator for assignment of a aBiMap from a list of data.
     //
     struct CommaAssignment {
       CommaAssignment( aBiMap& __bm, const key_type& __k)
       {
         bm_ = &__bm;
         k_  = __k;
       }

       CommaAssignment& operator,( const key_type& __k)
       {
         k_ = __k;
         return *this;
       }

       CommaAssignment& operator,( const mapped_type& __t)
       {
         bm_->set( k_, __t);
         return *this;
       }

       aBiMap* bm_;
       key_type k_;
     };

     /*! Add data pairs to an aBiMap using comma separated data.
      * @code
          aBiMap< int, aString> daysOfWeek;
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
     /*! Output stream friend function to output the contents of an aBiMap to a std::ostream.
      * @code
          cout << "Days of week are:\n" << daysOfWeek << endl;
      * @endcode
      */
     friend std::ostream& operator<< ( std::ostream &os, const aBiMap& __bm)
     {
       for ( typename fwd_map::const_iterator item = __bm.fwdMap_.begin(); item != __bm.fwdMap_.end(); ++item) {
         os << item->first << ", " << item->second << '\n';
       }
       return os;
     }

   private:
     fwd_map fwdMap_;
     rev_map revMap_;
  };

} // end namespace dstoute

#endif // End __ABIMAP_H__
