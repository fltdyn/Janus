#ifndef ASTRING_H_
#define ASTRING_H_

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
// Title:      aString Extended String Class
// Class:      aString
//
// Description:
//   Extension of the std::string class.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 23/03/2011
//

// C++ Includes.
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <valarray>
#include <algorithm>

// Ute Includes
#include "aList.h"
#include "aMath.h"
#include "aOptional.h"

namespace dstoute {

  inline bool double_equiv( const double &d1, const double &d2)
  {
    return dstomath::isZero( d1 - d2);
  }

  class aString;
  typedef aList< aString > aStringList;

  class aString : public std::string
  {
   public:
    aString() : std::string(), stringTokenPos_(0), delimiterTriggered_(0) {}
    aString( const char* str) : std::string( str), stringTokenPos_(0), delimiterTriggered_(0) {}
    aString( const std::string &str) : std::string( str), stringTokenPos_(0), delimiterTriggered_(0) {}

    template <typename _Tp> aString  arg   ( const _Tp &value, int precision = 12);
    template <typename _Tp> aString& setNum( const _Tp &value, int precision = 12);

    template <typename _Tp> aString  counterArg( const _Tp &value, int width = 3, char fill = '0') const;
    template <typename _Tp> aString& setCounter( const _Tp &value, int width = 3, char fill = '0');

    const char*   toChar() const;
    double        toDouble() const;
    bool          toBool() const;
    int           toInt() const;
    long          toLong() const;
    unsigned int  toUInt() const;
    unsigned long toULong() const;
    size_t        toSize_T() const;
    bool          isDecimal() const;
    bool          isNumeric() const;
    bool          isNumericOrTime() const;
    aStringList   toStringList( const aString &delimeters = ",\t\n;",  bool ignoreDuplicate = true) const;
    aDoubleList   toDoubleList( const aString &delimeters = " ,\t\n;", bool ignoreDuplicate = true) const;

    aStringList   toStringIndexList( const size_t start, const size_t number, const size_t increment = 1, 
                                     const bool fillAll = false, const int width = 0, const char fill = '0') const;

    // Input of form: 0.0:25.0:100.0,150.0:50.0:300.0
    aString               scaleDoubleList( const double &scale) const;
    aString               convertDoubleList( const double &scale, const double& offset) const;
    aDoubleList           interpretDoubleList( bool doUniqueSort = false) const;
    std::valarray<double> interpretDoubleVector( bool doUniqueSort = false) const;

    // Input of form: 1-10,15-20,72-100
    aIntList interpretIntList();

    // ExprTk math expression evaluation
    double evaluate() const;

    aString toLowerCase() const;
    aString toUpperCase() const;
    aString trim() const;
    aString trimLeft() const;
    aString trimRight() const;
    aString trimFirst( const aString& text, bool inclusive = true) const;
    aString trimLast( const aString& text, bool inclusive = true) const;
    aString strrep( const aString& oldSubString, const aString& newSubString) const;

    size_t  count() const { return size();}
    size_t  count( const aString &text) const;
    bool    contains( const aString &text) const;
    bool    beginsWith( const aString &text) const;
    bool    endsWith( const aString &text) const;

    aString random( const size_t &strLength = 10);

    // String tokeniser functions
    void    initStringToken() const;
    bool    hasStringTokens() const { return stringTokenPos_ != std::string::npos;}
    aString getStringToken( const aString &delimiters, bool ignoreDulicate = true) const;
    aString getStringTokenRemaining() const;
    char    delimeterTriggered() const { return delimiterTriggered_;}

   private:
    aString& replaceArg( const aString &arg1);
    mutable std::string::size_type stringTokenPos_;
    mutable char delimiterTriggered_;
  };

  inline aString& aString::replaceArg( const aString &arg1)
  {
    std::string::size_type pos = find_first_of( '%');
    if ( pos != std::string::npos) {
      replace( pos, 1, arg1);
    }
    return *this;
  }

