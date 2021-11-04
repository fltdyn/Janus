#ifndef __AMUTEX_H__
#define __AMUTEX_H__

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
// Title:      aMutex - An very simple mutex class.
// Class:      aMutex
// Module:     aMutex.h
//
// Description:
// This class is a very simple mutex system for systems that don't support
// the C++11 mutex extension.
//
// Written by: Shane Hill (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 30/09/2015
//

//
// Use C++11 extension if available.
//
#if __cplusplus > 199711L || _MSC_VER >= 1700

  #include <mutex>
  namespace dstoute {
    class aMutex : public std::mutex {
     public:
      aMutex() : std::mutex() {}
    };
  }

//
// Use simple mutex system instead.
//
#else

  #include "aSleep.h"

  namespace dstoute {

    class aMutex {
     public:
      aMutex()
        : locked_( false), waitfor_( 3) {};
      void setWait( unsigned int waitfor) { waitfor_ = waitfor;}

      void lock()
      {
        while ( locked_) {
          sleep( waitfor_);
        }
        locked_ = true;
      }

      void unlock()
      {
        locked_ = false;
      }

      static void sleep( int ms) { aSleep( ms);}

     private:
      bool locked_;
      unsigned int waitfor_;
    };

  } // end namespace dstoute

  #endif // End __AMUTEX_H__

#endif // __cpluplus > 199711L
