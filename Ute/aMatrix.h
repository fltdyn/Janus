#ifndef AMATRIX_H_
#define AMATRIX_H_

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
// Class:       aMatrix
// Description: SDH's Template Matrix / Vector class. Uses full operator
//              overloading to allow Matrix A = B * C + V - 5.0, etc.
//
// Written by: Shane D. Hill  (Shane.Hill@dsto.defence.gov.au)
// Helped by:  Geoff J. Brian (Geoff.Brian@dsto.defence.gov.au)
//             Based in part on Geoff's Matrix.h (double).
// Renamed from: aMatrix.h
//
// First Date: 21/01/2005
//

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

// C++ Includes
#include <iostream>
#include <valarray>
#include <vector>
#include <algorithm>
#include <limits>
#include <complex>
#ifdef _DSTO_MATH_RANGE_CHECK_
  #include <stdexcept>
#endif

// C Includes
#include <cstdlib>
#include <ctime>
#include <cfloat>
#include <cmath>

namespace dstomath {

  using std::valarray;
  using std::slice;
  using std::slice_array;

  namespace am_math {
    #if defined(_MSC_VER)
    #include <cfloat>
    template <typename T> bool isnan( const T &v)                 {return ( 0 != _isnan(v));}
    template <typename T> bool isValid( const T &v)               {return ( 0 == _isnan(v));}
    #else
    template <typename T> bool isnan( const T &v)                 {return !(v == v);}
    template <typename T> bool isValid( const T &v)               {return  (v == v);}
    #endif

    template <typename T> inline T abs( const T &t)               {return ((t < T(0)) ? -t : t);}
    template <typename T> inline T min( const T &t1, const T &t2) {return ((t1 < t2) ? t1 : t2);}
    template <typename T> inline T max( const T &t1, const T &t2) {return ((t1 > t2) ? t1 : t2);}
    inline double zero()                                          {return DBL_EPSILON * 100.0;}
    template <typename T> bool isZero( const T &v)
    {
      if ( am_math::isnan( v)) {
        return false;
      }
      return ( am_math::abs(v) < am_math::zero());
    }
    template <typename T> bool isNotZero( const T &v)             {return !isZero(v);}
  }

  enum Cc_Dir{
    CC_HORIZONTAL = 0,
    CC_VERTICAL   = 1
  };

  //
  // std::vector / std::valarray conversions.
  //
  template <typename T> inline valarray<T> toStdValArray( const std::vector<T> &v)
  {
    if ( v.empty()) {
      return valarray<T>{};
    }
    return valarray<T>{ &v[ 0], v.size()};
  }

  template <typename T> inline std::vector<T> toStdVector( const valarray<T> &v)
  {
    if ( v.size() == 0) {
      return std::vector<T>{};
    }
    return std::vector<T>{ std::begin( v), std::end( v)};
  }

  //
  // General math functions.
  //
  template <typename T> inline T sign( const T &a, const T &b)
  {
    return ( b >= T(0) ? am_math::abs(a) : -am_math::abs(a));
  }

  template <typename T> inline T pythag( T a, T b)
  {
    T c;
    a = am_math::abs( a);
    b = am_math::abs( b);
    if ( a > b) {
      c = b / a;
      return ( a * ::sqrt( T(1) + c * c));
    }
    if ( am_math::isZero( b)) {
      return T(0);
    }
    c = a / b;
    return ( b * ::sqrt( T(1) + c * c));
  }

  /** Matrix Slice
   *
   */
  class mslice {
  public:
    mslice( const size_t start_row, const size_t start_col, const size_t nrows, const size_t ncols)
      : startRow_( start_row), startCol_( start_col),
        rows_( nrows), cols_( ncols) {}
    mslice( const mslice &ms)
      : startRow_( ms.startRow_), startCol_( ms.startCol_),
        rows_( ms.rows_), cols_( ms.cols_) {}

    size_t size() const {return rows_ * cols_;}
    size_t rows() const {return rows_;}
    size_t cols() const {return cols_;}
    size_t startRow() const {return startRow_;}
    size_t startCol() const {return startCol_;}
    size_t endRow() const {return startRow_ + rows_;}
    size_t endCol() const {return startCol_ + cols_;}

  private:
    size_t startRow_;
    size_t startCol_;
    size_t rows_;
    size_t cols_;
  };

  template <typename T> class aSubVector;
  template <typename T> class aSubMatrix;

