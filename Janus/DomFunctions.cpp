//
// DST Janus Library (Janus DAVE-ML Interpreter Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2021 Commonwealth of Australia
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

//------------------------------------------------------------------------//
// Title:      Janus/DomFunctions
// Class:      DomFunctions
// Module:     DomFunctions.cpp
// First Date: 2017-04-04
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

// Local Includes
#include "DomFunctions.h"
#include "JanusConfig.h"

// Ute Includes
#include <Ute/aMessageStream.h>
#ifdef HAVE_OPENSSL
  #include <Ute/aCrypt.h>
#endif


using namespace std;
using namespace dstoute;

namespace DomFunctions
{

  const size_t MAX_INCLUDE_DEPTH = 20;

  //------------------------------------------------------------------------//

  XmlResult loadDocument( XmlDoc& document, const aString& dataFileName, const aString& keyFileName = "",
                          unsigned int xml_options = pugi::parse_default);
  XmlResult loadDocument( XmlDoc& document, const aString& dataFileName, const aString& keyFileName,
                          unsigned int xml_options)
  {
    static const aString functionName( "DomFunctions::loadDocument()");

#ifdef HAVE_OPENSSL
    if ( !keyFileName.empty() && keyFileName != "NoFile") {
      unsigned char *documentBuffer = 0;
      size_t documentBufferSize     = 0;
      size_t decodedBufferSize      = 0;
      try {
        aCrypt acrypt;
        acrypt.setPrivateRsaKey( keyFileName);
        documentBufferSize = acrypt.readFile( dataFileName, documentBuffer);
        decodedBufferSize  = acrypt.rsaDecryptBuffer( documentBuffer, documentBufferSize, documentBuffer);
      }
      catch ( exception &/*excep*/) {
        if ( documentBuffer != 0) {
          free( documentBuffer);
        }
        documentBuffer = 0;
        return document.load_file( dataFileName.c_str(), xml_options);
      }
      return document.load_buffer_inplace( documentBuffer, decodedBufferSize, xml_options);
    }
#endif

    return document.load_file( dataFileName.c_str(), xml_options);
  }

  //------------------------------------------------------------------------//

  void copyAllSiblings( XmlNode existingParent, XmlNode otherParent)
  {
    static const aString functionName( "DomFunctions::copyAllSiblings()");

    for ( XmlNode ic = otherParent.first_child(); ic; ic = ic.next_sibling()) {
      XmlNode newNode = existingParent.append_copy( ic);

      if ( !newNode) {
        throw_message( std::runtime_error,
          dstoute::setFunctionName( functionName)
          << "\n Error description: \"" << ic.name() << "\" not inserted."
        );
      }
    }
  }

  //------------------------------------------------------------------------//

