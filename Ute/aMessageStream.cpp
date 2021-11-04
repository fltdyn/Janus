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
// Title: aMessageStream
// Class: MessageStream, InfoStream, DebugStream, StatusStream, WarningStream, ErrorStream
//
// Description:
//   Streams to handle passing of messages to programs and GUIs.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 29/10/2002
// Based On: ErrorClass from Amiel.
//

// C++ Includes
#include <cstdlib>

// Local Includes
#include "aMessageStream.h"

using namespace std;

namespace dstoute {

  // MessageStream
  aString MessageStream::applicationName_;
  vector<MessageStream::MessageCallback*> MessageStream::messageCallback_( MessageStream::MSG_ALL_TYPES, 0);
  aStringList MessageStream::messageTypeName_ = ( aStringList()
    << "Info"
    << "Status"
    << "Warning"
    << "Error"
    << "Debug"
    << "Unknown"
  );
  enum MessageType {
    MSG_INFORMATION,
    MSG_STATUS,
    MSG_WARNING,
    MSG_ERROR,
    MSG_DEBUG,
    MSG_ALL_TYPES // Keep this last.
  };

  MessageStream::MessageStream( MessageType typeArg, unsigned int tagArg)
    : ostringstream(),
      type_( typeArg),
      tag_( tagArg),
      isVerbose_( false),
      isThrow_( false)
  {}

  MessageStream::MessageStream( const aString& strArg, MessageType typeArg, unsigned int tagArg)
    : ostringstream( strArg),
      type_( typeArg),
      tag_( tagArg),
      isVerbose_( false),
      isThrow_( false)
  {}

  MessageStream::~MessageStream()
  {
    if ( !isThrow_ && str().size()) {
      show();
    }
  }

  MessageStream::MessageCallback* MessageStream::callback( MessageType type)
  {
    return messageCallback_[ type];
  }

  void MessageStream::setCallback( MessageCallback *callback, MessageType type)
  {
    if ( type == MSG_ALL_TYPES) {
     for ( size_t i = 0; i < messageCallback_.size(); ++i) {
       messageCallback_[ i] = callback;
     }
    }
    else {
      messageCallback_[ type] = callback;
    }
  }

  void MessageStream::setType( MessageType typeArg)
  {
    if ( typeArg != type_) {
      type_ = typeArg;
      // Flush previous message if one exists.
      if ( !str().empty()) {
        show();
      }
    }
  }

  aString MessageStream::verboseStr() const
  {
    aString fmtStr;
    if ( !applicationName_.empty()) {
      fmtStr += applicationName_;
      fmtStr += " ";
    }
    fmtStr += messageTypeName_[ type_];
    fmtStr += ": ";
    if ( !functionName_.empty()) {
      fmtStr += functionName_;
      fmtStr += ": ";
    }
    fmtStr += str();

    return fmtStr;
  }

  aString MessageStream::throwStr()
  {
    isThrow_ = true;
    return verboseStr();
  }


  void MessageStream::show( bool noEOL)
  {
    if ( messageCallback_[ type_]) {
      messageCallback_[ type_]( *this);
      str("");
    }
    else {
      ostream &os = ( type_ < MSG_WARNING) ? cout : cerr;
      if ( isVerbose_ || !str().empty()) {
        os << ( isVerbose_ ? verboseStr() : str());
        str("");
      }
      if ( !noEOL) {
        os << endl;
      }
    }
    isVerbose_ = false; // Verbose is a one shot thing.
  }

  void MessageStream::forceSegFault()
  {
    cerr << "MessageStream::forceSegFault() - Forced SegFault for debug support." << endl;
      // Cause program to SegFault in debug mode for easier debugging (SDH)
    int *badMemory = 0;
    *badMemory = 42;
  }

} // End namespace Error.
