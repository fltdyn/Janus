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
// Title:      Janus/Janus
// Class:      Janus
// Module:     Janus.cpp
// First Date: 2003-07-18
// Reference:  Janus Reference Manual
//------------------------------------------------------------------------//

/**
 * \file Janus.cpp
 *
 * Janus performs XML initialisation the pugiXML parser loading the
 * supplied XML file or data buffer a DOM structure.  It holds the
 * data structure and accesses it on request, doing interpolation
 * or other computation as required for output.  It cleans up on termination.
 *
 * This header defines all the elements required to use the XML dataset for
 * flight modelling, and should be included in any source code intended
 * to activate an instance of the Janus class.
 *
 */

/*
 * This header also contains documentation that forms the basis of a
 * doxygen-generated manual.
 *
 * Author: D. M. Newman
 *
 * Modified :  G. Brian, S. Hill
 *
 */

// C++ Includes
#include <iostream>
#include <cstdio>
#include <sstream>
#include <stdexcept>

#include "DomFunctions.h"
#include "Janus.h"

// Ute Includes
#include <Ute/aMessageStream.h>
#include <Ute/aString.h>
#include <Ute/aFile.h>
#include <Ute/aMath.h>
#include <Ute/aOptional.h>

using namespace std;
using namespace dstoute;

namespace janus {

//------------------------------------------------------------------------//

Janus::Janus()
  :
  XmlElementDefinition(),
  documentCreated_( false),
  documentType_( "DAVEfunc"),
  isJanusInitialised_( false),
  hasCheckData_( false),
  isCheckDataCurrent_( false),
  doMathML_to_ExprTk_( true),
  kState_( 0)
{
}


Janus::Janus(
  const aFileString& documentName,
  const aFileString& keyFileName)
  :
  XmlElementDefinition(),
  documentCreated_( false),
  documentType_( "DAVEfunc"),
  isJanusInitialised_( false),
  hasCheckData_( false),
  isCheckDataCurrent_( false),
  doMathML_to_ExprTk_( true),
  kState_( 0)
{
  setXmlFileName( documentName, keyFileName);
}

Janus::Janus( const aFileString& documentName,
              const aFileString& perturbationDocumentName,
              const aFileString& keyFileName)
  :
  XmlElementDefinition(),
  documentCreated_( false),
  documentType_( "DAVEfunc"),
  isJanusInitialised_( false),
  hasCheckData_( false),
  isCheckDataCurrent_( false),
  doMathML_to_ExprTk_( true),
  kState_( 0)
{
  setXmlFileName( documentName, perturbationDocumentName, keyFileName);
}

Janus::Janus(
  unsigned char* documentBuffer,
  size_t documentBufferSize)
  :
  XmlElementDefinition(),
  documentCreated_( false),
  documentType_( "DAVEfunc"),
  isJanusInitialised_( false),
  hasCheckData_( false),
  isCheckDataCurrent_( false),
  doMathML_to_ExprTk_( true),
  kState_( 0)
{
  setXmlFileBuffer( documentBuffer, documentBufferSize);
}

Janus::Janus(
  const Janus &rhs)
  :
  documentCreated_( rhs.documentCreated_),
  dataFileName_( rhs.dataFileName_),
  documentType_( rhs.documentType_),
  isJanusInitialised_( rhs.isJanusInitialised_),
  fileHeader_( rhs.fileHeader_),
  variableDef_( rhs.variableDef_),
  propertyDef_( rhs.propertyDef_),
  breakpointDef_( rhs.breakpointDef_),
  griddedTableDef_( rhs.griddedTableDef_),
  ungriddedTableDef_( rhs.ungriddedTableDef_),
  function_( rhs.function_),
  checkData_( rhs.checkData_),
  hasCheckData_( rhs.hasCheckData_),
  isCheckDataCurrent_( rhs.isCheckDataCurrent_),
  outputIndex_( rhs.outputIndex_),
  nbp_( rhs.nbp_),
  frac_( rhs.frac_),
  bpa_( rhs.bpa_),
  bpi_( rhs.bpi_),
  nOrd_( rhs.nOrd_),
  fracBp_( rhs.fracBp_),
  doMathML_to_ExprTk_( true),
  kState_( 0)
{
  // Reset the Janus pointer in the lower level classes
  resetJanus();
}

Janus& Janus::operator=(
  const Janus &rhs)
{
  if ( this != &rhs) {
    documentCreated_    = rhs.documentCreated_;
    dataFileName_       = rhs.dataFileName_;
    documentType_       = rhs.documentType_;
    isJanusInitialised_ = rhs.isJanusInitialised_;
    fileHeader_         = rhs.fileHeader_;
    variableDef_        = rhs.variableDef_;
    propertyDef_        = rhs.propertyDef_;
    breakpointDef_      = rhs.breakpointDef_;
    griddedTableDef_    = rhs.griddedTableDef_;
    ungriddedTableDef_  = rhs.ungriddedTableDef_;
    function_           = rhs.function_;
    checkData_          = rhs.checkData_;
    hasCheckData_       = rhs.hasCheckData_;
    isCheckDataCurrent_ = rhs.isCheckDataCurrent_;
    outputIndex_        = rhs.outputIndex_;

    dstomath::assign( nbp_,  rhs.nbp_);
    dstomath::assign( frac_, rhs.frac_);
    dstomath::assign( bpa_,  rhs.bpa_);
    dstomath::assign( bpi_,  rhs.bpi_);
    dstomath::assign( nOrd_, rhs.nOrd_);
    fracBp_ = rhs.fracBp_;
    kState_ = 0;
    doMathML_to_ExprTk_ = rhs.doMathML_to_ExprTk_;

    // Reset the Janus pointer in the lower level classes
    resetJanus();
  }

  return *this;
}

//------------------------------------------------------------------------//

Janus::~Janus()
{
  static const aString functionName( "Janus::~Janus()");

  releaseJanusDomParser();
  deleteLuaState();
}

//------------------------------------------------------------------------//

void Janus::clear()
{
  if ( isJanusInitialised()) {
    deleteLuaState();
    *this = Janus();
  }
}

//------------------------------------------------------------------------//

void Janus::setXmlFileName( const dstoute::aFileString& documentName,
                            const dstoute::aFileString& keyFileName)
{
  setXmlFileName( documentName, "", keyFileName);
}

void Janus::setXmlFileName( const dstoute::aFileString& documentName,
                            const dstoute::aFileString& perturbationDocumentName,
                            const dstoute::aFileString& keyFileName)
{
  static const aString functionName( "Janus::setXmlFileName()");

  /*
   * Check if this Janus instance has already been initialised
   */
  clear();

  /*
   * Check that the file to open exists
   */
  aFile file( documentName);
  // If the file does not exist throw an error message
  if ( !file.exists()) {
    throw_message( std::invalid_argument,
       setFunctionName( functionName)
       << "\n - File \"" << documentName << "\" not found."
     );
  }

  /*
   * save a copy of the file name
   */
  dataFileName_= documentName;
  keyFileName_ = keyFileName;
  
  /*
   * Initialise an empty Janus instance, then populate the
   * DOM from the file.
   */
  try {
    DomFunctions::initialiseDOMForReading( document_, dataFileName_, keyFileName);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Can't read file \"" << dataFileName_ << "\".\n - "
      << excep.what()
    );
  }