  template <typename _Tp> inline aString aString::arg( const _Tp &value, int precision)
  {
    aString asValue;
    asValue.setNum( value, precision);
    aString newString( *this);
    return newString.replaceArg( asValue);
  }

  template <typename _Tp> inline aString& aString::setNum( const _Tp &value, int precision)
  {
    std::ostringstream os;
    os.precision( precision);
    os << value;
    *this = os.str();
    return *this;
  }

  template <typename _Tp> inline aString aString::counterArg( const _Tp &value, int width, char fill) const
  {
    aString asValue;
    asValue.setCounter( value, width, fill);
    aString newString( *this);
    return newString.replaceArg( asValue);
  }

  template <typename _Tp> inline aString& aString::setCounter( const _Tp &value, int width, char fill)
  {
    std::ostringstream os;
    os.fill( fill);
    os.width( width);
    os << value;
    *this = os.str();
    return *this;
  }

  inline const char* aString::toChar() const
  {
    return c_str();
  }

  inline double aString::toDouble() const
  {
    return atof( c_str());
  }

  inline bool aString::toBool() const
  {
    return ( atoi( c_str()) != 0);
  }

  inline int aString::toInt() const
  {
    return atoi( c_str());
  }

  inline long aString::toLong() const
  {
    return atol( c_str());
  }

  inline unsigned int aString::toUInt() const
  {
    return atoi( c_str());
  }

  inline unsigned long aString::toULong() const
  {
    return atol( c_str());
  }

  inline size_t aString::toSize_T() const
  {
    return size_t( strtoul( c_str(), 0x0, 10));
  }

  inline bool aString::isDecimal() const
  {
    return ( empty() ? false : trim().find_first_not_of( "0123456789-+") == std::string::npos);
  }

  inline bool aString::isNumeric() const
  {
    return ( empty() ? false : trim().find_first_not_of( "0123456789eEdDgG-+.") == std::string::npos);
  }

  inline bool aString::isNumericOrTime() const
  {
    return ( empty() ? false : trim().find_first_not_of( "0123456789eEdDgG-+.:") == std::string::npos);
  }

  inline aString aString::toLowerCase() const
  {
    std::string s( *this);
    std::transform( s.begin(), s.end(), s.begin(), []( unsigned char c) -> unsigned char { return std::tolower( c); });
    return s;
  }

  inline aString aString::toUpperCase() const
  {
    aString s( *this);
    std::transform( s.begin(), s.end(), s.begin(), []( unsigned char c) -> unsigned char { return std::toupper( c); });
    return s;
  }

  inline aString aString::trim() const
  {
    aString s = trimLeft();
    return s.trimRight();
  }

  inline aString aString::trimLeft() const
  {
    aString s( *this);
    s.erase( s.begin(), std::find_if_not(s.begin(), s.end(), []( unsigned char c) -> bool { return std::isspace( c); }));
    return s;
  }

  inline aString aString::trimRight() const
  {
    aString s( *this);
    s.erase( std::find_if_not( s.rbegin(), s.rend(), []( unsigned char c) -> bool { return std::isspace( c); }).base(), s.end());
    return s;
  }

  inline aString aString::trimFirst( const aString& text, bool inclusive) const
  {
    const size_t position = find( text, 0);
    if ( position != npos) {
      return substr( position + ( inclusive ? text.size() : 0));
    }
    return *this;
  }

  inline aString aString::trimLast( const aString& text, bool inclusive) const
  {
    const size_t position = rfind( text, npos);
    if ( position != npos) {
      return substr( 0, position + ( inclusive ? 0 : text.size()));
    }
    return *this;
  }

  inline aString aString::strrep( const aString& oldSubString, const aString& newSubString) const
  {
    aString cache( *this);
    size_t pos = cache.find( oldSubString, 0);
    const size_t oldLen = oldSubString.size();
    const size_t newLen = newSubString.size();
    while ( pos != npos) {
      cache.replace( pos, oldLen, newSubString);
      pos = cache.find( oldSubString, pos + newLen);
    }
    return cache;
  }

