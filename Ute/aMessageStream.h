#ifndef AMESSAGESTREAM_H_
#define AMESSAGESTREAM_H_

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
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <valarray>
#include <vector>

// Ute Includes
#include "aString.h"

namespace dstoute {

  class MessageStream : public std::ostringstream {
  public:
    typedef void (MessageCallback)( const MessageStream&);
    enum MessageType {
      MSG_INFORMATION,
      MSG_STATUS,
      MSG_WARNING,
      MSG_ERROR,
      MSG_DEBUG,
      MSG_ALL_TYPES // Keep this last.
    };
    struct _SetType         { MessageType  type;};
    struct _SetTag          { unsigned int tag;};
    struct _SetTagName      { aString  tagName;};
    struct _SetFunctionName { aString  functionName;};

    explicit MessageStream( MessageType typeArg = MSG_INFORMATION, unsigned int tagArg = 0);
    explicit MessageStream( const aString& strArg, MessageType typeArg = MSG_INFORMATION, unsigned int tagArg = 0);
    virtual ~MessageStream();

    static aString& applicationName() { return applicationName_;}
    static void setApplicationName( const aString &applicationName) { applicationName_ = applicationName;}

    static MessageCallback *callback( MessageType type = MSG_ALL_TYPES);
    static void setCallback( MessageCallback *callback, MessageType type = MSG_ALL_TYPES);

    static aStringList messageTypeNames() { return messageTypeName_;}
    static void setMessageTypeNames( const aStringList messageTypeName) { messageTypeName_ = messageTypeName;}

    static void forceSegFault();

    MessageType  type() const           { return type_;}
    const aString& typeName() const     { return messageTypeName_[ type_];}
    unsigned int tag() const            { return tag_;}
    const aString& tagName() const      { return tagName_;}
    const aString& functionName() const { return functionName_;}
    bool isVerbose() const              { return isVerbose_;}
    aString verboseStr() const;
    aString throwStr();

    void setType( MessageType typeArg);
    void setTag( unsigned int tagArg)                   { tag_           = tagArg;}
    void setTagName( const aString &tag_name)           { tagName_       = tag_name;}
    void setFunctionName( const aString &function_name) { functionName_  = function_name;}
    MessageStream& setVerbose( bool is_verbose = true)  { isVerbose_     = is_verbose; return *this;}

    virtual void show( bool noEOL = false);

    // Draw in all operator<< functions as required.
    template <typename T>
    MessageStream& operator<<( const T &t)
    {
      *static_cast<std::ostringstream*>( this) << t;
      return *this;
    }

    // valarray and vector operators
    template <typename T>
    MessageStream& operator<<( const std::valarray<T> &v)
    {
      for ( size_t i = 0; i < v.size(); ++i) {
        this->width( 11);
        *this << v[i];
        if ( i < (v.size() - 1)) {
          *this << ", ";
        }
      }
      return *this;
    }
    template <typename T>
    MessageStream& operator<<( const std::vector<T> &v)
    {
      for ( size_t i = 0; i < v.size(); ++i) {
        this->width( 11);
        *this << v[i];
        if ( i < (v.size() - 1)) {
          *this << ", ";
        }
      }
      return *this;
    }

    // Ensure stream manipulators are activated.
    typedef MessageStream& (*MessageStreamManipulator)(MessageStream&);
    MessageStream& operator<<( MessageStreamManipulator manipulator)
    {
      return manipulator( *this);
    }

    // Intercept std::endl and convert to '\n'.
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    typedef CoutType& (*StandardEndLine)(CoutType&);
    MessageStream& operator<<( StandardEndLine /* manipulator */)
    {
      *this << '\n';
      return *this;
    }

  protected:
    MessageType  type_;
    unsigned int tag_;
    aString      tagName_;
    aString      functionName_;
    bool         isVerbose_;
    bool         isThrow_;

  private:
    static aString                       applicationName_;
    static std::vector<MessageCallback*> messageCallback_;
    static aStringList                   messageTypeName_;
  };

  // Manipulator setType()
  inline MessageStream::_SetType setType( MessageStream::MessageType type)
  {
    MessageStream::_SetType stStruct;
    stStruct.type = type;
    return stStruct;
  }
  inline MessageStream& operator<<( MessageStream &messageStream, MessageStream::_SetType stStruct)
  {
    messageStream.setType( stStruct.type);
    return messageStream;
  }

  // Manipulator setTag()
  inline MessageStream::_SetTag setTag( unsigned int tag)
  {
    MessageStream::_SetTag stStruct;
    stStruct.tag = tag;
    return stStruct;
  }
  inline MessageStream& operator<<( MessageStream &messageStream, MessageStream::_SetTag stStruct)
  {
    messageStream.setTag( stStruct.tag);
    return messageStream;
  }

  // Manipulator setTagName()
  inline MessageStream::_SetTagName setTagName( const aString &tagName)
  {
    MessageStream::_SetTagName stnStruct;
    stnStruct.tagName = tagName;
    return stnStruct;
  }
  inline MessageStream& operator<<( MessageStream &messageStream, MessageStream::_SetTagName stnStruct)
  {
    messageStream.setTagName( stnStruct.tagName);
    return messageStream;
  }

  // Manipulator setFunctionName()
  inline MessageStream::_SetFunctionName setFunctionName( const aString &functionName)
  {
    MessageStream::_SetFunctionName sfnStruct;
    sfnStruct.functionName = functionName;
    return sfnStruct;
  }
  inline MessageStream& operator<<( MessageStream &messageStream, MessageStream::_SetFunctionName sfnStruct)
  {
    messageStream.setFunctionName( sfnStruct.functionName);
    return messageStream;
  }