  /*
   * If a seperate perturbations document exists,
   * load it and add its children to document_
   */
  if ( !perturbationDocumentName.empty()) {

    aFile perturbations( perturbationDocumentName);
    if ( !file.exists()) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName)
        << "\n - File \"" << perturbationDocumentName << "\" not found."
      );
    }

    perturbationsFileName_ = perturbationDocumentName;
    DomFunctions::XmlDoc perturbationsDoc;

    try {
      DomFunctions::initialiseDOMForReading( perturbationsDoc, perturbationsFileName_, keyFileName);
    }
    catch ( exception& excep) {
      throw_message( std::invalid_argument,
        setFunctionName( functionName)
        << "\n - Can't read file \"" << perturbationsFileName_ << "\".\n - "
        << excep.what()
      );
    }

    // Insert pertubations_ into the end of document_ 
    DomFunctions::copyAllSiblings( document_.child( documentType_.c_str()), perturbationsDoc.child( documentType_.c_str()));

    // Release perturbations_
    perturbationsDoc.reset();
  }

  /*
   * Parse the document object model for the XML data file.
   */
  try {
    parseDOM();
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Can't parse file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Initialise the dependencies specified in the document object model
   * and any internal cross-referencing.
   */
  try {
    initialiseDependencies();
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Can't initialise dependencies in file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Release the DOM parser instance for this Janus instance.
   */
  releaseJanusDomParser();


}

//------------------------------------------------------------------------//

void Janus::setXmlFileBuffer(
  unsigned char* documentBuffer,
  const size_t& documentBufferSize)
{
  static const aString functionName( "Janus::setXmlFileBuffer()");

  /*
   * Check if this Janus instance has already been initialised
   */
  clear();

  /*
   * Initialise an empty Janus instance, then populate the
   * DOM from the documentBuffer.
   */
  try {
    DomFunctions::initialiseDOMForReading( document_, documentBuffer, documentBufferSize);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Can't read buffer.\n - "
      << excep.what()
    );
  }

  /*
   * Parse the document object model for the XML data buffer.
   */
  try {
    parseDOM();
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Can't parse buffer.\n - "
      << excep.what()
    );
  }

  /*
   * Initialise the dependencies specified in the document object model
   * and any internal cross-referencing.
   */
  try {
    initialiseDependencies();
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Can't initialise dependencies.\n - "
      << excep.what()
    );
  }

  /*
   * Release the DOM parser instance for this Janus instance.
   */
  releaseJanusDomParser();

  /*
   * Release the documentBuffer for this Janus instance.
   */
  if ( documentBuffer != 0) {
    free( documentBuffer);
  }
  documentBuffer = 0;
}

//------------------------------------------------------------------------//

const CheckData& Janus::getCheckData(
  const bool &evaluate)
{
  if ( !evaluate) return checkData_;

  if ( !isCheckDataCurrent_) {
    /*
     * Perform checkData evaluations, so validity flags are set for all
     * subsequent access
     */
    if ( hasCheckData_) {
      vector<StaticShot>& staticShot = checkData_.getStaticShot();
      size_t numberStaticShot = staticShot.size();

      for ( size_t i = 0; i < numberStaticShot; ++i) {
        staticShot[i].verifyStaticShot();
      }

      isCheckDataCurrent_ = true;
    }
  }

  return checkData_;
}

//------------------------------------------------------------------------//

vector<size_t> Janus::getAllDescendents(
  size_t ix, bool checkRecursively)
{
  // Infinite loop detection in the case of recursively checking for descendents
  if ( variableDef_[ ix].getInEvaluation()) {
    throw_message( logic_error,
      "recursive dependencies detected."
    );
  }
  variableDef_[ ix].setInEvaluation( true);

  size_t nVariableDef = variableDef_.size();
  vector<size_t> descendantsRefs;

  if ( variableDef_[ix].getPerturbationTargetVarIndex().isValid()) {
    descendantsRefs.push_back( variableDef_[ix].getPerturbationTargetVarIndex());
  }

  for ( size_t i = (ix + 1); i < nVariableDef; ++i) {
    vector<size_t>& ancestors = (vector<size_t>&)variableDef_[ i].getAncestorsRef();
    size_t ancestorsSize = ancestors.size();

    for ( size_t j = 0; j < ancestorsSize; ++j) {
      if ( ix == ancestors[ j]) {
        descendantsRefs.push_back( i);

        if ( checkRecursively) {
          const vector<size_t> theseDescendents = getAllDescendents( i, true);
          descendantsRefs.insert( descendantsRefs.end(), theseDescendents.begin(), theseDescendents.end());
        }

        break;
      }
    }
  }

  /*
   * now eliminate repeated values (only applicable if recursively checked)
   */
  if ( checkRecursively) {
    vector<size_t>::iterator it;
    sort( descendantsRefs.begin(), descendantsRefs.end());
    it = unique( descendantsRefs.begin(), descendantsRefs.end());
    descendantsRefs.resize( it - descendantsRefs.begin());
  }

  variableDef_[ ix].setInEvaluation( false);
  return descendantsRefs;
}

//------------------------------------------------------------------------//