  inline size_t aString::count( const aString &text) const
  {
    size_t counter = 0;
    for ( size_t i = 0; i < size(); ++i) {
      if ( text.find( operator[]( i)) != std::string::npos) {
        ++counter;
      }
    }
    return counter;
  }

  inline bool aString::contains( const aString &text) const
  {
    if ( find( text) == std::string::npos) return false;
    return true;
  }

  inline bool aString::beginsWith( const aString &text) const
  {
    if ( text.size() > size()) return false;
    if ( substr( 0, text.size()) == text) return true;
    return false;
  }

  inline bool aString::endsWith( const aString &text) const
  {
    if ( text.size() > size()) return false;
    if ( substr( size() - text.size(), text.size()) == text) return true;
    return false;
  }

  inline void aString::initStringToken() const
  {
    stringTokenPos_     = 0;
    delimiterTriggered_ = 0;
  }

  inline aString aString::getStringToken( const aString &delimiters, bool ignoreDulicate) const
  {
    if ( stringTokenPos_ == std::string::npos) return aString();

    if ( stringTokenPos_) delimiterTriggered_ = at( stringTokenPos_ - 1);

    // Ignore duplicate delimiters.
    if ( ignoreDulicate) {
      std::string::size_type idx1 = find_first_not_of( delimiters, stringTokenPos_); // Ignore leading delimiters.
      if ( idx1 == std::string::npos) {                                              // No token found.
        stringTokenPos_ = std::string::npos;
        return aString();
      }
      std::string::size_type idx2 = find_first_of( delimiters, idx1);                // Find next delimiter.
      if ( idx2 == std::string::npos) {                                              // No delimiter found.
        stringTokenPos_ = std::string::npos;
        return substr( idx1);
      }
      std::string::size_type idx3 = find_first_not_of( delimiters, idx2);            // Find next token start.
      if ( idx3 == std::string::npos) {                                              // No next token.
        stringTokenPos_ = std::string::npos;
        return substr( idx1, idx2 - idx1);
      }
      stringTokenPos_ = idx3;
      return substr( idx1, idx2 - idx1);
    }

    // Force token on each delimiter, (e.g. for CSV files).
    std::string::size_type idx1 = stringTokenPos_;
    std::string::size_type idx2 = find_first_of( delimiters, stringTokenPos_);       // Find next delimiter.
    if ( idx2 == std::string::npos) {                                                // No delimiter found.
      stringTokenPos_ = std::string::npos;
      return substr( idx1);
    }
    stringTokenPos_ = idx2 + 1;
    return substr( idx1, idx2 - idx1);
  }

  inline aString aString::getStringTokenRemaining() const
  {
    std::string::size_type idx = stringTokenPos_;
    stringTokenPos_     = std::string::npos;
    delimiterTriggered_ = 0;
    return substr( idx);
  }


  /*
   * NB Default string delimiters are commas, tabs, newlines and semi-colons only,
   * so whitespace in strings will be preserved. *** Don't *** leave
   * whitespace at start of lines in array.
   */
  inline aStringList aString::toStringList( const aString &delimiters, bool ignoreDuplicate) const
  {
    aStringList stringList;
    initStringToken();
    while( hasStringTokens()) {
      stringList.push_back( getStringToken( delimiters, ignoreDuplicate));
    }
    return stringList;
  }

  inline aDoubleList aString::toDoubleList( const aString &delimiters, bool ignoreDuplicate) const
  {
    aDoubleList stringList;
    initStringToken();
    while( hasStringTokens()) {
      stringList.push_back( getStringToken( delimiters, ignoreDuplicate).toDouble());
    }
    return stringList;
  }

  inline aStringList aString::toStringIndexList( const size_t start, const size_t number, const size_t increment, 
                                                const bool fillAll, const int width, const char fill) const
  {
    aStringList stringList;
    for ( size_t i = 0; i < number; ++i) {
      aString tmp = counterArg<size_t>( start + i * increment, width, fill);
      while ( fillAll && tmp.contains( "%")) {
        tmp = tmp.counterArg<size_t>( start + i * increment, width, fill);
      }
      stringList.push_back( tmp);
    }
    return stringList;
  }