  /** Operator based matrix class.
   * aMatrix is an operator based matrix class in a template structure so that
   * matrix elements can be of any defined type (even another class if required).
   * This means that matrix calculations can be of the form C = A * B + C as in
   * the following example.
   *@code
   *  aMatrix<double> A(3,3), B(3,3), C;
   *  ...
   *  C = A * B + C; @endcode
   */
  template <typename T> class aMatrix {
  public:

    struct gDecompositionInfo {
      gDecompositionInfo() : rank( 0), determinant( T(0)) {}
      gDecompositionInfo( size_t r, T d)  : rank( r), determinant( T(d)) {}
      size_t rank;
      T      determinant;
    };

    //
    // Constructors and destructors.
    //
    aMatrix()                            {resize( 0, 0);}
    aMatrix( size_t /* dummy */)         {resize( 0, 0);} // This allows matrix of matrix.
    aMatrix( size_t nrows, size_t ncols) {resize( nrows, ncols);}
    aMatrix( const aMatrix<T> &m)      {copy( m);}
    aMatrix( size_t nrows, size_t ncols, const T &t)
    {
      resize( nrows, ncols);
      (*this) = t;
    }
    aMatrix( size_t nrows, size_t ncols, const T t[], bool doTransposeArray = false)
    {
      resize( nrows, ncols);
      if ( doTransposeArray) {
        size_t k = 0;
        for ( size_t j = 0; j < cols_; ++j) {
          for ( size_t i = 0; i < rows_; ++i) {
            (*this)(i,j) = t[k];
            ++k;
          }
        }
      }
      else {
        (*this) = t;
      }
    }
    aMatrix( size_t nrows, size_t ncols, const valarray<T> &v, bool doTransposeArray = false)
    {
      resize( nrows, ncols);
      if ( doTransposeArray) {
        math_range_check(
          if ( size_ != v.size()) {
            throw std::invalid_argument( "aMatrix<T>::constructor(): valarray has incorrect size.");
          }
        );
        size_t k = 0;
        for ( size_t j = 0; j < cols_; ++j) {
          for ( size_t i = 0; i < rows_; ++i) {
            (*this)(i,j) = v[k];
            ++k;
          }
        }
      }
      else {
        (*this) = v;
      }
    }

    //
    // Subscript and sub-matrix operators.
    //
    /** Extract a matrix row by value.
     * Returns a copy of a matrix row as a valarray.
     */
    valarray<T> operator[] ( size_t i) const
    {
      math_range_check(
        if ( i >= rows_) {
          throw std::out_of_range( "aMatrix<T>::operator[i]: Row i out of range.");
        }
      );
      return matrixData_[ slice( index(i,0), cols_, 1)];
    }

    aSubVector<T> operator[] ( size_t i)
    {
      math_range_check(
        if ( i >= rows_) {
          throw std::out_of_range( "aMatrix<T>::operator[i]: Row i out of range.");
        }
      );
      return aSubVector<T>( *this, slice( index(i,0), cols_, 1));
    }

    aMatrix<T> operator[] ( const mslice &ms) const
    {
      math_range_check(
        if ( ms.endRow() > rows_) {
          throw std::out_of_range( "aMatrix<T>::operator[mslice]: Row slice out of range.");
        }
        if ( ms.endCol() > cols_) {
          throw std::out_of_range( "aMatrix<T>::operator[mslice]: Column slice out of range.");
        }
      );
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( ms.rows(), ms.cols());
      size_t ii = ms.startRow();
      for ( size_t i = 0; i < ms.rows(); ++i, ++ii) {
        size_t jj = ms.startCol();
        for ( size_t j = 0; j < ms.cols(); ++j, ++jj) {
          cacheMatrix0_(i,j) = (*this)(ii,jj);
        }
      }
      return cacheMatrix0_;
    }

    aSubMatrix<T> operator[] ( const mslice &ms)
    {
      math_range_check(
        if ( ms.endRow() > rows_) {
          throw std::out_of_range( "aMatrix<T>::operator[mslice]: Row slice out of range.");
        }
        if ( ms.endCol() > cols_) {
          throw std::out_of_range( "aMatrix<T>::operator[mslice]: Column slice out of range.");
        }
      );
      return aSubMatrix<T>( *this, ms);
    }

    /** Extract a matrix column by value.
     * Returns a copy of a matrix column as a valarray.
     */
    valarray<T> operator() ( size_t j) const
    {
      math_range_check(
        if ( j >= cols_) {
          throw std::out_of_range( "aMatrix<T>::operator(j): Column j out of range.");
        }
      );
      return matrixData_[ slice( j, rows_, cols_)];
    }

    aSubVector<T> operator() ( size_t j)
    {
      math_range_check(
        if ( j >= cols_) {
          throw std::out_of_range( "aMatrix<T>::operator(j): Column j out of range.");
        }
      );
      return aSubVector<T>( *this, slice( j, rows_, cols_));
    }

    /** Extract a matrix element by reference.
     * Returns a the reference to a matrix element.
     */
    T& operator() ( size_t i, size_t j)
    {
      math_range_check(
        if ( i >= rows_) {
          throw std::out_of_range( "aMatrix<T>::operator(i,j): Row i out of range.");
        }
        if ( j >= cols_) {
          throw std::out_of_range( "aMatrix<T>::operator(i,j): Column j out of range.");
        }
      );
      return matrixData_[ index(i,j)];
    }

    /** Extract a matrix element by value.
     * Returns a copy of a matrix element.
     */
    const T& operator() ( size_t i, size_t j) const
    {
      math_range_check(
        if ( i >= rows_) {
          throw std::out_of_range( "aMatrix<T>::operator(i,j): Row i out of range.");
        }
        if ( j >= cols_) {
          throw std::out_of_range( "aMatrix<T>::operator(i,j): Column j out of range.");
        }
      );
      return matrixData_[ index(i,j)];
    }

    valarray<T> diag( int r = 0, int c = -1) const
    {
      math_range_check(
        if ( c < 0) {
          if ( r <= -int( rows_) || r >= int( cols_)) {
            throw std::out_of_range( "aMatrix<T>::diag(dCol): Index dCol out of range.");
          }
        }
        else {
          if ( r < 0 || r >= int( rows_)) {
            throw std::out_of_range( "aMatrix<T>::diag( row, col): Index row out of range.");
          }
          if ( c < 0 || c >= int( cols_)) {
            throw std::out_of_range( "aMatrix<T>::diag( row, col): Index col out of range.");
          }
        }
      );

      if ( c < 0) {
        c = r;
        r = 0;
      }

      int start  = r * cols_ + c;
      if ( start < 0) {
        start = - start * cols_;
        r = start / cols_;
      }

      return matrixData_[ slice( start,
                                 std::min(( cols_ - c), ( rows_ - r)),
                                 cols_ + 1)];

    }


    aSubVector<T> diag( int r = 0, int c = -1)
    {
      math_range_check(
        if ( c < 0) {
          if ( r <= -int( rows_) || r >= int( cols_)) {
            throw std::out_of_range( "aMatrix<T>::diag(dCol): Index dCol out of range.");
          }
        }
        else {
          if ( r < 0 || r >= int( rows_)) {
            throw std::out_of_range( "aMatrix<T>::diag( row, col): Index row out of range.");
          }
          if ( c < 0 || c >= int( cols_)) {
            throw std::out_of_range( "aMatrix<T>::diag( row, col): Index col out of range.");
          }
        }
      );

      if ( c < 0) {
        c = r;
        r = 0;
      }

      int start  = r * static_cast<int>( cols_) + c;
      if ( start < 0) {
        start = - start * static_cast<int>( cols_);
        r = start / static_cast<int>( cols_);
      }

      return aSubVector<T>( *this, slice( start,
                                          std::min(( cols_ - c), ( rows_ - r)),
                                          cols_ + 1));
    }

    //
    // Assignment operators.
    //
    /** Assignment of a matrix from another matrix.
     * Returns the reference to a copy of a given matrix. The assigned matrix is
     * resized to fit the new matrix data if required.
     *
     * Example: @include aMatrixMatrixAssignment.cpp
     * Output: @verbinclude aMatrixMatrixAssignment.out
     */
    aMatrix<T>& operator= ( const aMatrix<T> &m)
    {
      if ( &m != this) {
        copy( m);
      }
      return *this;
    }

    aMatrix<T>& operator= ( const valarray<T> &v)
    {
      math_range_check(
        if ( size_ != v.size()) {
          throw std::invalid_argument( "aMatrix<T>::operator=: valarray has incorrect size.");
        }
      );
      matrixData_ = v;
      return *this;
    }

    aMatrix<T>& operator= ( const T t[])
    {
      for ( size_t i = 0; i < size_; ++i) {
        matrixData_[i] = t[i];
      }
      return *this;
    }

    aMatrix<T>& operator= ( const T &t)
    {
      matrixData_ = t;
      return *this;
    }

    //
    // Computed assignment operators.
    //
    aMatrix<T>& operator*= ( const aMatrix<T> &m)
    {
      math_range_check(
        if ( cols_ != m.rows_) {
          throw std::invalid_argument( "aMatrix<T>::operator*=: Matrices not compatible for multiply.");
        }
      );
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_, m.cols_);
      T result;
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < m.cols_; ++j) {
          result = T(0);
          for ( size_t k = 0; k < m.rows_; ++k) {
            result += (*this)(i,k) * m(k,j);
          }
          cacheMatrix0_(i,j) = result;
        }
      }
      *this = cacheMatrix0_;
      return *this;
    }

    aMatrix<T>& operator*= ( const T &t)
    {
      matrixData_ *= t;
      return *this;
    }

    aMatrix<T>& operator/= ( const aMatrix<T> &m)
    {
      *this = *this * !m;
      return *this;
    }

    aMatrix<T>& operator/= ( const T &t)
    {
      matrixData_ *= 1.0 / t;
      return *this;
    }

    aMatrix<T>& operator+= ( const aMatrix<T> &m)
    {
      math_range_check(
        if ( (cols_ != m.cols_) || (rows_ != m.rows_)) {
          throw std::invalid_argument( "aMatrix<T>::operator+=: Matrices not compatible for addition.");
        }
      );
      matrixData_ += m.matrixData_;
      return *this;
    }

    aMatrix<T>& operator+= ( const T &t)
    {
      matrixData_ += t;
      return *this;
    }

    aMatrix<T>& operator-= ( const aMatrix<T> &m)
    {
      math_range_check(
        if ( (cols_ != m.cols_) || (rows_ != m.rows_)) {
          throw std::invalid_argument( "aMatrix<T>::operator-=: Matrices not compatible for subtraction.");
        }
      );
      matrixData_ -= m.matrixData_;
      return *this;
    }

    aMatrix<T>& operator-= ( const T &t)
    {
      matrixData_ -= t;
      return *this;
    }

    //
    // Unary operators.
    //
    aMatrix<T> operator+ () const {return *this;}
    aMatrix<T> operator- () const
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_, cols_);
      for ( size_t i = 0; i < size_; ++i) {
        cacheMatrix0_.matrixData_[i] = -matrixData_[i];
      }
      return cacheMatrix0_;
    };
    aMatrix<T> operator~ () const
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( cols_, rows_);
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < cols_; ++j) {
          cacheMatrix0_(j,i) = (*this)(i,j);
        }
      }
      return cacheMatrix0_;
    }

    aMatrix<T> operator! () const
    {
      aMatrix<T> cacheMatrix0_( *this);
      cacheMatrix0_.inverse();
      return cacheMatrix0_;
    }

    //
    // Binary Operators
    //
    friend aMatrix<T> operator* ( const aMatrix<T> &m1, const aMatrix<T> &m2)
    {
      math_range_check(
        if ( m1.cols_ != m2.rows_) {
          throw std::invalid_argument( "aMatrix<T> operator*: Matrices not compatible for multiply.");
        }
      );
      // Don't use operator*= here. Speed penalty.
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m1.rows_, m2.cols_);
      T result;
      for ( size_t i = 0; i < m1.rows_; ++i) {
        for ( size_t j = 0; j < m2.cols_; ++j) {
          result = 0;
          for ( size_t k = 0; k < m1.cols_; ++k) {
            result += m1(i,k) * m2(k,j);
          }
          cacheMatrix0_(i,j) = result;
        }
      }
      return cacheMatrix0_;
    };

    friend aMatrix<T> operator* ( const aMatrix<T> &m1, const T &t)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ *= t;
      return cacheMatrix0_;
    };

    friend aMatrix<T> operator* ( const T &t, const aMatrix<T> &m1)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ *= t;
      return cacheMatrix0_;
    }

    friend valarray<T> operator* ( const aMatrix<T> &m, const valarray<T> &v)
    {
      math_range_check(
        if ( m.cols_ != v.size()) {
          throw std::invalid_argument( "aMatrix<T>::operator*: Matrix and Vector not compatible.");
        }
      );
      valarray<T> vTemp( m.rows_);
      T result;
      for ( size_t i = 0; i < m.rows_; ++i) {
        result = T(0);
        for ( size_t j = 0; j < m.cols_; ++j) {
          result += m(i,j) * v[j];
        }
        vTemp[i] = result;
      }
      return vTemp;
    }

    friend valarray<T> operator* ( const valarray<T> &v, const aMatrix<T> &m)
    {
      math_range_check(
        if ( m.rows_ != v.size()) {
          throw std::invalid_argument( "aMatrix<T>::operator*: Vector and Matrix not compatible.");
        }
      );
      valarray<T> vTemp( m.cols_);
      T result;
      for ( size_t j = 0; j < m.cols_; ++j) {
        result = T(0);
        for ( size_t i = 0; i < m.rows_; ++i) {
          result += v[i] * m(i,j);
        }
        vTemp[j] = result;
      }
      return vTemp;
    }

    friend aMatrix<T> operator/ ( const aMatrix<T> &m1, const aMatrix<T> &m2)
    {
      return m1 * !m2;
    };

    friend aMatrix<T> operator/ ( const aMatrix<T> &m1, const T &t)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ /= t;
      return cacheMatrix0_;
    };

    friend aMatrix<T> operator/ ( const T &t, const aMatrix<T> &m1)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_.inverse();
      cacheMatrix0_ *= t;
      return cacheMatrix0_;
    }

    friend valarray<T> operator/ ( const aMatrix<T> &m, const valarray<T> &v)
    {
      return m * (T(1)/v);
    }

    friend valarray<T> operator/ ( const valarray<T> &v, const aMatrix<T> &m)
    {
      return v * !m;
    }

    friend aMatrix<T> operator+ ( const aMatrix<T> &m1, const aMatrix<T> &m2)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ += m2;
      return cacheMatrix0_;
    };

    friend aMatrix<T> operator+ ( const aMatrix<T> &m1, const T &t)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ += t;
      return cacheMatrix0_;
    };
    friend aMatrix<T> operator+ ( const T &t, const aMatrix<T> &m1)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ += t;
      return cacheMatrix0_;
    }

    friend aMatrix<T> operator- ( const aMatrix<T> &m1, const aMatrix<T> &m2)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ -= m2;
      return cacheMatrix0_;
    };

    friend aMatrix<T> operator- ( const aMatrix<T> &m1, const T &t)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ -= t;
      return cacheMatrix0_;
    };
    friend aMatrix<T> operator- ( const T &t, const aMatrix<T> &m1)
    {
      aMatrix<T> cacheMatrix0_( m1);
      cacheMatrix0_ -= t;
      return cacheMatrix0_;
    }

    //
    // Boolean operators.
    //
    friend bool operator== ( const aMatrix<T> &m1, const aMatrix<T> &m2)
    {
      if ( (m1.rows_ != m2.rows_) || (m1.cols_ != m2.cols_)) {
        return false;
      }
      for ( size_t i = 0; i < m1.rows_; ++i) {
        for ( size_t j = 0; j < m1.cols_; ++j) {
          if ( am_math::isNotZero( m1(i,j) - m2(i,j))) {
            return false;
          }
        }
      }
      return true;
    }

    friend bool operator!= ( const aMatrix<T> &m1, const aMatrix<T> &m2)
    {
      return !(m1 == m2);
    }

    bool isSquare() const {return ( rows_ == cols_);}

    bool isSingular() const
    {
      if ( isSquare() == false) {
        return false;
      }
      return am_math::isZero( determinant());
    }

    bool isDiagonal() const {
      if ( isSquare() == false) {
        return false;
      }
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < cols_; ++j) {
          if ( i != j && am_math::isNotZero( (*this)(i,j))) {
            return false;
          }
        }
      }
      return true;
    }

    bool isScalar() const
    {
      if ( !isDiagonal()) {
        return false;
      }
      T t = (*this)(0,0);
      for ( size_t i = 1; i < rows_; ++i) {
        if ( am_math::isNotZero( (*this)(i,i) - t)) {
          return false;
        }
      }
      return true;
    }

    bool isUnit() const
    {
      if ( !isDiagonal()) {
        return false;
      }
      for ( size_t i = 0; i < rows_; ++i) {
        if ( am_math::isNotZero( (*this)(i,i) - T(1))) {
          return false;
        }
      }
      return true;
    }

    bool isZero() const
    {
      for ( size_t i = 0; i < size_; ++i) {
        if ( am_math::isNotZero( matrixData_[i])) {
          return false;
        }
      }
      return true;
    }

    bool isNull() const {return isZero();}

    bool isSymmetric() const
    {
      if ( isSquare() == false) {
        return false;
      }
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < cols_; ++j) {
          if ( am_math::isNotZero( (*this)(i,j) - (*this)(j,i))) {
            return false;
          }
        }
      }
      return true;
    }

    bool isSkewSymmetric() const
    {
      if ( isSquare() == false) {
        return false;
      }
      for ( size_t j = 1; j < cols_; ++j) {
        for ( size_t i = 0; i < j; ++i) {
          if ( am_math::isNotZero( (*this)(i,j) + (*this)(j,i))) {
            return false;
          }
        }
      }
      return true;
    }

    bool isUpperTriangular() const
    {
      if ( isSquare() == false) {
        return false;
      }
      for ( size_t i = 1; i < rows_; ++i) {
        for ( size_t j = 0; j < i - 1; ++j) {
          if ( am_math::isNotZero( (*this)(i,j))) {
            return false;
          }
        }
      }
      return true;
    }

    bool isLowerTriangular() const
    {
      if ( isSquare() == false) {
        return false;
      }
      for ( size_t j = 1; j < cols_; ++j) {
        for ( size_t i = 0; i < j - 1; ++i) {
          if ( am_math::isNotZero( (*this)(i,j))) {
            return false;
          }
        }
      }
      return true;
    }

    bool isRowOrthogonal() const
    {
      aMatrix<T> cacheMatrix0_( *this);
      cacheMatrix0_ *= ~(*this);
      return cacheMatrix0_.isUnit();
    }

    bool isColumnOrthogonal() const
    {
      aMatrix<T> cacheMatrix0_( ~(*this));
      cacheMatrix0_ *= (*this);
      return cacheMatrix0_.isUnit();
    }

    bool isSameDimension( const aMatrix<T> &m1) const
    {
      return ( m1.rows_ == rows_) && ( m1.cols_ == cols_);
    }

    bool isSingleValue() const
    {
      return ( size_ == 1); // Single element.
    }

    bool isVector() const
    {
      return (( rows_ == 1) != ( cols_ == 1)); // One of rows or cols must be size 1, but not both.
    }

    bool isRowVector() const
    {
      return (( rows_ == 1) && ( cols_ > 1));
    }

    bool isColumnVector() const
    {
      return (( cols_ == 1) && ( rows_ > 1));
    }

    //
    // aMatrix Gauss/pivot functions.
    //
    const gDecompositionInfo& gInfo() const { return gInfo_;}

    gDecompositionInfo gDecomposition( valarray<size_t> &rowIndex)
    {
      math_range_check(
        if ( rowIndex.size() != rows_) {
          throw std::invalid_argument( "aMatrix<T>::gDecomposition(): Index vector not compatible with matrix.");
        }
      );
      size_t i, ii, j;
      size_t rowI, rowII, iMax;
      T scale, dValue, dMax;
      gInfo_.rank       = 0;
      gInfo_.determinant = T(1);
      for ( i = 0; i < rows_; ++i) {
        rowIndex[i] = i;
      }
      for ( i = 0; i < rows_ - 1; ++i) {
        // Find maximum value in column from this row down.
        iMax = i;
        rowI = rowIndex[iMax];
        dMax = am_math::abs( (*this)( rowI, i));
        for ( ii = i + 1; ii < rows_; ++ii) {
          rowII  = rowIndex[ii];
          dValue = am_math::abs( (*this)( rowII, i));
          if (  dValue > dMax) {
            dMax = dValue;
            iMax = ii;
          }
        }
        // Swap rows by index;
        if ( iMax != i) {
          ii                = rowIndex[iMax];
          rowIndex[iMax]    = rowIndex[i];
          rowIndex[i]       = ii;
          gInfo_.determinant = -gInfo_.determinant;
        }
        // Scale max value row.
        rowI   = rowIndex[i];
        dValue = (*this)( rowI, i);
        if ( am_math::isZero( dValue)) {
          // Singular Matrix.
          return gDecompositionInfo( i, T(0));
        }
        gInfo_.determinant *= dValue;
        scale = T(1) / dValue;
        for ( j = i; j < cols_; ++j) {
          (*this)( rowI, j) *= scale;
        }
        // Pivot other rows.
        for ( ii = i + 1; ii < rows_; ++ii) {
          rowII = rowIndex[ii];
          scale = (*this)( rowII, i);
          for ( j = i + 1; j < cols_; ++j) {
            (*this)( rowII, j) -= (*this)( rowI, j) * scale;
          }
        }
      }
      dValue = (*this)( rowIndex[i], i);
      if ( am_math::isZero( dValue)) {
        // Singular Matrix.
        return gDecompositionInfo( i, T(0));
      }
      gInfo_.rank         = am_math::min( rows_, cols_);
      gInfo_.determinant *= dValue;
      return gInfo_;
    }

    valarray<T> gVectorBackSubstitution( const valarray<size_t> &rowIndex) const
    {
      // Back substitute and reorder for solution vector.
      size_t i, j, rowI;
      valarray<T> vr( rows_);
      // Solve for last variable.
      i     = rows_ - 1;
      rowI  = rowIndex[i];
      vr[i] = (*this)( rowI, rows_) / (*this)( rowI, i);
      // Solve for all other variables.
      while ( i--) {
        rowI  = rowIndex[i];
        vr[i] = (*this)( rowI, rows_);
        for ( j = i + 1; j < rows_; ++j) {
          vr[i] -= (*this)( rowI, j) * vr[j];
        }
      }
      return vr;
    }

    aMatrix<T> gMatrixBackSubstitution( const valarray<size_t> &rowIndex) const
    {
      // Back substitute and reorder for solution matrix.
      size_t i, j, jj, rowI;
      T dValue;
      aMatrix<T> cacheMatrix1_;
      cacheMatrix1_.resize( rows_, cols_ - rows_);
      // Solve for last variable.
      i      = rows_ - 1;
      rowI   = rowIndex[i];
      dValue = T(1) / (*this)( rowI, i);
      for ( jj = 0; jj < cacheMatrix1_.cols_; ++jj) {
        cacheMatrix1_( i, jj) = (*this)( rowI, rows_ + jj) * dValue;
      }
      // Solve for all other variables.
      while ( i--) {
        rowI  = rowIndex[i];
        for ( jj = 0; jj < cacheMatrix1_.cols_; ++jj) {
          cacheMatrix1_( i, jj) = (*this)( rowI, rows_ + jj);
        }
        for ( j = i + 1; j < rows_; ++j) {
          dValue = (*this)( rowI, j);
          for ( jj = 0; jj < cacheMatrix1_.cols_; ++jj) {
            cacheMatrix1_( i, jj) -= dValue * cacheMatrix1_( j, jj);
          }
        }
      }
      return cacheMatrix1_;
    }

    void gBuildDecompositionMatrix( const valarray<T> &v, aMatrix<T> &md) const
    {
      md.resize( rows_, cols_ + 1);
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < cols_; ++j) {
          md( i, j) = (*this)( i, j);
        }
        md( i, cols_) = v[i];
      }
    }

    void gBuildDecompositionMatrix( const aMatrix<T> &m, aMatrix<T> &md) const
    {
      md.resize( rows_, cols_ + m.cols_);
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < cols_; ++j) {
          md( i, j) = (*this)( i, j);
        }
        for ( size_t j = 0; j < m.cols_; ++j) {
          md( i, cols_ + j) = m( i, j);
        }
      }
    }

    valarray<T> gSolve( const valarray<T> &v) const
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::gSolve(): Not a square matrix.");
        }
        if ( v.size() != rows_) {
          throw std::invalid_argument( "aMatrix<T>::gSolve(): Vector not compatible with matrix.");
        }
      );
      aMatrix<T> cacheMatrix0_;
      valarray<size_t> rowIndex( rows_);
      gBuildDecompositionMatrix( v, cacheMatrix0_);
      gInfo_ = cacheMatrix0_.gDecomposition( rowIndex);
      if ( gInfo_.rank == am_math::min( rows_, cols_)) {
        return cacheMatrix0_.gVectorBackSubstitution( rowIndex);
      }
      return valarray<T>( T(0), rows_);
    }

    aMatrix<T> gSolve( const aMatrix<T> &m) const
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::gSolve(): Not a square matrix.");
        }
        if ( m.rows_ != rows_) {
          throw std::invalid_argument( "aMatrix<T>::gSolve(): Input matrix not compatible.");
        }
      );
      aMatrix<T> cacheMatrix0_;
      valarray<size_t> rowIndex( rows_);
      gBuildDecompositionMatrix( m, cacheMatrix0_);
      gInfo_ = cacheMatrix0_.gDecomposition( rowIndex);
      if ( gInfo_.rank == am_math::min( rows_, cols_)) {
        return cacheMatrix0_.gMatrixBackSubstitution( rowIndex);
      }
      cacheMatrix0_.resize( m.rows_, m.cols_);
      cacheMatrix0_ = T(0);
      return cacheMatrix0_;
    }

    T gDeterminant() const
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::gDeterminant(): Not a square matrix.");
        }
      );
      aMatrix<T> cacheMatrix0_( *this);
      valarray<size_t> rowIndex( rows_);
      return cacheMatrix0_.gDecomposition( rowIndex).determinant;
    }

    size_t gRank() const
    {
      aMatrix<T> cacheMatrix0_( *this);
      valarray<size_t> rowIndex( rows_);
      return cacheMatrix0_.gDecomposition( rowIndex).rank;
    }

    //
    // aMatrix LU functions.
    //
    T luDecomposition( valarray<size_t> &rowIndex)
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::luDecomposition(): Not a square matrix.");
        }
      );

      size_t i, j, k;
      double currentMax, currentValue;
      T scale;

      T det = T(1);
      if ( rows_ != rowIndex.size()) {
        rowIndex.resize( rows_);
      }
      for ( i = 0; i < rows_; ++i) {
        rowIndex[i] = i;
      }
      for ( k = 0; k < rows_ - 1; ++k) {
        j = k;
        currentMax = am_math::abs( (*this)(rowIndex[k],k));
        for ( i = k + 1; i < rows_; ++i) {
          if ((currentValue = am_math::abs( (*this)(rowIndex[i],k))) > currentMax) {
            currentMax = currentValue;
            j = i;
          }
        }
        if ( j != k) {
          std::swap( rowIndex[j], rowIndex[k]);
          det = -det;
        }
        if ( am_math::isZero( (*this)(rowIndex[k],k))) {
          return T(0);
        }
        det *= (*this)(rowIndex[k],k);
        for ( i = k + 1; i < rows_; ++i) {
          scale = (*this)(rowIndex[i],k) /= (*this)(rowIndex[k],k);
          for ( j = k + 1; j < rows_; ++j) {
            (*this)(rowIndex[i],j) -= scale * (*this)(rowIndex[k],j);
          }
        }
      }
      det *= (*this)(rowIndex[k],k);
      return det;
    }

    valarray<T> luBackSubstitution( const valarray<size_t> &rowIndex, const valarray<T> &v) const
    {
      math_range_check(
        if ( v.size() != rows_) {
          throw std::invalid_argument( "aMatrix<T>::luBackSubstitution(): Incorrect valarray size.");
        }
        if ( rowIndex.size() != rows_) {
          throw std::invalid_argument( "aMatrix<T>::luBackSubstitution(): Incorrect rowIndex size.");
        }
      );

      size_t i, j, k, ri;
      bool equalToZero = true;

      valarray<T> solution( rows_);
      k = 0;
      for ( i = 0; i < rows_; ++i) {
        ri = rowIndex[i];
        solution[i] = v[ri];
        if ( !equalToZero) {
          for ( j = k; j < i; ++j) {
            solution[i] -= (*this)(ri,j) * solution[j];
          }
        }
        else if ( am_math::isNotZero( solution[i])) {
          k = i;
          equalToZero = false;
        }
      }
      for ( i = rows_ - 1; ; --i) {
        ri = rowIndex[i];
        for ( j = i + 1; j < rows_; ++j) {
          solution[i] -= (*this)(ri,j) * solution[j];
        }
        solution[i] /= (*this)(ri,i);
        if ( i == 0) break;
      }
      return solution;
    }

    valarray<T> luSolve( const valarray<T> &v) const
    {
      aMatrix<T> cacheMatrix0_( *this);
      valarray<size_t> rowIndex( rows_);
      math_range_check(
        T det = cacheMatrix0_.luDecomposition( rowIndex);
        if ( am_math::isZero( det)) {
          throw std::range_error( "aMatrix<T>::luDecomposition(): Singular matrix.");
        }
      );
      else_no_math_range_check(
        cacheMatrix0_.luDecomposition( rowIndex);
      );
      return cacheMatrix0_.luBackSubstitution( rowIndex, v);
    }

    T luDeterminant() const
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::luDeterminant(): Not a square matrix.");
        }
      );
      aMatrix<T> cacheMatrix0_( *this);
      valarray<size_t> rowIndex( rows_);
      return cacheMatrix0_.luDecomposition( rowIndex);
    }

    void inverse()
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::inverse(): Not a square matrix.");
        }
      );

      size_t i, j, k;
      T scale(0);

      valarray<size_t> rowIndex( rows_);
      for ( i = 0; i < rows_; ++i)
        rowIndex[i] = i;
      for ( k = 0; k < rows_; ++k) {
        i = k;
        auto currentMax = std::abs( (*this)( k,k));
        for ( j = k + 1; j < rows_; ++j) {
          const auto currentValue = std::abs( (*this)( j,k));
          if ( currentValue > currentMax) {
            currentMax = currentValue;
            i  = j;
          }
        }
        math_range_check(
          if ( am_math::isZero( currentMax)) {
            throw std::range_error( "aMatrix<T>::inverse(): Singular matrix.");
          }
        );
        if ( i != k) {
          std::swap( rowIndex[k], rowIndex[i]);
          for ( j = 0; j < cols_; ++j) {
            std::swap( (*this)(k,j), (*this)(i,j));
          }
        }
        scale = T(1) / (*this)(k,k);
        (*this)(k,k) = T(1);
        for ( j = 0; j < cols_; ++j) {
          (*this)(k,j) *= scale;
        }
        for ( i = 0; i < rows_; ++i) {
          if ( i != k) {
            scale        = (*this)(i,k);
            (*this)(i,k) = T(0);
            for ( j = 0; j < cols_; ++j) {
              (*this)(i,j) -= scale * (*this)(k,j);
            }
          }
        }
      }

      for ( j = 0; j < rows_; ++j) {
        if ( j != rowIndex[j]) {
          k = j + 1;
          while ( j != rowIndex[k]) {
            ++k;
          }
          for ( i = 0; i < rows_; ++i) {
            std::swap( (*this)(i,j), (*this)(i,k));
          }
          std::swap( rowIndex[j], rowIndex[k]);
        }
      }
    }

    /** Singular Value Decomposition.
     * Modified Numerical Recipes 'svdcmp' function.
     * This function computes the singular value decomposition of a matrix 'a':
     *  [a] = [u][diag(w)][v]^T
     *
     * Base Matrix  a(m,n) : Matrix to decompose.
     * Base Matrix  u(m,n) : LH matrix of decomposition (left orthogonal
     *                       transformation) is returned to base matrix.
     * @return      v(n,n) : RH matrix of decomposition (transpose of right
     *                       orthogonal transformation (**not v^T **)).
     * @return      w[n]   : Diagonal singular values vector.
     * @code a.svDecomposition( w, v); @endcode
     */
    void svDecomposition( aMatrix<T> &v, valarray<T> &w, T tolerance = am_math::zero())
    {
      if ( v.rows() != cols_ || v.cols() != cols_) {
        v.resize( cols_, cols_);
      }
      if ( w.size() != cols_) {
        w.resize( cols_);
      }

      valarray<T> rv1( cols_);

      size_t flag;
      size_t i, j, jj, k, l(0), nm;
      size_t iterations;
      T c, f, h, s, x, y, z, tmp;
      T g(0), scale(0), anorm(0);
      for ( i = 0; i < cols_; ++i) {
        l      = i + 1;
        rv1[i] = scale * g;
        g      = T(0);
        s      = T(0);
        scale  = T(0);
        if ( i < rows_) {
          for ( k = i; k < rows_; ++k) {
            scale += am_math::abs( (*this)(k,i));
          }
          if ( scale > tolerance) {
            for ( k = i; k < rows_; ++k) {
              tmp = (*this)(k,i) /= scale;
              s  += tmp * tmp;
            }
            f = (*this)(i,i);
            g = -dstomath::sign( ::sqrt(s), f);
            h = f * g - s;
            (*this)(i,i) = f - g;

            for ( j = l; j < cols_; ++j) {
              s = T(0);
              for ( k = i; k < rows_; ++k) {
                s += (*this)(k,i) * (*this)(k,j);
              }
              f = s / h;
              for ( k = i; k < rows_; ++k) {
                (*this)(k,j) += f * (*this)(k,i);
              }
            }
            for ( k= i; k < rows_; ++k) {
              (*this)(k,i) *= scale;
            }
          }
        }
        w[i]   = scale * g;
        g      = T(0);
        s      = T(0);
        scale  = T(0);
        if ( i < rows_ && i != cols_-1) {
          for ( k = l; k < cols_; ++k) {
            scale += am_math::abs((*this)(i,k));
          }
          if ( scale > tolerance) {
            for ( k = l; k < cols_; ++k) {
              tmp = (*this)(i,k) /= scale;
              s  += tmp * tmp;
            }
            f = (*this)(i,l);
            g = -dstomath::sign( ::sqrt(s), f);
            h = f * g - s;
            (*this)(i,l) = f - g;
            for ( k = l; k < cols_; ++k) {
              rv1[k] = (*this)(i,k) / h;
            }
            for ( j = l; j < rows_; ++j) {
              s = T(0);
              for ( k = l; k < cols_; ++k) {
                s += (*this)(j,k) * (*this)(i,k);
              }
              for ( k = l; k < cols_; ++k) {
                (*this)(j,k) += s * rv1[k];
              }
            }
            for ( k = l; k < cols_; ++k) {
              (*this)(i,k) *= scale;
            }
          }
        }
        anorm = am_math::max( anorm, am_math::abs( w[i]) + am_math::abs( rv1[i]));
      }

      i = cols_;
      do {
        --i;
        if ( i < (cols_ - 1)) {
          if ( am_math::abs(g) > tolerance) {
            for ( j = l; j < cols_; ++j) {
              v(j,i) = ((*this)(i,j) / (*this)(i,l)) / g;
            }
            for ( j = l; j < cols_; ++j) {
              s = T(0);
              for ( k = l; k < cols_; ++k) {
                s += (*this)(i,k) * v(k,j);
              }
              for ( k = l; k < cols_; ++k) {
                v(k,j) += s * v(k,i);
              }
            }
          }
          for ( j = l; j < cols_; ++j) {
            v(i,j) = v(j,i) = T(0);
          }
        }
        v(i,i) = T(1);
        g = rv1[i];
        l = i;
      } while ( i != 0);

      i = am_math::min( rows_, cols_);
      do {
        --i;
        l = i + 1;
        g = w[i];
        for ( j = l; j < cols_; ++j) {
          (*this)(i,j) = T(0);
        }
        if ( am_math::abs(g) > tolerance) {
          g = T(1) / g;
          for ( j = l; j < cols_; ++j) {
            s = T(0);
            for ( k = l; k < rows_; ++k) {
              s += (*this)(k,i) * (*this)(k,j);
            }
            f = ( s / (*this)(i,i)) * g;
            for ( k = i; k < rows_; ++k) {
              (*this)(k,j) += f * (*this)(k,i);
            }
          }
          for (j = i; j < rows_; ++j) {
            (*this)(j,i) *= g;
          }
        }
        else {
          for ( j = i; j < rows_; ++j) {
            (*this)(j,i) = T(0);
          }
        }
        ++(*this)(i,i);
      } while (i != 0);

      k = cols_;
      do {
        --k;
        for ( iterations = 1; iterations <= 30; ++iterations) {
          flag = 1;
          l = k + 1;
          do {
            --l;
            nm = l - 1;
            if ( am_math::abs( rv1[l]) < tolerance) {
              flag = 0;
              break;
            }
            if ( am_math::abs( w[nm]) < tolerance) {
              break;
            }
          } while ( l != 0);
          if ( flag) {
            c = T(0);
            s = T(1);
            for ( i = l; i <= k; ++i) {
              f      = s * rv1[i];
              rv1[i] = c * rv1[i];
              if ( am_math::abs(f) < tolerance) {
                break;
              }
              g    = w[i];
              h    = dstomath::pythag( f, g);
              w[i] = h;
              h    = 1.0 / h;
              c    = g * h;
              s    = -f * h;
              for ( j = 0; j < rows_; ++j) {
                y = (*this)(j,nm);
                z = (*this)(j,i);
                (*this)(j,nm) = y * c + z * s;
                (*this)(j,i)  = z * c - y * s;
              }
            }
          }
          z = w[k];
          if ( l == k) {
            if ( z < T(0)) {
              w[k] = -z;
              for ( j = 0; j < cols_; ++j) {
                v(j,k) = -v(j,k);
              }
            }
            break;
          }

          if ( iterations == 30) {
            math_range_check(
              throw std::range_error( "aMatrix<T>::svDecomposition: No convergence after 30 iterations.");
            );
            break;
          }

          x  = w[l];
          nm = k - 1;
          y  = w[nm];
          g  = rv1[nm];
          h  = rv1[k];
          f  = ((y-z) * (y+z) + (g-h) * (g+h)) / (2.0 * h * y);
          g  = dstomath::pythag( f, T(1));
          f  = ((x-z) * (x+z) + h * ((y / (f+dstomath::sign( g, f))) - h)) / x;
          c  = T(1);
          s  = T(1);
          for ( j = l; j <= nm; ++j) {
            i      = j + 1;
            g      = rv1[i];
            y      = w[i];
            h      = s * g;
            g      = c * g;
            z      = dstomath::pythag( f, h);
            rv1[j] = z;
            c      = f / z;
            s      = h / z;
            f      = x * c + g * s;
            g      = g * c - x * s;
            h      = y * s;
            y     *= c;
            for ( jj = 0; jj < cols_; ++jj) {
              x = v(jj,j);
              z = v(jj,i);
              v(jj,j) = x * c + z * s;
              v(jj,i) = z * c - x * s;
            }
            z    = dstomath::pythag( f, h);
            w[j] = z;
            if ( am_math::abs(z) > tolerance) {
              z = 1.0 / z;
              c = f * z;
              s = h * z;
            }
            f = c * g + s * y;
            x = c * y - s * g;
            for ( jj = 0; jj < rows_; ++jj) {
              y = (*this)(jj,j);
              z = (*this)(jj,i);
              (*this)(jj,j) = y * c + z * s;
              (*this)(jj,i) = z * c - y * s;
            }
          }
          rv1[l] = 0.0;
          rv1[k] = f;
          w[k]   = x;
        }
      } while ( k != 0);
      return;
    }

    /** Singular Value Back Substitution.
     * Modified Numerical Recipes 'svbksb' function.
     * This functions solves A.X = B for a vector X, where A is represented by
     * a set of matrices determined from singular value decomposition of A.
     * The function evaluates:
     *    [y_out] = [v] * [diag(1/w)] * [u]^T * [y_in]
     *
     * Base Matrix u(m,n) : LH matrix of decomposition.
     * @param      v(n,n) : RH matrix of decomposition.
     * @param      w[n]   : Singular values.
     * @param      b[m]   : RHS to be solved for.
     * @return     s[m]   : Solution vector.
     * @code s = a.svBackSubstitution( v, w, b); @endcode
     */
    valarray<T> svBackSubstitution( const aMatrix<T> &v, const valarray<T> &w,
                                    const valarray<T> &b) const
    {
      math_range_check(
        if ( v.rows() != cols_ || v.cols() != cols_) {
          throw std::invalid_argument( "aMatrix<T>::svBackSubstitution(): Incorrect Matrix size for v.");
        }
        if ( w.size() != cols_) {
          throw std::invalid_argument( "aMatrix<T>::svBackSubstitution(): Incorrect valarray size for w.");
        }
        if ( b.size() != rows_) {
          throw std::invalid_argument( "aMatrix<T>::svBackSubstitution(): Incorrect valarray size for b.");
        }
      );

      //
      // Only multiply matrix elements for w[j] > ZERO.
      // This increases matrix speed over a Vect2 = Matrix * Vect1
      // and prevents a divide by ZERO later.
      //
      valarray<T> tmp( cols_);
      for ( size_t j = 0; j < cols_; ++j) {
        T result(0);
        if ( am_math::isNotZero( w[j])) {
          for ( size_t i = 0; i < rows_; ++i) {
            result += (*this)(i,j) * b[i];
          }
          result /= w[j];
        }
        tmp[j] = result;
      }

      return ( v * tmp);
    }

    /** Singular Value Back Substitution.
     * Modified Numerical Recipes 'svbksb' function.
     * This functions solves A.X = B for a vector X, where A is represented by
     * a set of matrices determined from singular value decomposition of A.
     * The function evaluates:
     *    [y_out] = [v] * [diag(1/w)] * [u]^T * [y_in]
     *
     * Base Matrix u(m,n) : LH matrix of decomposition.
     * @param      v(n,n) : RH matrix of decomposition.
     * @param      w[n]   : Singular values.
     * @param      b[m]   : RHS to be solved for.
     * @return     s[m]   : Solution vector.
     * @code s = a.svBackSubstitution( v, w, b); @endcode
     */
    aMatrix<T> svBackSubstitution( const aMatrix<T> &v, const valarray<T> &w,
                                     const aMatrix<T> &b) const
    {
      math_range_check(
        if ( v.rows() != cols_ || v.cols() != cols_) {
          throw std::invalid_argument( "aMatrix<T>::svBackSubstitution(): Incorrect Matrix size for v.");
        }
        if ( w.size() != cols_) {
          throw std::invalid_argument( "aMatrix<T>::svBackSubstitution(): Incorrect valarray size for w.");
        }
        if ( b.rows() != rows_) {
          throw std::invalid_argument( "aMatrix<T>::svBackSubstitution(): Incorrect Matrix size for b.");
        }
      );

      //
      // Only multiply matrix elements for w[j] > ZERO.
      // This increases matrix speed over a Vect2 = Matrix * Vect1
      // and prevents a divide by ZERO later.
      //
      aMatrix<T> tmp( cols_, b.cols());
      for ( size_t j = 0; j < cols_; ++j) {
        for ( size_t k = 0; k < b.cols(); ++k) {
          T result(0);
          if ( am_math::isNotZero( w[j])) {
            for ( size_t i = 0; i < rows_; ++i) {
              result += (*this)(i,j) * b(i,k);
            }
            result /= w[j];
          }
          tmp(j,k) = result;
        }
      }

      return ( v * tmp);
    }

    /** Solve Set of Equations using Singular Value Decomposition.
     * This functions solves A.X = B for a vector X, where A is represented by
     * a set of matrices determined from singular value decomposition of A.
     * The function evaluates:
     *    [s] = [v] * [diag(1/w)] * [u]^T * [b]
     *
     * Base Matrix a(m,n) : Matrix to decompose and left const.
     * @param      b[m]   : RHS to be solved for.
     * @return     s[m]   : Solution vector.
     * @code s = a.svBackSubstitution( v, w, b); @endcode
     */
    valarray<T> svSolve( const valarray<T> &b) const
    {
      aMatrix<T> cacheMatrix1_( *this);
      aMatrix<T> cacheMatrix2_;
      valarray<T> w;
      cacheMatrix1_.svDecomposition( cacheMatrix2_, w);
      /*
       * This is a very simple test for rank deficient matrices, setting
       * the factors of the deficient basis functions to zero.  Some
       * better algorithm may be required in future.
       */
      T cutoff = w.max() * T(1.0e-5);
      valarray<bool> tCut = ( w - cutoff ) > T(0);
      w = w * T(0) + static_cast<valarray<double> >( w[ tCut ] );
      return cacheMatrix1_.svBackSubstitution( cacheMatrix2_, w, b);
    }

    /** Solve Set of Equations using Singular Value Decomposition.
     * This functions solves A.X = B for a vector X, where A is represented by
     * a set of matrices determined from singular value decomposition of A.
     * The function evaluates:
     *    [s] = [v] * [diag(1/w)] * [u]^T * [b]
     *
     * Base Matrix a(m,n) : Matrix to decompose and left const.
     * @param      b[m]   : RHS to be solved for.
     * @return     s[m]   : Solution vector.
     * @code s = a.svBackSubstitution( v, w, b); @endcode
     */
    aMatrix<T> svSolve( const aMatrix<T> &b) const
    {
      aMatrix<T> cacheMatrix1_( *this);
      aMatrix<T> cacheMatrix2_;
      valarray<T> w;
      cacheMatrix1_.svDecomposition( cacheMatrix2_, w);
      /*
       * This is a very simple test for rank deficient matrices, setting
       * the factors of the deficient basis functions to zero.  Some
       * better algorithm may be required in future.
       */
      T cutoff = w.max() * T(1.0e-5);
      valarray<bool> tCut = ( w - cutoff ) > T(0);
      w = w * T(0) + static_cast<valarray<double> >( w[ tCut ] );
      return cacheMatrix1_.svBackSubstitution( cacheMatrix2_, w, b);
    }

    /** Covariance of Singular Value Decomposition.
     * Modified Numerical Recipes 'svdvar' function.
     * This function evaluates the covariance matrix for the solution of
     * A.X = B achieved using singular value decomposition. This is a modified
     * version of the Numerical Recipes 'svdvar' function with data
     * encapsulated as vectors and matrices instead of arrays of floats.
     * Base Matrix v(n,n) : RH matrix of decomposition.
     * @param      w[n]   : Singular values.
     * @return covar(n,n) : The covariance matrix.
     * @code covar = v.svCovarience( w); @endcode
     */
    aMatrix<T> svCovariance( const valarray<T> &w)
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::svCovariance(): Matrix must be square.");
        }
        if ( w.size() != cols_) {
          throw std::invalid_argument( "aMatrix<T>::svCovariance(): Incorrect valarray size for w.");
        }
      );

      aMatrix<T> cacheMatrix1_;
      aMatrix<T> cacheMatrix2_;
      cacheMatrix1_.resize( rows_, cols_);
      cacheMatrix2_.resize( rows_, cols_);
      for ( size_t i = 0; i < rows_; i++) {
        if ( am_math::isNotZero( w[i])) {
          cacheMatrix1_(i) = (*this)(i) / w[i];
        }
        else {
          cacheMatrix1_(i) = T(0);
        }
      }
      for ( size_t j = 0; j < rows_; ++j) {
        for ( size_t k = j; k < rows_; ++k) {
          cacheMatrix2_(j,k) = dot( cacheMatrix1_[j], cacheMatrix1_[k]);
          if ( k != j) {
            cacheMatrix2_(k,j) = cacheMatrix2_(j,k);
          }
        }
      }
      return cacheMatrix2_;
    }

    /** Variance of Singular Value Decomposition.
     * Modified Numerical Recipes 'svdvar' function.
     * This function evaluates the covariance matrix for the solution of
     * A.X = B achieved using singular value decomposition. This is a modified
     * version of the Numerical Recipes 'svdvar' function with data
     * encapsulated as vectors and matrices instead of arrays of floats.
     * Base Matrix v(n,n) : RH matrix of decomposition.
     * @param      w[n]   : Singular values.
     * @return   var[n]   : The variance vector.
     * @code var = v.svVarience( w); @endcode
     */
    valarray<T> svVariance( const valarray<T> &w)
    {
      math_range_check(
        if ( isSquare() == false) {
          throw std::range_error( "aMatrix<T>::svVariance(): Matrix must be square.");
        }
        if ( w.size() != cols_) {
          throw std::invalid_argument( "aMatrix<T>::svVariance(): Incorrect valarray size for w.");
        }
      );

      aMatrix<T> cacheMatrix1_;
      cacheMatrix1_.resize( rows_, cols_);
      valarray<T> variance( rows_);
      for ( size_t i = 0; i < rows_; i++) {
        if ( am_math::isNotZero( w[i])) {
          cacheMatrix1_(i) = (*this)(i) / w[i];
        }
        else {
          cacheMatrix1_(i) = T(0);
        }
      }
      for ( size_t j = 0; j < rows_; j++) {
        variance[j] = dot( cacheMatrix1_[j], cacheMatrix1_[j]);
      }
      return variance;
    }

    size_t svRank() const
    {
      aMatrix<T> cacheMatrix1_( *this);
      aMatrix<T> cacheMatrix2_;
      valarray<T> w( cols_);
      cacheMatrix1_.svDecomposition( cacheMatrix2_, w);
      size_t count = 0;
      for ( size_t i = 0; i < cols_; ++i) {
        if ( am_math::isnan( w[i])) {
          return size_t(0);
        }
        if ( am_math::isNotZero( w[i])) {
          ++count;
        }
      }
      return count;
    }

    T svCondition() const
    {
      aMatrix<T> cacheMatrix1_( *this);
      aMatrix<T> cacheMatrix2_;
      valarray<T> w;
      cacheMatrix1_.svDecomposition( cacheMatrix2_, w);
      return w.max()/w.min();
    }

    //
    // The default methods.
    //
    valarray<T> solve( const valarray<T> &v) const { return gSolve( v);}
    T           determinant()                const { return gDeterminant();}
    size_t      rank()                       const { return svRank();}
    T           condition()                  const { return svCondition();}

    //
    // Utility functions.
    //
    void   assign( const aMatrix& m) { copy( m);}
    size_t cols() const       {return cols_;}
    size_t rows() const       {return rows_;}
    size_t size() const       {return size_;}
    size_t typesize() const   {return sizeof(T);}

    valarray<T>& matrixData() {return matrixData_;}
    const valarray<T>& matrixData() const {return matrixData_;}

    void resize( size_t nrows, size_t ncols, const T& defValue = T(0))
    {
      rows_ = nrows;
      cols_ = ncols;
      size_ = rows_ * cols_;
      if ( size_ > matrixData_.size()) {
        matrixData_.resize( size_, defValue);
      }
    }

    T min() const
    {
      if ( size_ == matrixData_.size()) {
        return matrixData_.min();
      }
      T result = matrixData_[0];
      for ( size_t i = 1; i < size_; ++i) {
        result = am_math::min( result, matrixData_[i]);
      }
      return result;
    }

    T max() const
    {
      if ( size_ == matrixData_.size()) {
        return matrixData_.max();
      }
      T result = matrixData_[0];
      for ( size_t i = 1; i < size_; ++i) {
        result = am_math::max( result, matrixData_[i]);
      }
      return result;
    }

    T sum() const
    {
      if ( size_ == matrixData_.size()) {
        return matrixData_.sum();
      }
      T result = T(0);
      for ( size_t i = 0; i < size_; ++i) {
        result += matrixData_[i];
      }
      return result;
    }

    T average() const         {return sum() / size_;}
    T trace( int i = 0) const {valarray<T> d = diag(i); return d.sum();}

    void unit()               {zero(); (*this).diag(0) = T(1);}
    void zero()               {(*this) = T(0);}
    void null()               {zero();}
    void rand( T dRandMin = T(-1), T dRandMax = T(1), int randSeed = 0)
    {
      T dR = dRandMax - dRandMin;
      if ( randSeed == 0) {
        randSeed = (unsigned) time(0);
      }
      srand( randSeed);
      for ( size_t i = 0; i < size_; ++i) {
        matrixData_[i] = ( ::rand() * dR / RAND_MAX) + dRandMin;
      }
    }

    T norm() const
    {
      T t(0);
      for ( size_t i = 0; i < size_; ++i) {
        t += matrixData_[i] * matrixData_[i];
      }
      return ::sqrt( t);
    }

    aMatrix<T> normalise() const
    {
      aMatrix<T> cacheMatrix0_( *this);

      T datamin = min();
      T deltaMaxMin = max() - datamin;

      if ( am_math::isNotZero( deltaMaxMin)) {
        cacheMatrix0_ -= min();
        cacheMatrix0_ /= deltaMaxMin;
      }
      return cacheMatrix0_;
    }

    aMatrix<T> apply( T(*fn)(T)) const
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_, cols_);
      for ( size_t i = 0; i < size_; ++i) {
        cacheMatrix0_.matrixData_[i] = fn( matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    aMatrix<T> apply( T(*fn)(const T&)) const
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_, cols_);
      for ( size_t i = 0; i < size_; ++i) {
        cacheMatrix0_.matrixData_[i] = fn( matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    aMatrix<T> apply( T(*fn)(size_t,size_t,T)) const
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_, cols_);
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < cols_; ++j) {
          cacheMatrix0_(i,j) = fn( i, j, (*this)(i,j));
        }
      }
      return cacheMatrix0_;
    }

    aMatrix<T> apply( T(*fn)(size_t,size_t,const T&)) const
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_, cols_);
      for ( size_t i = 0; i < rows_; ++i) {
        for ( size_t j = 0; j < cols_; ++j) {
          cacheMatrix0_(i,j) = fn( i, j, (*this)(i,j));
        }
      }
      return cacheMatrix0_;
    }

    void bound( const aMatrix<T> &mMin, const aMatrix<T> &mMax)
    {
      math_range_check(
        if ( ( cols_ != mMin.cols_) || ( rows_ != mMin.rows_)) {
          throw std::invalid_argument( "bound(): Incompatible minimum bound matrix.");
        }
        if ( ( cols_ != mMax.cols_) || ( rows_ != mMax.rows_)) {
          throw std::invalid_argument( "bound(): Incompatible maximum bound matrix.");
        }
      );
      for ( size_t i = 0; i < size(); ++i) {
        matrixData_[i] = am_math::max( matrixData_[i], am_math::min( mMin.matrixData_[i], mMax.matrixData_[i]));
        matrixData_[i] = am_math::min( matrixData_[i], am_math::max( mMin.matrixData_[i], mMax.matrixData_[i]));
      }
    }

    void eliminateRow( const size_t ii)
    {
      math_range_check(
        if ( rows_ == 1) {
          throw std::range_error( "aMatrix<T>::eliminateRow(i): Matrix must have more than one row.");
        }
      );
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_ - 1, cols_);
      size_t k = 0;
      for ( size_t i = 0; i < rows_; ++i) {
        if ( i != ii) {
          for ( size_t j = 0; j < cols_; ++j) {
            cacheMatrix0_(k,j) = (*this)(i,j);
          }
          ++k;
        }
      }
      *this = cacheMatrix0_;
    }

    void eliminateColumn( const size_t jj)
    {
      math_range_check(
        if ( cols_ == 1) {
          throw std::range_error( "aMatrix<T>::eliminateColumn(j): Matrix must have more than one column.");
        }
      );
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( rows_, cols_ - 1);
      size_t k = 0;
      for ( size_t j = 0; j < cols_; ++j) {
        if ( j != jj) {
          for ( size_t i = 0; i < rows_; ++i) {
            cacheMatrix0_(i,k) = (*this)(i,j);
          }
          ++k;
        }
      }
      *this = cacheMatrix0_;
    }

    void eulerTransformMatrix( const valarray<T> &v)
    {
      math_range_check(
        if ( v.size() != 3) {
          throw std::invalid_argument( "eulerTransformMatrix(valarray<T>): Array must have length 3.");
        }
      );
      eulerTransformMatrix( v[0], v[1], v[2]);
    }

    void eulerTransformMatrix( const T &psi, const T &theta, const T &phi)
    {
      T sin1_ = ::sin(phi);
      T sin2_ = ::sin(theta);
      T sin3_ = ::sin(psi);
      T cos1_ = ::cos(phi);
      T cos2_ = ::cos(theta);
      T cos3_ = ::cos(psi);

      (*this).resize(3,3);

      /* Transform matrix row #1 */
      (*this)(0,0) = (cos2_ * cos3_);
      (*this)(0,1) = (cos2_ * sin3_);
      (*this)(0,2) = (-1.0 * sin2_);

      /* Transform matrix row #2 */
      (*this)(1,0) = ((sin1_ * sin2_ * cos3_) - (cos1_ * sin3_));
      (*this)(1,1) = ((sin1_ * sin2_ * sin3_) + (cos1_ * cos3_));
      (*this)(1,2) = (sin1_ * cos2_);

      /* Transform matrix row #3 */
      (*this)(2,0) = ((cos1_ * sin2_ * cos3_) + (sin1_ * sin3_));
      (*this)(2,1) = ((cos1_ * sin2_ * sin3_) - (sin1_ * cos3_));
      (*this)(2,2) = (cos1_ * cos2_);
    }

    void eulerTransformMatrixPsi( const T &psi)
    {
      T sin3_ = ::sin(psi);
      T cos3_ = ::cos(psi);

      (*this).resize(3,3);

      /* Transform matrix row #1 */
      (*this)(0,0) = cos3_;
      (*this)(0,1) = sin3_;
      (*this)(0,2) = T(0);

      /* Transform matrix row #2 */
      (*this)(1,0) = -sin3_;
      (*this)(1,1) = cos3_;
      (*this)(1,2) = T(0);

      /* Transform matrix row #3 */
      (*this)(2,0) = T(0);
      (*this)(2,1) = T(0);
      (*this)(2,2) = T(1);
    }

    void eulerTransformMatrixTheta( const T &theta)
    {
      T sin2_ = ::sin(theta);
      T cos2_ = ::cos(theta);

      (*this).resize(3,3);

      /* Transform matrix row #1 */
      (*this)(0,0) = cos2_;
      (*this)(0,1) = T(0);
      (*this)(0,2) = -sin2_;

      /* Transform matrix row #2 */
      (*this)(1,0) = T(0);
      (*this)(1,1) = T(1);
      (*this)(1,2) = T(0);

      /* Transform matrix row #3 */
      (*this)(2,0) = sin2_;
      (*this)(2,1) = T(0);
      (*this)(2,2) = cos2_;
    }

    void eulerTransformMatrixPhi( const T &phi)
    {
      T sin1_ = ::sin(phi);
      T cos1_ = ::cos(phi);

      (*this).resize(3,3);

      /* Transform matrix row #1 */
      (*this)(0,0) = T(1);
      (*this)(0,1) = T(0);
      (*this)(0,2) = T(0);

      /* Transform matrix row #2 */
      (*this)(1,0) = T(0);
      (*this)(1,1) = cos1_;
      (*this)(1,2) = sin1_;

      /* Transform matrix row #3 */
      (*this)(2,0) = T(0);
      (*this)(2,1) = -sin1_;
      (*this)(2,2) = cos1_;
    }

    void eulerTransformMatrixPsiTheta( const T &psi, const T &theta)
    {
      T sin2_ = ::sin(theta);
      T sin3_ = ::sin(psi);
      T cos2_ = ::cos(theta);
      T cos3_ = ::cos(psi);

      (*this).resize(3,3);

      /* Transform matrix row #1 */
      (*this)(0,0) = (cos2_ * cos3_);
      (*this)(0,1) = (cos2_ * sin3_);
      (*this)(0,2) = -sin2_;

      /* Transform matrix row #2 */
      (*this)(1,0) = -sin3_;
      (*this)(1,1) = cos3_;
      (*this)(1,2) = T(0);

      /* Transform matrix row #3 */
      (*this)(2,0) = (sin2_ * cos3_);
      (*this)(2,1) = (sin2_ * sin3_);
      (*this)(2,2) = cos2_;
    }

    void eulerTransformMatrixPsiPhi( const T &psi, const T &phi)
    {
      T sin1_ = ::sin(phi);
      T sin3_ = ::sin(psi);
      T cos1_ = ::cos(phi);
      T cos3_ = ::cos(psi);

      (*this).resize(3,3);

      /* Transform matrix row #1 */
      (*this)(0,0) = cos3_;
      (*this)(0,1) = sin3_;
      (*this)(0,2) = T(0);

      /* Transform matrix row #2 */
      (*this)(1,0) = -(cos1_ * sin3_);
      (*this)(1,1) =  (cos1_ * cos3_);
      (*this)(1,2) =   sin1_;

      /* Transform matrix row #3 */
      (*this)(2,0) =  (sin1_ * sin3_);
      (*this)(2,1) = -(sin1_ * cos3_);
      (*this)(2,2) =   cos1_;
    }

    void eulerTransformMatrixThetaPhi( const T &theta, const T &phi)
    {
      T sin1_ = ::sin(phi);
      T sin2_ = ::sin(theta);
      T cos1_ = ::cos(phi);
      T cos2_ = ::cos(theta);

      (*this).resize(3,3);

      /* Transform matrix row #1 */
      (*this)(0,0) = cos2_;
      (*this)(0,1) = T(0);
      (*this)(0,2) = -sin2_;

      /* Transform matrix row #2 */
      (*this)(1,0) = (sin1_ * sin2_);
      (*this)(1,1) = cos1_;
      (*this)(1,2) = (sin1_ * cos2_);

      /* Transform matrix row #3 */
      (*this)(2,0) = (cos1_ * sin2_);
      (*this)(2,1) = -sin1_;
      (*this)(2,2) = (cos1_ * cos2_);
    }

    static aMatrix<T> getEulerTransformMatrix( const valarray<T> &v)
    {
      math_range_check(
        if ( v.size() != 3) {
          throw std::invalid_argument( "getEulerTransformMatrix(valarray<T>): Array must have length 3.");
        }
      );
      return getEulerTransformMatrix( v[0], v[1], v[2]);
    }

    static aMatrix<T> getEulerTransformMatrix( const T &psi, const T &theta, const T &phi)
    {
      aMatrix<T> cache;
      cache.eulerTransformMatrix( psi, theta, phi);
      return cache;
    }

    static aMatrix<T> getEulerTransformMatrixPsi( const T &psi)
    {
      aMatrix<T> cache;
      cache.eulerTransformMatrixPsi( psi);
      return cache;
    }

    static aMatrix<T> getEulerTransformMatrixTheta( const T &theta)
    {
      aMatrix<T> cache;
      cache.eulerTransformMatrixTheta( theta);
      return cache;
    }

    static aMatrix<T> getEulerTransformMatrixPhi( const T &phi)
    {
      aMatrix<T> cache;
      cache.eulerTransformMatrixPhi( phi);
      return cache;
    }

    static aMatrix<T> getEulerTransformMatrixPsiTheta( const T &psi, const T& theta)
    {
      aMatrix<T> cache;
      cache.eulerTransformMatrixPsiTheta( psi, theta);
      return cache;
    }

    static aMatrix<T> getEulerTransformMatrixPsiPhi( const T &psi, const T& phi)
    {
      aMatrix<T> cache;
      cache.eulerTransformMatrixPsiPhi( psi, phi);
      return cache;
    }

    static aMatrix<T> getEulerTransformMatrixThetaPhi( const T &theta, const T& phi)
    {
      aMatrix<T> cache;
      cache.eulerTransformMatrixThetaPhi( theta, phi);
      return cache;
    }

    void transformAngleAboutVector( const T& alpha, const valarray<T>& vec)
    {
      math_range_check(
        if ( vec.size() != 3) {
          throw std::invalid_argument( "transformAngleAboutVector(alpha, vec): vec must be size = 3.");
        }
      );

      const T cosa = ::cos( alpha);
      const T sina = ::sin( alpha);
      const T cosa1 = T( 1) - cosa;
      const valarray<T> v = vec / ::sqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);

      (*this).resize( 3, 3);

      (*this)( 0, 0) = cosa + cosa1 * v[0] * v[0];
      (*this)( 0, 1) = cosa1 * v[1] * v[0] + sina * v[2];
      (*this)( 0, 2) = cosa1 * v[2] * v[0] - sina * v[1];

      (*this)( 1, 0) = cosa1 * v[0] * v[1] - sina * v[2];
      (*this)( 1, 1) = cosa + cosa1 * v[1] * v[1];
      (*this)( 1, 2) = cosa1 * v[2] * v[1] + sina * v[0];

      (*this)( 2, 0) = cosa1 * v[0] * v[2] + sina * v[1];
      (*this)( 2, 1) = cosa1 * v[1] * v[2] - sina * v[0];
      (*this)( 2, 2) = cosa + cosa1 * v[2] * v[2];
    }

    static aMatrix<T> getTransformAngleAboutVector( const T& alpha, const valarray<T>& vec)
    {
      aMatrix<T> cache;
      cache.transformAngleAboutVector( alpha, vec);
      return cache;
    }

    //
    // The cross function with one vector argument returns the skew-symmetric
    // matrix associated with the cross product.
    //
    void cross( const valarray<T> &v1 )
    {
      math_range_check(
        if ( v1.size() != 3) {
          throw std::invalid_argument( "cross(v): Vector must be size = 3.");
        }
      );

      (*this).resize(3,3);

      (*this)(0,0) =  T(0);
      (*this)(0,1) = -v1[2];
      (*this)(0,2) =  v1[1];
      (*this)(1,0) =  v1[2];
      (*this)(1,1) =  T(0);
      (*this)(1,2) = -v1[0];
      (*this)(2,0) = -v1[1];
      (*this)(2,1) =  v1[0];
      (*this)(2,2) =  T(0);
    }

    static aMatrix<T> getCross( const valarray<T>& v1)
    {
      aMatrix<T> cache;
      cache.cross( v1);
      return cache;
    }

    //
    // Misc. functions.
    //
    friend aMatrix<T> abs( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray abs() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = am_math::abs( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> ceil( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray abs() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::ceil( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> floor( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray abs() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::floor( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> pow( const aMatrix<T> &m, size_t n)
    {
      math_range_check(
        if ( m.isSquare() == false) {
          throw std::range_error( "pow(aMatrix<T>): Matrix not square.");
        }
      );

      aMatrix<T> cacheMatrix1_;
      if ( n == 0) {
        cacheMatrix1_.resize( m.rows_, m.cols_);
        cacheMatrix1_.unit();
        return cacheMatrix1_;
      }

      aMatrix<T> cacheMatrix2_( m);
      bool firstTime = true;
      while( true) {
        if ( n & 1) {
          if ( firstTime) {
            cacheMatrix1_ = cacheMatrix2_;
            firstTime     = false;
          }
          else {
            cacheMatrix1_ *= cacheMatrix2_;
          }
        }
        n >>= 1;
        if ( n == 0) {
          break;
        }
        cacheMatrix2_ *= cacheMatrix2_;
      }
      return cacheMatrix1_;
    }

    friend aMatrix<T> acos( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray acos() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::acos( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> asin( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray asin() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::asin( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> atan( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray atan() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::atan( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> atan2( const aMatrix<T> &m1, const aMatrix<T> &m2)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m1.rows_, m1.cols_);
      // for loop faster than using valarray atan2() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::atan2( m1.matrixData_[i], m2.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> atan2( const aMatrix<T> &m, const T &v)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray atan2() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::atan2( m.matrixData_[i], v);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> atan2( const T &v, const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray atan2() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::atan2( m.matrixData_[i], v);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> cos( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray cos() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::cos( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> cosh( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray cosh() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::cosh( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> exp( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray exp() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::exp( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> log( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray log() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::log( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> log10( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray log10() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::log10( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> sin( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray sin() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::sin( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> sinh( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray sinh() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::sinh( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> sqrt( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray sqrt() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::sqrt( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> tan( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray tan() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::tan( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> tanh( const aMatrix<T> &m)
    {
      aMatrix<T> cacheMatrix0_;
      cacheMatrix0_.resize( m.rows_, m.cols_);
      // for loop faster than using valarray tanh() function.
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = ::tanh( m.matrixData_[i]);
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> concatenate( const aMatrix<T> &m1, const aMatrix<T> &m2 )
    {
      return concatenate( m1, m2, CC_HORIZONTAL );
    }

    friend aMatrix<T> concatenate( const aMatrix<T> &m1, const aMatrix<T> &m2,
                                     const enum Cc_Dir cc_dir )
    {
      math_range_check(
        if ( cc_dir == CC_HORIZONTAL && m1.rows_ != m2.rows_ ) {
          throw std::invalid_argument( "concatenate(m1,m2,CC_HORIZONTAL): Matrices must have same row size.");
        }
        else if ( cc_dir == CC_VERTICAL && m1.cols_ != m2.cols_ ) {
          throw std::invalid_argument( "concatenate(m1,m2,CC_VERTICAL): Matrices must have same column size.");
        }
      );
      aMatrix<T> cacheMatrix0_;
      if ( cc_dir == CC_HORIZONTAL ) {
        cacheMatrix0_.resize( m1.rows_, m1.cols_ + m2.cols_);
        for ( size_t i = 0; i < m1.rows_; ++i) {
          {
            for ( size_t j = 0; j < m1.cols_; ++j) {
              cacheMatrix0_(i,j) = m1(i,j);
            }
          }
          {
            for ( size_t j = 0; j < m2.cols_; ++j) {
              cacheMatrix0_(i,j+m1.cols_) = m2(i,j);
            }
          }
        }
      } else {
        cacheMatrix0_.resize( m1.rows_ + m2.rows_, m1.cols_);
        for ( size_t j = 0; j < m1.cols_; ++j) {
          {
            for ( size_t i = 0; i < m1.rows_; ++i) {
              cacheMatrix0_(i,j) = m1(i,j);
            }
          }
          {
            for ( size_t i = 0; i < m2.rows_; ++i) {
              cacheMatrix0_(i+m1.rows_,j) = m2(i,j);
            }
          }
        }
      }
      return cacheMatrix0_;
    }

    friend aMatrix<T> bound( const aMatrix<T> &m, const aMatrix<T> &mMin, const aMatrix<T> &mMax)
    {
      math_range_check(
        if ( ( m.cols_ != mMin.cols_) || ( m.rows_ != mMin.rows_)) {
          throw std::invalid_argument( "bound(): Incompatible minimum bound matrix.");
        }
        if ( ( m.cols_ != mMax.cols_) || ( m.rows_ != mMax.rows_)) {
          throw std::invalid_argument( "bound(): Incompatible maximum bound matrix.");
        }
      );
      aMatrix<T> cacheMatrix0_( m);
      for ( size_t i = 0; i < cacheMatrix0_.size(); ++i) {
        cacheMatrix0_.matrixData_[i] = am_math::max( cacheMatrix0_.matrixData_[i], am_math::min( mMin.matrixData_[i], mMax.matrixData_[i]));
        cacheMatrix0_.matrixData_[i] = am_math::min( cacheMatrix0_.matrixData_[i], am_math::max( mMin.matrixData_[i], mMax.matrixData_[i]));
      }
      return cacheMatrix0_;
    }

    //
    // External Friend functions. Actual code defined near bottom of this file.
    // This is required of friend functions that don't pass/return aMatrix<T> as an argument.
    //
    template <typename U>
    friend void swap( const aMatrix<U> &m1, const aMatrix<U> &m2);

    template <typename U>
    friend aMatrix<U> eulerTransformMatrix( const valarray<U> &v);

    template <typename U>
    friend aMatrix<U> eulerTransformMatrix( const U &psi, const U &theta, const U &phi);

    template <typename U>
    friend aMatrix<U> cross( const valarray<U> &v);

    //
    // IO Stream functions.
    //
    friend std::ostream& operator<< ( std::ostream &os, const aMatrix<T> &m)
    {
      if ( m.rows() == 0) {
        os << "Null Matrix";
      }
      else {
        os << m[0];
        for ( size_t i = 1; i < m.rows(); ++i) {
          os << '\n' << m[i];
        }
      }
      return os;
    }

    friend std::istream& operator>> ( std::istream &is, aMatrix<T> &m)
    {
      for ( size_t i = 0; i < m.size_; ++i) {
        is >> m.matrixData_[i];
      }
      return is;
    }

    //
    // Comma operator for assignment of a matrix from different data forms.
    //
    struct CommaAssignment {
      CommaAssignment( aMatrix<T> &m, const T &t)
      {
        math_range_check(
          if ( m.size_ == 0) {
            throw std::invalid_argument( "CommaAssignment: Matrix has not been sized.");
          }
        );
        m_    = &m;
        index = 0;
        m_->matrixData_[ index] = t;
      }

      CommaAssignment& operator, ( const T &t)
      {
        ++index;
        math_range_check(
          if ( index >= m_->size_) {
            throw std::out_of_range( "CommaAssignment::operator,(t): Assignment outside matrix range.");
          }
        );
        m_->matrixData_[ index] = t;
        return *this;
      }

      aMatrix<T> *m_;
      size_t index;
    };


    CommaAssignment operator<< ( const T &t)
    {
      return CommaAssignment( *this, t);
    }

  protected:
    size_t rows_;
    size_t cols_;
    size_t size_;
    valarray<T> matrixData_;
    mutable gDecompositionInfo gInfo_;

  private:
    void copy( const aMatrix<T> &m)
    {
      resize( m.rows_, m.cols_);
      for ( size_t i = 0; i < size_; ++i)
        matrixData_[i] = m.matrixData_[i];
    }

    size_t index( size_t i, size_t j) const
    {
      return ((i * cols_) + j);
    }

    friend class aSubVector<T>;
    friend class aSubMatrix<T>;
  };

  template <typename T> class aSubVector {
  public:
    aSubVector( aMatrix<T> &matrix, const slice &sl)
      : matrix_( matrix), slice_( sl) {}
    aSubVector( const aSubVector &subV)
      : matrix_( subV.matrix_), slice_( subV.slice_) {}

    size_t size() const          {return slice_.size();}

    // This converts SubVector into type valarray.
    operator valarray<T>() const {return getVector();}

    // This is for A[i][j] = n;
    T& operator[] ( size_t i) const
    {
      math_range_check(
        if ( i >= size()) {
          throw std::out_of_range( "aMatrix<T>& operator[i][j]: Column j out of range.");
        }
      );
      return matrix_.matrixData_[ slice_.start()+i*slice_.stride()];
    }

    //
    // Assignment operators.
    //
    // This is for A[i] = A[j], when A[j] can't be a valarray?
    aSubVector<T>& operator= ( const aSubVector<T> &subV)
    {
      math_range_check(
        if ( size() != subV.size()) {
          throw std::invalid_argument( "SubVector<T>& operator=: Vector slice not compatible.");
        }
      );
      matrix_.matrixData_[slice_] = subV.getVector();
      return *this;
    }

    // This is for A[i] = valarray.
    void operator= ( const valarray<T> &v) const
    {
      math_range_check(
        if ( size() != v.size()) {
          throw std::invalid_argument( "void operator=: valarray not compatible.");
        }
      );
      matrix_.matrixData_[slice_] = v;
    }

    // This is for A[i] = vector.
    void operator= ( const std::vector<T> &v) const
    {
      math_range_check(
        if ( size() != v.size()) {
          throw std::invalid_argument( "void operator=: vector not compatible.");
        }
      );
      matrix_.matrixData_[slice_] = toStdValArray( v);
    }

    void operator= ( const T &t) const
    {
      matrix_.matrixData_[slice_] = t;
    }

    //
    // Computed assignment operators.
    //
    void operator*= (const T &t) const
    {
      matrix_.matrixData_[slice_] = getVector() * t;
    }
    void operator/= (const T &t) const
    {
      matrix_.matrixData_[slice_] = getVector() / t;
    }
    void operator+= (const T &t) const
    {
      matrix_.matrixData_[slice_] = getVector() + t;
    }
    void operator-= (const T &t) const
    {
      matrix_.matrixData_[slice_] = getVector() - t;
    }
    void operator*= (const valarray<T> &v) const
    {
      math_range_check(
        if ( size() != v.size()) {
          throw std::invalid_argument( "void operator*=: valarray not compatible.");
        }
      );
      matrix_.matrixData_[slice_] *= v;
    }
    void operator/= (const valarray<T> &v) const
    {
      math_range_check(
        if ( size() != v.size()) {
          throw std::invalid_argument( "void operator/=: valarray not compatible.");
        }
      );
      matrix_.matrixData_[slice_] /= v;
    }
    void operator+= (const valarray<T> &v) const
    {
      math_range_check(
        if ( size() != v.size()) {
          throw std::invalid_argument( "void operator+=: valarray not compatible.");
        }
      );
      matrix_.matrixData_[slice_] += v;
    }
    void operator-= (const valarray<T> &v) const
    {
      math_range_check(
        if ( size() != v.size()) {
          throw std::invalid_argument( "void operator-=: valarray not compatible.");
        }
      );
      matrix_.matrixData_[slice_] -= v;
    }

    //
    // Binary Operators with type T.
    //
    friend valarray<T> operator* ( const aSubVector &subV, const T &t)
    {
      return subV.getVector() * t;
    }
    friend valarray<T> operator/ ( const aSubVector &subV, const T &t)
    {
      return subV.getVector() / t;
    }
    friend valarray<T> operator+ ( const aSubVector &subV, const T &t)
    {
      return subV.getVector() + t;
    }
    friend valarray<T> operator- ( const aSubVector &subV, const T &t)
    {
      return subV.getVector() - t;
    }
    friend valarray<T> operator* ( const T &t, const aSubVector &subV)
    {
      return subV.getVector() * t;
    }
    friend valarray<T> operator/ ( const T &t, const aSubVector &subV)
    {
      return subV.getVector() / t;
    }
    friend valarray<T> operator+ ( const T &t, const aSubVector &subV)
    {
      return subV.getVector() + t;
    }
    friend valarray<T> operator- ( const T &t, const aSubVector &subV)
    {
      return subV.getVector() - t;
    }

    //
    // Binary Operators with type valarray<T>.
    //
    friend valarray<T> operator* ( const aSubVector &subV, const valarray<T> &v)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator*: valarray not compatible.");
        }
      );
      return subV.getVector() * v;
    }
    friend valarray<T> operator/ ( const aSubVector &subV, const valarray<T> &v)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator/: valarray not compatible.");
        }
      );
      return subV.getVector() / v;
    }
    friend valarray<T> operator+ ( const aSubVector &subV, const valarray<T> &v)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator+: valarray not compatible.");
        }
      );
      return subV.getVector() + v;
    }
    friend valarray<T> operator- ( const aSubVector &subV, const valarray<T> &v)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator-: valarray not compatible.");
        }
      );
      return subV.getVector() - v;
    }
    friend valarray<T> operator* ( const valarray<T> &v, const aSubVector &subV)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator*: valarray not compatible.");
        }
      );
      return v * subV.getVector();
    }
    friend valarray<T> operator/ ( const valarray<T> &v, const aSubVector &subV)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator/: valarray not compatible.");
        }
      );
      return v / subV.getVector();
    }
    friend valarray<T> operator+ ( const valarray<T> &v, const aSubVector &subV)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator+: valarray not compatible.");
        }
      );
      return v + subV.getVector();
    }
    friend valarray<T> operator- ( const valarray<T> &v, const aSubVector &subV)
    {
      math_range_check(
        if ( subV.size() != v.size()) {
          throw std::invalid_argument( "valarray<T> operator-: valarray not compatible.");
        }
      );
      return v - subV.getVector();
    }

    //
    // Misc. functions.
    //
    bool isZero() const
    {
      for ( size_t i = 0; i < size(); ++i) {
        if ( am_math::isNotZero( (*this)[i])) {
          return false;
        }
      }
      return true;
    }

    valarray<T> normalise() const
    {
      valarray<T> vTemp(*this);

      T datamin = getVector().min();
      T deltaMaxMin = getVector().max() - datamin;

      if ( am_math::isNotZero( deltaMaxMin)) {
        vTemp -= datamin;
        vTemp /= deltaMaxMin;
      }
      return vTemp;
    }


    friend void swap( const aSubVector<T> &subV1, const aSubVector<T> &subV2)
    {
      valarray<T> vTemp( subV1);
      subV1 = subV2;
      subV2 = vTemp;
    }

    //
    // IO Stream functions.
    //
    friend std::ostream& operator<< ( std::ostream &os, const aSubVector<T> &subV)
    {
      os << subV.getVector() << '\n';
      return os;
    }

    friend std::istream& operator<< ( std::istream &is, aSubVector<T> &subV)
    {
      for ( size_t i = 0; i < subV.size(); ++i) {
        is >> subV[i];
      }
      return is;
    }

  private:
    valarray<T> getVector() const {return matrix_.matrixData_[ slice_];}
    aMatrix<T> &matrix_;
    slice slice_;
  };

  template <typename T> class aSubMatrix {
  public:
    aSubMatrix( aMatrix<T> &matrix, const mslice &ms)
      : matrix_( matrix), mslice_( ms) {}
    aSubMatrix( const aSubMatrix &subM)
      : matrix_( subM.matrix_), mslice_( subM.mslice_) {}

    size_t rows() const {return mslice_.rows();}
    size_t cols() const {return mslice_.cols();}

    aMatrix<T> getMatrix() const
    {
      aMatrix<T> cacheSubMatrix0_;
      cacheSubMatrix0_.resize( rows(), cols());
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          cacheSubMatrix0_(i,j) = (*this)(i,j);
        }
      }
      return cacheSubMatrix0_;
    }

    // This converts SubMatrix into type aMatrix.
    operator aMatrix<T>() const {return getMatrix();}

    T& operator() ( size_t i, size_t j)
    {
      math_range_check(
        if ( i >= rows()) {
          throw std::out_of_range( "SubMatrix<T>::operator(i,j): Row i out of range.");
        }
        if ( j >= cols()) {
          throw std::out_of_range( "SubMatrix<T>::operator(i,j): Column j out of range.");
        }
      );
      return matrix_( mslice_.startRow() + i, mslice_.startCol() + j);
    }

    const T& operator() ( size_t i, size_t j) const
    {
      math_range_check(
        if ( i >= rows()) {
          throw std::out_of_range( "SubMatrix<T>::operator(i,j): Row i out of range.");
        }
        if ( j >= cols()) {
          throw std::out_of_range( "SubMatrix<T>::operator(i,j): Column j out of range.");
        }
      );
      return matrix_( mslice_.startRow() + i, mslice_.startCol() + j);
    }

    //
    // Assignment operators.
    //
    void operator= ( const aMatrix<T> &m)
    {
      math_range_check(
        if ((rows() != m.rows_) || (cols() != m.cols_)) {
          throw std::invalid_argument( "SubMatrix::operator=: Sub matrix and matrix are not the same size.");
        }
      );
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) = m(i,j);
        }
      }
    }

    void operator= ( const T &t)
    {
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) = t;
        }
      }
    }

    //
    // Computed assignment operators.
    //
    void operator*= ( const aMatrix<T> &m)
    {
      math_range_check(
        if ((rows() != m.rows_) || (cols() != m.cols_)) {
          throw std::invalid_argument( "SubMatrix::operator*=: Sub matrix and matrix are not the same size.");
        }
      );
      aMatrix<T> cacheSubMatrix0_;
      cacheSubMatrix0_.resize( rows(), cols());
      T result;
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          result = T(0);
          for ( size_t k = 0; k < rows(); ++k) {
            result += (*this)(i,k) * m(k,j);
          }
          cacheSubMatrix0_(i,j) = result;
        }
      }
      *this = cacheSubMatrix0_;
    }

    void operator*= ( const T &t)
    {
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) *= t;
        }
      }
    }

    void operator/= ( const T &t)
    {
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) /= t;
        }
      }
    }

    void operator+= ( const aMatrix<T> &m)
    {
      math_range_check(
        if ((rows() != m.rows_) || (cols() != m.cols_)) {
          throw std::invalid_argument( "SubMatrix::operator+=: Sub matrix and matrix are not the same size.");
        }
      );
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) += m(i,j);
        }
      }
    }

    void operator+= ( const T &t)
    {
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) += t;
        }
      }
    }

    void operator-= ( const aMatrix<T> &m)
    {
      math_range_check(
        if ((rows() != m.rows_) || (cols() != m.cols_)) {
          throw std::invalid_argument( "SubMatrix::operator-=: Sub matrix and matrix are not the same size.");
        }
      );
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) -= m(i,j);
        }
      }
    }

    void operator-= ( const T &t)
    {
      for ( size_t i = 0; i < rows(); ++i) {
        for ( size_t j = 0; j < cols(); ++j) {
          (*this)(i,j) -= t;
        }
      }
    }

    //
    // IO Stream functions.
    //
    friend std::ostream& operator<< ( std::ostream &os, const aSubMatrix<T> &subM)
    {
      os << subM.getMatrix();
      return os;
    }

    friend std::istream& operator>> ( std::istream &is, aSubMatrix<T> &subM)
    {
      for ( size_t i = 0; i < subM.rows(); ++i) {
        for ( size_t j = 0; j < subM.cols(); ++j) {
          is >> subM(i,j);
        }
      }
      return is;
    }

  private:
    aMatrix<T> &matrix_;
    mslice mslice_;
  };

  //
  // External Friend Functions for aMatrix that pass/return variables other than aMatrix<T>.
  //
  template <typename T> inline void swap( const aMatrix<T> &m1, const aMatrix<T> &m2)
  {
    aMatrix<T> cacheMatrix0_( m1);
    m1 = m2;
    m2 = cacheMatrix0_;
  }

  template <typename T> inline aMatrix<T> cross( const valarray<T> &v)
  {
    aMatrix<T> cacheMatrix0_;
    cacheMatrix0_.cross( v);
    return cacheMatrix0_;
  }

  template <typename T> inline aMatrix<T> eulerTransformMatrix( const valarray<T> &v)
  {
    aMatrix<T> cacheMatrix0_;
    cacheMatrix0_.eulerTransformMatrix( v);
    return cacheMatrix0_;
  }

  template <typename T> inline aMatrix<T> eulerTransformMatrix( const T &psi, const T &theta, const T &phi)
  {
    aMatrix<T> cacheMatrix0_;
    cacheMatrix0_.eulerTransformMatrix( psi, theta, phi);
    return cacheMatrix0_;
  }

  template <typename T> inline size_t findAbsMin( const aSubVector<T> &v, size_t start = 0);
  template <typename T> inline size_t findAbsMin( const aSubVector<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findAbsMin(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = abs( v[ index]);
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( abs( v[i]) < dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  template <typename T> inline size_t findAbsMax( const aSubVector<T> &v, size_t start = 0);
  template <typename T> inline size_t findAbsMax( const aSubVector<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findAbsMax(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = am_math::abs( v[ index]);
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( am_math::abs( v[i]) > dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  template <typename T> inline size_t findMin( const aSubVector<T> &v, size_t start = 0);
  template <typename T> inline size_t findMin( const aSubVector<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findMin(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = v[ index];
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( v[i] < dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  template <typename T> inline size_t findMax( const aSubVector<T> &v, size_t start = 0);
  template <typename T> inline size_t findMax( const aSubVector<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findMax(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = v[ index];
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( v[i] > dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  //
  // The following dot product functions are duplicated for speed increase.
  //
  template <typename T> inline T dot( const valarray<T> &v1, const aSubVector<T> &v2)
  {
    math_range_check(
      if ( v1.size() != v2.size()) {
        throw std::invalid_argument( "dot(v1,v2): Vectors must be same length.");
      }
    );
    T dp = T(0);
    for ( size_t i = 0; i < v1.size(); i++) {
      dp += v1[i] * v2[i];
    }
    return dp;
  }

  template <typename T> inline T dot( const aSubVector<T> &v1, const valarray<T> &v2)
  {
    math_range_check(
      if ( v1.size() != v2.size()) {
        throw std::invalid_argument( "dot(v1,v2): Vectors must be same length.");
      }
    );
    T dp = T(0);
    for ( size_t i = 0; i < v1.size(); i++) {
      dp += v1[i] * v2[i];
    }
    return dp;
  }

  template <typename T> inline T dot( const aSubVector<T> &v1, const aSubVector<T> &v2)
  {
    math_range_check(
      if ( v1.size() != v2.size()) {
        throw std::invalid_argument( "dot(v1,v2): Vectors must be same length.");
      }
    );
    T dp = T(0);
    for ( size_t i = 0; i < v1.size(); i++) {
      dp += v1[i] * v2[i];
    }
    return dp;
  }

  //
  // The following cross product functions are duplicated for speed increase.
  //
  template <typename T> inline valarray<T> cross( const valarray<T> &v1, const aSubVector<T> &v2)
  {
    math_range_check(
      if ( (v1.size() != 3) || (v2.size() !=3)) {
        throw std::invalid_argument( "cross(v1,v2): Vectors must be size = 3.");
      }
    );
    valarray<T> cp(3);
    cp[0] = ( v1[1] * v2[2]) - ( v1[2] * v2[1]);
    cp[1] = ( v1[2] * v2[0]) - ( v1[0] * v2[2]);
    cp[2] = ( v1[0] * v2[1]) - ( v1[1] * v2[0]);
    return cp;
  }

  template <typename T> inline valarray<T> cross( const aSubVector<T> &v1, const valarray<T> &v2)
  {
    math_range_check(
      if ( (v1.size() != 3) || (v2.size() !=3)) {
        throw std::invalid_argument( "cross(v1,v2): Vectors must be size = 3.");
      }
    );
    valarray<T> cp(3);
    cp[0] = ( v1[1] * v2[2]) - ( v1[2] * v2[1]);
    cp[1] = ( v1[2] * v2[0]) - ( v1[0] * v2[2]);
    cp[2] = ( v1[0] * v2[1]) - ( v1[1] * v2[0]);
    return cp;
  }

  template <typename T> inline valarray<T> cross( const aSubVector<T> &v1, const aSubVector<T> &v2)
  {
    math_range_check(
      if ( (v1.size() != 3) || (v2.size() !=3)) {
        throw std::invalid_argument( "cross(v1,v2): Vectors must be size = 3.");
      }
    );
    valarray<T> cp(3);
    cp[0] = ( v1[1] * v2[2]) - ( v1[2] * v2[1]);
    cp[1] = ( v1[2] * v2[0]) - ( v1[0] * v2[2]);
    cp[2] = ( v1[0] * v2[1]) - ( v1[1] * v2[0]);
    return cp;
  }

  //
  // The following outer product functions are duplicated for speed increase.
  //
  template <typename T> inline aMatrix<T> outerProduct( const valarray<T> &v1, const valarray<T> &v2)
  {
    aMatrix<T> op( v1.size(), v2.size());
    for ( size_t i = 0; i < v1.size(); ++i) {
      for ( size_t j = 0; j < v2.size(); ++j) {
        op(i,j) = v1[i] * v2[j];
      }
    }
    return op;
  }

  template <typename T> inline aMatrix<T> outerProduct( const valarray<T> &v1, const aSubVector<T> &v2)
  {
    aMatrix<T> op( v1.size(), v2.size());
    for ( size_t i = 0; i < v1.size(); ++i) {
      for ( size_t j = 0; j < v2.size(); ++j) {
        op(i,j) = v1[i] * v2[j];
      }
    }
    return op;
  }

  template <typename T> inline aMatrix<T> outerProduct( const aSubVector<T> &v1, const valarray<T> &v2)
  {
    aMatrix<T> op( v1.size(), v2.size());
    for ( size_t i = 0; i < v1.size(); ++i) {
      for ( size_t j = 0; j < v2.size(); ++j) {
        op(i,j) = v1[i] * v2[j];
      }
    }
    return op;
  }

  template <typename T> inline aMatrix<T> outerProduct( const aSubVector<T> &v1, const aSubVector<T> &v2)
  {
    aMatrix<T> op( v1.size(), v2.size());
    for ( size_t i = 0; i < v1.size(); ++i) {
      for ( size_t j = 0; j < v2.size(); ++j) {
        op(i,j) = v1[i] * v2[j];
      }
    }
    return op;
  }

  //
  // The following norm functions are duplicated for speed increase.
  //
  template <typename T> inline T norm( const aSubVector<T> &v1)
  {
    T t(0);
    for ( size_t i = 0; i < v1.size(); ++i) {
      t += v1[i] * v1[i];
    }
    return ::sqrt( t);
  }

  template <typename T> inline bool isZero( const aSubVector<T> &v1)
  {
    for ( size_t i = 0; i < v1.size(); ++i) {
      if ( am_math::isNotZero( v1[i])) {
        return false;
      }
    }
    return true;
  }

#ifndef __valarray_functions_H
#define __valarray_functions_H
  //
  // Functions for valarray / vector
  //

  template <typename T> inline void assign( valarray<T> &v1, const valarray<T> &v2)
  {
    if ( v1.size() != v2.size()) {
      v1.resize( v2.size());
    }
    v1 = v2;
  }

  template <typename T> inline void assign( std::vector<T> &v1, const valarray<T> &v2)
  {
    if ( v1.size() != v2.size()) {
      v1.resize( v2.size());
    }
    for ( size_t i = 0; i < v1.size(); ++i) {
      v1[i] = v2[i];
    }
  }

  template <typename T> inline void assign( valarray<T> &v1, const std::vector<T> &v2)
  {
    if ( v1.size() != v2.size()) {
      v1.resize( v2.size());
    }
    for ( size_t i = 0; i < v1.size(); ++i) {
      v1[i] = v2[i];
    }
  }

  template <typename T> inline size_t findAbsMin( const valarray<T> &v, size_t start = 0);
  template <typename T> inline size_t findAbsMin( const valarray<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findAbsMin(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = am_math::abs( v[ index]);
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( am_math::abs( v[i]) < dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  template <typename T> inline size_t findAbsMax( const valarray<T> &v, size_t start = 0);
  template <typename T> inline size_t findAbsMax( const valarray<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findAbsMax(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = am_math::abs( v[ index]);
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( am_math::abs( v[i]) > dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  template <typename T> inline size_t findMin( const valarray<T> &v, size_t start = 0);
  template <typename T> inline size_t findMin( const valarray<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findMin(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = v[ index];
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( v[i] < dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  template <typename T> inline size_t findMax( const valarray<T> &v, size_t start = 0);
  template <typename T> inline size_t findMax( const valarray<T> &v, size_t start)
  {
    math_range_check(
      if ( v.size() == 0) {
        throw std::invalid_argument( "size_t findMax(): Vector is empty.");
      }
    );
    size_t index = start;
    T dMin = v[ index];
    for ( size_t i = start + 1; i < v.size(); ++i) {
      if ( v[i] > dMin) {
        dMin  = v[i];
        index = i;
      }
    }
    return index;
  }

  //
  // The following dot product functions are duplicated for speed increase.
  //
  template <typename T> inline T dot( const valarray<T> &v1, const valarray<T> &v2)
  {
    math_range_check(
      if ( v1.size() != v2.size()) {
        throw std::invalid_argument( "dot(v1,v2): Vectors must be same length.");
      }
    );
    T dp = T(0);
    for ( size_t i = 0; i < v1.size(); i++) {
      dp += v1[i] * v2[i];
    }
    return dp;
  }

  //
  // The following cross product functions are duplicated for speed increase.
  //
  template <typename T> inline valarray<T> cross( const valarray<T> &v1, const valarray<T> &v2)
  {
    math_range_check(
      if ( (v1.size() != 3) || (v2.size() !=3)) {
        throw std::invalid_argument( "cross(v1,v2): Vectors must be size = 3.");
      }
    );
    valarray<T> cp(3);
    cp[0] = ( v1[1] * v2[2]) - ( v1[2] * v2[1]);
    cp[1] = ( v1[2] * v2[0]) - ( v1[0] * v2[2]);
    cp[2] = ( v1[0] * v2[1]) - ( v1[1] * v2[0]);
    return cp;
  }

  //
  // The following norm functions are duplicated for speed increase.
  //
  template <typename T> inline T norm( const valarray<T> &v1)
  {
    T t(0);
    for ( size_t i = 0; i < v1.size(); ++i) {
      t += v1[i] * v1[i];
    }
    return ::sqrt( t);
  }

  template <typename T> inline bool isZero( const valarray<T> &v1)
  {
    for ( size_t i = 0; i < v1.size(); ++i) {
      if ( am_math::isNotZero( v1[i])) {
        return false;
      }
    }
    return true;
  }

  template <typename T> valarray<T> bound( valarray<T> v, const valarray<T> &vMin, const valarray<T> &vMax)
  {
    math_range_check(
      if ( v.size() != vMin.size()) {
        throw std::invalid_argument( "bound(): Incompatible minimum bound valarray.");
      }
      if ( v.size() != vMax.size()) {
        throw std::invalid_argument( "bound(): Incompatible maximum bound valarray.");
      }
    );
    for ( size_t i = 0; i < v.size(); ++i) {
      v[i] = am_math::max( v[i], am_math::min( vMin[i], vMax[i]));
      v[i] = am_math::min( v[i], am_math::max( vMin[i], vMax[i]));
    }
    return v;
  }

  template <typename T> std::vector<T> bound( std::vector<T> v, const std::vector<T> &vMin, const std::vector<T> &vMax)
  {
    math_range_check(
      if ( v.size() != vMin.size()) {
        throw std::invalid_argument( "bound(): Incompatible minimum bound vector.");
      }
      if ( v.size() != vMax.size()) {
        throw std::invalid_argument( "bound(): Incompatible maximum bound vector.");
      }
    );
    for ( size_t i = 0; i < v.size(); ++i) {
      v[i] = am_math::max( v[i], am_math::min( vMin[i], vMax[i]));
      v[i] = am_math::min( v[i], am_math::max( vMin[i], vMax[i]));
    }
    return v;
  }

  //
  // IO Stream functions for valarray/vector
  //
  template <typename T> std::ostream& operator<< ( std::ostream &os, const valarray<T> &v)
  {
    if ( v.size() == 0) {
      os << "Null Vector";
    }
    else {
      os.width(11);
      if ( am_math::isZero( v[0])) {
        os << 0;
      }
      else {
        os << v[0];
      }
      for ( size_t i=1; i < v.size(); ++i) {
        os << ' ';
        os.width(11);
        if ( am_math::isZero( v[i])) {
          os << 0;
        }
        else {
          os << v[i];
        }
      }
    }
    return os;
  }

  template <typename T> std::istream& operator>> ( std::istream &is, valarray<T> &v)
  {
    for ( size_t i = 0; i < v.size(); ++i) {
      is >> v[i];
    }
    return is;
  }

  template <typename T> std::ostream& operator<< ( std::ostream &os, const std::vector<T> &v)
  {
    if ( v.size() == 0) {
      os << "Null Vector";
    }
    else {
      os.width(11);
      if ( am_math::isZero( v[0])) {
        os << 0;
      }
      else {
        os << v[0];
      }
      for ( size_t i=1; i < v.size(); ++i) {
        os << ' ';
        os.width(11);
        if ( am_math::isZero( v[i])) {
          os << 0;
        }
        else {
          os << v[i];
        }
      }
    }
    return os;
  }

  template <typename T> std::istream& operator>> ( std::istream &is, std::vector<T> &v)
  {
    for ( size_t i = 0; i < v.size(); ++i) {
      is >> v[i];
    }
    return is;
  }

#endif // __valarray_functions_H

  // Common typedefs for aMatrix.
  typedef valarray<double>             DVector;
  typedef valarray<int>                IVector;
  typedef valarray<bool>               BVector;
  typedef valarray< valarray<double> > DVVector;

  typedef aMatrix<double>              DMatrix;
  typedef aMatrix<int>                 IMatrix;
  typedef aMatrix<bool>                BMatrix;
  typedef aMatrix< aMatrix<double> >   DMMatrix;
  typedef valarray< aMatrix<double> >  DMVector;
  typedef aMatrix< valarray<double> >  DVMatrix;

} // End namespace DstoMath

#endif // End AMATRIX_H_