vector<size_t> Janus::getAllAncestors(
  size_t ix)
{
  // Note:: This is a recursive function

  vector<size_t> ancestors = variableDef_[ ix].getIndependentVarRef();
  size_t nIndVarRef = ancestors.size();

  for ( size_t i = 0; i < nIndVarRef; ++i) {
    vector<size_t> theseAncestors = getAllAncestors( ancestors[ i]);
    ancestors.insert( ancestors.end(), 
                      theseAncestors.begin(), theseAncestors.end());
  }

  /*
   * now eliminate repeated values
   */
  vector<size_t>::iterator it;
  std::sort( ancestors.begin(), ancestors.end());
  it = std::unique( ancestors.begin(), ancestors.end());
  ancestors.resize( it - ancestors.begin());

  return ancestors;
}

//------------------------------------------------------------------------//

vector<size_t> Janus::getIndependentAncestors(
  size_t ix)
{
  /*
   * get ancestors at all levels of dependency
   */
  vector<size_t> ancestors = getAllAncestors( ix);

  /*
   * now eliminate all except TYPE_INPUT or TYPE_OUTPUT variables
   */
  size_t i = 0;
  while ( i < ancestors.size()) {
    if ( variableDef_[ ancestors[ i]].getType() == VariableDef::TYPE_INTERNAL) {
      ancestors.erase( ancestors.begin() + i);
    } 
    else {
      i++;
    }
  }

  return ancestors;
}

//------------------------------------------------------------------------//

Uncertainty::UncertaintyPdf Janus::getPdfFromAntecedents(
  size_t varIndex)
{
  Uncertainty::UncertaintyPdf pdf = Uncertainty::UNKNOWN_PDF;
  /*
   * NB variable can only depend on those defined before it in the XML,
   * and all contributing variables must have the same type of pdf
   */
  // Check the MATH-ML elements
  for ( size_t i = 0; i < variableDef_[ varIndex].getIndependentVarCount(); ++i) {

    Uncertainty::UncertaintyPdf thisPdf = 
      variableDef_[ variableDef_[ varIndex].
                       getIndependentVarRef()[ i]].
                       getUncertainty().getPdf();

    if ( Uncertainty::UNKNOWN_PDF == pdf) {
      pdf = thisPdf;
    } 
    else if ( ( Uncertainty::NORMAL_PDF == pdf && 
                Uncertainty::UNIFORM_PDF == thisPdf) ||
              ( Uncertainty::UNIFORM_PDF == pdf &&
                Uncertainty::NORMAL_PDF == thisPdf)) {
      pdf = Uncertainty::ERROR_PDF;
      return pdf;
    }
  }

  /*
   * if it depends on gridded or ungridded tables, check those also
   */
  aOptionalSizeT functRef = variableDef_[ varIndex].getFunctionRef();

  if ( functRef.isValid()) {
    Function& thisFunction = function_[ functRef];
    Uncertainty::UncertaintyPdf thisPdf;

    if ( ELEMENT_GRIDDEDTABLE == thisFunction.getTableType()) {
      thisPdf = griddedTableDef_[ thisFunction.getTableRef()].
          getUncertainty().getPdf();
    }
    else {
      thisPdf = ungriddedTableDef_[ thisFunction.getTableRef()].
          getUncertainty().getPdf();
    }

    if ( Uncertainty::UNKNOWN_PDF == pdf) {
      pdf = thisPdf;
    }
    else if (( Uncertainty::NORMAL_PDF == pdf &&
        Uncertainty::UNIFORM_PDF == thisPdf) ||
        ( Uncertainty::UNIFORM_PDF == pdf &&
            Uncertainty::NORMAL_PDF == thisPdf)) {
      pdf = Uncertainty::ERROR_PDF;
      return pdf;
    }
  }

  if (( pdf == Uncertainty::NORMAL_PDF) ||
      ( pdf == Uncertainty::UNIFORM_PDF)) {
    variableDef_[ varIndex].setHasUncertainty( true);
  }

  return pdf;
}

//------------------------------------------------------------------------//

VariableDef& Janus::getVariableDef( const aString& varID)
{
  for ( size_t i = 0; i < variableDef_.size(); ++i) {
    if ( variableDef_[i].getVarID() == varID) {
      return variableDef_[i];
    }
  }
  throw_message( std::range_error,
    setFunctionName( "Janus::getVariableDef()")
    << "\n - Can't find varID \"" << varID << "\"."
  );
}

const VariableDef& Janus::getVariableDef( const dstoute::aString& varID) const
{
  for ( size_t i = 0; i < variableDef_.size(); ++i) {
    if ( variableDef_[i].getVarID() == varID) {
      return variableDef_[i];
    }
  }
  throw_message( std::range_error,
    setFunctionName( "Janus::getVariableDef()")
    << "\n - Can't find varID \"" << varID << "\"."
  );
}

//------------------------------------------------------------------------//

VariableDef* Janus::findVariableDef( const aString& varID)
{
  for ( size_t i = 0; i < variableDef_.size(); ++i) {
    if ( variableDef_[i].getVarID() == varID) {
      return &variableDef_[i];
    }
  }

  return 0;
}

//------------------------------------------------------------------------//

PropertyDef& Janus::getPropertyDef( const aString& ptyID)
{
  for ( size_t i = 0; i < propertyDef_.size(); ++i) {
    if ( propertyDef_[i].getPtyID() == ptyID) {
      return propertyDef_[i];
    }
  }
  throw_message( std::range_error,
    setFunctionName( "Janus::getPropertyDef()")
    << "\n - Can't find ptyID \"" << ptyID << "\"."
  );
}

const PropertyDef& Janus::getPropertyDef( const dstoute::aString& ptyID) const
{
  for ( size_t i = 0; i < propertyDef_.size(); ++i) {
    if ( propertyDef_[i].getPtyID() == ptyID) {
      return propertyDef_[i];
    }
  }
  throw_message( std::range_error,
    setFunctionName( "Janus::getPropertyDef()")
    << "\n - Can't find ptyID \"" << ptyID << "\"."
  );
}

//------------------------------------------------------------------------//

PropertyDef* Janus::findPropertyDef(
  const aString& ptyID)
{
  for ( size_t i = 0; i < propertyDef_.size(); ++i) {
    if ( propertyDef_[i].getPtyID() == ptyID) {
      return &propertyDef_[i];
    }
  }

  return 0;
}

//------------------------------------------------------------------------//