  // Manipulator verbose()
  inline MessageStream& verbose( MessageStream& messageStream)
  {
    messageStream.setVerbose();
    return messageStream;
  }

  // Manipulator endmsg()
  inline MessageStream& endmsg( MessageStream& messageStream)
  {
    messageStream.show();
    return messageStream;
  }
  inline MessageStream& endmsg_noeol( MessageStream& messageStream)
  {
    messageStream.show( true);
    return messageStream;
  }

  class ErrorStream : public MessageStream {
  public:
    ErrorStream( MessageType typeArg = MSG_ERROR, unsigned int tagArg = 0)
      : MessageStream( typeArg, tagArg) {}
    ErrorStream( const aString& strArg, MessageType typeArg = MSG_ERROR, unsigned int tagArg = 0)
      : MessageStream( strArg, typeArg, tagArg) {}
    virtual ~ErrorStream() {}

    static MessageCallback *callback( MessageType typeArg = MSG_ERROR) { return MessageStream::callback( typeArg);}
    static void setCallback( MessageCallback *callback, MessageType typeArg = MSG_ERROR) { MessageStream::setCallback( callback, typeArg);}
  };

  class WarningStream : public MessageStream {
  public:
    WarningStream( MessageType typeArg = MSG_WARNING, unsigned int tagArg = 0)
      : MessageStream( typeArg, tagArg) {}
    WarningStream( const aString& strArg, MessageType typeArg = MSG_WARNING, unsigned int tagArg = 0)
      : MessageStream( strArg, typeArg, tagArg) {}
    virtual ~WarningStream() {}

    static MessageCallback *callback( MessageType typeArg = MSG_WARNING) { return MessageStream::callback( typeArg);}
    static void setCallback( MessageCallback *callback, MessageType typeArg = MSG_WARNING) { MessageStream::setCallback( callback, typeArg);}
  };

  class StatusStream : public MessageStream {
  public:
    StatusStream( MessageType typeArg = MSG_STATUS, unsigned int tagArg = 0)
    : MessageStream( typeArg, tagArg) {}
    StatusStream( const aString& strArg, MessageType typeArg = MSG_STATUS, unsigned int tagArg = 0)
      : MessageStream( strArg, typeArg, tagArg) {}
    virtual ~StatusStream() {}

    static MessageCallback *callback( MessageType typeArg = MSG_STATUS) { return MessageStream::callback( typeArg);}
    static void setCallback( MessageCallback *callback, MessageType typeArg = MSG_STATUS) { MessageStream::setCallback( callback, typeArg);}
  };

  class DebugStream : public MessageStream {
  public:
    DebugStream( MessageType typeArg = MSG_DEBUG, unsigned int tagArg = 0)
    : MessageStream( typeArg, tagArg) {}
    DebugStream( const aString& strArg, MessageType typeArg = MSG_DEBUG, unsigned int tagArg = 0)
      : MessageStream( strArg, typeArg, tagArg) {}
    virtual ~DebugStream() {}

    static MessageCallback *callback( MessageType typeArg = MSG_DEBUG) { return MessageStream::callback( typeArg);}
    static void setCallback( MessageCallback *callback, MessageType typeArg = MSG_DEBUG) { MessageStream::setCallback( callback, typeArg);}
  };

  class InfoStream : public MessageStream {
  public:
    InfoStream( MessageType typeArg = MSG_INFORMATION, unsigned int tagArg = 0)
      : MessageStream( typeArg, tagArg) {}
    InfoStream( const aString& strArg, MessageType typeArg = MSG_INFORMATION, unsigned int tagArg = 0)
      : MessageStream( strArg, typeArg, tagArg) {}
    virtual ~InfoStream() {}

    static MessageCallback *callback( MessageType typeArg = MSG_INFORMATION) { return MessageStream::callback( typeArg);}
    static void setCallback( MessageCallback *callback, MessageType typeArg = MSG_INFORMATION) { MessageStream::setCallback( callback, typeArg);}
  };

  //
  // Message Stream Macros
  //
  // info_message(), status_message(), error_message, throw_message()
  //
  #define info_message( params)          \
    do {                                 \
      dstoute::InfoStream mout;          \
      mout << params << dstoute::endmsg; \
    } while ( false)

  #define status_message( params)        \
    do {                                 \
      dstoute::StatusStream mout;        \
      mout << params << dstoute::endmsg; \
    } while ( false)

  #define warning_message( params)       \
    do {                                 \
      dstoute::WarningStream mout;       \
      mout << params << dstoute::endmsg; \
    } while ( false)

  #define error_message( params)         \
    do {                                 \
      dstoute::ErrorStream mout;         \
      mout << params << dstoute::endmsg; \
    } while ( false)

  #define throw_message( throwtype, params) \
    do {                                    \
      dstoute::ErrorStream mout;            \
      mout << params;                       \
      throw throwtype( mout.throwStr());    \
    } while ( false)

  //
  // debug_message(): only defined in _DEBUG_ mode, i.e. DEBUGMESSAGES.
  //
  #ifdef _DEBUG_
    #define debug_message( params) \
      { \
        dstoute::DebugStream mout; \
        mout << params << dstoute::endmsg; \
      }
  #else
    #define debug_message( params)
  #endif

  //
  // debug_code(): only execute in DEBUG mode, i.e. DEBUGMESSAGES.
  //
  #ifdef _DEBUG_
    #define debug_code( ...) \
      __VA_ARGS__
  #else
    #define debug_code( ...)
  #endif

} // namespace dstoute

#endif // AMESSAGESTREAM_H_
