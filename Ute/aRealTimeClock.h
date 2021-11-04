#ifndef _AREALTIMECLOCK_H_
#define _AREALTIMECLOCK_H_

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
// Title:      aReal-Time Clock Utilities
// Class       aRealTimeClock
// Module:     aRealTimeClock.h
//
// Written by: Shane D. Hill (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 24/11/2005
//

// C Includes
#include <ctime>

// System Specific Includes
#ifdef __unix
  #include <unistd.h>
#endif
#if defined(WIN32) || defined(_WIN32) ||defined(__WIN32__)
  //
  // Must define NOMINMAX to prevent WINDEF.H from defining the min()
  // and max() macros. This prevents Bill form causing us problems
  // with the dstomath namespace.
  //
  #undef  NOMINMAX
  #define NOMINMAX
  #ifndef WIN32_LEAN_AND_MEAN
  #  define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
#endif

namespace dstoute {

  class aRealTimeClock {
   public:
    aRealTimeClock() : tStart_(0), tEnd_(0) {}

    /** Start the real-time clock timer.
     */
    void start() {tStart_ = clock();}

    /** Stop the real-time clock timer.
     */
    void stop()  {tEnd_   = clock();}

    /** Return the duration of the last start/stop cycle.
     */
    double getDuration() const {return double( tEnd_ - tStart_) / CLOCKS_PER_SEC;}

    /** Return the ratio of real-time given a cycle time.
     * @param time is the user time for the cycle.
     */
    double getRealTimeRatio( const double &time) const {return time / getDuration();}

    /** Sleep for a set number of milliseconds.
     * @param ms is an integer time in milliseconds.
     */
    #ifdef __unix
    void sleep( int ms) const
    {
      int sec = ms / 1000;
      int msecleft = (ms % 1000);
      if ( sec > 0) {
        sleep (sec);
      }
      if ( msecleft > 0) {
        usleep ( msecleft * 1000);
      }
    }
    #else
    void sleep( int ms) const { Sleep( ms);}
    #endif

    /** Sleep for a set number of seconds.
     * @param time is a double in seconds.
     */
    void sleep( const double &time) const { if ( time > 0.0) sleep( int( time * 1000.0));}

   private:
    clock_t tStart_;
    clock_t tEnd_;
  };

} // end namespace dstomath

#endif /*_AREALTIMECLOCK_H_*/