dstoute::aString Janus::getProperty(
  const dstoute::aString& ptyID,
  const dstoute::aString& defProperty)
{
  PropertyDef* ptyDef = findPropertyDef( ptyID);
  if ( ptyDef) return ptyDef->getProperty();

  // Backwards compatibility.
  VariableDef* varDef = findVariableDef( ptyID);
  if ( varDef) return varDef->getDescription();

  return defProperty;
}

//------------------------------------------------------------------------//

dstoute::aStringList Janus::getPropertyList(
  const dstoute::aString& ptyID,
  const dstoute::aStringList& defPropertyList)
{
  PropertyDef* ptyDef = findPropertyDef( ptyID);
  if ( ptyDef) return ptyDef->getPropertyList();

  return defPropertyList;
}

//------------------------------------------------------------------------//

bool Janus::propertyExists( const dstoute::aString& ptyID)
{
  PropertyDef* ptyDef = findPropertyDef( ptyID);
  if ( ptyDef) return true;

  // Backwards compatibility.
  VariableDef* varDef = findVariableDef( ptyID);
  if ( varDef) return true;

  return false;
}

//------------------------------------------------------------------------//

SignalDef& Janus::getSignalDef( const aString& sigID)
{
  for ( size_t i = 0; i < signalDef_.size(); ++i) {
    if ( signalDef_[i].getSigID() == sigID) {
      return signalDef_[i];
    }
  }
  throw_message( std::range_error,
    setFunctionName( "Janus::getSignalDef()")
    << "\n - Can't find sigID \"" << sigID << "\"."
  );
}

//------------------------------------------------------------------------//

SignalDef* Janus::findSignalDef( const aString& sigID)
{
  for ( size_t i = 0; i < signalDef_.size(); ++i) {
    if ( signalDef_[i].getSigID() == sigID) {
      return &signalDef_[i];
    }
  }

  return 0;
}

//------------------------------------------------------------------------//

aOptionalSizeT Janus::crossReferenceId(
  ElementDefinitionEnum elementType,
  const aString& checkID) const
{
  switch ( elementType) {
    case ELEMENT_MODIFICATION:
      for ( size_t i = 0; i < fileHeader_.getModificationCount(); ++i) {
        if ( fileHeader_.getModification()[ i].getModID() == checkID) {
          return i;
        }
      }
	  break;
	  
    case ELEMENT_VARIABLE:
      for ( size_t i = 0; i < variableDef_.size(); ++i) {
        if ( variableDef_[ i].getVarID() == checkID) {
          return i;
        }
	  }
	  break;
	  
    case ELEMENT_VARIABLE_OUTPUT:
      for ( size_t i = 0; i < outputIndex_.size(); ++i) {
        if ( variableDef_[ outputIndex_[ i]].getVarID() == checkID) {
          return outputIndex_[ i];
        }
      }
      break;

    case ELEMENT_PROPERTY:
      for ( size_t i = 0; i < propertyDef_.size(); ++i) {
        if ( propertyDef_[ i].getPtyID() == checkID) {
          return i;
        }
	  }
	  break;

    case ELEMENT_GRIDDEDTABLE:
      for ( size_t i = 0; i < griddedTableDef_.size(); ++i) {
        if ( griddedTableDef_[ i].getGtID() == checkID) {
          return i;
        }
      }
      break;

    case ELEMENT_UNGRIDDEDTABLE:
      for ( size_t i = 0; i < ungriddedTableDef_.size(); ++i) {
        if ( ungriddedTableDef_[ i].getUtID() == checkID) {
          return i;
        }
      }
      break;

	  default:
	    break;
  }

  return aOptionalSizeT();
}

//------------------------------------------------------------------------//

aOptionalSizeT Janus::crossReferenceName(
  ElementDefinitionEnum elementType,
  const aString& checkName) const
{
  switch ( elementType) {
    case ELEMENT_VARIABLE:
      for ( size_t i = 0; i < variableDef_.size(); ++i) {
        if ( variableDef_[ i].getName() == checkName) {
          return i;
        }
      }
      break;

    case ELEMENT_VARIABLE_OUTPUT:
      for ( size_t i = 0; i < outputIndex_.size(); ++i) {
        if ( variableDef_[ outputIndex_[i]].getName() == checkName) {
          return outputIndex_[ i];
        }
      }
      break;

    default:
      break;
  }

  return aOptionalSizeT();
}

//------------------------------------------------------------------------//

size_t Janus::isUniqueID( ElementDefinitionEnum& elementType,
                          const dstoute::aString& checkID) const
{
  size_t uniqueNumber = 0;

  switch ( elementType) {
    case ELEMENT_MODIFICATION:
      for ( size_t i = 0; i < fileHeader_.getModificationCount(); ++i) {
        if ( fileHeader_.getModification()[i].getModID() == checkID) {
          uniqueNumber++;
        }
      }
    break;

    case ELEMENT_VARIABLE:
      for ( size_t i = 0; i < variableDef_.size(); ++i) {
        if ( variableDef_[i].getVarID() == checkID) {
          uniqueNumber++;
        }
    }
    break;

    case ELEMENT_PROPERTY:
      for ( size_t i = 0; i < propertyDef_.size(); ++i) {
        if ( propertyDef_[i].getPtyID() == checkID) {
          uniqueNumber++;
        }
    }
    break;

    case ELEMENT_GRIDDEDTABLE:
      for ( size_t i = 0; i < griddedTableDef_.size(); ++i) {
        if ( griddedTableDef_[i].getGtID() == checkID) {
          uniqueNumber++;
        }
      }
      break;

    case ELEMENT_UNGRIDDEDTABLE:
      for ( size_t i = 0; i < ungriddedTableDef_.size(); ++i) {
        if ( ungriddedTableDef_[i].getUtID() == checkID) {
          uniqueNumber++;
        }
      }
      break;

    case ELEMENT_SIGNAL:
      for ( size_t i = 0; i < signalDef_.size(); ++i) {
        if ( signalDef_[i].getSigID() == checkID) {
          uniqueNumber++;
        }
      }
      break;

    default:
      break;
  }

  return uniqueNumber;
}

//------------------------------------------------------------------------//

