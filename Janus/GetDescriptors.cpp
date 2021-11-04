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
// Title:      Janus/GetDescriptors
// Class:      Janus
// Module:     GetDescriptors.cpp
// First Date: 2011-11-19
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file GetDescriptors.cpp
 *
 * This code is used during interrogation of an instance the Janus class, 
 * and provides the calling program access to the descriptive elements 
 * contained in a DOM that complies with the DAVE-ML DTD.
 *
 * In keeping with the data's descriptive nature, most returns from these
 * functions are strings, although there are a few numerical
 * values and an enum.
 */

/*
 * Author:  D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes

// Ute Includes
#include <Ute/aString.h>
#include <Ute/aOptional.h>

// Local Includes
#include "Janus.h"

using namespace std;
using namespace dstoute;

namespace janus
{

  const Provenance EMPTY_PROVENANCE;

//------------------------------------------------------------------------//

  const char* Janus::getJanusVersion( VersionType versionType) const
  {
    if ( Janus::SHORT == versionType) {
      return JANUS_VERSION_SHORT;
    }
    else if ( Janus::LONG == versionType) {
      return JANUS_VERSION_LONG;
    }
    else {
      return JANUS_VERSION_HEX;
    }
  }

//------------------------------------------------------------------------//

  const Provenance& Janus::retrieveProvenanceReference(
    const aString& parentID,
    size_t provIndex)
  {
    /*
     * check if a header entry is required
     */
    if ( ( parentID == "fileHeader") && provIndex < fileHeader_.getProvenanceCount()) {
      return fileHeader_.getProvenance()[ provIndex];
    }

    /*
     * check parentID against varID, gtID, utID, function name, checkID
     */
    aOptionalSizeT elementRef;

    elementRef = crossReferenceId( ELEMENT_VARIABLE, parentID);
    if ( elementRef.isValid()) {
      if ( variableDef_[ elementRef].hasProvenance()) {
        return variableDef_[ elementRef].getProvenance();
      }
    }

    elementRef = crossReferenceId( ELEMENT_GRIDDEDTABLE, parentID);
    if ( elementRef.isValid()) {
      if ( griddedTableDef_[ elementRef].hasProvenance()) {
        return griddedTableDef_[ elementRef].getProvenance();
      }
    }

    elementRef = crossReferenceId( ELEMENT_UNGRIDDEDTABLE, parentID);
    if ( elementRef.isValid()) {
      if ( ungriddedTableDef_[ elementRef].hasProvenance()) {
        return ungriddedTableDef_[ elementRef].getProvenance();
      }
    }

    elementRef = crossReferenceId( ELEMENT_FUNCTION, parentID);
    if ( elementRef.isValid()) {
      if ( function_[ elementRef].hasProvenance()) {
        return function_[ elementRef].getProvenance();
      }
    }

    return EMPTY_PROVENANCE;
  }

//------------------------------------------------------------------------//

}// namespace Janus
