#ifndef AFILEBUFFER_H_
#define AFILEBUFFER_H_

#include "UteConfig.h"

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
// Title: aFileBuffer Template Class
// Class: aFileBuffer
//
// Description:
//   Holds the contents of a file into a buffer of any type.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 11/08/2014
//

// C/C++ Includes
#include <cstdio>
#include <stack>

// Local Includes
#include "aString.h"

namespace dstoute {

  template <typename T> class aFileBuffer {
   public:
    aFileBuffer()
     : data_( 0), dataPtr_( 0), dataEnd_( 0), dataSize_( 0) {}

    aFileBuffer( const dstoute::aString &fileName)
     : data_( 0), dataPtr_( 0), dataEnd_( 0), dataSize_( 0) { open( fileName);}

    ~aFileBuffer()
    {
      delete [] data_;
      data_     = 0;
      dataSize_ = 0;
    }

    bool open( const dstoute::aString &fileName)
    {
      close();

      // Open the file
      FILE *fd = 0;
      if ( !( fd = fopen( fileName.c_str(), "rb"))) {
        return false;
      }

      // Determine size of the file
      fseek( fd, 0, SEEK_END);
      dataSize_ = ftell( fd) / sizeof( T);
      fseek( fd, 0, SEEK_SET);

      // Allocate space for the file
      data_    = new T [ dataSize_];
      dataPtr_ = data_;
      dataEnd_ = data_ + dataSize_;

      // Read the file into the buffer
      if ( fread( data_, sizeof( T), dataSize_, fd) != dataSize_) {
        fclose( fd);
        close();
        return false;
      }

      return true;
    }

    void close()
    {
      delete [] data_;
      data_     = 0;
      dataPtr_  = 0;
      dataEnd_  = 0;
      dataSize_ = 0;
    }

    void rewind()                        { dataPtr_ = data_;}

    size_t size() const { return dataSize_;}
    const T* begin() const               { return data_;}
    const T* pos() const                 { return dataPtr_;}
    const T* end() const                 { return dataEnd_;}
    const T* operator++()                { return ++dataPtr_;}
    const T* operator+=( size_t delta)   { dataPtr_ += delta; return dataPtr_;}
    const T* operator-=( size_t delta)   { dataPtr_ -= delta; return dataPtr_;}

    void setPos( T* pos)                 { if ( pos >= data_ && pos < dataEnd_) dataPtr_ = pos;}
    void push()                          { posStack.push( dataPtr_);}
    void pop()                           { if ( posStack.size()) { dataPtr_ = posStack.top(); posStack.pop();}}

    // @TODO: Do we need these?
    const T* operator[]( size_t i) const { return data_ + i;}
    const T* at( size_t i) const         { return data_ + i;}

    const T  pos_value() const           { return *dataPtr_;}

   private:
    T *data_;
    T *dataPtr_;
    T *dataEnd_;
    std::stack< T* > posStack;

    size_t dataSize_;
  };

} // namespace dstoute

#endif // AFILEBUFFER_H_