void Janus::readDefinitionFromDom(
  const DomFunctions::XmlNode& xmlElement)
{
  switch ( elementType_) {
    case ELEMENT_FILEHEADER:
      fileHeader_.initialiseDefinition( xmlElement);
      break;

    case ELEMENT_VARIABLE:
      variableDef_.push_back ( VariableDef( this, xmlElement));
      break;

    case ELEMENT_PROPERTY:
      propertyDef_.push_back ( PropertyDef( this, xmlElement));
      break;

    case ELEMENT_BREAKPOINTS:
      breakpointDef_.push_back ( BreakpointDef( xmlElement));
      break;

    case ELEMENT_GRIDDEDTABLE:
      griddedTableDef_.push_back ( GriddedTableDef( this, xmlElement));
      break;

    case ELEMENT_UNGRIDDEDTABLE:
      ungriddedTableDef_.push_back ( UngriddedTableDef( this, xmlElement));
      break;

    case ELEMENT_FUNCTION:
      function_.push_back ( Function( this, xmlElement));
      break;

    case ELEMENT_SIGNAL:
      signalDef_.push_back ( SignalDef( xmlElement));
      break;

    case ELEMENT_CHECKDATA:
      checkData_.initialiseDefinition( xmlElement, this);
      hasCheckData_ = true;
      break;

    case ELEMENT_NOTSET:
    default:
      break;
  }

  return;
}

//------------------------------------------------------------------------//

DomFunctions::XmlNode Janus::getDomDocumentRootNode() const
{
  return DomFunctions::getDocumentRootNode( document_, documentType_,
    aString( "getDocumentLevel0(): Wrong DML file type (should be %) or DTD not found.").arg( documentType_));
}

//------------------------------------------------------------------------//

void Janus::exportToDocumentObjectModel(
  const aString& documentType)
{
  if ( isJanusInitialised_) {
    DomFunctions::XmlNode documentElement = document_.child( documentType.c_str());

    // File header definition
    DomFunctions::setComment( documentElement, " File Header ");
    fileHeader_.exportDefinition( documentElement);

    // Variable definitions
    if ( variableDef_.size() > 0) {
      DomFunctions::setComment( documentElement, " Variable Definitions ");
    }
    for ( size_t i = 0; i < variableDef_.size(); ++i) {
      variableDef_[ i].exportDefinition( documentElement);
    }

    // Property definitions
    if ( propertyDef_.size() > 0) {
      DomFunctions::setComment( documentElement, " Property Definitions ");
    }
    for ( size_t i = 0; i < propertyDef_.size(); ++i) {
      propertyDef_[ i].exportDefinition( documentElement);
    }

    // breakpoint definitions
    if ( breakpointDef_.size() > 0) {
      DomFunctions::setComment( documentElement, " Breakpoint Definitions ");
    }
    for ( size_t i = 0; i < breakpointDef_.size(); ++i) {
      breakpointDef_[ i].exportDefinition( documentElement);
    }

    // Gridded table definitions
    if ( griddedTableDef_.size() > 0) {
      DomFunctions::setComment( documentElement, " Gridded Table Definitions ");
    }
    for ( size_t i = 0; i < griddedTableDef_.size(); ++i) {
      griddedTableDef_[i].exportDefinition( documentElement);
    }

    // Ungridded table definitions
    if ( ungriddedTableDef_.size() > 0) {
      DomFunctions::setComment( documentElement, " Ungridded Table Definitions ");
    }
    for ( size_t i = 0; i < ungriddedTableDef_.size(); ++i) {
      ungriddedTableDef_[ i].exportDefinition( documentElement);
    }

    // Function definitions
    if ( function_.size() > 0) {
      DomFunctions::setComment( documentElement, " Function Definitions ");
    }
    for ( size_t i = 0; i < function_.size(); ++i) {
      function_[ i].exportDefinition( documentElement);
    }

    // Check data definitions
    if ( hasCheckData_) {
      // signalDef definitions
      if ( signalDef_.size() > 0) {
        DomFunctions::setComment( documentElement, " SignalDef Definitions ");
      }
      for ( size_t i = 0; i < signalDef_.size(); ++i) {
        signalDef_.at( i).exportDefinition( documentElement);
      }

      DomFunctions::setComment( documentElement, " Checkdata Definitions ");
      checkData_.exportDefinition( documentElement);
    }
  }
}

//------------------------------------------------------------------------//

bool Janus::writeDocumentObjectModel( const dstoute::aFileString& dataFileName) const
{
  return document_.save_file( dataFileName.c_str(), "  ", pugi::format_default);
}

//------------------------------------------------------------------------//

bool Janus::writeDocumentObjectModel( std::ostringstream& sstr) const
{
  document_.save( sstr, "  ", pugi::format_default);
  return true;
}

//------------------------------------------------------------------------//

void Janus::resetJanus()
{
  /*
   * Reset the Janus pointer in the Function classes
   */
  for ( size_t i = 0; i < function_.size(); ++i) {
    function_[ i].resetJanus( this);
  }

  /*
   * Reset the Janus pointer in the VariableDef classes
   */
  for ( size_t i = 0; i < variableDef_.size(); ++i) {
    variableDef_[ i].resetJanus( this);
  }

  /*
   * Reset the Janus pointer in the PropertyDef classes
   */
  for ( size_t i = 0; i < propertyDef_.size(); ++i) {
    propertyDef_[ i].resetJanus( this);
  }

  /*
   * Reset the Janus pointer in the GriddedTableDef classes
   */
  for ( size_t i = 0; i < griddedTableDef_.size(); ++i) {
    griddedTableDef_[ i].resetJanus( this);
  }

  /*
   * Reset the Janus pointer in the UnGriddedTableDef classes
   */
  for ( size_t i = 0; i < ungriddedTableDef_.size(); ++i) {
    ungriddedTableDef_[ i].resetJanus( this);
  }

  /* Not required for the signalDef class at this stage */
}

//------------------------------------------------------------------------//