  // Input of form: 0.0:25.0:100.0,150.0:50.0:300.0
  inline aString aString::scaleDoubleList(const double& scale) const
  {
    return convertDoubleList( scale, 0.0);
  }

  // Input of form: 0.0:25.0:100.0,150.0:50.0:300.0
  inline aString aString::convertDoubleList( const double &scale, const double &offset) const
  {
    aString scaledString;
    initStringToken();
    while ( hasStringTokens()) {
      aString levelToken = getStringToken( ",");
      levelToken.initStringToken();
      if ( levelToken.contains( ":")) {
        double pMin  = levelToken.getStringToken( ":", false).toDouble() * scale + offset;
        double pStep = 1 * scale;
        if ( levelToken.count( ":") == 2) {
          pStep = levelToken.getStringToken( ":", false).toDouble() * scale;
        }
        double pMax = levelToken.getStringToken( ":", false).toDouble() * scale + offset;
        if ( scaledString.empty()) {
          scaledString = aString( "%:%:%").arg( pMin).arg( pStep).arg( pMax);
        }
        else {
          scaledString += aString( ",%:%:%").arg( pMin).arg( pStep).arg( pMax);
        }
      }
      else {
        if ( scaledString.empty()) {
          scaledString = aString( "%").arg( levelToken.toDouble() * scale + offset);
        }
        else {
          scaledString += aString( ",%").arg( levelToken.toDouble() * scale + offset);
        }
      }
    }
    initStringToken();
    return scaledString;
  }

  // Input of form: 0.0:25.0:100.0,150.0:50.0:300.0
  inline aDoubleList aString::interpretDoubleList( bool doUniqueSort) const
  {
    aDoubleList dList;
    initStringToken();
    while ( hasStringTokens()) {
      aString levelToken = getStringToken( ",");
      levelToken.initStringToken();
      if ( levelToken.contains( ":")) {
        double pMin  = levelToken.getStringToken( ":", false).toDouble();
        double pStep = 1;
        if ( levelToken.count( ":") == 2) {
          pStep = levelToken.getStringToken( ":", false).toDouble();
        }
        double pMax = levelToken.getStringToken( ":", false).toDouble();
        // Make sure our pStep is working in the correct direction.
        if ( pStep != 0.0 && dstomath::sign( pStep) == dstomath::sign( pMax - pMin)) {
          size_t nLevels = static_cast<size_t>( floor( ( pMax - pMin) / pStep + dstomath::zero())) + 1;
          for ( size_t i = 0; i < nLevels; ++i) {
            dList.push_back( pMin + pStep * double( i));
          }
        }
      }
      else {
        dList.push_back( levelToken.toDouble());
      }
    }
    if ( doUniqueSort) {
      // Sort elements in ascending order and remove duplicate values.
      std::sort( dList.begin(), dList.end());
      dList.erase( std::unique( dList.begin(), dList.end(), double_equiv), dList.end());
    }
    initStringToken();
    return dList;
  }

  // Input of form: 0.0:25.0:100.0,150.0:50.0:300.0
  inline std::valarray<double> aString::interpretDoubleVector( bool doUniqueSort) const
  {
    aDoubleList dList = interpretDoubleList( doUniqueSort);
    std::valarray<double> dVector( &dList[0], dList.size());
    return dVector;
  }

  // Input of form: 1-10,15-20,72-100
  inline aIntList aString::interpretIntList()
  {
    aIntList iList;
    initStringToken();
    while ( hasStringTokens()) {
      aString levelToken = getStringToken( ", \t");
      if ( levelToken.contains( "-")) {
        int pStart = levelToken.getStringToken( "-", false).toInt();
        int pEnd   = levelToken.getStringToken( "-", false).toInt();
        for ( int level = pStart; level <= pEnd; ++level) {
          iList.push_back( level);
        }
      }
      else {
        iList.push_back( levelToken.toInt());
      }
    }
    initStringToken();
    return iList;
  }