  void insertIncludesIntoDOM( XmlDoc& document, size_t &includeDepth, const aString& dataFileName = "", const aString& keyFileName = "");
  void insertIncludesIntoDOM( XmlDoc& document, size_t &includeDepth, const aString& dataFileName, const aString& keyFileName)
  {
    static const aString functionName( "DomFunctions::insertIncludesIntoDOM()");

    if ( ++includeDepth > MAX_INCLUDE_DEPTH) {
      throw_message( std::runtime_error,
        dstoute::setFunctionName( functionName)
        << "\n - XML include depth exceeds " << MAX_INCLUDE_DEPTH << " levels at \"" << dataFileName << "\". Check for recursive include."
      );
    }

    //
    // handle xinclude segments - without fallback or encoding
    //
    XmlNodeList includeList, includeList2;
    getNodesByName( document, "xi:include", includeList);
    getNodesByName( document, "include",    includeList2);
    includeList << includeList2;

    for ( size_t i = 0 ; i < includeList.size(); i++) {
      XmlNode includeNode = includeList[ i];
      aFileString href = getAttribute( includeNode, "href", true);
      aString includeFileName = aFileString( dataFileName).path( true);
      if ( !includeFileName.empty() && href.isRelative()) {
        includeFileName.append( href);
      }
      else {
        includeFileName = href;
      }
      aString parse = getAttribute( includeNode, "parse", false);
      aString xpointer;
      if ( 0 == parse.length() || 0 == parse.compare( "xml")) {
        xpointer = getAttribute( includeNode, "xpointer", false);
      }

      XmlDoc includeDoc;
      XmlResult includeResult = loadDocument( includeDoc, includeFileName, keyFileName, pugi::parse_default | pugi::parse_fragment);
        
      if ( !includeResult) {
        throw_message( std::runtime_error,
          dstoute::setFunctionName( functionName)
          << "\n - XML include [\"" << includeFileName << "\"] " << "loading errors."
          << "\n Error description: " << includeResult.description()
        );
      }

      //
      // Process any includes within this included file first.
      //
      insertIncludesIntoDOM( includeDoc, includeDepth, includeFileName, keyFileName);

      XmlNode currentNode = includeNode;
      if ( 0 < xpointer.length()) {
        // pointer only finds attributes of type "ID"
        XmlNodeList ptrList;
        getNodesByAttributeName( includeDoc, xpointer, ptrList);
        if ( 1 == ptrList.size()) {
          XmlNode ic = ptrList[ 0];
          XmlNode newNode = includeNode.parent().insert_copy_after( ic, currentNode);

          if ( !newNode) {
            throw_message( std::runtime_error,
              dstoute::setFunctionName( functionName)
              << "\n - XML include [\"" << includeFileName << "\"] " << "new node errors."
              << "\n Error description: \"" << ic.name() << "\" not inserted."
            );
          }
        }
        else {
          throw_message( std::runtime_error,
            dstoute::setFunctionName( functionName)
            << "\n - XML include [\"" << includeFileName << "\"] " << "xpointer errors."
            << "\n Error description: Unique attribute \"" << xpointer << "\" not found."
          );
        }
      }
      else {
        for ( XmlNode ic = includeDoc.first_child(); ic; ic = ic.next_sibling()) {
          XmlNode newNode = includeNode.parent().insert_copy_after( ic, currentNode);

          if ( !newNode) {
            throw_message( std::runtime_error,
              dstoute::setFunctionName( functionName)
              << "\n - XML include [\"" << includeFileName << "\"] " << "new node errors."
              << "\n Error description: \"" << ic.name() << "\" not inserted."
            );
          }
          currentNode = newNode;
        }
      }

      includeNode.parent().remove_child( includeNode);
    }

    --includeDepth;
  }

  //------------------------------------------------------------------------//

  XmlResult initialiseDOMForReading( XmlDoc& document, const aString& dataFileName, const aString& keyFileName)
  {
    static const aString functionName( "DomFunctions::initialiseDOMForReading()");

    XmlResult result = loadDocument( document, dataFileName, keyFileName);

    if ( !result) {
      throw_message( std::runtime_error,
        setFunctionName( functionName)
        << "\n - XML file [\"" << dataFileName << "\"] " << "parsed with errors."
        << "\n Error description: " << result.description()
      );
    }

    size_t includeDepth = 0;
    insertIncludesIntoDOM( document, includeDepth, dataFileName, keyFileName);

    return result;
  }

  //------------------------------------------------------------------------//

  XmlResult initialiseDOMForReading( XmlDoc& document, unsigned char* documentBuffer, const size_t& size)
  {
    static const aString functionName( "DomFunctions::initialiseDOMForReading()");
    XmlResult result;
    result = document.load_buffer_inplace( documentBuffer, size, pugi::parse_default);

    if ( !result) {
      throw_message( std::runtime_error,
        dstoute::setFunctionName( functionName)
        << "\n - XML Buffer " << "parsed with errors."
        << "\n Error description: " << result.description()
      );
    }

    size_t includeDepth = 0;
    insertIncludesIntoDOM( document, includeDepth);

    return result;
  }

  //------------------------------------------------------------------------//

} // End namespace DomFunctions