void Janus::parseDOM()
{
  static const aString functionName( "Janus::parseDOM()");

  /*
   * This function is used to parse the document object model instantiated
   * from either an XML file or a buffer containing the contents of an XML
   * file. The function is called from the setXMLFileName() and the
   * setXMLFileBuffer() functions.
   */

  DomFunctions::XmlNode documentElement = document_.child( documentType_.c_str());

  /*
   * Read and instantiate the fileHeader entries as class within Janus structure
   */
  elementType_ = ELEMENT_FILEHEADER;
  try {
    DomFunctions::initialiseChild( this,
                                   documentElement,
                                   dataFileName_,
                                   "fileHeader",
                                   true);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Read and instantiate the Level 1 variableDef elements.
   */
  elementType_ = ELEMENT_VARIABLE;
  try {
    DomFunctions::initialiseChildren( this,
                                      documentElement,
                                      dataFileName_,
                                      "variableDef",
                                      false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Read and instantiate the propertyDef elements.
   */
  elementType_ = ELEMENT_PROPERTY;
  try {
    DomFunctions::initialiseChildren( this,
                                      documentElement,
                                      dataFileName_,
                                      "propertyDef",
                                      false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  if ( variableDef_.size() == 0 && propertyDef_.size() == 0) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - There must be at least one propertyDef or variableDef defined in file \"" << dataFileName_ << "\"."
    );
  }

  /*
   * Read and instantiate the Level 1 breakpointDef elements.
   * Includes the simple function definitions which uses independentVarPts as
   * breakpoints
   */
  elementType_ = ELEMENT_BREAKPOINTS;
  try {
    DomFunctions::initialiseChildren( this,
                                      documentElement,
                                      dataFileName_,
                                      "breakpointDef",
                                      false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Read and instantiate the Level 1 griddedTableDef elements (excluding those
   * embedded within functions). Includes dependentVarPts from simple
   * functionDefn entries.
   */
  elementType_ = ELEMENT_GRIDDEDTABLE;
  try {
    DomFunctions::initialiseChildren( this,
                                      documentElement,
                                      dataFileName_,
                                      "griddedTableDef",
                                      false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Read and instantiate the Level 1 ungriddedTableDef elements (excluding those
   * embedded within functions).
   */
  elementType_ = ELEMENT_UNGRIDDEDTABLE;
  try {
    DomFunctions::initialiseChildren( this,
                                      documentElement,
                                      dataFileName_,
                                      "ungriddedTableDef",
                                      false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Read and instantiate the Level 1 function elements.
   * Includes gridded and ungridded table definitions embedded within the function.
   * Includes setting the dependent and independent variable references
   * within the variableDef_ vector.
   */
  elementType_ = ELEMENT_FUNCTION;
  try {
    DomFunctions::initialiseChildren( this,
                                      documentElement,
                                      dataFileName_,
                                      "function",
                                      false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Read and instantiate the Level 1 signalDef elements.
   */
  elementType_ = ELEMENT_SIGNAL;
  try {
    DomFunctions::initialiseChildren( this,
                                      documentElement,
                                      dataFileName_,
                                      "signalDef",
                                      false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Read and instantiate the Level 1 checkData elements.
   */
  elementType_ = ELEMENT_CHECKDATA;
  try {
    DomFunctions::initialiseChild( this,
                                   documentElement,
                                   dataFileName_,
                                   "checkData",
                                   false);
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - for file \"" << dataFileName_ << "\"\n - "
      << excep.what()
    );
  }

  /*
   * Reset elementType_ to ensure readDefinitionFromDom does nothing if
   * called later by a derived class.
   */
  elementType_ = ELEMENT_NOTSET;

}

//------------------------------------------------------------------------//

void Janus::initialiseDependencies()
{
  static const aString functionName( "Janus::initialiseDependencies()");

  /*
   * Set up dependent variable cross-references.
   *  This sets up cross references of function definitions in dependent variableDefs.
   */
  aOptionalSizeT xRef;
  for ( size_t i = 0; i < function_.size(); ++i) {
    xRef = function_[ i].getDependentVarRef();

    if ( xRef.isValid()) {
      variableDef_[ xRef].setFunctionRef( i);
      variableDef_[ xRef].setMethod( VariableDef::METHOD_FUNCTION);
    }
    else {
      throw_message( std::runtime_error,
        setFunctionName( functionName)
        << "\n - for file \"" << dataFileName_
        << "\n - In function \"" << function_[ i].getName() << "\", dependent variable varID not defined in VariableDef list."
      );
    }
  }

  /*
   * Update perturbation targets now that Janus knows about all perturbations.
   * Has to be done before setting descendants and ancestors.
   */
  size_t variableDefSize = variableDef_.size();

  bool recursivelyCheckDependencies = false;
  for ( size_t i = 0; i < variableDefSize; ++i) {
    const aOptionalSizeT targetIdx = variableDef_[i].getPerturbationTargetVarIndex();
    if ( targetIdx.isValid()) {
      variableDef_[ targetIdx].setPerturbationVarIndex( i);
      recursivelyCheckDependencies = true;
    }
  }

  /*
   * The dependentVarRef_ arrays are only partly set up at this stage with first
   * level (direct) dependencies included.  Now have to recurse down the
   * dependency tree to ensure that all lower level dependencies are
   * included, but no variables are repeated.
   */
  for ( size_t i = 0; i < variableDefSize; ++i) {
    variableDef_[ i].setAncestorsRef( getAllAncestors( i));
  }

  for ( size_t i = 0; i < variableDefSize; ++i) {
    variableDef_[ i].setDescendantsRef( getAllDescendents( i, recursivelyCheckDependencies));
  }

  /*
   * Set the MATHML cross-references.
   */
  for ( size_t i = 0; i < variableDefSize; ++i) {
    if ( variableDef_[ i].getMethod() == VariableDef::METHOD_MATHML) {
      variableDef_[ i].setMathMLDependencies();
    }
  }

  for ( size_t i = 0; i < variableDefSize; ++i) {
    if ( variableDef_[ i].isOutput()) {
      switch ( variableDef_[ i].getMethod()) {
      case VariableDef::METHOD_FUNCTION:
      case VariableDef::METHOD_MATHML:
      case VariableDef::METHOD_SCRIPT:
        outputIndex_.push_back( i);
        break;

      default:
        break;
      }
    }
  }

  /*
   * The initialisation of Function table cross references has been delayed
   * until this point so that all relevant variableDef processing has been
   * completed first.
   */
  const FunctionList& functionList = getFunction();
  for ( size_t i = 0; i < functionList.size(); ++i) {
    functionList[i].getTableRef();
  }

  /*
   * The Uncertainty initialisation has been delayed until this point
   * so that all the other variableDef, griddedTableDef, ungriddedTableDef
   * and function processing has been completed. This was required to ensure
   * cross-indexing based on varIDs has been completed for each variable and
   * table. The uncertainty for the griddedTableDefs and ungriddedTableDefs
   * is processed prior to the variableDefs.
   */
  size_t nDef = griddedTableDef_.size();
  for ( size_t i = 0; i < nDef; ++i) {
    if ( griddedTableDef_[i].hasUncertainty()) {
      Uncertainty& uncertaintyElement = griddedTableDef_[i].getUncertainty();
      uncertaintyElement.initialiseDefinition(
        this,
        DomFunctions::getChild( griddedTableDef_[i].getDOMElement(), "uncertainty"));
    }
  }

  nDef = ungriddedTableDef_.size();
  for ( size_t i = 0; i < nDef; ++i) {
    if ( ungriddedTableDef_[i].hasUncertainty()) {
      Uncertainty& uncertaintyElement = ungriddedTableDef_[i].getUncertainty();
      uncertaintyElement.initialiseDefinition(
        this,
        DomFunctions::getChild( ungriddedTableDef_[i].getDOMElement(), "uncertainty"));
    }
  }

  for ( size_t i = 0; i < variableDefSize; ++i) {
    if ( variableDef_[i].hasUncertainty()) {
      Uncertainty& uncertaintyElement = variableDef_[i].getUncertainty();
      uncertaintyElement.initialiseDefinition(
        this,
        DomFunctions::getChild( variableDef_[i].getDOMElement(), "uncertainty"));
    }
    else {
      /*
       * If No uncertainty element entered for the variable then
       * check if the uncertainty is set for antecedents (?) and
       * set accordingly.
       */
      variableDef_[ i].getUncertainty().setPdf( getPdfFromAntecedents( i));

      if ( Uncertainty::NORMAL_PDF == variableDef_[ i].
           getUncertainty().getPdf()) {
        variableDef_[ i].getUncertainty().setBoundsSize( 1);
      }
      else if ( Uncertainty::UNIFORM_PDF ==
                variableDef_[ i].getUncertainty().getPdf()) {
        variableDef_[ i].getUncertainty().setBoundsSize( 2);
      }
    }
  }

  /*
   * Allocate work space arrays, based on max of 32 DoF.  Pointers for these
   * are set up in Janus.h.  By setting these up just once, and using them
   * from the various interpolation programs, run time execution is
   * greatly improved.
   */
  nbp_.resize( MAXDOF, 0);                   // linear interp num of bps
  frac_.resize( MAXDOF, 0.0);                // linear interp normalised x input
  bpa_.resize( MAXDOF, 0);                   // linear interp bp index
  bpi_.resize( MAXDOF, 0);                   // linear interp bp 0-1 counter
  nOrd_.resize( MAXDOF, 0);                  // poly interp order
  fracBp_.resize( MAXDOF, MAXPOLYORDER + 1); // poly interp normalised bps

  /*
   * NOTE::
   * CheckData evaluations could be performed here to ensure the validity flags
   * are set for all subsequent access. However, it has been chosen not to do
   * this by default and leave the CheckData evaluation to be called
   * explicitly by the external application. This is achieved by calling the
   * getCheckData() function:
   *
   * For example:
   * ------------
   * ...
   * getCheckData();
   * ...
   */

  /*
   * Convert MathML scripts into ExprTk scripts if possible and as required.
   */
  if ( doMathML_to_ExprTk_) {
    for ( size_t i = 0; i < variableDefSize; ++i) {
      variableDef_[ i].testForMathML_to_ExprTkScript();
    }
  }

  /*
   * Check to see if variables:
   *   1. isInput have been initialised
   *   2. isInput that are unrequired
   *   3. isInput don't have calculations defined for them
   *   4. functions don't require initialisation
   */
  aList<size_t> uninitialisedVars;
  aList<size_t> unrequiredInputVars;
  aList<size_t> haveCalcsInputVars;
  aList<size_t> initialisedFunctionVars;
  for ( size_t i = 0; i < variableDefSize; ++i) {
    // Input Variables.
    if ( variableDef_[ i].getType() == VariableDef::TYPE_INPUT) {
      // Check if used.
      if ( !variableDef_[ i].isReferencedExternally() &&
           !variableDef_[ i].hasDescendantsRef()) {
        unrequiredInputVars << i;
      }
      // Check if calculation defined.
      switch ( variableDef_[ i].getMethod()) {
      case VariableDef::METHOD_PLAIN_VARIABLE:
      case VariableDef::METHOD_ARRAY:
        break;

      default:
        haveCalcsInputVars << i;
      }
    }
    // Check if uninitialised.
    if ( variableDef_[ i].getMethod() == VariableDef::METHOD_PLAIN_VARIABLE &&
         variableDef_[ i].hasDescendantsRef() &&
         variableDef_[ i].getInitialValueString().empty()) {
      uninitialisedVars << i;
    }
    // Check if initialised and shouldn't be.
    if ( variableDef_[ i].getMethod() == VariableDef::METHOD_FUNCTION &&
         !variableDef_[ i].getInitialValueString().empty()) {
      initialisedFunctionVars << i;
    }
  }

  /*
   * Check to see if any uniquely identifying ids have been duplicated
   */
  #define _DUPLICATE_CHECK_MACRO_( toCheck, checkMethod, offending) \
    aList<size_t> offending;                                        \
    {                                                               \
      const size_t toCheck##size = toCheck.size();                  \
      for ( size_t i = 0; i < toCheck##size; ++i) {                 \
        const aString& ref = toCheck[i].checkMethod();              \
        for ( size_t j = i + 1; j < toCheck##size; ++j) {           \
          if ( toCheck[j].checkMethod() == ref) {                   \
            offending.push_back( j);                                \
            break;                                                  \
          }                                                         \
        }                                                           \
      }                                                             \
    }

  // VariableDef
  _DUPLICATE_CHECK_MACRO_( variableDef_, getVarID, duplicateVars);

  // PropertyDef
  _DUPLICATE_CHECK_MACRO_( propertyDef_, getPtyID, duplicateProps);

  // Breakpoint
  _DUPLICATE_CHECK_MACRO_( breakpointDef_, getBpID, duplicateBps);

  // GriddedTable
  _DUPLICATE_CHECK_MACRO_( griddedTableDef_, getGtID, duplicateGts);

  // UngriddedTable
  _DUPLICATE_CHECK_MACRO_( ungriddedTableDef_, getUtID, duplicateUgts);

  #undef _DUPLICATE_CHECK_MACRO_

  //
  // _WARN_MACRO_: Used below to reduce code duplication.
  //
  #define _WARN_MACRO_( inputList, elementList, nameAccessor, comment) \
  if ( inputList.size()) {                                             \
    aString varList;                                                   \
    for ( size_t i = 0; i < inputList.size(); ++i) {                   \
      varList += "\n   - \"";                                          \
      varList += elementList[ inputList[ i]].nameAccessor();           \
      varList += "\"";                                                 \
    }                                                                  \
    warning_message(                                                   \
      setFunctionName( functionName)                                   \
      << verbose                                                       \
      << "\n - For Janus DAVE-ML file: \"" << dataFileName_ << "\""    \
      << "\n - " << comment                                            \
      << varList << '\n'                                               \
    );                                                                 \
  }

  _WARN_MACRO_( uninitialisedVars,       variableDef_,       getVarID, "The following VariableDefs require an \"initialValue\" attribute for safety:");
  _WARN_MACRO_( unrequiredInputVars,     variableDef_,       getVarID, "The following input VariableDefs have not been used:");
  _WARN_MACRO_( haveCalcsInputVars,      variableDef_,       getVarID, "The following input VariableDefs should not have functions or calculations defined:");
  _WARN_MACRO_( initialisedFunctionVars, variableDef_,       getVarID, "The following function VariableDefs don't require initialisation:");
  _WARN_MACRO_( duplicateVars,           variableDef_,       getVarID, "Multiple declarations of the following VariableDefs have been found:");
  _WARN_MACRO_( duplicateProps,          propertyDef_,       getPtyID, "Multiple declarations of the following PropertyDefs have been found:");
  _WARN_MACRO_( duplicateBps,            breakpointDef_,     getBpID,  "Multiple declarations of the following BreakpointDefs have been found:");
  _WARN_MACRO_( duplicateGts,            griddedTableDef_,   getGtID,  "Multiple declarations of the following GriddedTableDefs have been found:")
  _WARN_MACRO_( duplicateUgts,           ungriddedTableDef_, getUtID,  "Multiple declarations of the following UngriddedTableDefs have been found:");

  #undef _WARN_MACRO_

  /*
   * Set the Janus instance initialisation flag to true.
   */
  isJanusInitialised( true);
}

//------------------------------------------------------------------------//

size_t Janus::exportTo(
  ExportObjectType exportObjectType,
  const aFileString& dataFileName,
  ostringstream& dataFileBuffer)
{
  static const aString functionName( "Janus::exportTo()");

  /*
   * Initiate the DOM with the DAVEfunc document type
   */
  try {
    initiateDocumentObjectModel();
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Error initiating the DOM.\n - "
      << excep.what()
    );
  }

  /*
   * Export the Janus constructs to the DOM.
   */
  try {
    Janus::exportToDocumentObjectModel();
  }
  catch ( exception &excep) {
    throw_message( std::invalid_argument,
      setFunctionName( functionName)
      << "\n - Error exporting the Janus data to the DOM.\n - "
      << excep.what()
    );
  }

  /*
   * Write the DOM to the output file or buffer
   */
  size_t returnSize = 0;
  switch ( exportObjectType) {
    case Janus::FILE:
      if ( !writeDocumentObjectModel( dataFileName)) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName)
          << "\n - Error writing the DOM to a data file."
        );
      }
      break;

    case Janus::BUFFER:
      if ( !writeDocumentObjectModel( dataFileBuffer)) {
        throw_message( std::invalid_argument,
          setFunctionName( functionName)
          << "\n - Error writing the DOM to a data buffer."
        );
      }

      dataFileBuffer.seekp(0, ios_base::end);
      returnSize = dataFileBuffer.tellp();
      dataFileBuffer.seekp(0, ios_base::beg);
      break;

    default:
      break;
  }

  /*
   * Release the memory associated with the document.
   */
  releaseJanusDomParser();

  return returnSize;
}

//------------------------------------------------------------------------//

void Janus::displayCheckDataSummary(
  const CheckData& checkData)
{
  static const aString functionName( "Janus::displayCheckDataSummary()");

  /*
   * This function display a summary of the Check Data compliance.
   * It is currently a private function used internally with the
   * Janus class; however, at a later stage is made be accessible
   * as a public function.
   *
   * A more comprehensive summary of the Check Data compliance may be
   * sourced using the testCheckData program.
   */

  WarningStream wout;
  wout << setFunctionName( functionName) << endl;

  /*
   * Display the check data for each of the StaticShot cases
   */
  size_t nss = checkData.getStaticShotCount();
  wout << " Number of static shots = " << nss << endl;

  for ( size_t j = 0; j < nss; ++j) {
    StaticShot staticShot = checkData.getStaticShot( j);
    CheckInputs checkInputs = staticShot.getCheckInputs();
    CheckOutputs checkOutputs = staticShot.getCheckOutputs();

    /*
     * A summary of the check case inputs, internal values and outputs
     */
    wout << " staticShot[" << j << "] : " << endl
         << "      Name                      = "
         << staticShot.getName() << endl
         << "      Number of check inputs    = "
         << staticShot.getCheckInputs().getSignalCount() << endl;
    if ( staticShot.hasInternalValues()) {
      wout << "      Number of internal values = "
           << staticShot.getInternalValues().getSignalCount() << endl;
    }
    wout << "      Number of check outputs   = "
         << staticShot.getCheckOutputs().getSignalCount() << endl;

    /*
     * A listing of the Internal and Output variables that failed
     * their check case comparisons.
     */
    size_t nInvalid = staticShot.getInvalidVariableCount();
    if ( 0 < nInvalid) {
      wout << "Invalid Variable Count: " << nInvalid << endl;
      for ( size_t k = 0; k < nInvalid; ++k) {
        aString failedVariable =
          staticShot.getInvalidVariableMessage( int( k));
        wout << "  Invalid variable : " << failedVariable << endl;
      }
    }
    else {
      wout << "  No invalid cases from staticShot evaluation : " << j << " ... " << endl;
    }
  }
  wout << endmsg;
}

//------------------------------------------------------------------------//

} // End namespace janus