  // Create a random string of length strLength
  inline aString aString::random( const size_t &strLength)
  {
    aString s;
    s.resize( strLength);
    size_t iLen = strLength - 1;

    static const char alphanum[] = "0123456789"
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for ( size_t j = 0 ; j < iLen ; j++ ) {
      int iof = rand() % ( iLen );
      s.at( j ) = alphanum[ iof ];
    }
    s.at( iLen) = '\0';

    return s;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class aFileString : public aString
  {
   public:
    aFileString() : aString() {}
    aFileString( const char* str) : aString( str) {}
    aFileString( const std::string &str) : aString( str) {}

    aFileString path( bool keepSlash = false) const;
    aFileString fileName() const;
    aFileString baseName() const;
    aFileString fullBaseName() const;
    aFileString suffix() const;
    aFileString fullSuffix() const;
    aFileString extension( bool complete = true) const;
    aFileString trimExtension( bool complete = true) const;

    bool isAbsolute() const;
    bool isRelative() const;
  };

  inline aFileString aFileString::path( bool keepSlash) const {
    std::string::size_type lpos;
    if (( lpos = find_last_of( "/\\")) != std::string::npos) {
      if ( keepSlash) lpos += 1;
      return aFileString( substr( 0, lpos));
    }
    return aFileString();
  }

  inline aFileString aFileString::baseName() const {
    aFileString basename = *this;
    std::string::size_type lpos;
    if (( lpos = basename.find_last_of( "/\\")) != std::string::npos) {
      basename = substr( lpos + 1);
    }
    if (( lpos = basename.find_first_of( '.')) != std::string::npos) {
      return basename.substr( 0, lpos);
    }
    return basename;
  }

  inline aFileString aFileString::fullBaseName() const {
    aFileString basename = *this;
    std::string::size_type lpos;
    if (( lpos = basename.find_last_of( "/\\")) != std::string::npos) {
      basename = substr( lpos + 1);
    }
    if (( lpos = basename.find_last_of( '.')) != std::string::npos) {
      return basename.substr( 0, lpos);
    }
    return basename;
  }

  inline aFileString aFileString::fileName() const {
    std::string::size_type lpos;
    if (( lpos = find_last_of( "/\\")) != std::string::npos) {
      return aFileString( substr( lpos + 1));
    }
    return *this;
  }

  inline aFileString aFileString::suffix() const {
    std::string::size_type lpos;
    if (( lpos = find_last_of( '.')) != std::string::npos) {
      return aFileString( substr( lpos + 1));
    }
    return aFileString();
  }

  inline aFileString aFileString::fullSuffix() const {
    std::string::size_type lpos;
    aFileString thisFileName( fileName());
    if (( lpos = thisFileName.find_first_of( '.')) != std::string::npos) {
      return aFileString( thisFileName.substr( lpos + 1));
    }
    return aFileString();
  }

  inline aFileString aFileString::extension( bool complete) const {
    return complete ? fullSuffix() : suffix();
  }

  inline aFileString aFileString::trimExtension( bool complete) const {
    aFileString ext = complete ? fullSuffix() : suffix();
    return aFileString( substr( 0, size() - ext.size() - 1));
  }

  inline bool aFileString::isAbsolute() const {
    if ( size() == 0) return false;

    // Linux & Windows starting with "/" or "//" or "\" or "\\".
    if ( substr( 0, 1) == "/") return true;
    if ( substr( 0, 1) == "\\") return true;

    // Windows
    if ( size() < 3) return false;
    if ( substr( 1, 2) == ":/") return true;
    if ( substr( 1, 2) == ":\\") return true;

    return false;
  }

  inline bool aFileString::isRelative() const {
    return !isAbsolute();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // aOptional support for aString
  template<>
  struct aOptionalValidator<aString>
  {
    static bool isValid( const aString& v)
    {
      return v != aString( "\0\0");
    }
    static aString invalidValue()
    {
      return aString( "\0\0");
    }
  };

  typedef aOptional<aString> aOptionalAString;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} /* End namespace dstoute */

#endif /* ASTRING_H_ */
